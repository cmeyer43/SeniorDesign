#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class udpRecieve
{
public:
    udpRecieve(void);
    ~udpRecieve(void);
    int recieve(char* msg, int len);
    int respond(char* msg);
    
private:
    int socket_desc;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char server_message[2000], client_message[2000];
    int client_struct_length;
};
