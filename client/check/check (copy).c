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

#define BYTES 100
static char list_array[4][20];

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
}table;


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
			printf("Pass\n");
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
			printf("%s\t%s\t%d\n",parsed_buf,file_name,file_num );
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
        printf("\nFile exists!\n");
        return 0;
    }
    else
        return -1;
}


void main() 
{
   	FILE *fp;

   	char* filename = malloc(BYTES);
   	//char list_array_1[100];
   	struct list_ls ls1,ls2,ls3,ls4,ls[5];
   //	bzero(ls1.elements[100][100],sizeof(ls1.elements[100][100]));
 

   	bzero(list_array,sizeof(list_array));
   	int i = 1;
   	for(int i =1; i<=4; i++)
   	{
   	bzero(filename,sizeof(filename));
   	bzero(list_array,sizeof(list_array));
   	sprintf(filename,"ls_file.txt.%d",i);
   	if(doesFileExist(filename) == -1)
   	{
   		printf("Data from Server %d is not available! \n",i);
   		ls[i].valid = false;
   		continue;
   	}
   	ls[i] =  ls_file_extract(filename,i);
   	ls[i].valid = true;
/*   	 switch(i)
   	{

   		
   		case 1:
   		{
   			ls1 = ls_file_extract(filename,i);
   			ls[0] = ls_file_extract(filename,i);
   			break;
   		}
   		case 2:
   		{
   			ls2 = ls_file_extract(filename,i);
   			break;
   		}
   		case 3:
   		{
   			ls3 = ls_file_extract(filename,i);
   			break;
   		}
   		case 4:
   		{
   			ls4 = ls_file_extract(filename,i);
   			break;
   		}
   	}
*/	   	
	
   	}


/*
   	for(int j = 0; j<4; j++ )
   	{
   		for(int k = 0; strncmp(ls[j].elements[k],"",1) == 0; k++)
   		{
   			printf("%d %d %s\n",j,k,ls[j].elements[k]);		
   		}
   	}
*/
	
	size_t k = 1;

	int array_element = 0;
	//char total_list_name[100][100]; //4*100
	
	struct final_ls total_list_name;
	int total_file_num[5][100];
	int server_name[5][100];

	printf("Before while\n");
	for(size_t k = 1; k<=4; k++)
	{
		size_t j = 0;
		if(ls[k].valid == false)
		{
			printf("Server %d LS file not available\n",k );
			continue;
		}
		int ind =0;
		while(strcmp(ls[k].elements[j],"NULL")!=0)
		{
			 printf("Inside while %s\n",ls[k].elements[j]);
			 
			 char* file = malloc(100);
			 char* num = malloc(100);
			 bzero(num,sizeof(num));
			 bzero(file,sizeof(file));
			 
			 sscanf(ls[k].elements[j],"%s %s",file,num);
			 //ind = ((k-1)*100) + j;
			 strcpy(total_list_name.details[k*100 +j],file);
			 total_file_num[k][j] = atoi(num);
			 server_name[k][j] = k;
			 
			 printf(" copy %s %d %d\n",total_list_name.details[k*100+j],total_file_num[k][j],server_name[k][j] );
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
		//ind = ((q-1)*100)+p;
		//printf("%d %d\n",q,p );
		
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
			printf("%s %d %d %d\n",total_list_name.details[q*100+p],total_file_num[q][p],server_name[q][p],x );
			

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
	printf("Table Name %s %d\n",table.name[name_id],name_id );
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
			/*if(check_files.server_num[y] == check_files.server_num[z])
			{
				printf("skip\n");
				
			}*/

			printf("Fild Name - %s File Num - %d Server Num - %d\n",check_files.details[z],check_files.file_num[z],check_files.server_num[z] );

			switch(check_files.file_num[z])
			{
				case 1:
				{
					/*if(table.file1[name_id] > 0)
					{
						printf("Do nothing\n");
					}
					else*/
					{
						table.file1[name_id] = check_files.file_num[z];
						table.server1[name_id] = check_files.server_num[z];

					}
					break;		
				}
				case 2:
				{
					/*if(table.file2[name_id] > 0)
					{
						printf("Do nothing\n");
					}
					else*/
					{
						table.file2[name_id] = check_files.file_num[z];
						table.server2[name_id] = check_files.server_num[z];
					}
					break;
				}
				case 3:
				{
					/*if(table.file3[name_id] > 0)
					{
						printf("Do nothing\n");
					}
					else*/
					{
						table.file3[name_id] = check_files.file_num[z];
						table.server3[name_id] = check_files.server_num[z];
					}
					break;		
				}
				case 4:
				{
					/*if(table.file4[name_id] > 0)
					{
						printf("Do nothing\n");
					}
					else*/
					{
						table.file4[name_id] = check_files.file_num[z];
						table.server4[name_id] = check_files.server_num[z];
					}
					break;		
				}
			}


			//printf("Match Occured %s %d %d\n",check_files.details[z],check_files.file_num[z],check_files.server_num[z]);
			printf("%s %d %d %d %d %d %d %d %d\n",table.name[name_id],table.file1[name_id],table.file2[name_id],table.file3[name_id],table.file4[name_id],table.server1[name_id],table.server2[name_id],table.server3[name_id],table.server4[name_id]);
		}

			
	}
	name_id++;

	if(check_files.file_num[y] == 0)
		{
			new_flag = true;
			break;
		}
	
}


for(int b = 0;b<100;b++)
{
	printf("%s %d %d %d %d %d %d %d %d\n",table.name[b],table.file1[b],table.file2[b],table.file3[b],table.file4[b],table.server1[b],table.server2[b],table.server3[b],table.server4[b]);
}


}