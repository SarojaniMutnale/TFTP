#include "tftp.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

void recv_data(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet, int ret_open, int mode)
{
    if (mode == 2)     // implement octal mode
    {
        printf(BLUE"INFO : "RESET"Mode - OCTAL\n");
        int count = 1;
        do
        {
            memset(packet, 0, sizeof(*packet));              //initializing all members of packet with 0
            client_len = sizeof(client_addr);

            int r = recvfrom(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, &client_len);
            //printf(BLUE"INFO : "RESET"Packet %d received successfully\n",count);
            //fwrite(packet->body.data_packet.data, 1, packet->body.data_packet.data_size, stdout);
            int w = write(ret_open, packet->body.data_packet.data, packet->body.data_packet.data_size);
            if (w == -1)
            {
                printf(RED"\nERROR : "RESET"write failed (ret - %d) (errno = %d)\n", w, errno);
                return;
            }
            packet->body.ack_packet.block_number = packet->body.data_packet.block_number + 1;
            packet->body.ack_packet.data_size = packet->body.data_packet.data_size;
            
            //printf(BLUE"INFO : "RESET"Sending packet %d ack\n", count++);
            sendto(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        } while (packet->body.data_packet.data_size == 1);
    }
    else // implement netascii mode
    {
        printf(BLUE"INFO : "RESET"Mode - NETASCII\n");
        int count = 1;
        do
        {
            memset(packet, 0, sizeof(*packet));            //initializing all members of packet with 0
            client_len = sizeof(client_addr);

            int r = recvfrom(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, &client_len);
            //printf(BLUE"INFO : "RESET"Packet %d received successfully\n",count);
            //fwrite(packet->body.data_packet.data, 1, packet->body.data_packet.data_size, stdout);
            int w = write(ret_open, packet->body.data_packet.data, packet->body.data_packet.data_size);
            if (w == -1)
            {
                printf(RED"\nERROR : "RESET"write failed (ret - %d) (errno = %d)\n", w, errno);
                return;
            }
            packet->body.ack_packet.block_number = packet->body.data_packet.block_number + 1;
            packet->body.ack_packet.data_size = packet->body.data_packet.data_size;
            
            //printf(BLUE"INFO : "RESET"Sending packet %d ack\n", count++);
            sendto(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        } while (packet->body.data_packet.data_size >= 1);
    }
}

void send_data(int sockfd, struct sockaddr_in server_addr, socklen_t server_len, tftp_packet *packet, int ret_open, int mode)
{
    if (mode == 0 || mode == 1)
    {
        printf(BLUE"INFO : "RESET"Mode - DEFAULT\n");
        //printf(BLUE"INFO : "RESET"Received ack from server\n"BLUE"INFO : "RESET"Ready to send data from client\n");
        int i = 1; // to keep a track of packet number
        int ret = 512;
        do
        {
            if (ret < 512)
            {
                printf(BLUE"\n***DATA TRANSFER COMPLETE***\n"RESET);
                return;
            }
            memset(packet, 0, sizeof(*packet));          //initializing all members of packet with 0
            ret = read(ret_open, packet->body.data_packet.data, 512); // reading 512 bytes of data from file
            packet->body.data_packet.data_size = ret;
            packet->body.data_packet.block_number = i;

            //printf(BLUE"INFO : "RESET"Sent %d packet\n",i);
            sendto(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

            //printf(BLUE"INFO : "RESET"Waiting for %d packet ack from server\n",i);
            int r = recvfrom(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&server_addr, &server_len);
            if (r == -1)
            {
                printf(RED"\nERROR : "RESET"recv failed\n");
                return;
            }
            i++;
        } while ((packet->body.ack_packet.block_number + 1 > packet->body.data_packet.block_number));
    }
    if (mode == 2)
    {
        printf(BLUE"INFO : "RESET"Mode - OCTAL\n");   
        //printf(BLUE"INFO : "RESET"Received ack from server\n"BLUE"INFO : "RESET"Ready to send data from client\n");
        int i = 1; // to keep a track of packet number
        int ret = 1;
        do
        {
            if (ret < 1)
            {
                printf(BLUE"\n***DATA TRANSFER COMPLETE***\n"RESET);
                return;
            }
            memset(packet, 0, sizeof(*packet));           //initializing all members of packet with 0
            ret = read(ret_open, packet->body.data_packet.data, 1); // reading 1 bytes of data from file
            packet->body.data_packet.data_size = ret;
            packet->body.data_packet.block_number = i;

            //printf(BLUE"INFO : "RESET"Sent %d packet\n",i);
            sendto(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

            //printf(BLUE"INFO : "RESET"Waiting for %d packet ack from server\n",i);
            int r = recvfrom(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&server_addr, &server_len);
            if (r == -1)
            {
                printf(RED"\nERROR : "RESET"recv failed\n");
                return;
            }
            i++;
        } while ((packet->body.ack_packet.block_number + 1 > packet->body.data_packet.block_number));
    }
    if (mode == 3)
    {
        printf(BLUE"INFO : "RESET"Mode - NETASCII\n");
        //printf(BLUE"INFO : "RESET"Received ack from server\n"BLUE"INFO : "RESET"Ready to send data from client\n");
        int i = 1; // to keep a track of packet number
        int ret = 8;
        do
        {
            if ((ret == 0) || (!strcmp(packet->body.data_packet.data, "\0")))
            {
                printf(BLUE"\n***DATA TRANSFER COMPLETE***\n"RESET);
                return;
            }
            char buf[2];

            memset(packet, 0, sizeof(*packet));               //initializing all members of packet with 0
            ret = read(ret_open, buf, 1);
            if (buf[0] == '\n')
            {
                buf[0] = '\r';
                buf[1] = '\n';
                strcpy(packet->body.data_packet.data, buf);         //substituting '\n' with '\r\n'
                packet->body.data_packet.data_size = 2;
            }
            else
            {
                packet->body.data_packet.data[0] = buf[0];          //no substitution of characters
                packet->body.data_packet.data_size = ret;
            }
            packet->body.data_packet.block_number = i;
            
            //printf(BLUE"INFO : "RESET"Sent %d packet\n",i);
            sendto(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

            //printf(BLUE"INFO : "RESET"Waiting for %d packet ack from server\n",i);
            int r = recvfrom(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&server_addr, &server_len);
            if (r == -1)
            {
                printf(RED"\nERROR : "RESET"recv failed\n");
                return;
            }
            i++;
        } while ((packet->body.ack_packet.block_number + 1 > packet->body.data_packet.block_number));
    }
    return;
}
