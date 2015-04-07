/*
 * ====================================================
 *
 *       Filename:  sws.c
 *
 *    Description:  a simple webservice
 *
 *        Version:  1.0
 *        Created:  2015年04月07日 10时09分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tianer (tjl), q2719833@126.com
 *   Organization:  
 *
 * ====================================================
 */


#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

/* 
 * ===  FUNCTION  ==================================
 *         Name:  main
 *  Description:  
 * =================================================
 */
int main ( int argc, char *argv[] )
{
	int sockfd, clientfd;	
	struct sockaddr_in seraddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		return -1;

	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	seraddr.sin_port = htons(8080);
	int res = bind(sockfd, (struct sockaddr*)&seraddr, sizeof(seraddr));
	if(res < 0)
		return -1;
	listen(sockfd, 20);
	socklen_t len = sizeof(cliaddr);
	char msg[1024] = {'\0'};

	while(1)
	{
		printf("in accept\n");
		memset(msg, 0, sizeof(msg));
		clientfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len);
		printf("after accept\n");
		recv(clientfd, msg, sizeof(msg), 0);
		printf("%s\n", msg);
		int fp = open("./index.html", O_RDONLY, 0666);
		char buf[1024] = {'\0'};
		char bufall[1100] = {'\0'}; 
		read(fp, buf, sizeof(buf));
		strcat(bufall, "HTTP/1.0 200 OK\r\n");
		strcat(bufall, "Content-type:text/html\r\n\r\n");
		strcat(bufall, buf);
		send(clientfd, bufall, strlen(bufall), 0);
		close(fp);
		close(clientfd);
	}

	return EXIT_SUCCESS;
}
