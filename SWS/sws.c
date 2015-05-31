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
#include <wait.h>
#include "sws_work_process.h"


int sws_init_socket(int *listen_fd, struct sockaddr_in *server_addr)
{
	*listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd < 0)
	{
		perror("socket");
		return -1;
	}

	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(8866);
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(*listen_fd, (struct sockaddr*)server_addr, sizeof(struct sockaddr_in)) < 0)
		return -1;

	listen(*listen_fd, 20);

	return 1;
}


/* 
 * ===  FUNCTION  ==================================
 *         Name:  main
 *  Description:  
 * =================================================
 */
int main ( void )
{
	int listen_fd;	
	int status;
	struct sockaddr_in server_addr;

    if( sws_init_socket(&listen_fd, &server_addr) < 0 )
		return -1;

	pid_t pid = fork();

	if(pid < 0)
	{
		return -1;
	}
	if(pid == 0)
	{
		sws_work_process(&listen_fd, &server_addr);

		exit(0);
	}
	else 
	{
		close(listen_fd);

		waitpid(pid, &status, WUNTRACED | WCONTINUED);

	}

	return 0;
}
