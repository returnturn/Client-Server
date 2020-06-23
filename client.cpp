// From: www.thegeekstuff.com/2011/12/c-socket-programming
// Note that port# 5000 is hard-coded into this implementation

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <time.h>
#include <string>
#include <iostream>

using namespace std;

int TransSave = 0;

void Trans( int n ) {
    long i, j;

    // Use CPU cycles 
    j = 0;
    for( i = 0; i < n * 100000; i++ ) {
        j += i ^ (i+1) % (i+n);
    }
    TransSave += j;
    TransSave &= 0xff;
}

void Sleep( int n ) {
    struct timespec sleep;

    // Make sure pass a valid nanosecond time to nanosleep
    if( n <= 0 || n >= 100 ) {
        n = 1;
    }

    // Sleep for less than one second
    sleep.tv_sec  = 0;
        sleep.tv_nsec = n * 10000000 + TransSave;
    if( nanosleep( &sleep, NULL ) < 0 ) {
        perror ("NanoSleep" );
    }
}

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0, l=1024;
    char recvBuff[1024],sendBuff[1024];
    struct sockaddr_in serv_addr; 

    if(argc != 3)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1])); 
    /*char *s=argv[2];
    const char *tmp = s;
    serv_addr.sin_addr.s_addr = inet_addr(s);*/

    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    char hostname[l];
    hostname[100] = '\0';
    gethostname(hostname, 1023);

    char Pid[10];
    sprintf(Pid,"%d",getpid());

    char fileName[100];
    sprintf(fileName,"%s.%s",hostname,Pid);

    //TODO: change filename!!!!
    FILE *f = fopen("fileName", "w");
    fprintf(f, " In        %s Output\n", fileName);
    fprintf(f, "     Using port %s\n", argv[1]);
    fprintf(f, "     Using server address %s\n", argv[2]);
    fprintf(f, "     Host %s\n", fileName);

    int count=0;
    string input;
    while(getline(cin, input)){
        memset(sendBuff, '\0', sizeof(sendBuff));
        strcpy(sendBuff,input.c_str());
        int num=stoi(input.substr(1));
        send(sockfd, sendBuff, strlen(sendBuff), 0);

        if(input[0]=='S'){
            fprintf(f, "%-5sSleep %d units\n", sendBuff, num);
            Sleep(num);
        }
        else{
            ++count;
            time_t now;
            time(&now);
            struct timeval tv;
            gettimeofday(&tv, NULL);
            int ps=(int) (tv.tv_usec / 1000);
            string sps = to_string(ps);

            fprintf(f, "%-5s%ld.%s: Send (%c%3d)\n", sendBuff, now, sps.substr(0,2).c_str(),sendBuff[0],num);

            memset(recvBuff, '\0', sizeof(recvBuff));
            recv(sockfd, recvBuff, sizeof(recvBuff),0);

            time_t now1;
            time(&now1);
            struct timeval tv1;
            gettimeofday(&tv1, NULL);
            int ps1=(int) (tv1.tv_usec / 1000);
            string sps1 = to_string(ps1);
            int num1=stoi(recvBuff);

            fprintf(f, "     %ld.%s: Recv (D%3d)\n", now1, sps1.substr(0,2).c_str(),num1);
        }

        //sleep(1);
    }
    fprintf(f, "     Sent %d transactions\n", count);
    fclose(f);

    if(n < 0)
    {
        printf("\n Read error \n");
    } 

    return 0;
}
