

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
#include <stdbool.h>


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

//#define BYTES 100

static char list_array[4][20];

int server_sock_1;
int server_sock_2;
int server_sock_3;
int server_sock_4;

char* file_path;

bool server_status[4];

struct file_in_server
{
    char filename[100];
    int part1;
    int part2;
    int part3;
    int part4; 

};



struct list_ls
{
    char elements[100][100];
    bool valid;
};

struct final_ls
{
    char details[1000][100];
};

struct files_to_be_checked
{
    char details[100][100];
    int file_num [100];
    int server_num[100];
};

struct table_data
{
    char name[100][100];
    int file1[100];
    int file2[100];
    int file3[100];
    int file4[100];
    int server1[100];
    int server2[100];
    int server3[100];
    int server4[100];
};


struct list_ls ls_file_extract(char* filename,int list_num)
{
    FILE *ls_file =  fopen(filename, "r+" );
    char* parsed_buf = malloc(BYTES);
    char* parsed_buf_copy = malloc(BYTES);
    char* tok_str = malloc(BYTES);
    //char list_array[100][100];
    struct list_ls list_struct;
    //bzero(list_array,sizeof(list_array));
    bzero(tok_str,sizeof(tok_str));
    bzero(parsed_buf,sizeof(parsed_buf));
    int i = 0;

    while(fscanf(ls_file,"%s",parsed_buf) != EOF)
    {
        //printf("Parsed_buf %s and its length %d\n",parsed_buf,strlen(parsed_buf) );
        
        if((strcmp(parsed_buf,".") == 0) || (strcmp(parsed_buf,"..") == 0))
        {
            //printf("Pass\n");
        }
        else
        {
            char* file_name = malloc(BYTES);
            bzero(file_name,sizeof(file_name));
            bzero(parsed_buf_copy,sizeof(parsed_buf_copy));
            strcpy(parsed_buf_copy,parsed_buf);
            int file_num;
            
            char *token;
            tok_str = strtok(parsed_buf_copy, ".");
            //printf( "Strtok %s\n", tok_str );
            sprintf(file_name,"%s.",tok_str);
                //bzero(tok_str,sizeof(tok_str));
            tok_str = strtok(NULL, ".");
            sprintf(file_name,"%s%s",file_name,tok_str);
            //printf("%s \n", tok_str);
            tok_str = strtok(NULL, ".");
            if(tok_str == NULL)
            {
                //printf("No file num\n");
                file_num = 0;
            }
            else
            {
                file_num = atoi(tok_str);   
            }
            

            //printf("Befpr\n");
            sprintf(list_struct.elements[i],"%s %d",file_name,file_num);        
            //printf("After\n");
            i++;
            //printf("File Name - %s %d\n", file_name,file_num);


            //sscanf(parsed_buf,".%s.%d",file_name,file_num);
            //printf("%s\t%s\t%d\n",parsed_buf,file_name,file_num );
        }
        sprintf(list_struct.elements[i],"NULL");

        bzero(parsed_buf,sizeof(parsed_buf));
    }
    fclose(ls_file);

    return list_struct;

}


int doesFileExist(const char *filename) 
{
    struct stat st;
    int result = stat(filename, &st);
    if(result == 0)
    {
        //printf("\nFile exists!\n");
        return 0;
    }
    else
        return -1;
}

struct table_data make_table_zero(struct table_data table)
{
    memset(table.name, 0, sizeof(table.name[0][0]) *100 *100);
    memset(table.file1, 0, sizeof(table.file1[0]) *100);
    memset(table.file2, 0, sizeof(table.file2[0]) *100);
    memset(table.file3, 0, sizeof(table.file3[0]) *100);
    memset(table.file4, 0, sizeof(table.file4[0]) *100);
    memset(table.server1, 0, sizeof(table.server1[0]) *100);
    memset(table.server2, 0, sizeof(table.server2[0]) *100);
    memset(table.server3, 0, sizeof(table.server3[0]) *100);
    memset(table.server4, 0, sizeof(table.server4[0]) *100);

    return table;

}

