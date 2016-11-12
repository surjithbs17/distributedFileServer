

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


#define MESSAGE_LENGTH 1024
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


int server_sock_1;
int server_sock_2;
int server_sock_3;
int server_sock_4;

char* file_path;


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





int file_splitter(char* filename)
{
    char* split_cmd = malloc(300);
    
    char concatenated_dir[1024];
    sprintf(concatenated_dir,"%s/files",file_path);
    chdir(concatenated_dir);

    sprintf(split_cmd,"split --number=l/%d ${fspec} %s %s",4,filename,filename);
    if(doesFileExist(filename) == -1)
    {
        printf("File does not exist\n");
        printf("%s\n%s\n",concatenated_dir,filename );
        return -1;
    }

    printf("%s\n",split_cmd );
    if(system(split_cmd) == -1)
    {
        printf(RED"System Call Failed! Split\n"RESET);
        chdir(file_path);
        exit(1);
    }
    else
    {
        printf(GRN"File split success\n"RESET);
        chdir(file_path);
        return 0;
    }


}

int create_connection(int port)
{
    int sock = socket(AF_INET , SOCK_STREAM , 0);
    struct sockaddr_in server;
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( port );

    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("Looks like Server 1 is down!\nDo you want to retry?(Y/N)\n");
        char input[100];
        bzero(input,sizeof(input));
        gets(input);
        if(strcmp("Y",input)==0)
        {
            create_connection(port);     
        }
        else
        {
            printf("As you say, I am continuing without this server at port %d\n",port );
        }
    }
    else
    {
        printf("Connection Created - %d\n",sock );
    }
    return sock;
}


void servers_connect()
{
    char* port_1 = data_finder("server",1);
    char* port_2 = data_finder("server",2);
    char* port_3 = data_finder("server",3);
    char* port_4 = data_finder("server",4);

    server_sock_1 = create_connection(atoi(port_1));
    server_sock_2 = create_connection(atoi(port_2));
    server_sock_3 = create_connection(atoi(port_3));
    server_sock_4 = create_connection(atoi(port_4));
}

int send_file_pair(server_socket,file_num1,file_num2)
{
    
}

int put_handler(char* filename)
{
    printf("In put handler %s\n",filename );
    if(file_splitter(filename) == -1)
    {
        return -1;
    }
    char concatenated_dir[1024];
    bzero(concatenated_dir,sizeof(concatenated_dir));
    sprintf(concatenated_dir,"%s/files",file_path);
    chdir(concatenated_dir);


    char md5command[256];
    char* value = malloc(100);
    char* last_4 = malloc(10);
    bzero(value,sizeof(value));
    bzero(last_4,sizeof(last_4));
    bzero(md5command,sizeof(md5command));
    sprintf(md5command,"md5sum %s > md5value.txt",filename);       
    system(md5command);
    FILE* md5file = fopen("md5value.txt","r+");
    fscanf(md5file,"%s",value);
    printf("\nMD5 Value - %s\n",value);
    system("rm md5value.txt");
    int length = strlen(value);
    printf("Passed length\n");
    last_4 = value + length - 1;
    char* ptr;
    long ret = strtol(last_4, &ptr, 16);
    printf("md5sum %s\nLast 4 characters %s\n converted decimal %ld\n",value,last_4,ret);

    switch(ret%4)
    {
        case 0:
            {
                send_file_pair(server_sock_1,1,2);
                send_file_pair(server_sock_2,2,3);
                send_file_pair(server_sock_3,3,4);
                send_file_pair(server_sock_4,4,1);
            }
        case 1:
            {
                send_file_pair(server_sock_1,4,1);
                send_file_pair(server_sock_2,1,2);
                send_file_pair(server_sock_3,2,3);
                send_file_pair(server_sock_4,3,4);
            }
        case 2:
            {
                send_file_pair(server_sock_1,3,4);
                send_file_pair(server_sock_2,4,1);
                send_file_pair(server_sock_3,1,2);
                send_file_pair(server_sock_4,2,3);
            }
        case 4:
            {
                send_file_pair(server_sock_1,2,3);
                send_file_pair(server_sock_2,3,4);
                send_file_pair(server_sock_3,4,1);
                send_file_pair(server_sock_4,1,2);
            }


    }






    chdir(file_path);
    return 0;


}


