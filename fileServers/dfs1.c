


#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>    
#include <pthread.h> 
#include <fcntl.h>
#include <sys/syscall.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>



#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"



int socket_creation(char* port)
{
    struct addrinfo server;
    struct addrinfo *res, *p;
    int socket_desc;


    memset (&server, 0, sizeof(server));
    // getaddrinfo for host
    server.ai_family = AF_INET;
    server.ai_socktype = SOCK_STREAM;
    server.ai_flags = AI_PASSIVE;

    printf(GRN"\nListening in Port %s\n"RESET,port );
    if (getaddrinfo( NULL, port, &server, &res) != 0)
    {
        printf(RED"get address error"RESET);
        exit(1);
    }

    for (p = res; p!=NULL; p=p->ai_next)
    {
        socket_desc = socket(p->ai_family, p->ai_socktype, 0);
        if (socket_desc == -1) continue;
        if (bind(socket_desc, p->ai_addr, p->ai_addrlen) == 0) break;
        printf(MAG"Wait for sometime or Change the port \n"RESET);
    }

    if (p==NULL)
    {
        printf (RED"Socket Creation/Bind Issue\n"RESET);
        exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections
    if ( listen (socket_desc, 100) != 0 )
    {
        printf(RED"ERROR During Listening"RESET);
        exit(1);
    }

    return socket_desc;

}

void main(int argc , char *argv[])
{
    int socket_num;
    int port_num = atoi(argv[1]);
    char* server_name = argv[0];
    

    printf("Port Number - %d, Server Name - %s",port_num,server_name);
    //socket_num = socket_creation(argv[1]);



}