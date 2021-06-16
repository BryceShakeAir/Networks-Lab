#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<errno.h>

#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<time.h>
#include<sys/time.h>
#include<signal.h>
#include<math.h>

volatile int timeup =0;

static void sigalrmHandler(int sig)
{
	timeup = 1;
}


typedef struct msgFormat{

	int type;//0-ACK, 1-DATA
	int seqNo;
	char buff[1001];

} msg;

void randstring(msg *s1,size_t length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";        
    char *randomString = NULL;

    if (length) {
        randomString = malloc(sizeof(char) * (length +1));

        if (randomString) {            
            for (int n = 0;n < length;n++) {            
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    strcpy(s1->buff,randomString);
    return;
}


int main()
{
	
	int si =100,flag=0,sockFd;
	
	msg buff1[si], rcvBuff;
	for(int i=0;i<si;i++){

		buff1[i].type = 1;
		buff1[i].seqNo = i;
		randstring(&buff1[i],pow(10,i%4));
	}


	if( ( sockFd = socket(AF_INET,SOCK_DGRAM,0))<0)
		perror("Invalid socket");

	//Making dest addr
	struct sockaddr_in hostAddr,destAddr;
	destAddr.sin_port = htons(9002);
	destAddr.sin_family = AF_INET;
	destAddr.sin_addr.s_addr = INADDR_ANY;
	hostAddr.sin_port = htons(9003);
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_addr.s_addr = INADDR_ANY;

	bind(sockFd,(struct sockaddr *)&hostAddr,sizeof(hostAddr));

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigalrmHandler;
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        perror("sigaction");

    
	struct itimerval itv,rtv;
	itv.it_value.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_interval.tv_sec = 0;
	itv.it_value.tv_usec = 0;


	for(int i=0;i<si;i++)
	{

		//printf("Hi\n");
		itv.it_value.tv_sec = 1;
		if( setitimer(ITIMER_REAL,&itv,NULL)<0)
			perror("itimer Failed");

		printf("Sending data %d \n", i );
		if(
			sendto(sockFd,&buff1[i],sizeof(buff1[i]),0,
			(struct sockaddr *)&destAddr,sizeof(destAddr)
			)
			< 0 
		){

			perror("sendto");
		}


		printf("Waiting for Ack\n");
		//Waiting to rcv Ack 
		//if rcvd flag bit is set to 1
		//otherwise break if timeout
		while(1)
		{
			// a NON blocking rcv from
			if(recvfrom(sockFd,&rcvBuff,sizeof(rcvBuff),MSG_DONTWAIT,NULL,NULL)>0 
				&& (rcvBuff.type==0) && rcvBuff.seqNo== i )
			{
				flag = 1;
				if( getitimer(ITIMER_REAL,&rtv)<0)
					perror("getitimer");
			
				//stop timer int
				itv.it_value.tv_sec = 1;
				if(setitimer(ITIMER_REAL,&itv,0)<0)
					perror("setitimer 2nd");
				break;
			}
			if(timeup)
			{
				i--;
				timeup = 0;
				break;
			}
		}

		if(flag){
			printf("ACK rcvd for frame %d\n",i);
			long  rtt = 1000000 - (rtv.it_value.tv_usec);
			printf("rtt is %ld micro seconds\n", rtt );
			flag = 0;
		}
		else
			printf("ACK not rcvd for frame %d\n",i+1);

	}

	return 0;
}