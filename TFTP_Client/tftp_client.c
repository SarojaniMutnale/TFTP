/*
Name:Sarojani Mutnale
reg no:25031_107
project name:TFTP
*/

//including header files
#include "tftp.h"
#include "tftp_client.h"

int g_mode = 0;

int main()       
{

    tftp_client_t client;             
    memset(&client, 0, sizeof(client)); 

    // Main loop for command-line interface
    printf("\n-------------------------------------------------\n");
    printf(BLUE"                TFTP - CLIENT\n"RESET);
    printf("-------------------------------------------------\n");
    while (1)
    {
        // print the main menu
        printf(BLUE"\nMAIN MENUE\n"RESET);
        printf("1.Connect\n");
        printf("2.Put\n");
        printf("3.Get\n");
        printf("4.Mode\n");
        printf("5.Exit\n");

        // read the choice from the user
        int choice;
        printf("\nEnter your choice: ");
        scanf("%d", &choice);                  //collecting users choice

        // based on the choice perform operation
        switch (choice)
        {
        case 1:
            /* code to connect to server */
            {
                connect_to_server(&client);
            }
            break;

        case 2:
            /* code to put file */
            {
                put_file(&client);
            }
            break;

        case 3:
            /* code to get file from server*/
            {
                get_file(&client);
            }
            break;

        case 4:
            /* code to select which mode to get/put file */
            {
                select_mode();
            }
            break;

        case 5:
            /* code to exit program */
            {
                printf("\n-------------------------------------------------\n");
                printf(RED"            TERMINATING PROGRAM\n"RESET);
                printf("-------------------------------------------------\n");
                exit(0);
            }
            break;

        default:
        {
            printf("ERROR : Entered option out of range\n");             
        }
        break;
        }
    }

    return 0;
}         //end of main

// This function is to initialize socket with given server IP, no packets sent to server in this function
void connect_to_server(tftp_client_t *client)
{
    // Create UDP socket
    client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client->sockfd == -1)
    {
        printf("Error : Client socket creation failed\n");
        return;
    }

    // read the server address and port no
    printf("\nEnter server IP address : ");
    getchar();
    scanf("%[^\n]", client->server_ip);                 

    printf("Enter port number : ");
    scanf("%d", &client->server_port);                  

    // validate the information
    int ret_ip = validate_ip(client->server_ip);          
    if (ret_ip == 0)
    {
        printf(RED"ERROR :"RESET" Invalid IP Address\n");
        return;
    }

    if (!(client->server_port >= 1024 && client->server_port <= 65535))           
    {
        printf(RED"ERROR :"RESET" Invalid Port number\n");
        return;
    }

    // bind
    memset(&client->server_addr, 0, sizeof(client->server_addr));            
    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(client->server_port);
    client->server_addr.sin_addr.s_addr = inet_addr(client->server_ip);

    printf(BLUE"INFO :"RESET" Connection Successful with Server\n");
}

void put_file(tftp_client_t *client)                     //implementing put file
{
    printf(BLUE"\nOPERATION : PUT\n"RESET);
    // read the file name from the user
    char file_name[20];
    printf("Enter file name : ");
    scanf("%s", file_name);              
    printf("\n");

    // validate the file is exist or not
    int ret_open = open(file_name, O_RDONLY);               //open file in read mode
    if (ret_open == -1)
    {
        printf(RED"ERROR :"RESET" %s File not present\n", file_name);
        return;
    }

    // Send WRQ request and send file
    tftp_packet packet;
    memset(&packet, 0, sizeof(packet));                 //initializing all members of packet with 0
    strcpy(packet.body.request.filename, file_name);
    packet.opcode = WRQ;
    packet.body.request.mode = g_mode;

    printf(BLUE"INFO : "RESET"Sending file name and opcode to server\n");
    sendto(client->sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr));

    // waiting for acknowledgement from server
    int ack;
    printf(BLUE"INFO : "RESET"Waiting for ack from server\n");

    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);
    int r = recvfrom(client->sockfd, &ack, 4, 0, (struct sockaddr *)&server_addr, &server_len);
    if (r == -1)          
    {
        printf(RED"\nERROR : "RESET"recv function failed\n");
        return;
    }
    if (ack == 0)
    {
        printf(RED"\nERROR : "RESET"Server denied permission\n");
        return;
    }
    else         //if ack == 1
    {
        if (g_mode == 1 || g_mode == 0)             //in mode default
        {
            printf(BLUE"INFO : "RESET"Received ack from server\n"BLUE"INFO : "RESET"Ready to send data from client\n");
            int i = 1; 
            int ret = 512;
            do
            {
                if (ret < 512)
                {
                    printf(BLUE"\n***DATA TRANSFER COMPLETE***\n"RESET);
                    return;
                }
                memset(&packet, 0, sizeof(packet));             //initializing all members of packet with 0
                ret = read(ret_open, packet.body.data_packet.data, 512); // reading 511 bytes of data from file
                packet.body.data_packet.data_size = ret;
                packet.body.data_packet.block_number = i;

                printf(BLUE"INFO : "RESET"Sent %d packet\n",i);
                sendto(client->sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr));

                printf(BLUE"INFO : "RESET"Waiting for %d packet ack from server\n",i);
                r = recvfrom(client->sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&server_addr, &server_len);
                if (r == -1)
                {
                    printf(RED"\nERROR : "RESET"recv failed\n");
                    return;
                }
                i++;
            } while ((packet.body.ack_packet.block_number + 1 > packet.body.data_packet.block_number));
        }
        else if(g_mode == 2)
        {
            send_file(client, &packet, ret_open, g_mode);            //call to octal mode execution
        }
        else{
            send_file(client, &packet, ret_open, g_mode);             //call to netascii mode execution
        }  
    }
}

