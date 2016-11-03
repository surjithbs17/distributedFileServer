

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


char* data_finder(char* format,int server_num)
{
    int server,content_type;
    char parsed_string[10000];
    char line_parsed[200];
    char* content_parsed = malloc(200);
    char* content_parsed_1 = malloc(200);
    char* content_parsed_2 = malloc(200);
    char* content_processed = malloc(200);
    char* port_string = malloc(200);
    char* root_string = malloc(200);
    char* token;
    token = "1";


    //printf("%s\n",format );
    char data[100];
    bzero(data,sizeof(data));


    //printf("%s\n",data );

    FILE *f;
    bzero(content_parsed,sizeof(content_parsed));
    //bzero(content_parsed[1],sizeof(content_parsed[1]));   
    if((f = fopen("dfc.conf", "r")) == NULL)
    {
        printf(RED"dfc.conf file not found\n"RESET);
        exit(1);
    }

    server = strcmp(format,"server");
    if(server == 0)
    {
        sprintf(data,"Server DFS%d",server_num);
        //strcpy(data,);

    }

    int root = strcmp(format,"user");
    if(root == 0)
    {
        strcpy(data,"Username:");
    }

    int alive = strcmp(format,"pass");
    if(alive == 0)
    {
        strcpy(data,"Password:");
    }



    while(token != NULL)
    {
        if((token = fgets(parsed_string,10000,f)) == NULL)
            {
                return 0;
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
            
            if(server == 0)
            {
                sscanf(parsed_string,"%s %s %s",content_parsed,content_parsed_1,content_parsed_2);
                memset(content_processed,'\0',sizeof(content_processed));
                sprintf(content_processed,"%s %s",content_parsed,content_parsed_1);
                memset(content_parsed_1,'\0',sizeof(content_parsed_1));
                memset(content_parsed,'\0',sizeof(content_parsed));

                int comp = strcmp(content_processed,data);
                printf("Strcmp %s %s\n",content_processed,data );
                if(comp == 0)
                {
                    port_string = strtok(content_parsed_2,":");
                    port_string = strtok(NULL,":");
                    printf("POrt string %s\n",port_string );
                    return port_string;
                    break;
                }
                else
                {
                    continue;
                }
                
            }
            else
            {
                sscanf(parsed_string,"%s %s",content_parsed,content_parsed_1);
                int comp = strcmp(content_parsed,data);
                if(comp == 0)
                {
                    printf("%s\n",content_parsed_1 );
                    return content_parsed_1;
                    break;
                }
            }

        }
    }


}


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


void file_splitter(char* filename)
{
    char* split_cmd = malloc(300);
    sprintf(split_cmd,"split --number=l/%d ${fspec} %s %s",4,filename,filename);
    printf("%s\n",split_cmd );
    if(system(split_cmd) == -1)
    {
        printf(RED"System Call Failed! Split\n"RESET);
        exit(1);
    }
    else
    {
        printf(GRN"File split success\n"RESET);
    }

}


void main(int argc , char *argv[])
{

    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    char* port;
    char* server_port = data_finder("pass",0); //getting the root directory and storing it in variable ROOT

    printf("In Main, Server1 %s\n",server_port);


    char* filename = "files/image.jpg";
    file_splitter(filename);


    //char* alive_time = data_finder("KeepaliveTime");
    //int time_to_live = atoi(alive_time);

    //printf("\n Time to live - %d",time_to_live);

    //port = data_finder("port");
    /*
    int num_port = atoi(port);

    printf("%s -  %d\n",port,num_port );
    if(num_port < 1024)
    {
        printf(RED"Port Number is less than 1024, change it in ws.conf file and restart the webserver- %d\n"RESET,num_port );
        exit(1);
    }
    socket_desc = socket_creation(port);
    int optval = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(int));

    struct socket_info *sock_info = malloc(sizeof(struct socket_info));

    
     
    int len = sizeof(client);
    */
    exit(1);
}