#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char** argv)
{
#if 0
	if(argc != 2)	
	{
		printf("useage: %s hostname.\n", argv[1]);
		exit(0);
	}
#endif
	char name[50];

	struct addrinfo hints, *res, *curr;
	char ip[20];

	gethostname(name, sizeof(name));

	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(name, NULL, &hints, &res);

	for(curr = res; curr != NULL; curr = curr->ai_next)
	{
		inet_ntop(AF_INET, &(((struct sockaddr_in *)(curr->ai_addr))->sin_addr), ip, sizeof(ip));
		printf("%s\n", ip);
	}

	freeaddrinfo(res);
	return 0;
}