struct table_data ls_process(int print) 
{
    FILE *fp;



    struct table_data table;
    table = make_table_zero(table);
/*

    for(int b = 0;b<=100;b++)
    {
        printf("%s %d %d %d %d %d %d %d %d\n",table.name[b],table.file1[b],table.file2[b],table.file3[b],table.file4[b],table.server1[b],table.server2[b],table.server3[b],table.server4[b]);
    }
*/
    char* filename = malloc(BYTES);
    
    struct list_ls ls1,ls2,ls3,ls4,ls[5];
   
 

    bzero(list_array,sizeof(list_array));
    int i = 1;
    for(int i =1; i<=4; i++)
    {
    bzero(filename,sizeof(filename));
    bzero(list_array,sizeof(list_array));
    sprintf(filename,"%s/temp/zzz_ls_file.txt.%d",file_path,i);
    if(doesFileExist(filename) == -1)
    {
        //printf("Data from Server %d is not available! \n",i);
        ls[i].valid = false;
        continue;
    }
    ls[i] =  ls_file_extract(filename,i);
    ls[i].valid = true;
    
    
    }
    size_t k = 1;

    int array_element = 0;
    
    struct final_ls total_list_name;
    int total_file_num[5][100];
    int server_name[5][100];

    //printf("Before while\n");
    for(size_t k = 1; k<=4; k++)
    {
        size_t j = 0;
        if(ls[k].valid == false)
        {
            //printf("Server %d LS file not available\n",k );
            continue;
        }
        int ind =0;
        while(strcmp(ls[k].elements[j],"NULL")!=0)
        {
             //printf("Inside while %s\n",ls[k].elements[j]);
             
             char* file = malloc(100);
             char* num = malloc(100);
             bzero(num,sizeof(num));
             bzero(file,sizeof(file));
             
             sscanf(ls[k].elements[j],"%s %s",file,num);
             //ind = ((k-1)*100) + j;
             strcpy(total_list_name.details[k*100 +j],file);
             total_file_num[k][j] = atoi(num);
             server_name[k][j] = k;
             
            // printf(" copy %s %d %d\n",total_list_name.details[k*100+j],total_file_num[k][j],server_name[k][j] );
            j++;
        }

        strcpy(total_list_name.details[k*100 +j],"NULL");
        total_file_num[k][j] = 0;
        server_name[k][j] = 0;


    }



int x = 0;
struct files_to_be_checked check_files;
bool flag = false;
for(int q=1;q<5;q++)
{
    if(ls[q].valid== false)
        continue;
    int ind = 0;
    for(int p = 0;p<100;p++)
    {

        if(strcmp(total_list_name.details[q*100 + p],"NULL") == 0)
        {
            //printf("Going out of loop\n");
            flag = true;
            break;
        }
        else
        {
            //printf("pq %d %d\n",p,q );
            strcpy(check_files.details[x],total_list_name.details[q*100+p]);
            check_files.file_num[x] =total_file_num[q][p];
            check_files.server_num[x] = server_name[q][p]; 
            //printf("%s %d %d %d\n",total_list_name.details[q*100+p],total_file_num[q][p],server_name[q][p],x );
            

            x++;
            
        }
        //printf("Last %s\n",total_list_name.details[q*100 + p]);
    }
    
}

bool new_flag = false;
int name_id = 0;
for(int y=0;y<=x;y++)
{
    
    strcpy(table.name[name_id],check_files.details[y]);
    if(strcmp(table.name[name_id],table.name[name_id-1]) == 0)
    {
        continue;
    }
    //printf("Table Name %s %d\n",table.name[name_id],name_id );
    switch(check_files.file_num[y])
    {
        case 1:
        {
            table.file1[name_id] = check_files.file_num[y];
            table.server1[name_id] = check_files.server_num[y];
            break;      
        }
        case 2:
        {
            table.file2[name_id] = check_files.file_num[y];
            table.server2[name_id] = check_files.server_num[y];
            break;      
        }
        case 3:
        {
            table.file3[name_id] = check_files.file_num[y];
            table.server3[name_id] = check_files.server_num[y];
            break;      
        }
        case 4:
        {
            table.file4[name_id] = check_files.file_num[y];
            table.server4[name_id] = check_files.server_num[y];
            break;      
        }
    }
    

    for(int z =y+1; z<=x;z++)
    {

        if(strcmp(check_files.details[y],check_files.details[z])==0)
        {

            //printf("Fild Name - %s File Num - %d Server Num - %d\n",check_files.details[z],check_files.file_num[z],check_files.server_num[z] );

            switch(check_files.file_num[z])
            {
                case 1:
                {
                    {
                        table.file1[name_id] = check_files.file_num[z];
                        table.server1[name_id] = check_files.server_num[z];

                    }
                    break;      
                }
                case 2:
                {
                    {
                        table.file2[name_id] = check_files.file_num[z];
                        table.server2[name_id] = check_files.server_num[z];
                    }
                    break;
                }
                case 3:
                {
                    {
                        table.file3[name_id] = check_files.file_num[z];
                        table.server3[name_id] = check_files.server_num[z];
                    }
                    break;      
                }
                case 4:
                {
                    {
                        table.file4[name_id] = check_files.file_num[z];
                        table.server4[name_id] = check_files.server_num[z];
                    }
                    break;      
                }
            }


            //printf("Match Occured %s %d %d\n",check_files.details[z],check_files.file_num[z],check_files.server_num[z]);
            //printf("%s %d %d %d %d %d %d %d %d\n",table.name[name_id],table.file1[name_id],table.file2[name_id],table.file3[name_id],table.file4[name_id],table.server1[name_id],table.server2[name_id],table.server3[name_id],table.server4[name_id]);
        }

            
    }
    name_id++;

    if(check_files.file_num[y] == 0)
        {
            new_flag = true;
            break;
        }
    
}


if(print == 1)
{


for(int b = 0;b<100;b++)
{
    //printf("%s %d %d %d %d %d %d %d %d\n",table.name[b],table.file1[b],table.file2[b],table.file3[b],table.file4[b],table.server1[b],table.server2[b],table.server3[b],table.server4[b]);
    //printf(" Variable %s  num %d char %c\n",table.name[b],atoi(table.name[b]),table.name[b][0] );
    if((table.name[b][0]) == NULL)
    {
        break;
    }

    if(strcmp(table.name[b],"zzz_ls_file.txt") == 0)
    {
        continue;
    }

    if(table.file1[b] !=0 )
    {
        if(table.file2[b] !=0 )
        {

            if(table.file3[b] !=0 )
            {

                if(table.file4[b] !=0 )
                {

                    if((table.server1[b] !=0))
                    {
                        if((table.server2[b] !=0) )
                        {
                            if((table.server3[b] !=0) )
                            {
                                if((table.server4[b] !=0) )
                                {
                                    printf(GRN"%s\n"RESET,table.name[b]);
                                }
                                else
                                {
                                    printf(RED"%s[incomplete]\n"RESET, table.name[b]);
                                    continue;
                                }

                            }
                            else
                            {
                                printf(RED"%s[incomplete]\n"RESET, table.name[b]);
                                continue;
                            }

                        }
                        else
                        {
                            printf(RED"%s[incomplete]\n"RESET, table.name[b]);
                            continue;
                        }
                    }
                    else
                    {
                        printf(RED"%s[incomplete]\n"RESET, table.name[b]);
                        continue;
                    }
                }
                else
                {
                    printf(RED"%s[incomplete]\n"RESET, table.name[b]);
                    continue;
                }
            }
            else
            {
                printf(RED"%s[incomplete]\n"RESET, table.name[b]);
                continue;
            }

        }
        else
        {
            printf(RED"%s[incomplete]\n"RESET, table.name[b]);
            continue;
        }

    }
    else
    {
        printf(RED"%s[incomplete]\n"RESET, table.name[b]);
        continue;
    }


    //printf("%s %d %d %d %d %d %d %d %d\n",table.name[b],table.file1[b],table.file2[b],table.file3[b],table.file4[b],table.server1[b],table.server2[b],table.server3[b],table.server4[b]);

}
}
/*
 for(int b = 0;b<=100;b++)
    {
        printf("%s %d %d %d %d %d %d %d %d\n",table.name[b],table.file1[b],table.file2[b],table.file3[b],table.file4[b],table.server1[b],table.server2[b],table.server3[b],table.server4[b]);
    }
*/
char* rm_cmd = malloc(200);
bzero(rm_cmd,sizeof(rm_cmd));
sprintf(rm_cmd,"rm %s/temp/zzz_ls_file*",file_path);
system(rm_cmd);

return table;


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
                //printf("Strcmp %s %s\n",content_processed,data );
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
    sprintf(concatenated_dir,"%s/files/encrypted/%s",file_path,filename);
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

int create_connection(int port,int server_num)
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
        printf("Looks like Server %d in port %d is down!\n",server_num+1,port);
        
