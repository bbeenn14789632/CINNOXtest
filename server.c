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
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr, cliaddr;
		
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
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
	while(1){
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
        buffer[n] = '\0';
        printf("Client Message: %s\n", buffer);
        sendto(sockfd, (const char *)buffer, strlen(buffer),
            MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
                len);
        printf("Massage echo.\n");
    }	
	return 0;
}
