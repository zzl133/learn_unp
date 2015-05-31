
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "sws_http_request.h"


void sws_request_to_user(int *client_fd)
{
	char sws_buf[1024];

	bzero(sws_buf, sizeof(sws_buf));

	strcat(sws_buf, "HTTP/1.1 200 OK\r\n");
	strcat(sws_buf, "Server: sws webServer\r\n");
	strcat(sws_buf, "Content-type:text/html");
	strcat(sws_buf, "\r\n\r\n");
	strcat(sws_buf, "HelloWorld!");
	/*printf("sws_buf = %s\n", sws_buf);*/

	write(*client_fd, sws_buf, strlen(sws_buf));

}

