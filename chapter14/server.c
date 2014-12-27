#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>

int main(int argc, char** argv)
{
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;
	char *ip = "127.0.0.1";

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("socket error!\n");
		exit(0);
	}

	bzero(&servaddr, sizeof(servaddr));
	bzero(&cliaddr, sizeof(cliaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5555);
	
	inet_pton(AF_INET, ip, (void*)&(servaddr.sin_addr));


	int res = 0;
	if((res = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0)
	{
		printf("bind failed!\n");
		exit(0);
	}

	char buf[20] = {'\0'};
	socklen_t len = sizeof(cliaddr);
	recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&cliaddr, &len);
		
	printf("%s\n", buf);

	sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&cliaddr, len);

	while(1);

	close(sockfd);

	return 0;
}