void get_file(tftp_client_t *client)                 //implementing get file 
{
    printf(BLUE"\nOPERATION : GET\n"RESET);
    // Send RRQ and recive file
    char filename[20];
    printf("Enter file name: ");
    scanf("%s",filename);                  //getting file name from user
    printf("\n");

    tftp_packet packet;
    memset(&packet, 0, sizeof(packet));              
    strcpy(packet.body.request.filename, filename);
    packet.opcode = RRQ;
    packet.body.request.mode = g_mode;

    printf(BLUE"INFO : "RESET"Sending file name and opcode to server\n");
    sendto(client->sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr));

    int ack;
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);

    printf(BLUE"INFO : "RESET"Waiting for ack from server\n");             // waiting for acknowledgement from server
    int r = recvfrom(client->sockfd, &ack, 4, 0, (struct sockaddr *)&server_addr, &server_len);
    if (r == -1)             //if recv failed
    {
        printf(RED"\nERROR : "RESET"recv failed\n");
        return;
    }
    if (ack == 0)
    {
        printf(RED"INFO : "RESET"File not present in server side\n");
        printf(RED"\nERROR : "RESET"Server denied\n");
        return;
    }
    else           //ack == 1
    {
        int ret_open = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);           //opening file in write mode, truncating it...if present (if not present then create new file)
        if (ret_open == -1)         //open fn failed
        {
            printf(RED"INFO : "RESET"Permission denied by file to write to it\n");
            printf(RED"ERROR : "RESET"GET operation failed\n");
            return;
        }
        
        ack=1;
        sendto(client->sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr));
        printf(BLUE"INFO : "RESET"Sending ready ack\n");
        
        if(g_mode == 0 || g_mode == 1)
        {
            recv_data(client, &packet, ret_open, g_mode);           
        }
        if(g_mode == 2)
        {
            recv_data(client, &packet, ret_open, g_mode);           
        }
        if(g_mode == 3)
        {
            recv_data(client, &packet, ret_open, g_mode);           
        }
    }

}

void select_mode()                 //implementing mode selection
{
    printf(BLUE"\nMODE SELECTION\n"RESET);
    printf("MAIN MENUE\n");
    printf("1.Default\n2.Octal\n3.Netascii\n");
    int choice;
    printf("\nEnter your choice : ");
    scanf("%d", &choice);
    if (choice == 1)             //default
    {
        g_mode = 1;
        printf(BLUE"\nINFO : Mode set to DEFAULT\n"RESET);
    }
    else if (choice == 2)            //octal
    {
        g_mode = 2;
        printf(BLUE"\nINFO : Mode set to OCTAL\n"RESET);
    }
    else if (choice == 3)                //netascii
    {
        g_mode = 3;
        printf(BLUE"\nINFO : Mode set to NETASCII\n"RESET);
    }
    else
    {
        printf(RED"\nERROR : "RESET"Entered choice not in range\n");
    }
    return;
}

int validate_ip(char *str)                //validating the ip address
{
    if(str[0] == '.')    //should not start with '.'
        return 0;
    int i = 0;
    int count = 0;
    while (str[i])
    {
        if ((str[i] == '.') && (str[i-1] == '.'))
            return 0;
        if (str[i] == '.')
            count++;
        if ((str[i] >= 48 && str[i] <= 57) || str[i] == '.')
        {
            i++;
            continue;
        }
        else
            return 0;
    }
    if (count == 3)             
        return 1;
    else
        return 0;
}