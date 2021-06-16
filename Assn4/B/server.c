#include<stdio.h>
#include<stdlib.h>

#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>

typedef struct msgFormat{

	int type;//0-ACK, 1-DATA
	int seqNo;
	char buff[10001];

} msg;



int main()
{
	int si =100,len1;
	int sockFd = socket(AF_INET,SOCK_DGRAM,0);

	struct sockaddr_in hostAddr,destAddr;
	hostAddr.sin_port = htons(9002);
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_addr.s_addr = INADDR_ANY;
	destAddr.sin_port = htons(9003);
	destAddr.sin_family = AF_INET;
	destAddr.sin_addr.s_addr = INADDR_ANY;

	bind(sockFd,(struct sockaddr*) &hostAddr ,sizeof(hostAddr));

	msg buff1;

	for(int i=0;i<si;i++)
	{
		//Waiting for the msg
		printf("Waiting for new msg ...\n");
		recvfrom(sockFd,&buff1,sizeof(buff1),0,NULL,NULL);
		
		//Check if it is of correct type and sequence
		if(buff1.type==1&& buff1.seqNo==i)
		{
			printf("Msg %d rcvd\n",i);
			printf("'%s'\n",buff1.buff);
		}
		else{
			i--;
			printf("Duplicate Msg %d \n Discarding .....\n",buff1.seqNo);
		}

		//Return ACK of same seq;
		buff1.type = 0;
		printf("Sending Ack\n");
		sendto(sockFd,&buff1,sizeof(buff1),0,(struct sockaddr *)&destAddr,sizeof(destAddr));
	}
	return 0;
}