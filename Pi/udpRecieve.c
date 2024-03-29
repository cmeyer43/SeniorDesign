#include "udpRecieve.h"

udpRecieve::udpRecieve(void)
{
    client_struct_length = sizeof(client_addr);
    
    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
                 
    // Create UDP socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                            
    if(socket_desc < 0)
    {
        printf("Error while creating socket\n");
        return;
    }
    printf("Socket created successfully\n");
                                                             
    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("192.168.0.145");
    
    // Bind to the set port and IP:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Couldn't bind to the port\n");
        return;
    }
    
}

udpRecieve::~udpRecieve(void)
{
    // Close the socket:
    close(socket_desc);
}

int udpRecieve::recieve(char* msg, int len)
{
    printf("Listening for incoming messages...\n\n");

    // Receive client's message:
    if (recvfrom(socket_desc, msg, len, 0,
            (struct sockaddr*)&client_addr, (socklen_t*)&client_struct_length) < 0)
    {
        printf("Couldn't receive\n");
        return -1;
    }
    printf("Received message from IP: %s and port: %i\n",
    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    
    printf("Msg from client: %s\n", client_message);
    
    
     return 0;
}

int udpRecieve::respond(char* msg)
{
    //// Respond to client:
    
    if (sendto(socket_desc, msg, strlen(msg), 0,
            (struct sockaddr*)&client_addr, client_struct_length) < 0)
    {
         printf("Can't send\n");
         return -1;
    }
    return 1;
}