        server_status[server_num] = false;
        char input[100];
        bzero(input,sizeof(input));
        
    }
    else
    {
       server_status[server_num] = true; 
        //printf("Connection Created - %d\n",sock );
    }


    struct timeval tv;
    tv.tv_sec = 1;       /* Timeout in seconds */
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));


    return sock;
}


void servers_connect()
{
    char* port_1 = data_finder("server",1);
    char* port_2 = data_finder("server",2);
    char* port_3 = data_finder("server",3);
    char* port_4 = data_finder("server",4);

    for (int i=0;i<4;i++)
    {
        server_status[i] = false;
    }

    server_sock_1 = create_connection(atoi(port_1),0);
    server_sock_2 = create_connection(atoi(port_2),1);
    server_sock_3 = create_connection(atoi(port_3),2);
    server_sock_4 = create_connection(atoi(port_4),3);
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
            //printf(RED"Send failed \n"RESET);
        }
        
        bzero(recv_buf,sizeof(recv_buf));
        if(recv(server_socket , recv_buf , BYTES,0) <= 0)
        {
        
            //printf(RED"Send failed\n"RESET);
        
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
        else if(strcmp(recv_buf,"AUTH FAILURE") == 0)
        {
            
            printf("Invalid Username/Password.  Please  try again.\n");
        } 


                
    }
} 


                

