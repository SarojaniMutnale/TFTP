# TFTP Implementation

A complete implementation of the Trivial File Transfer Protocol (TFTP) in C, featuring both client and server components.

## Overview

This project provides a simple file transfer mechanism based on TFTP (RFC 1350). It supports both uploading (PUT) and downloading (GET) files between a client and server.

## Features

- **TFTP Client**: Connect to a TFTP server to upload or download files
- **TFTP Server**: Listen for TFTP requests and handle file transfers
- **Multiple Transfer Modes**:
  - NetASCII mode
  - Octet (binary) mode
- **UDP-based communication** using standard Berkeley sockets
- Color-coded console output for better readability

## Project Structure

```
TFTP/
├── TFTP_Client/
│   ├── tftp.c          # Client data send/recv functions
│   ├── tftp.h          # Client shared header
│   ├── tftp_client.c   # Client main implementation
│   ├── tftp_client.h   # Client struct and function prototypes
│   ├── file.txt        # Sample file for testing
│   └── get.txt         # Output file for received data
└── TFTP_Server/
    ├── tftp.c          # Server data send/recv functions
    ├── tftp.h          # Server shared header
    ├── tftp_server.c   # Server main implementation
    ├── file.txt        # Sample file for testing
    └── get.txt         # Output file for received data
```

## Requirements

- GCC or compatible C compiler
- POSIX-compliant system (Linux, macOS, or Windows with WSL)
- Standard networking libraries

## Building

### Client

```bash
cd TFTP_Client
gcc -o tftp_client tftp_client.c tftp.c -Wall
```

### Server

```bash
cd TFTP_Server
gcc -o tftp_server tftp_server.c tftp.c -Wall
```

## Usage

### Start the Server

```bash
./tftp_server [port]
```

Default port: 69 (TFTP standard port)

### Run the Client

```bash
./tftp_client <server_ip> <port>
```

Once connected, you can:
- **GET**: Download a file from the server
- **PUT**: Upload a file to the server

## Protocol Details

The implementation follows RFC 1350 with these packet types:
- **RRQ**: Read Request (client → server)
- **WRQ**: Write Request (client → server)
- **DATA**: Data packet (server → client or client → server)
- **ACK**: Acknowledgment packet
- **ERROR**: Error packet

Each data packet contains:
- Block number
- Data (up to 512 bytes per packet)
- The transfer completes when a data packet with less than 512 bytes is received

## License

This project is for educational purposes.

## Author

SarojaniMutnale