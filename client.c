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
#define MAXLINE 1024
#define MSG_CONFIRM 0 // using macOS for development so need to add this, a linux environment shouldn't worry about this
#define min(a,b)             \ 
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})


int main() {
    // the variable for the exponetial backoff
    int maxTry =10;
    float base = 0.5;
    int multiplier = 2;
    int maxInterval = 8;
    int PORT;
    char addrstr[15];// 192.168.0.1
    // the variable for the socket
	int sockfd; // a linux file descriptor
	char buffer[MAXLINE]; // incoming massage buffer
	char client_message[2000]; // the message want to send
	struct sockaddr_in	 servaddr; // the server address info structure
    // get the message to send
    printf("Target ip: ");
    scanf("%s",addrstr);
    printf("Target port: ");
    scanf("%d",&PORT);
	printf("Enter message: ");
    scanf("%s",client_message);
    printf("Max Retry(enter 0 if not needed): ");
    scanf("%d",&maxTry);
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
    // config the time interval of request sending and response receiving  
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
    // init the server address info
	memset(&servaddr, 0, sizeof(servaddr));	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	// servaddr.sin_addr.s_addr = INADDR_ANY;// to any address
    servaddr.sin_addr.s_addr = inet_addr(addrstr);// to specific address
	int n, len;
	int try=1;
    // send the message
	sendto(sockfd, (const char *)client_message, strlen(client_message),
		MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
    printf("Message sent\n");
    // check if the response is normal if not retry 
	while((n = recvfrom(sockfd, (char *)buffer, MAXLINE,MSG_WAITALL, (struct sockaddr *) &servaddr,&len))<0){
        // ignore the case of other error, treated every thing as timeout
        // the max try limitation if exceed exit with 1
        if(try>maxTry){
            if(!maxTry) {printf("Connection timeout, no retry attemp.\n");return 1;}
            printf("Max retry is reached, stop trying\n");
            return 1;
        }
        // resend the message
        sendto(sockfd, (const char *)client_message, strlen(client_message),MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
        if(tv.tv_sec<8) tv.tv_sec = (int)min(maxInterval,base*pow(multiplier,try));// the interval would be the min between maxInterval and the exponetial backoff
        tv.tv_usec = 0;
        //reconfigure the time out interval
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        printf("response timeout,retrying...... %ld secs\n",tv.tv_sec);
        try ++;
    };
    // print the echo message if everything works fine
	buffer[n] = '\0';
	printf("Server echo: %s\n", buffer);
	close(sockfd);
	return 0;
}