void encrypt_file(char* filename)
{

    char concatenated_dir[1024],encrypted_dir[1024];
    char* openssl_cmd = malloc(BYTES);
    bzero(openssl_cmd,sizeof(openssl_cmd));
    char *temp_dir = malloc(100);
    bzero(temp_dir,sizeof(temp_dir));

    char* uname = malloc(100);
    char* pword = malloc(100);
    bzero(uname,sizeof(uname));
    bzero(pword,sizeof(pword));
    uname = data_finder("user",0);
    pword = data_finder("pass",0);

    bzero(encrypted_dir,sizeof(encrypted_dir));
    bzero(concatenated_dir,sizeof(concatenated_dir));
    sprintf(concatenated_dir,"%s/files/%s",file_path,filename);
    sprintf(encrypted_dir,"%s/files/encrypted/%s",file_path,filename);

    sprintf(openssl_cmd,"openssl enc -aes-256-cbc -salt -in %s -out %s -k %s%s",concatenated_dir,encrypted_dir,uname,pword);
    system(openssl_cmd);
    printf(MAG"Encryption done - %s\n"RESET,filename );

}

int put_handler(char* filename)
{
    printf("In put handler %s\n",filename );


    encrypt_file(filename);

    if(file_splitter(filename) == -1)
    {
        return -1;
    }
    char concatenated_dir[1024];
    bzero(concatenated_dir,sizeof(concatenated_dir));
    sprintf(concatenated_dir,"%s/files",file_path);
    chdir(concatenated_dir);
    //printf("%s\n", );

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




    char* rm_cmd = malloc(1024);
    bzero(rm_cmd,sizeof(rm_cmd));

    sprintf(rm_cmd,"rm %s/temp/%s*",file_path,filename);
    system(rm_cmd);
    bzero(rm_cmd,sizeof(rm_cmd));
    sprintf(rm_cmd,"rm %s/files/encrypted/%s",file_path,filename);
    system(rm_cmd);

    chdir(file_path);
    return 0;


}






