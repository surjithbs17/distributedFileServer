


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


#define MAX_LINE 2000
#define MESSAGE_LENGTH 10000
#define BYTES 1024
#define UPLOAD 2
#define DOWNLOAD 1


#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

char* server_file_path;
int server_number;

int socket_creation(char* inp)
{
    struct addrinfo server;
    struct addrinfo *res, *p;
    int socket_desc;
    char *port = malloc(100);
    memset(port,'\0',sizeof(port));
    strcpy(port,inp);

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
    printf("\n After Listening");
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

int doesFileExist(const char *filename) 
{
    struct stat st;
    int result = stat(filename, &st);
    if(result == 0)
    {
        printf(GRN"\nFile exists!\n"RESET);
        return 0;
    }
    else
        return -1;
}


void serve_client(int client_sock, char* file_name,int action)
{
    char* local_file_path = malloc(MESSAGE_LENGTH);
    int file_status = 0;


    strcpy(local_file_path,server_file_path);
    //strcat(local_file_path,file_name);
    sprintf(local_file_path,"%s/%s",server_file_path,file_name);
    printf("File that is being handled %s\n",local_file_path );

    if (action == UPLOAD)
    {
        if((file_status = doesFileExist(local_file_path)) == 0)
        {
            send(client_sock, "DFS ALREADY_EXIST\n", 18, 0);
        }
        else
        {
            send(client_sock, "DFS GOOD_UPLOAD\n", 15, 0);   
        } 
    }
    else if (action == DOWNLOAD)
    {

        if((file_status = doesFileExist(local_file_path)) == 0)
        {
            send(client_sock, "DFS OK_EXIST\n", 13, 0);

        }

    }
}

void client_handler(int client_sock,char* server_file_path)
{

    char recv_buf[MESSAGE_LENGTH],recv_buf_backup[MESSAGE_LENGTH],auth_string[MESSAGE_LENGTH],uname[MESSAGE_LENGTH],pword[MESSAGE_LENGTH],file_name[MESSAGE_LENGTH],file_num[MESSAGE_LENGTH],action[MESSAGE_LENGTH];
    bzero(recv_buf,sizeof(recv_buf));
    bzero(recv_buf_backup,sizeof(recv_buf_backup));
    int s = recv(client_sock, recv_buf, MESSAGE_LENGTH, 0);

    printf("Inside handler\n%s\n",recv_buf);
    if( s < 0 )
    {    
        printf("Recieve error\n");
    }
    else if (s == 0)
    { 
        printf(YEL"Client disconnected (recv returns zero).\n"RESET);
    }
    else
    {
        strcpy(recv_buf_backup, &recv_buf);
        if(strncmp("Auth_string",recv_buf_backup,11) == 0)
        {
            sscanf(recv_buf_backup,"%s %s %s %s %s",auth_string,uname,pword,file_name,action);

            if(authentication(uname,pword) == 0)
            {
                printf(GRN"Authentication Successful\n"RESET);
                serve_client(client_sock,file_name,atoi(action));
            }
            else
            {
                printf(RED"Authentication not successful\n"RESET);
            }

        }
        else
        {
            printf("Auth String parsing failed\n");
            // Send reply to client saying invalid auth string
        }

    }

}

void list()
{
    
    FILE *ls_fp;
    system("ls > 'ls_file.txt'");
    ls_fp = fopen("ls_file.txt","r");
    char parsed_buf[100];
    //gets();
    while(fscanf(ls_fp,"%s",parsed_buf) != EOF) 
    {
        printf("%s\n",parsed_buf);
        //printf("Inside while\n");
        bzero(parsed_buf,sizeof(parsed_buf));
    }
    
}

void find_file_num(char* file_name)
{

}

void main(int argc , char *argv[])
{
    int socket_num;
    struct sockaddr_in server , client;
    
     printf("Printing %s %s\n",argv[1],argv[2] );
    if (argc < 2)
    {
        printf("Less Arguments \nUsage: ./dfs1 DFS1 portnum\n");
        exit(1);
    } 
    printf("%s %s\n",argv[1],argv[2] );


    int port_num = atoi(argv[2]);
    
    char cwd[1024];
    char concatenated_dir[1024];
    getcwd(cwd, sizeof(cwd));
    sprintf(concatenated_dir,"%s/%s",cwd,argv[1]);
    chdir(concatenated_dir);
    printf("Before strcpy\n");
    server_file_path=malloc(MESSAGE_LENGTH);
    strcpy(server_file_path,concatenated_dir);    
    printf("Current Working Directory - %s \n%s\n",cwd,concatenated_dir );
    //printf("Return Value %d\n",authentication("Alice","Simplepassword") );

    list();
    int server_sock = socket_creation(argv[2]);

    printf("Socket Created\n");
    int optval = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(int));
    int len = sizeof(client);
    //while(1);
    int client_sock;
    while((client_sock = accept(server_sock, (struct sockaddr *)&client, (socklen_t*)&len)) > 0)
    {

        printf(BLU"Master Accepted Sock --- %d\nThread Generated Sock --- %d\n"RESET,server_sock,client_sock);
        pthread_t client_serve_thread;
        
        if( pthread_create( &client_serve_thread , NULL ,  &client_handler , (void *)client_sock) < 0)
        {
            perror("could not create thread");
            exit(1);
        }
         
        printf(BLU "Inside Accept While Loop\n" RESET);


    }

    

    //printf("Port Number - %d, Server Name - %s",port_num,server_name);
    //socket_num = socket_creation(argv[1]);



}