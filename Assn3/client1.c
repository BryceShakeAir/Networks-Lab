#include<stdio.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>

int main()
{
	int clientFd = socket(AF_INET,SOCK_DGRAM,0);

	struct sockaddr_in clAddr,serAddr;
	clAddr.sin_family = AF_INET;
	clAddr.sin_port = htons(9002);
	clAddr.sin_addr.s_addr = INADDR_ANY;

	int x = 10, serLen = sizeof(serAddr);
	char msg[100] = "I am sending u ";

	sendto(clientFd,msg,sizeof(msg),0,(struct sockaddr *) &clAddr,sizeof(clAddr));
	sendto(clientFd,&x,sizeof(x),0,(struct sockaddr *) &clAddr,sizeof(clAddr));

	recvfrom(clientFd,msg,sizeof(msg),0,(struct sockaddr *) &serAddr,&serLen);
	printf("%s\n",msg);

	return 0;
}