void list_cmd(char* send_buf,int server_socket,int server_num)
{
    char* recv_buf = malloc(MESSAGE_LENGTH);
    bzero(recv_buf,sizeof(recv_buf));

    //printf("Inside List cmd %s %d %d\n",send_buf,server_num,server_socket);

    if(send(server_socket , send_buf , strlen(send_buf),MSG_NOSIGNAL) <= 0)
    {
        //printf(RED"Send failed\n"RESET);
        //return;
    }
        
    bzero(recv_buf,sizeof(recv_buf));
    if(recv(server_socket , recv_buf , BYTES,0) <= 0)
    {
        
        //printf(RED"Recv failed\n"RESET);

        //return;

        
    }
    //printf("auth - %s\n",recv_buf );
    if(strncmp(recv_buf,"AUTH SUCCESS",12) == 0)
    {
        //printf("Auth success\n");
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
        //printf("recv_buf %s\n",recv_buf);



        sprintf(file_name,"%s/temp/zzz_ls_file.txt.%d",file_path,server_num);
        FILE* file_p = fopen(file_name, "w+");
        int bytesReceived,no_of_times = 0;
        
        bzero(recv_buf,sizeof(recv_buf));
        int read_bytes = BYTES;

        int file_size = atoi(content_len);
        int remain_data = file_size;

        //printf("Before Recieveing\n");

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
                    //printf("Breaking it\n");
                    break;
                    
                }

            }
            
            bzero(recv_buf,sizeof(recv_buf));

            
            //printf("\nReceive %d bytes and we hope :- %d bytes\n", bytesReceived, remain_data);
            //printf("Passed print\n");
        }
        fclose(file_p);

        //printf(MAG"list recieved from  %d\n\n"RESET,server_num);
        //printf(RED"Send failed\n"RESET);
            
     }
     else if(strcmp(recv_buf,"AUTH FAILURE") == 0)
     {
        
        printf("Invalid Username/Password.  Please  try again.\n");
     }
     

}

struct table_data list_handler(int print)
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
    
    //printf("LS from 1 done\n");
    bzero(send_buf,sizeof(send_buf));

    sprintf(send_buf,"Auth_string %s %s nan nan nan nan list",uname,pword);

    list_cmd(send_buf,server_sock_2,2);
    //printf("LS from 2 done\n");
    bzero(send_buf,sizeof(send_buf));
    
    sprintf(send_buf,"Auth_string %s %s nan nan nan nan list",uname,pword);

    list_cmd(send_buf,server_sock_3,3);
    //printf("LS from 3 done\n");
    bzero(send_buf,sizeof(send_buf));
    
    sprintf(send_buf,"Auth_string %s %s nan nan nan nan list",uname,pword);

    list_cmd(send_buf,server_sock_4,4);
    //printf("LS from 4 done\n");

    struct table_data table;
    table = ls_process(print);
    
    return table;

}


