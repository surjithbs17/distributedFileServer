

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
#define BYTES 1024
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
    char* dfc_file = malloc(100);
    bzero(dfc_file,sizeof(dfc_file));
    token = "1";


    //printf("%s\n",format );
    char data[100];
    bzero(data,sizeof(data));


    //printf("%s\n",data );

    FILE *f;
    bzero(content_parsed,sizeof(content_parsed));
    //bzero(content_parsed[1],sizeof(content_parsed[1]));  

    sprintf(dfc_file,"%s/dfc.conf",file_path);
    //printf("DFC Path%s\n",dfc_file );

    if((f = fopen(dfc_file, "r")) == NULL)
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
                    //printf("POrt string %s\n",port_string );
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
                    //printf("%s\n",content_parsed_1 );
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
    char *temp_dir = malloc(100);
    bzero(temp_dir,sizeof(temp_dir));
    bzero(concatenated_dir,sizeof(concatenated_dir));
    sprintf(concatenated_dir,"%s/files/%s",file_path,filename);
    //chdir(concatenated_dir);
    sprintf(temp_dir,"%s/temp/%s",file_path,filename);
    printf("Path names for split %s\n%s\n",concatenated_dir,temp_dir );
    sprintf(split_cmd,"split --number=l/%d ${fspec} %s %s",4,concatenated_dir,temp_dir);
    if(doesFileExist(concatenated_dir) == -1)
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

/*
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
*/

int send_file_pair(int server_socket,char* filename,int file_num1)
{
    char send_buf[MESSAGE_LENGTH],recv_buf[MESSAGE_LENGTH],data_to_send[BYTES];
    bzero(send_buf,sizeof(send_buf));
    bzero(recv_buf,sizeof(recv_buf));

    char* uname = malloc(100);
    char* pword = malloc(100);
    char* filename1 = malloc(100);
    
    bzero(filename1,sizeof(filename1));
    
    bzero(uname,sizeof(uname));
    bzero(pword,sizeof(pword));
    uname = data_finder("user",0);
    pword = data_finder("pass",0);
    
    sprintf(filename1,"%s/temp/%sa%c",file_path,filename,96+file_num1);
    
    printf("%s\n",filename1 );
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
        sprintf(send_buf,"Auth_string %s %s %s %d %s %s put",uname,pword,filename,file_num1,md5value,content_len);
        int bytes_read = 0;
        


        if(send(server_socket , send_buf , strlen(send_buf),0) <= 0)
        {
            printf(RED"Send failed\n"RESET);
        }
        
        bzero(recv_buf,sizeof(recv_buf));
        if(recv(server_socket , recv_buf , BYTES,0) <= 0)
        {
        
            printf(RED"Send failed\n"RESET);
        
        }

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

    char* value = malloc(100);
    char* last_4 = malloc(10);
    bzero(value,sizeof(value));
    bzero(last_4,sizeof(last_4));

    value = md5calculator(filename);

    int length = strlen(value);
    last_4 = value + length - 1;
    char* ptr;
    long ret = strtol(last_4, &ptr, 16);
    printf("md5sum %s\nLast hex character %s\n converted decimal %ld\n",value,last_4,ret);

    switch(ret%4)
    {
        case 0:
            {
                printf("Case 0\n");
                printf("1 - %d\n2 - %d\n3 - %d\n4 - %d\n",server_sock_1,server_sock_2,server_sock_3,server_sock_4 );
                send_file_pair(server_sock_1,filename,1);
                send_file_pair(server_sock_1,filename,2);
                send_file_pair(server_sock_2,filename,2);
                send_file_pair(server_sock_2,filename,3);
                send_file_pair(server_sock_3,filename,3);
                send_file_pair(server_sock_3,filename,4);
                send_file_pair(server_sock_4,filename,4);
                send_file_pair(server_sock_4,filename,1);
                break;
            }
        case 1:
            {
                printf("Case 1\n");
                printf("1 - %d\n2 - %d\n3 - %d\n4 - %d\n",server_sock_1,server_sock_2,server_sock_3,server_sock_4 );
                send_file_pair(server_sock_1,filename,4);
                send_file_pair(server_sock_1,filename,1);
                send_file_pair(server_sock_2,filename,1);
                send_file_pair(server_sock_2,filename,2);
                send_file_pair(server_sock_3,filename,2);
                send_file_pair(server_sock_3,filename,3);
                send_file_pair(server_sock_4,filename,3);
                send_file_pair(server_sock_4,filename,4);
                break;
            }
        case 2:
            {
                printf("Case 2\n");
                printf("1 - %d\n2 - %d\n3 - %d\n4 - %d\n",server_sock_1,server_sock_2,server_sock_3,server_sock_4 );
                send_file_pair(server_sock_1,filename,3);
                send_file_pair(server_sock_1,filename,4);
                send_file_pair(server_sock_2,filename,4);
                send_file_pair(server_sock_2,filename,1);
                send_file_pair(server_sock_3,filename,1);
                send_file_pair(server_sock_3,filename,2);
                send_file_pair(server_sock_4,filename,2);
                send_file_pair(server_sock_4,filename,3);
                break;
            }
        case 3:
            {
                printf("Case 3\n");
                printf("1 - %d\n2 - %d\n3 - %d\n4 - %d\n",server_sock_1,server_sock_2,server_sock_3,server_sock_4 );
                send_file_pair(server_sock_1,filename,2);
                send_file_pair(server_sock_1,filename,3);
                send_file_pair(server_sock_2,filename,3);
                send_file_pair(server_sock_2,filename,4);
                send_file_pair(server_sock_3,filename,4);
                send_file_pair(server_sock_3,filename,1);
                send_file_pair(server_sock_4,filename,1);
                send_file_pair(server_sock_4,filename,2);
                break;
            }


    }






    chdir(file_path);
    return 0;


}



