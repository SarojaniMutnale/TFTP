#include "tftp.h"

int g_mode = 0;

int main()          
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;      
    socklen_t client_len = sizeof(client_addr);
    tftp_packet packet;

    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);            
    if (sockfd == -1)         
    {
        printf(RED"ERROR : "RESET"Server socket creation failed\n");
        exit(1);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    
    int ret_bind = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret_bind == -1)        //if bind function failed
    {
        printf(RED"ERROR : "RESET"Bind function failed in server\n");
        exit(1);
    }


    printf("\n-------------------------------------------------\n");
    printf(BLUE"                TFTP - SERVER\n"RESET);
    printf("-------------------------------------------------\n");

    // handle incoming requests
    while (1)
    {
        printf(BLUE"\nINFO : TFTP Server listening on port %d...\n"RESET, PORT);
        int n = recvfrom(sockfd, &packet, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);        //waiting for client connection
        if (n == -1)          //if recvfrom function failed
        {
            printf(RED"ERROR : "RESET"recvfrom function failed\n");
            exit(1);
        }

        printf(BLUE"\nINFO : "RESET"Client connected\n");
        printf(BLUE"INFO : "RESET"Received client message\n");

        handle_client(sockfd, client_addr, client_len, &packet);         //function call to handel client and its requirements
    }
    close(sockfd);
    return 0;
}

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet)       
{
    
    printf(BLUE"INFO : "RESET"Filename - %s\n", packet->body.request.filename);
    g_mode = packet->body.request.mode;         

    if (packet->opcode == WRQ)           
    {
        printf(BLUE"INFO : "RESET"Operation - WRQ\n");
        int ret_open = open(packet->body.request.filename, O_WRONLY | O_TRUNC);             
        if (ret_open == -1)            //if open command failed
        {
            ret_open = open(packet->body.request.filename, O_CREAT | O_WRONLY, 0666);       
            printf(BLUE"INFO : "RESET"Creating file (fd - %d)\n", ret_open);
        }
        else
        {
            printf(BLUE"INFO : "RESET"File exists (fd - %d)\n", ret_open);                   
        }

        int ack = 1;
        sendto(sockfd, &ack, 4, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));     

        if (g_mode == 0 || g_mode == 1)        
        {
            printf(BLUE"INFO : "RESET"Mode - DEFAULT\n");
            int count = 1;
            do
            {
                memset(packet, 0, sizeof(*packet));          
                client_len = sizeof(client_addr);

                int r = recvfrom(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, &client_len);
                int w = write(ret_open, packet->body.data_packet.data, packet->body.data_packet.data_size);
                if (w == -1)
                {
                    printf(RED"\nERROR : "RESET"write failed (ret - %d) (errno = %d)\n", w, errno);
                    return;
                }
                packet->body.ack_packet.block_number = packet->body.data_packet.block_number + 1;
                packet->body.ack_packet.data_size = packet->body.data_packet.data_size;
                
            
                sendto(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));         //sending back ack
            } while (packet->body.data_packet.data_size == 512);
        }
        else if (g_mode == 2)
        {
            recv_data(sockfd, client_addr, client_len, packet, ret_open, g_mode);
        }
        else if (g_mode == 3)
        {
            recv_data(sockfd, client_addr, client_len, packet, ret_open, g_mode);
        }
        printf(BLUE"\n***DATA TRANSFER DONE***\n"RESET);
        return;
    }
    if (packet->opcode == RRQ)
    {
        printf(BLUE"INFO : "RESET"Operation - RRQ\n");
        int ret_open = open(packet->body.request.filename, O_RDONLY);
        if (ret_open == -1)
        {
            printf(RED"INFO : "RESET"%s File not present in server\n",packet->body.request.filename);
            int ack=0;
            sendto(sockfd, &ack, 4, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
            printf(RED"INFO : "RESET"Sent ack to client(failure- filename and operation)\n");
            return;
        }
        

        int ack = 1;
        sendto(sockfd, &ack, 4, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        
        recvfrom(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&client_addr, &client_len);
        if(ack == 0)
        {
            printf(RED"ERROR : "RESET"Client failed\n");
            return;
        }
        else
        {
            if(g_mode == 0 || g_mode == 1)
            {
                send_data(sockfd, client_addr, client_len, packet, ret_open, g_mode);
            }
            else if(g_mode == 2)
            {
                send_data(sockfd, client_addr, client_len, packet, ret_open, g_mode);
            }
            else if(g_mode == 3)
            {
                send_data(sockfd, client_addr, client_len, packet, ret_open, g_mode);
            }
        }
    }
}