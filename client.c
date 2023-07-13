// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <math.h>
#define PORT	 8080
#define MAXLINE 1024
#define MSG_CONFIRM 0 // using macOS for development so need to add this, a linux environment shouldn't worry about this
#define min(a,b)             \ 
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})


int main() {
    int maxTry =10;
    float base = 0.5;
    int multiplier = 2;
    int maxInterval = 8;
	int sockfd;
	char buffer[MAXLINE];
	char client_message[2000];
	struct sockaddr_in	 servaddr;
    
	printf("Enter message: ");
    scanf("%s",client_message);
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv)<0){
        perror("error");
        exit(EXIT_FAILURE);
    }
    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)<0){
        perror("error");
        exit(EXIT_FAILURE);
    };
	memset(&servaddr, 0, sizeof(servaddr));	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	// servaddr.sin_addr.s_addr = INADDR_ANY;// to any address
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");// to specific address
	int n, len;
	int try=1;
	sendto(sockfd, (const char *)client_message, strlen(client_message),
		MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
    printf("Message sent\n");
	while((n = recvfrom(sockfd, (char *)buffer, MAXLINE,MSG_WAITALL, (struct sockaddr *) &servaddr,&len))<0){
        if(try>maxTry){
            printf("Max retry is reached, stop trying\n");
            return 1;
        }
        
        sendto(sockfd, (const char *)client_message, strlen(client_message),MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
        if(tv.tv_sec<8) tv.tv_sec = (int)min(maxInterval,base*pow(multiplier,try));// the interval would be the min between maxInterval and the exponetial backoff
        tv.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        printf("response timeout,retrying...... %ld secs\n",tv.tv_sec);
        try ++;
    };
    // n = recvfrom(sockfd, (char *)buffer, MAXLINE,MSG_WAITALL, (struct sockaddr *) &servaddr,&len);
	buffer[n] = '\0';
	printf("Server echo: %s\n", buffer);
	close(sockfd);
	return 0;
}