void get_handler(char* filename)
{

}


void list_cmd(char* send_buf,int server_socket,int server_num)
{
    char* recv_buf = malloc(MESSAGE_LENGTH);
    bzero(recv_buf,sizeof(recv_buf));

    printf("Inside List cmd %s %d %d\n",send_buf,server_num,server_socket);

    if(send(server_socket , send_buf , strlen(send_buf),0) <= 0)
    {
        printf(RED"Send failed\n"RESET);
    }
        
    bzero(recv_buf,sizeof(recv_buf));
    if(recv(server_socket , recv_buf , BYTES,0) <= 0)
    {
        
        printf(RED"Send failed\n"RESET);
        
    }
    printf("auth - %s\n",recv_buf );
    if(strncmp(recv_buf,"AUTH SUCCESS",12) == 0)
    {
        char* content_len = malloc(100);
        bzero(content_len,sizeof(content_len));
        
        bzero(recv_buf,sizeof(recv_buf));
        if(recv(server_socket , recv_buf , BYTES,0) <= 0)
        {
            
            printf(RED"Send failed\n"RESET);
            
        }

        sscanf(recv_buf,"AUTH SUCCESS %s",content_len);
        char* file_name = malloc(200);
        bzero(file_name,sizeof(file_name));
        printf("recv_buf %s\n",recv_buf);



        sprintf(file_name,"%s/temp/ls_file.txt.%d",file_path,server_num);
        FILE* file_p = fopen(file_name, "w+");
        int bytesReceived,no_of_times = 0;
        
        bzero(recv_buf,sizeof(recv_buf));
        int read_bytes = BYTES;

        int file_size = atoi(content_len);
        int remain_data = file_size;

        printf("Before Recieveing\n");

        if(send(server_socket , "SEND" , 4,0) <= 0)
        {
            printf(RED"Send failed\n"RESET);
        }
        
        while(((bytesReceived = recv(server_socket, recv_buf, read_bytes,0)) > 0) && (remain_data > 0))
        {
            //printf("Bytes received %d\n %s\n",bytesReceived,recv_buf);
            //printf("happening 1 \n");
            fwrite(recv_buf, sizeof(char),bytesReceived,file_p);
            //printf("happening 2\n");
            
            
            remain_data -= bytesReceived;
            
            //printf("file name %s  \nRemaining data - %d\n",recv_buf,remain_data );
            if(remain_data< BYTES)
            {
                read_bytes = remain_data;
                if(remain_data == 0)
                {
                    printf("Breaking it\n");
                    break;
                    
                }

            }
            
            bzero(recv_buf,sizeof(recv_buf));

            
            printf("\nReceive %d bytes and we hope :- %d bytes\n", bytesReceived, remain_data);
            //printf("Passed print\n");
        }
        fclose(file_p);

        printf("Recieved\n");
        //printf(RED"Send failed\n"RESET);
            
     }
     else
     {
        printf("Authentication Failure\n");
     }

}