void get_cmd(char* send_buf,int server_socket,int server_num,char* filename, int file_num)
{
    char* recv_buf = malloc(MESSAGE_LENGTH);
    bzero(recv_buf,sizeof(recv_buf));

    //printf("Inside get cmd  %d %d\n",server_num,server_socket);

    if(send(server_socket , send_buf , strlen(send_buf),MSG_NOSIGNAL) <= 0)
    {
        printf(RED"Send failed\n"RESET);
        //return;
    }
        
    
    bzero(recv_buf,sizeof(recv_buf));
    //printf("auth - %s\n",recv_buf );
    int s;
    bzero(recv_buf,sizeof(recv_buf));
    if((s = recv(server_socket , recv_buf , BYTES,0)) <= 0)
    {
        
        //printf(RED"Recv failed\n"RESET);

        //return;
    }
    
    //printf("S = %d",s);
    //printf("auth - %s  %d\n ",recv_buf,s );
    if(strncmp(recv_buf,"AUTH SUCCESS",12) == 0)
    {
        //printf("Auth success\n");

        

        bzero(recv_buf,sizeof(recv_buf));
        if(recv(server_socket , recv_buf , BYTES,MSG_NOSIGNAL) <= 0)
        {
            
            printf(RED"Send failed\n"RESET);
            
        }

        char* content_len = malloc(100);
        bzero(content_len,sizeof(content_len));
        
        //printf("Recieved the message, %s\n",recv_buf);
        char* md5sum = malloc(100);
        bzero(md5sum,sizeof(md5sum));

        //printf("%s\n",recv_buf );

        if(strncmp("FILE_EXISTS",recv_buf,11) == 0)
        {

            sscanf(recv_buf,"FILE_EXISTS %s %s",content_len,md5sum);
            //printf("recv_buf %s\n",recv_buf);
            

            char* file_name = malloc(200);
            bzero(file_name,sizeof(file_name));
            
            //printf("recv_buf %s\n",recv_buf);



            sprintf(file_name,"%s/temp/get/%s.%d",file_path,filename,file_num);
            FILE* file_p = fopen(file_name, "w+");
            int bytesReceived,no_of_times = 0;
            
            bzero(recv_buf,sizeof(recv_buf));
            int read_bytes = BYTES;

            int file_size = atoi(content_len);
            int remain_data = file_size;

            //printf("Before Recieveing\n");

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
                        //printf("Breaking it\n");
                        break;
                        
                    }

                }
                
                bzero(recv_buf,sizeof(recv_buf));

                
                //printf("\nReceive %d bytes and we hope :- %d bytes\n", bytesReceived, remain_data);
                //printf("Passed print\n");
            }
            fclose(file_p);

        //    printf("Recieved\n");
            printf(GRN"File Recieved %s %d\n"RESET,filename,file_num);


        }
        else if(strcmp(recv_buf,"NO_FILE_EXISTS") == 0)
        {
            printf(RED"File does not exist in server\n"RESET);
        }
        else
        {
            printf("RECV buf %s\n",recv_buf );
        }
    }
     else if(strcmp(recv_buf,"AUTH FAILURE") == 0)
     {
        
        printf("Invalid Username/Password.  Please  try again.\n");
     }

}

void join_files(char* filename)
{
    char* input_files = malloc(BYTES);
    bzero(input_files,sizeof(input_files));
    char* output_file = malloc(BYTES);
    bzero(output_file,sizeof(output_file));
    char* cat_cmd = malloc(BYTES);
    bzero(cat_cmd,sizeof(cat_cmd));
    sprintf(input_files,"%s/temp/get/%s*",file_path,filename);
    sprintf(output_file,"%s/temp/get/joined_files/%s",file_path,filename);

    sprintf(cat_cmd,"cat %s > %s",input_files,output_file);
    system(cat_cmd);
    printf(MAG"File Merged\n"RESET);

    char* rm_cmd = malloc(1024);
    bzero(rm_cmd,sizeof(rm_cmd));

    sprintf(rm_cmd,"rm %s",input_files);
    system(rm_cmd);


}


void decrypt_file(char* filename)
{
    char* uname = malloc(100);
    char* pword = malloc(100);
    bzero(uname,sizeof(uname));
    bzero(pword,sizeof(pword));
    uname = data_finder("user",0);
    pword = data_finder("pass",0);

    char merged_file[1024],decrypted_file[1024];
    char* openssl_cmd = malloc(BYTES);
    bzero(openssl_cmd,sizeof(openssl_cmd));
   


    bzero(decrypted_file,sizeof(decrypted_file));
    bzero(merged_file,sizeof(merged_file));
    sprintf(merged_file,"%s/temp/get/joined_files/%s",file_path,filename);
    sprintf(decrypted_file,"%s/files/download/%s",file_path,filename);


    sprintf(openssl_cmd,"openssl enc -aes-256-cbc -d -in %s -out %s -k %s%s",merged_file,decrypted_file,uname,pword);
    system(openssl_cmd);

    printf(MAG"Decryption done - %s\n"RESET,filename );

    char* rm_cmd = malloc(1024);
    bzero(rm_cmd,sizeof(rm_cmd));

    sprintf(rm_cmd,"rm %s",merged_file);
    system(rm_cmd);

}

