# SYN-Flooder (Linux)

A high-performance, multithreaded SYN-flood tool written in C. It is designed to run on **Linux systems** to generate a high volume of SYN packets with spoofed source IPs.

> **Important Note:** This code is written for Linux. It uses POSIX network headers (`arpa/inet.h`, `sys/socket.h`, etc.) that are not native to Windows. To run this on Windows, you must use a Linux compatibility layer like WSL.

---

## Supported Operating Systems

- **Linux** (Native)
- **Windows** (via Windows Subsystem for Linux - WSL)
- **macOS** (Should work, may require minor adjustments)

---

## Installation Guide

### Option 1: On a Native Linux System (Ubuntu, Debian, etc.)

If you are already running Linux, you just need the build tools.

1.  **Install Build Tools:**
    Open your terminal and install the `build-essential` package, which includes `gcc` and `pthread`.
    ```bash
    sudo apt update
    sudo apt install build-essential
    ```

2.  **Proceed to Compilation.**

### Option 2: On Windows 10/11 (Using WSL)

WSL (Windows Subsystem for Linux) lets you run a real Linux environment directly on Windows. This is the recommended way to use this tool on Windows.

1.  **Install WSL:**
    Open PowerShell **as an Administrator** and run the following command. This will install WSL and the default Ubuntu distribution.
    ```powershell
    wsl --install
    ```
    After the installation, **restart your computer**.

2.  **Set up Linux:**
    After restarting, open the Start Menu and launch "Ubuntu". You will be prompted to create a username and password for your Linux environment. These do not need to match your Windows credentials.

3.  **Install Build Tools inside WSL:**
    Inside the Ubuntu terminal, update the package list and install the necessary compiler tools.
    ```bash
    sudo apt update
    sudo apt install build-essential
    ```

4.  **Locate Your Files:**
    Your Windows drives (like C:) are automatically mounted inside WSL. Your Desktop folder, for example, is located at `/mnt/c/Users/YourWindowsUsername/Desktop`. Navigate to the directory where you saved the `syn_flooder.c` file.
    ```bash
    # Example for the user 'fsoci'
    cd /mnt/c/Users/fsoci/Desktop/ddos
    ```

5.  **Proceed to Compilation.**

---

## Compilation

Once you have a Linux terminal (either native or via WSL), navigate to the directory containing `syn_flooder.c` and run:

```bash
gcc syn_flooder.c -o syn_flooder -pthread

This will create an executable file named syn_flooder.
Usage

You must run this tool with root privileges, as creating raw sockets requires administrative access.

Command Syntax:
bash

sudo ./syn_flooder <target_ip> <target_port> <threads> <packets_per_thread>

Parameters:

    <target_ip>: The IP address of the target server.
    <target_port>: The port on the target server to attack (e.g., 80 for HTTP).
    <threads>: The number of threads to use. A good starting point is the number of your CPU cores.
    <packets_per_thread>: The number of packets each thread will send before exiting.

Example:
To attack 192.168.1.100 on port 80 using 8 threads, with each thread sending 1,000,000 packets:
bash

sudo ./syn_flooder 192.168.1.100 80 8 1000000

Disclaimer

This software is for educational and authorized testing purposes only. The user is solely responsible for any and all actions and consequences resulting from the use of this tool. The authors do not endorse or encourage any illegal activity.
