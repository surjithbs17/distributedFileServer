


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

int authentication(char* username,char* password)
{

    char* token;
    token = '1';
    char* user_parsed = malloc(100);
    char* pw_parsed = malloc(100);
    char* parsed_string = malloc(300);
    FILE *f;
    int ret = -1;
    printf("Inside Auth\n");
    if((f = fopen("dfs.conf", "r")) == NULL)
    {
        printf(RED"dfs.conf file not found\n"RESET);
        exit(1);
    }


    while(token != NULL)
    {
        bzero(parsed_string,sizeof(parsed_string));
        if((token = fgets(parsed_string,10000,f)) == NULL)
            {
                //return ret;
                break;
            }

        //printf(" -- %s",parsed_string);
        if(parsed_string[0] == '#')
        {   
            //printf("It is a comment \n");
            continue;
        }
        else
        {
            bzero(user_parsed,sizeof(user_parsed));
            sscanf(parsed_string,"%s %s",user_parsed,pw_parsed);
            
            int user = strcmp(user_parsed,username);
            //printf(" -- %s %s %d\n",user_parsed,username,user);
            if(user == 0)
            {
                //printf("Inside PW\n");
                int pw = strcmp(pw_parsed,password);
                if(pw == 0)
                {
                    printf(GRN"Username Password Match\n"RESET);
                    ret = 0;
                    break;
                }
                else
                {
                    printf(RED"Password did not match\n"RESET);
                }
            }
            else
            {
                continue;
            }
        }

    }

    
    return ret;

}

void main(int argc , char *argv[])
{
    int socket_num;
     struct sockaddr_in server , client;
    if (argc < 2)
    {
        printf("Less Arguments \nUsage: ./dfs1 /DFS1 portnum\n");
        exit(1);
    } 
    printf("%s %s\n",argv[1],argv[2] );
    int port_num = atoi(argv[2]);
    char* server_name = malloc(100);
    memset(server_name,'\0',sizeof(server_name));
    strcpy(server_name,argv[1]);

    //printf("Return Value %d\n",authentication("Alice","Simplepassword") );


    int server_sock = socket_creation(argv[2]);
    int optval = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(int));
    int len = sizeof(client);


    while((client_sock = accept(socket_sock, (struct sockaddr *)&client, (socklen_t*)&len)) > 0)
    {








    }

    if(authentication("Alce","SimplePassword") == 0)
    {
        printf(GRN"Authentication Successful\n"RESET);
    }
    else
    {
        printf(RED"Authentication not successful\n"RESET);
    }
    

    printf("Port Number - %d, Server Name - %s",port_num,server_name);
    //socket_num = socket_creation(argv[1]);



}