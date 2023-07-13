// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
	
#define PORT	 8080
#define MAXLINE 1024
#define MSG_CONFIRM 0 // using macOS for development so need to add this, a linux environment shouldn't worry about this
// Driver code
int main() {
	// claim the crucial variable
	int sockfd;// a linux file descriptor
	char buffer[MAXLINE];// the container for the incomming message
	struct sockaddr_in servaddr, cliaddr; // structure for the address information
		
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	// initial the address info
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	// servaddr.sin_addr.s_addr = INADDR_ANY;// any address
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");// specific address
	servaddr.sin_port = htons(PORT);
		
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	int len, n;
	
	len = sizeof(cliaddr); 
	// loop for listening the incomming message
	while(1){
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
        buffer[n] = '\0';// add a null character at the end as the indicator of the end of string
        printf("Client Message: %s\n", buffer);
		//echo back the message to the client
        sendto(sockfd, (const char *)buffer, strlen(buffer),
            MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
                len);
        printf("Massage echo.\n");
    }	
	return 0;
}
