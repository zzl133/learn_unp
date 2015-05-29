#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/epoll.h>
#include "sws_work_process.h"


#define BUF_SIZE 10000
#define MAXCLIENT 1000

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

int sws_work_process(int *listen_fd, struct sockaddr_in *server_addr)
{
	struct sockaddr_in client_addr;
	struct epoll_event ev, events[MAXCLIENT];

	socklen_t sin_size;
	char buf[BUF_SIZE];

	int epfd, conn_fd, client_fd;
	int sws_fd_count;
	int i;


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
				//request
				client_fd = events[i].data.fd;

				sws_request_to_user(&client_fd);
			}
			else if (events[i].events & EPOLLOUT) 
			{
				//out
			}
		}
	}

	return 0;
}

