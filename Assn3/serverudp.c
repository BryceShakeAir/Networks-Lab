#include<stdio.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>

int main()
{
	int serverFd = socket(AF_INET,SOCK_DGRAM,0);

	struct sockaddr_in serverAddr,clientAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(9002);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	bind(serverFd,(struct sockaddr *) &serverAddr,sizeof(serverAddr));

	char msg[100000],msg2[100000];

	int x,len1 = sizeof(clientAddr);

	recvfrom(serverFd,msg,sizeof(msg),0,
		(struct sockaddr *) &clientAddr, &len1 );
	recvfrom(serverFd,&x,sizeof(x),0,
		(struct sockaddr *) &clientAddr, &len1 );

	printf("recieved : \n");
	printf("%d\n",x);

	sendto(serverFd,msg2,sizeof(msg2),0,
		(struct sockaddr *) &clientAddr,len1);


}