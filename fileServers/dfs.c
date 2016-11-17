


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
#define MESSAGE_LENGTH 1024
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
    char* filename = malloc(200);
    bzero(filename,sizeof(filename));

    sprintf(filename,"%s/dfs.conf",server_file_path);
    if((f = fopen(filename, "r")) == NULL)
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

char* getFilesize(const char* filename) 
{
    char* size_string = malloc(200);
    bzero(size_string,sizeof(size_string));
    struct stat *file_size;
    
    file_size = malloc(sizeof(struct stat));
    memset(file_size,0,sizeof(file_size));
    
    if(stat(filename, file_size) != 0) {
        return 0;
    }
    size_t size =  file_size->st_size;
    sprintf(size_string,"%d",(int)size);
    
    return size_string;
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

char* md5calculator(char* filename)
{
    char md5command[256];
    char* value = malloc(100);
    //char* last_4 = malloc(10);
    bzero(value,sizeof(value));
    //bzero(last_4,sizeof(last_4));
    bzero(md5command,sizeof(md5command));
    sprintf(md5command,"md5sum %s > md5value.txt",filename);       
    system(md5command);
    FILE* md5file = fopen("md5value.txt","r+");
    fscanf(md5file,"%s",value);
    printf("\nMD5 Value - %s\n",value);
    system("rm md5value.txt");
    return value;
}


int send_file_pair(int server_socket,char* filename,int file_num1,char* uname,int cmd)
{
    char send_buf[MESSAGE_LENGTH],recv_buf[MESSAGE_LENGTH],data_to_send[BYTES];
    bzero(send_buf,sizeof(send_buf));
    bzero(recv_buf,sizeof(recv_buf));

    //char* uname = malloc(100);
    //char* pword = malloc(100);
    char* filename1 = malloc(100);
    
    bzero(filename1,sizeof(filename1));
    
    /*
    bzero(uname,sizeof(uname));
    bzero(pword,sizeof(pword));
    uname = data_finder("user",0);
    pword = data_finder("pass",0);
    */
    printf("Inside send file pair\n");
    if(cmd == 1)
    {
        sprintf(filename1,"%s/%s/ls_file.txt",server_file_path,uname);
        printf("cmd 1 -list\n");
    }
    else if(cmd == 2)
    {
        sprintf(filename1,"%s/%s/%sa%c",server_file_path,uname,filename,96+file_num1);
    }
    
    
    printf("File Name - %s\n",filename1 );
    FILE *file;

    //printf(YEL"Inside send_file_pair\n"RESET);

    if ( (file=open(filename1, O_RDONLY))!=-1 )    //FILE FOUND
    {
        printf("Filename - %s",filename1);
                    
        char* content_len = malloc(100);
        bzero(content_len,sizeof(content_len));
        content_len = getFilesize(filename1);
        char* md5value = malloc(100);
        bzero(md5value,sizeof(md5value));

        md5value = md5calculator(filename1);
        bzero(send_buf,sizeof(send_buf));
        char* action = malloc(100);
        bzero(action,sizeof(action));

        if(cmd == 1)
        {
            char* user_dir = malloc(200);
            bzero(user_dir,sizeof(user_dir));
            sprintf(user_dir,"%s/%s/ls_file.txt",server_file_path,uname);
            printf("user_dir(ls file name) - %s\n",user_dir );
            //file_num1 = 0;
            char* content_len = malloc(100);
            bzero(content_len,sizeof(content_len));
            content_len = getFilesize(user_dir);

            
            //FILE* file = fopen(user_dir,"r+");
            bzero(data_to_send,sizeof(data_to_send));
            sprintf(data_to_send,"AUTH SUCCESS %s",content_len);
            printf("Data to send %s\n",data_to_send );
            int s;

            if((s = send(server_socket, data_to_send,strlen(data_to_send),0)) <= 0)
            {
                printf("Send Error\n");
            }
            else
            {
                printf("Sent ACK Auth success send file pair\n");
                bzero(data_to_send,sizeof(data_to_send));
            }
            int bytes_read;

            bzero(recv_buf,sizeof(recv_buf));
            if(recv(server_socket , recv_buf , 4,0) <= 0)
            {
            
                printf(RED"Send failed\n"RESET);
            
            }
            if(strcmp(recv_buf,"SEND") == 0)
            {
                printf("Send command from client\n");
            }


            while ( (bytes_read=read(file, data_to_send, BYTES)) > 0 )
            {    
                printf("%s \nbytes_read - %d\n",data_to_send );
                send (server_socket, data_to_send, bytes_read,0);
                bzero(data_to_send,sizeof(data_to_send));
            }
            printf("data_to_send %s bytes_read %d\n",data_to_send,bytes_read );
            printf(GRN"File Sent Successfully 1\n"RESET);

        }
        if(cmd == 2)
        {
            strcpy(action,"get");
            sprintf(send_buf,"Auth_string %s %s %s %d %s %s %s",uname,filename,file_num1,md5value,content_len,action);
            if(send(server_socket , send_buf , strlen(send_buf),0) <= 0)
            {
                printf(RED"Send failed\n"RESET);
            }
            
            bzero(recv_buf,sizeof(recv_buf));
            if(recv(server_socket , recv_buf , BYTES,0) <= 0)
            {
            
                printf(RED"Send failed\n"RESET);
            
            }

             int bytes_read = 0;
        
            if(strcmp(recv_buf,"AUTH SUCCESS") == 0)
            {
                bzero(data_to_send,sizeof(data_to_send));
                while ( (bytes_read=read(file, data_to_send, BYTES)) > 0 )
                {    
                    send (server_socket, data_to_send, bytes_read,0);
                    bzero(data_to_send,sizeof(data_to_send));
                }
                printf(GRN"File Sent Successfully 1\n"RESET);
            } 

        }


        printf("send_buf %s\n",send_buf );
//        sprintf(send_buf,"Auth_string %s %s %s %d %s %s put",uname,pword,filename,file_num1,md5value,content_len);
       



        

                
    }
} 


void recv_chunk(char* uname,char* pword,char* filename,char* file_num,char* md5sum,int client_sock,char* content_len)
{
    FILE *file_p;

    FILE *f;

   

    char* recv_buf = malloc(MESSAGE_LENGTH);
    char* file_name = malloc(200);
    bzero(file_name,sizeof(file_name));
    char* dir_path = malloc(200);
    bzero(dir_path,sizeof(dir_path));



    sprintf(dir_path,"%s/%s/",server_file_path,uname);
    if(mkdir(dir_path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
    {
        printf("Looks like dir already exists\n");
    }
    chdir(dir_path);

    

   int file_size = atoi(content_len);
   int remain_data = file_size;

    sprintf(file_name,"%s.%s.%s",dir_path,filename,file_num);
    printf("File Name =  %s\n", file_name);
    
    file_p = fopen(file_name, "w+");
    int bytesReceived,no_of_times = 0;
    printf("Inside recv_chunk\n");
    bzero(recv_buf,sizeof(recv_buf));
    int read_bytes = BYTES;
    while(((bytesReceived = recv(client_sock, recv_buf, read_bytes,0)) > 0) && (remain_data > 0))
    {
        //printf("Bytes received %d\n %s\n",bytesReceived,recv_buf);
        //printf("happening 1 \n");
        fwrite(recv_buf, sizeof(char),bytesReceived,file_p);
        //printf("happening 2\n");
        //printf("%s",recv_buf );
        
        remain_data -= bytesReceived;
        
        if(remain_data< BYTES)
        {
            read_bytes = remain_data;
            if(remain_data == 0)
            {
                break;
                printf("Breaking it\n");
            }

        }
        
        bzero(recv_buf,sizeof(recv_buf));

        
        printf("\nReceive %d bytes and we hope :- %d bytes\n", bytesReceived, remain_data);
        //printf("Passed print\n");
    }


    printf("Out of loop\n");
    fclose(file_p);
    
    printf("Is it happening?\n");
    char* md5value = malloc(100);
    bzero(md5value,sizeof(md5value));

    md5value = md5calculator(file_name);

    if (strcmp(md5sum,md5value) == 0)
    {
        printf("File match Success - md5 equal\n");
        //send (client_sock, "TRANSFER SUCCESS",16 ,0);
    }



    long long int total_bytes_recv = no_of_times*BYTES + bytesReceived;
    printf("Num of bytes recieved - %lld\n",total_bytes_recv );
}

void serve_client(int client_sock, char* file_name,char* user,char* pword, char* file_num,char* md5value,char* action,char* content_len)
{
    char* local_file_path = malloc(MESSAGE_LENGTH);
    int file_status = 0;
    char recv_buf[MESSAGE_LENGTH],send_buf[MESSAGE_LENGTH];
   
    
    //sprintf(local_file_path,"%s/%s/%s",server_file_path,user,file_name);
    
    int s;
    if((s = send(client_sock, "AUTH SUCCESS", 12,0)) <= 0)
    {
        printf("Send Error\n");
    }
    else
    {
        printf("Sent ACK\n");
    }


    printf("Inside serve client %s\n",action );
    if (strcmp(action,"put") == 0)
    {
        printf("Inside Upload\n");
        recv_chunk(user,pword,file_name,file_num,md5value,client_sock,content_len);
    }
        /*    }
            bzero(recv_buf,sizeof(recv_buf));
            s = recv(client_sock, recv_buf, MESSAGE_LENGTH, 0);
            if(s > 0 )
            {
                sscanf(recv_buf,"%s %s %s %s %s",uname,pword,filename,filenum2,md5sum);
                if(authentication(uname,pword) == 0)
                {
                    printf("Inside Upload\n");
                    recv_chunk(uname,pword,filename,filenum2,md5sum,client_sock);
                }
                else
                {
                    printf(RED"Authentication not successful\n"RESET);
                }
            }
            else
                printf("File chunk Upload Completed  %d\n",s);
        }
        */ 
    
    else if (strcmp(action,"list") == 0)
    {
        char* user_dir = malloc(200);
        bzero(user_dir,sizeof(user_dir));
        sprintf(user_dir,"%s/%s/",server_file_path,user);
        chdir(user_dir);
        printf("inside list %s\n",user_dir );
        system("ls -a > 'ls_file.txt'");
        send_file_pair(client_sock,"ls_file.txt",0,user,1);
        system("rm ls_file.txt");
       /* if((file_status = doesFileExist(local_file_path)) == 0)
        {
            send(client_sock, "DFS OK_EXIST\n", 13, 0);

        }
*/
    }

    //printf("Going out of serve client\n");
}

void client_handler(int client_sock,char* server_file_path)
{

    char recv_buf[MESSAGE_LENGTH],recv_buf_backup[MESSAGE_LENGTH],auth_string[MESSAGE_LENGTH],uname[MESSAGE_LENGTH],pword[MESSAGE_LENGTH],file_name[MESSAGE_LENGTH],file_num[MESSAGE_LENGTH],action[MESSAGE_LENGTH];
    bzero(recv_buf,sizeof(recv_buf));
    //bzero(recv_buf_backup,sizeof(recv_buf_backup));
    bzero(uname,sizeof(uname));
    bzero(pword,sizeof(pword));
    bzero(file_num,sizeof(file_num));

    bzero(action,sizeof(action));
    char* md5value = malloc(100);
    char* content_len =malloc(100);
    bzero(content_len,sizeof(content_len));
    bzero(md5value,sizeof(md5value));
    int s;

    printf("Inside handler\n%s\n",recv_buf);
    


 /*   if(( s = recv(client_sock, recv_buf, MESSAGE_LENGTH, 0)) < 0 )
    {    
        printf("Recieve error\n");
    }
    else if (s == 0)
    { 
        printf(YEL"Client disconnected (recv returns zero).\n"RESET);
    }*/
    while((s = recv(client_sock, recv_buf, MESSAGE_LENGTH, 0)) > 0 )
    {
        //strcpy(recv_buf_backup, &recv_buf);
        printf("Recv Buf %s\n",recv_buf );
        if(strncmp("Auth_string",recv_buf,11) == 0)
        {
            sscanf(recv_buf,"Auth_string %s %s %s %s %s %s %s",uname,pword,file_name,file_num,md5value,content_len,action);
            //sprintf(send_buf,"Auth_string %s %s %s %d %s put",uname,pword,filename,file_num1,md5value);

            if(authentication(uname,pword) == 0)
            {
                printf(GRN"Authentication Successful\n"RESET);
                
                serve_client(client_sock,file_name,uname,pword,file_num,md5value,action,content_len);
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
        bzero(recv_buf,sizeof(recv_buf));
        bzero(action,sizeof(action));
    }
    if(s == 0)
    {
        printf("Client disconnected\n");
    }
    printf("Out of client handler\n");

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

    //list();
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