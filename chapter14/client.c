#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <errno.h>

int main(int argc, char** argv)
{
	int sockfd;
	struct sockaddr_in cliaddr;
	char* ip = "127.0.0.1";

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("socket error!\n");
		exit(0);
	}

	bzero(&cliaddr, sizeof(cliaddr));

	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(5555);
	inet_pton(AF_INET, ip, (void*)&(cliaddr.sin_addr));

	sendto(sockfd, "HelloWorld!", 12, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	while(1)
	{
		char buf[20] = {'\0'};
		int res = 0;
		res = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
		if(res < 0)
		{
			if(errno == EWOULDBLOCK)
			{
				printf("timeout!\n");
			}
			else
			{
				printf("recvfrom error!\n");
			}
		}
		printf("%s\n", buf);
		printf("test timeout!\n");
	}
	return 0;
}
