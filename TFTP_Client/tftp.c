#include "tftp.h"
#include "tftp_client.h"

void send_file(tftp_client_t *client, tftp_packet *packet, int ret_open, int mode)
{
    if (mode == 2)                 
    {
        struct sockaddr_in server_addr;
        socklen_t server_len = sizeof(server_addr);

        printf(BLUE"INFO : "RESET"Received ack from server\n"BLUE"INFO : "RESET"Ready to send data from client\n");
        int i = 1; 
        int ret = 8;
        do
        {
            if (!strcmp(packet->body.data_packet.data, "\0"))
            {
                printf(BLUE"\n***DATA TRANSFER COMPLETE***\n"RESET);
                return;
            }

            memset(packet, 0, sizeof(*packet));             
            ret = read(ret_open, packet->body.data_packet.data, 1); 
            packet->body.data_packet.data_size = ret;
            packet->body.data_packet.block_number = i;

            printf(BLUE"INFO : "RESET"Sent %d packet\n",i);
            sendto(client->sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr));

            printf(BLUE"INFO : "RESET"Waiting for %d packet ack from server\n",i);
            int r = recvfrom(client->sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&server_addr, &server_len);
            if (r == -1)
            {
                printf(RED"\nERROR : "RESET"recv failed\n");
                return;
            }
            i++;

        } while ((packet->body.ack_packet.block_number + 1 > packet->body.data_packet.block_number));
    }
    else // code for netascii
    {
        struct sockaddr_in server_addr;
        socklen_t server_len = sizeof(server_addr);

        printf(BLUE"INFO : "RESET"Received ack from server\n"BLUE"INFO : "RESET"Ready to send data from client\n");
        int i = 1;                  // to keep a track of packet number
        int ret = 8;
        do
        {
            if ((ret == 0) || (!strcmp(packet->body.data_packet.data, "\0")))
            {
                printf(BLUE"\n***DATA TRANSFER COMPLETE***\n"RESET);
                return;
            }
            char buf[2];

            memset(packet, 0, sizeof(*packet));           
            ret = read(ret_open, buf, 1);
            if (buf[0] == '\n')
            {
                buf[0] = '\r';
                buf[1] = '\n';
                strcpy(packet->body.data_packet.data, buf);
                packet->body.data_packet.data_size = 2;
            }
            else
            {
                packet->body.data_packet.data[0] = buf[0];
                packet->body.data_packet.data_size = ret;
            }
            packet->body.data_packet.block_number = i;

            printf(BLUE"INFO : "RESET"Sent %d packet\n",i);
            sendto(client->sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr));

            printf(BLUE"INFO : "RESET"Waiting for %d packet ack from server\n",i);
            int r = recvfrom(client->sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&server_addr, &server_len);
            if (r == -1)
            {
                printf(RED"\nERROR : "RESET"recv failed\n");
                return;
            }
            i++;
        } while ((packet->body.ack_packet.block_number + 1 > packet->body.data_packet.block_number));
    }
}

void recv_data(tftp_client_t *client, tftp_packet *packet, int ret_open, int mode)
{
    struct sockaddr_in client_addr;
    socklen_t client_len =  sizeof(client_addr);

    if (mode == 0 || mode == 1)            
    {
        int count = 1;
        do
        {
            memset(packet, 0, sizeof(*packet));          
            client_len = sizeof(client_addr);

            int r = recvfrom(client->sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, &client_len);
            printf(BLUE"INFO : "RESET"Packet %d received successfully\n",count);
            //fwrite(packet->body.data_packet.data, 1, packet->body.data_packet.data_size, stdout);
            int w = write(ret_open, packet->body.data_packet.data, packet->body.data_packet.data_size);
            if (w == -1)
            {
                printf(RED"\nERROR : "RESET"write failed (ret - %d) (errno = %d)\n", w, errno);
                return;
            }
            packet->body.ack_packet.block_number = packet->body.data_packet.block_number + 1;
            packet->body.ack_packet.data_size = packet->body.data_packet.data_size;

            printf(BLUE"INFO : "RESET"Sending packet %d ack\n", count++);
            sendto(client->sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        } while (packet->body.data_packet.data_size == 512);
    }
    else if (mode == 2)            
    {
        int count = 1;
        do
        {
            memset(packet, 0, sizeof(*packet));      
            client_len = sizeof(client_addr);

            int r = recvfrom(client->sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, &client_len);
            printf(BLUE"INFO : "RESET"Packet %d received successfully\n",count);
            
            int w = write(ret_open, packet->body.data_packet.data, packet->body.data_packet.data_size);
            if (w == -1)
            {
                printf(RED"\nERROR : "RESET"write failed (ret - %d) (errno = %d)\n", w, errno);
                return;
            }
            packet->body.ack_packet.block_number = packet->body.data_packet.block_number + 1;
            packet->body.ack_packet.data_size = packet->body.data_packet.data_size;

            printf(BLUE"INFO : "RESET"Sending packet %d ack\n", count++);
            sendto(client->sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        } while (packet->body.data_packet.data_size == 1);
    }
    else if (mode == 3)              
    {
        int count = 1;
        do
        {
            memset(packet, 0, sizeof(*packet));           
            client_len = sizeof(client_addr);

            int r = recvfrom(client->sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, &client_len);
            printf(BLUE"INFO : "RESET"Packet %d received successfully\n",count);
            
            int w = write(ret_open, packet->body.data_packet.data, packet->body.data_packet.data_size);
            if (w == -1)
            {
                printf(RED"\nERROR : "RESET"write failed (ret - %d) (errno = %d)\n", w, errno);
                return;
            }
            packet->body.ack_packet.block_number = packet->body.data_packet.block_number + 1;
            packet->body.ack_packet.data_size = packet->body.data_packet.data_size;

            printf(BLUE"INFO : "RESET"Sending packet %d ack\n", count++);
            sendto(client->sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        } while (packet->body.data_packet.data_size >= 1);
    }
    printf(BLUE"\n***DATA TRANSFER DONE***\n"RESET);
    return;
}