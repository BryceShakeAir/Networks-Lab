#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <sys/time.h>
#include <time.h>
#include<signal.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX 80
#define PORT 8081
#define SA struct sockaddr
#define MAX_LINE 4096
#define LINSTENPORT 7788
#define SERVERPORT 8877
#define BUFFSIZE 4096
#define SA struct sockaddr
ssize_t total = 0;
ssize_t chunk = 0;
#define TIME_SLOT 0.1

volatile int timeup = 0;

static void sigalrmHandler(int sig)
{
    timeup = 1;
}

void recFile(int sockfd, FILE *fp)
{
    int n;
    int i;
    int count = 0;
    char buff[MAX_LINE] = {0};
    clock_t begin, end;
    begin = clock();
    double plots[MAX];

    struct itimerval itv;
    itv.it_value.tv_sec = 0 ;
    itv.it_interval.tv_usec = 1000;
    itv.it_interval.tv_sec = 0;
    itv.it_value.tv_usec = 1000;

    if(setitimer(ITIMER_REAL,&itv,NULL)<0){
        perror("itimer ");
        return;
    }

    while (1)
    {
        n = recv(sockfd, buff, MAX_LINE, 0);
        chunk += n;
        

        fwrite(buff, sizeof(char), n, fp);
        bzero(buff, MAX_LINE);
        if (n < BUFFSIZE)
        {
            total += n;
            break;
        }
        total += n;
        if (timeup)
        {
            plots[count++] = chunk;
            begin = clock();
            printf("%ld\n", chunk);
            chunk = 0;
            timeup = 0;

        }
    }

    //Disarming the timer
    itv.it_value.tv_sec = 0 ;
    itv.it_interval.tv_usec = 0;
    itv.it_interval.tv_sec = 0;
    itv.it_value.tv_usec = 0;

    if(setitimer(ITIMER_REAL,&itv,NULL)<0){
        perror("itimer ");
        return;
    }
    printf("im here : %f\n", plots[0]);
    // Plot function
    printf("%d\n",count);
    char *commandsForGNUPlot[] = {"set title \"RATE MB/1 millisec\"", "plot 'data.temp'"};
    FILE *temp = fopen("data.temp", "w");
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");

    for (i = 0; i < count; i++)
    {
        printf("%lf %lf \n", TIME_SLOT * i, plots[i] / 1000000);
        fprintf(temp, "%lf %lf \n", TIME_SLOT * i, plots[i] / 1000000);
    }
    fflush(temp);
    for (i = 0; i < 2; i++)
    {
        fprintf(gnuplotPipe, "%s \n", commandsForGNUPlot[i]);
    }
    fflush(gnuplotPipe);

    printf("im finished\n");
}

void func(int sockfd)
{
    char buff[MAX];
    int n;
    for (;;)
    {
        bzero(buff, sizeof(buff));
        printf("\tEnter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        if ((strncmp(buff, "video", 5)) == 0)
        {
            write(sockfd, buff, sizeof(buff));
            // printf("Recieving Video\n");
            char *filename = "output.txt";

            FILE *fp = fopen(filename, "wb");
            if (fp == NULL)
            {
                perror("Can't open file");
                exit(1);
            }

            printf("Start receive file: %s\n", filename);
            recFile(sockfd, fp);
            printf("Receive Success, NumBytes = %ld\n", total);
            fclose(fp);
            // break;
        }
        else if ((strncmp(buff, "exit", 4)) == 0)
        {
            write(sockfd, buff, sizeof(buff));
            printf("Client Exit...\n");
            break;
        }
        else
        {
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            printf("From Server : %s", buff);
        }
    }
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET,"127.0.0.1",(void *)&servaddr.sin_addr);
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigalrmHandler;
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        perror("sigaction");

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
}