void main(int argc , char *argv[])
{

    int socket_desc , client_sock , c , *new_sock;
    char send_buf[MESSAGE_LENGTH],recv_buf[MESSAGE_LENGTH];
    struct sockaddr_in server , client;
    char* cmd = malloc(200);
    char* filename = malloc(200); 
    char* port;
    char* server_port = data_finder("pass",0); //getting the root directory and storing it in variable ROOT

    file_path = malloc(200);
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    strcpy(file_path,cwd);

    printf("In Main, Server1 %s\n",server_port);

    while(1)
    {
        printf("\n--- MENU ---\nType the command \n\tPossible Options\n1.LIST\n2.GET\n3.PUT\n");
        char *inp = malloc(100);
        
        bzero(inp,sizeof(inp));
        gets(inp);

        bzero(cmd,sizeof(cmd));
        bzero(filename,sizeof(filename));

        char* uname = malloc(100);
        char* pword = malloc(100);
        bzero(uname,sizeof(uname));
        bzero(pword,sizeof(pword));
        uname = data_finder("user",0);
        pword = data_finder("pass",0);

        //int server_sock = create_connection(10003);
        servers_connect();
        int ls_cmd = strcmp("LIST",inp);
        if (ls_cmd!=0)
        {

            sscanf(inp,"%s %s",cmd,filename);
            int get_cmd = strncmp("GET",inp,3);
            int put_cmd = strncmp("PUT",inp,3);
            if(get_cmd == 0)
            {
                printf("Inside GET command\n");
                
                sprintf(send_buf,"Auth_string %s %s %s 1",uname,pword,filename);
                printf("Send Buf %s\n", send_buf);
                bzero(cmd,sizeof(cmd));
                
                if( send(server_sock_3 , send_buf , strlen(send_buf) , 0) < 0)
                {
                    puts("Send failed");
                    return 1;
                }
                
                if(recv(server_sock_3, recv_buf, MESSAGE_LENGTH, 0) < 0)
                {
                    printf("Recv Error\n");
                }
                else
                {
                    printf("Recieved %s\n",recv_buf );
                }

                bzero(send_buf,sizeof(send_buf));
                bzero(recv_buf,sizeof(recv_buf));
                bzero(filename,sizeof(filename));
            }

            else if(put_cmd == 0)
            {
                printf("Inside PUT command\n");
                
                sprintf(send_buf,"Auth_string %s %s %s 2",uname,pword,filename);
                printf("Send Buf %s\n", send_buf);
                bzero(cmd,sizeof(cmd));
                //bzero(filename,sizeof(filename));
                if( send(server_sock_3 , send_buf , strlen(send_buf) , 0) < 0)
                {
                    puts("Send failed");
                    return 1;
                }
                bzero(send_buf,sizeof(send_buf));
                bzero(recv_buf,sizeof(recv_buf));
                if(recv(server_sock_3, recv_buf, sizeof(recv_buf), 0) < 0)
                {
                    printf("Recv Error\n");
                }
                else
                {
                    printf("Recieved %s\n",recv_buf );
                    if(strncmp("DFS GOOD_UPLOAD",recv_buf,15)==0)
                    {
                        printf("Its a good req - msg from server %s\n%s\n",cmd,filename);
                        put_handler(filename);    
                    }
                    

                }
                bzero(filename,sizeof(filename));
                
                
            }
        }

        //char* send_buf = malloc(300);
        //bzero(send_buf,sizeof(send_buf));

        

    }

    //char* filename = "files/image.jpg";
    




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