void get_handler(char* filename,struct table_data table)
{

    char* send_buf = malloc(MESSAGE_LENGTH);
    bzero(send_buf,sizeof(send_buf));

    char* uname = malloc(100);
    char* pword = malloc(100);
    bzero(uname,sizeof(uname));
    bzero(pword,sizeof(pword));
    uname = data_finder("user",0);
    pword = data_finder("pass",0);

    //struct table_data table;
    //printf("Inside Get Handler\n");


    int server_socks[5];
    server_socks[1] = server_sock_1;

    server_socks[2] = server_sock_2;


    server_socks[3] = server_sock_3;


    server_socks[4] = server_sock_4;



    

    //printf ("Inside Get handler and after the table\n");

    for(int i=0;i<100;i++)
    {

        //printf("%s %s %d\n",table.name[i],filename,i);
        if(strcmp(filename,table.name[i]) == 0)
        {
            //printf("Strcmp matched\n");
            int file_num1 = table.file1[i];
            int server_num1 = table.server1[i]; 
            sprintf(send_buf,"Auth_string %s %s %s %d nan nan get",uname,pword,filename,file_num1);
        
            get_cmd(send_buf,server_socks[server_num1],1,filename,file_num1);

            int file_num2 = table.file2[i];
            int server_num2 = table.server2[i]; 
            sprintf(send_buf,"Auth_string %s %s %s %d nan nan get",uname,pword,filename,file_num2);
            get_cmd(send_buf,server_socks[server_num2],2,filename,file_num2);

            int file_num3 = table.file3[i];
            int server_num3 = table.server3[i]; 
            sprintf(send_buf,"Auth_string %s %s %s %d nan nan get",uname,pword,filename,file_num3);
            get_cmd(send_buf,server_socks[server_num3],3,filename,file_num3);

            int file_num4 = table.file4[i];
            int server_num4 = table.server4[i]; 
            sprintf(send_buf,"Auth_string %s %s %s %d nan nan get",uname,pword,filename,file_num4);
            get_cmd(send_buf,server_socks[server_num4],4,filename,file_num4);

            join_files(filename);
            decrypt_file(filename);


            break;
        }
        if(i==99)
        {
            printf(RED"File Doesnt Exist\n"RESET);
        }
    }

    
    //sscanf(recv_buf,"Auth_string %s %s %s %s %s %s %s",uname,pword,file_name,file_num,md5value,content_len,action);


}



bool is_it_folder(char* filename)
{

    bool valid;
    if(filename[0] == '/')
    {
        printf("It is a valid folder format\n");
        valid = true;
    }
    else
    {
        valid = false;
    }
    return valid;
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
        servers_connect();

        //int server_sock = create_connection(10003);
        //servers_connect();
        int ls_cmd = strncmp("LIST",inp,4);
        if (ls_cmd!=0)
        {

            sscanf(inp,"%s %s",cmd,filename);
            int get_cmd = strncmp("GET",inp,3);
            int put_cmd = strncmp("PUT",inp,3);
            if(get_cmd == 0)
            {
                //printf("Inside GET command\n");
                struct table_data table;
                table = list_handler(0);

                get_handler(filename,table);
            }

            else if(put_cmd == 0)
            {
                //printf("Inside PUT command\n");
               // printf(server_status[0] ? "true" : "false");
               // printf(server_status[1] ? "true" : "false");
               // printf(server_status[2] ? "true" : "false");
               // printf(server_status[3] ? "true" : "false");

               if((server_status[0] && server_status[1] && server_status[2] && server_status[3]) == true)
                { 
                    put_handler(filename);   
                }
                else
                {
                    printf(RED"One or the other server is down, cant upload the file\n"RESET);
                }
            }
        }
        else if(ls_cmd == 0)
        {
            //printf("Inside LS command\n");
            //sscanf(inp,"%s %s",cmd,filename);
            //struct folder_data f_data;

           list_handler(1);    
            

        }
       

    }

   
    exit(1);
}