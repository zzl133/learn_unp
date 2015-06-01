
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/epoll.h>
#include <string.h>
#include "sws_work_process.h"
#include "sws_http_request.h"
#include "sws.h"


#define BUF_SIZE 10000
#define MAXCLIENT 1000

sws_session_t sws_session_s[MAXCLIENT] = 
{
	{NULL, NULL}
};

int sws_setnonblocking(int listen_fd)
{
	int opts;

	opts = fcntl(listen_fd, F_GETFL);
	if(opts < 0)
	{   
		perror("fcntl(F_GETFL)");

		return -1; 
	}   

opts = opts | O_NONBLOCK;

	if(fcntl(listen_fd, F_SETFL, opts) < 0)
	{   
		perror("fcntl(F_SETFL)");

		return -1; 
	}   

	return 1;

}

int sws_work_process(int *listen_fd)
{
	struct sockaddr_in client_addr;
	struct epoll_event ev, events[MAXCLIENT];

	socklen_t sin_size;

	char buf[BUF_SIZE];

	int epfd, conn_fd, client_fd;
	int sws_fd_count;
	int i, index;

	sws_setnonblocking(*listen_fd);

	sin_size = sizeof(client_addr);

	epfd = epoll_create(1024);

	ev.data.fd = *listen_fd;
	ev.events = EPOLLIN|EPOLLET;

	epoll_ctl(epfd, EPOLL_CTL_ADD, *listen_fd, &ev);

	while(1) 
	{
		sws_fd_count = epoll_wait(epfd, events, MAXCLIENT, -1);

		for (i = 0; i < sws_fd_count; ++i)
		{
			if(events[i].data.fd == *listen_fd)
			{
				conn_fd = accept(*listen_fd, (struct sockaddr*)&client_addr, &sin_size);

				while(conn_fd > 0)
				{
					ev.data.fd = conn_fd;
					ev.events = EPOLLIN|EPOLLET;

					epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &ev);

					conn_fd = accept(*listen_fd, (struct sockaddr*)&client_addr, \
						   	&sin_size);
				}
			}
			else if (events[i].events & EPOLLIN) 
			{
				client_fd = events[i].data.fd;

#if 0
				//read(client_fd, buf, sizeof(buf));
				//printf("%s\n", buf);
#endif

				/*deal the request*/
				index = 0;

				char *message_buf = (char*)malloc(BUF_SIZE);

				read(client_fd, buf, sizeof(buf));
				memcpy(message_buf, buf, strlen(buf));
				/*printf("%s\n", message_buf);*/


				while(index < MAXCLIENT)
				{
					if (sws_session_s[index].fd == NULL && sws_session_s[index].buf == NULL)
					{
						sws_session_s[index].fd = &client_fd;
						sws_session_s[index].buf = message_buf; 

						break;
					}

					index++;

					if (index > MAXCLIENT)
					{
						index = 0;
					}
				}

				ev.data.fd = client_fd;
				ev.events = EPOLLOUT|EPOLLET;

				epoll_ctl(epfd, EPOLL_CTL_MOD, client_fd, &ev);

			}
			else if (events[i].events & EPOLLOUT) 
			{
				client_fd = events[i].data.fd;
				
                /*deal the response*/
				index = 0;

				while(index < MAXCLIENT)
				{
					if(*(sws_session_s[index].fd) == client_fd)
					{
						sws_request_to_user(&sws_session_s[index]);

						break;
					}

					index++;
				}

				ev.data.fd = client_fd;

				epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, &ev);

				close(client_fd);

				free(sws_session_s[index].buf);

				sws_session_s[index].buf = NULL;
				sws_session_s[index].fd = NULL;
			}
		}
	}

	return 0;
}

