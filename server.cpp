// From: www.thegeekstuff.com/2011/12/c-socket-programming
// Note that port# 5000 is hard-coded into this implementation

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <iostream>
#include <thread>

int TransSave = 0,D=1;

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


// Sleep simulates pauses between transactions
// Parameter given expresses wait time in hundreds of a nanosecond.

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

void socketThread(int connfd){
do{  
            char sendBuff[1025], recvBuff[1025];
            recv(connfd, recvBuff, sizeof(recvBuff),0);
            char temp[1025];
            memcpy(temp, &recvBuff[1], sizeof(*recvBuff));
            int ia = atoi(temp);
            if(recvBuff[0]=='T'){
                Trans(ia);
                memset(sendBuff, '\0', sizeof(sendBuff));
                char buf[10];
                sprintf(buf,"%d",D++);
                const char* p = buf;
                strcat(sendBuff, p);
                send(connfd, sendBuff, strlen(sendBuff), 0);
            }

            printf( "%s\n", recvBuff);
        }while(1);
}

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1])); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 
    std::thread th[10];
    int x=0;
    while(1)
    {

        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        std::thread a(socketThread,connfd);
        a.join();
        close(connfd);
        sleep(1);
     }
}
