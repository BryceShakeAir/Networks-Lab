#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>

#include <signal.h>
#include <unistd.h> 
#include <netinet/in.h>
#include <errno.h>
char name[100];

void recvFile(int sd)
{
	
	strcat(name,".txt");
	FILE *fp = fopen(name,"w+");
	char msg[200];
	while( read(sd,msg,200) > 0 ){
  		fprintf(fp,"%s",msg);
  		printf("%s\n",msg);
	}
	
	fclose(fp);
	return;
}
#define MAX_MSG_LENGTH 100

void send_cmd(int sock, int pid) {
	
	char str[MAX_MSG_LENGTH] = {0};
	while(1){

		fgets(str, MAX_MSG_LENGTH, stdin);
		if(send(sock, str, strlen(str)+1, 0) < 0) perror("send");
		if(strcmp(str,"Bye")==0)
			break;
		
	}
		
	kill(pid,SIGKILL);
	return;
}

void receive(int sock) {
	
	while(1)
	{	
		char buf[MAX_MSG_LENGTH] = {0};
		int filled = 0;	
		while(filled = recv(sock, buf, MAX_MSG_LENGTH-1, 0)) {
			if(strcmp(buf,"send")==0)
			{
					
				recvFile(sock);			
				printf("Hooray file recieved\n");
				fflush(stdout);
				break;
			}
			buf[filled] = '\0';
			printf("%s", buf);
			fflush(stdout);		
		}
		
	}
		
	printf("Server disconnected.\n");
}

int main()
{
  
  printf("Please specify name of text file you might recieve\n");
  scanf("%s",name);
  //Create a socket
  int network_socket = socket(AF_INET,SOCK_STREAM,0);

  //Specifying address
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(9034);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  //Connect to the server

  if(connect(network_socket,(struct sockaddr *)&server_addr,sizeof(server_addr) )==-1)
    printf("Cant Connect\n");
  else
  {
    int pid;	
	if(pid = fork()) send_cmd(network_socket, pid);
	else receive(network_socket);
  
  }

  return 0;
}