void list_handler()
{

    char* send_buf = malloc(MESSAGE_LENGTH);
    bzero(send_buf,sizeof(send_buf));

    char* uname = malloc(100);
    char* pword = malloc(100);
    bzero(uname,sizeof(uname));
    bzero(pword,sizeof(pword));
    uname = data_finder("user",0);
    pword = data_finder("pass",0);

    sprintf(send_buf,"Auth_string %s %s nan nan nan nan list",uname,pword);

    list_cmd(send_buf,server_sock_1,1);
    
    printf("LS from 1 done\n");
    bzero(send_buf,sizeof(send_buf));

    sprintf(send_buf,"Auth_string %s %s nan nan nan nan list",uname,pword);

    list_cmd(send_buf,server_sock_2,2);
    printf("LS from 2 done\n");
    bzero(send_buf,sizeof(send_buf));
    
    sprintf(send_buf,"Auth_string %s %s nan nan nan nan list",uname,pword);

    list_cmd(send_buf,server_sock_3,3);
    printf("LS from 3 done\n");
    bzero(send_buf,sizeof(send_buf));
    
    sprintf(send_buf,"Auth_string %s %s nan nan nan nan list",uname,pword);

    list_cmd(send_buf,server_sock_4,4);
    printf("LS from 4 done\n");
    
    

}

int main(int argc , char *argv[])
{

    int socket_desc , client_sock , c , *new_sock;
    char send_buf[MESSAGE_LENGTH],recv_buf[MESSAGE_LENGTH];
    struct sockaddr_in server , client;
    char* cmd = malloc(200);
    char* filename = malloc(200); 
    char* port;
    //char* server_port = data_finder("pass",0); 

    file_path = malloc(200);
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    strcpy(file_path,cwd);
    printf("File Path%s\n",file_path );



    //printf("In Main, Server %s\n",server_port);
    servers_connect();
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
        //servers_connect();
        int ls_cmd = strcmp("LIST",inp);
        if (ls_cmd!=0)
        {

            sscanf(inp,"%s %s",cmd,filename);
            int get_cmd = strncmp("GET",inp,3);
            int put_cmd = strncmp("PUT",inp,3);
            if(get_cmd == 0)
            {
                printf("Inside GET command\n");
                
                get_handler(filename);
                sprintf(send_buf,"Auth_string %s %s %s 1",uname,pword,filename);
                printf("Send Buf %s\n", send_buf);
                bzero(cmd,sizeof(cmd));
                int s;
                

                if( send(server_sock_1 , send_buf , strlen(send_buf) , 0) < 0)
                {
                    puts("Send failed");
                    //return 1;
                }
                
                if(recv(server_sock_1, recv_buf, MESSAGE_LENGTH, 0) < 0)
                {
                    printf("Recv Error\n");
                }
                else
                {
                    printf("Recieved %s\n",recv_buf );
                }
                printf(RED"Sock 3 %d\n"RESET,server_sock_3);
                bzero(send_buf,sizeof(send_buf));
                bzero(recv_buf,sizeof(recv_buf));
                bzero(filename,sizeof(filename));
            }

            else if(put_cmd == 0)
            {
                printf("Inside PUT command\n");
                
                
                put_handler(filename);   
            }
        }
        else if(ls_cmd == 0)
        {
            printf("Inside LS command\n");
            list_handler();
        }
       

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