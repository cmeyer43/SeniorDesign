#include "udpRecieve.h"
#include <cerrno>

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
    server_addr.sin_addr.s_addr = inet_addr("10.42.0.1");
    
    // Bind to the set port and IP:
    int b = (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)));
    if(b < 0)
    {
        printf("Couldn't bind to the port %d, %s\n", b, strerror(errno));
        return;
    }
    printf("UDP port created successfully\n");
    
}

udpRecieve::~udpRecieve(void)
{
    // Close the socket:
    close(socket_desc);
}

int udpRecieve::recieve(uint8_t* msg, int len)
{
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(socket_desc, &rfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;

    int sizeRead = 0;
    int stat = select(socket_desc + 1, &rfds, NULL, NULL, &timeout);
    int ret = 0;

    if (stat > 0)
    {
        ret = recvfrom(socket_desc, msg, len, 0,
                (struct sockaddr*)&client_addr, (socklen_t*)&client_struct_length);
        // Receive client's message:
        if (!ret)
        {
            //printf("Couldn't receive\n");
            return ret;
        }
        //printf("Received message from IP: %s and port: %i\n",
        //inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        //printf("%d,%d,%d,%d,%d,%d\n", ((int16_t*)msg)[0], ((int16_t*)msg)[1], ((int16_t*)msg)[2], ((int16_t*)msg)[3], ((int16_t*)msg)[4], ((int16_t*)msg)[5]);
        return ret;
    }
    
    
     return 1;
}

int udpRecieve::respond(uint8_t* msg, int len)
{
    //// Respond to client:
    
    if (sendto(socket_desc, msg, len, 0,
            (struct sockaddr*)&client_addr, client_struct_length) < 0)
    {
         printf("Can't send\n");
         return -1;
    }
    return 1;
}
