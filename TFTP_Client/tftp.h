#ifndef TFTP_CLIENT_H
#define TFTP_CLIENT_H

#include <netinet/in.h>   // sockaddr_in
#include <sys/socket.h>   // socklen_t
#include <arpa/inet.h>    // INET_ADDRSTRLEN

#include "tftp.h"
#include "tftp_client.h"

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// typedef struct tftp_packet tftp_packet;

typedef struct 
{
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t server_len;
    char server_ip[INET_ADDRSTRLEN];
    int server_port;
    
} tftp_client_t;

#define BLUE "\033[1;34m"           //bold blue macro
#define RED "\033[1;31m"            //bold red macro
#define RESET "\033[0m"             //black macro

// Function prototypes
void connect_to_server(tftp_client_t *client);
void put_file(tftp_client_t *client);
void get_file(tftp_client_t *client);
int validate_ip(char* str);
void select_mode();
void send_file(tftp_client_t *client, tftp_packet* packet, int ret_open, int mode);
void recv_data(tftp_client_t *client, tftp_packet *packet, int ret_open, int mode);


#endif