
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "sws_http_request.h"
#include "sws.h"

#define URI_SIZE        128

char *sws_get_uri(char *request_header, char *uri_buf)
{
    int index = 0;

	bzero(uri_buf, 0);

	while((request_header[index] != '/') && (request_header[index] != '\0'))
	{
		index++;
	}

	int uri_start = index;

	while((request_header[index] != '\0') && \
		(request_header[index] != ' ') && \
		(index - uri_start < URI_SIZE))
	{
		index++;
	}

	if(index > URI_SIZE) 
	{
		printf("uri too long!\n");
		return NULL;
	}

	if ((request_header[index - 1] == '/') && request_header[index] == ' ')
	{
	    strcpy(uri_buf, "index.html");	
		return uri_buf;
	}

	strncpy(uri_buf, request_header + uri_start + 1, index - uri_start - 1);

	return uri_buf;
}

int sws_get_file_buf(char *sws_uri_buf, char *sws_file_buf)
{
	struct stat st;
	unsigned long st_size;
	int count;

	int fd = open(sws_uri_buf, O_RDONLY);
	if(fd == -1)
	{
		perror("open()");
		return -1;
	}

	if(fstat(fd, &st) == -1)
	{
		perror("fstat");
		return -1;
	}

	st_size = st.st_size;
	if(st_size > FILE_MAX_SIZE)
	{
		fprintf(stderr, "the file %s is too large.\n", sws_uri_buf);
		return -1;
	}

	if((count = read(fd, sws_file_buf, FILE_MAX_SIZE)) == -1)
	{
		perror("read()");
	}

	return count;
}

char *sws_get_mime_type(char *sws_uri_buf)
{
	int type_len;
	int len = strlen(sws_uri_buf);
	int dot = len - 1;

	char *type_off;

	while( dot >= 0 && sws_uri_buf[dot] != '.')
	{
		dot--;
	}

	if(dot == 0)        
	{
		return NULL;
	}

	if(dot < 0)          
	{
		return "text/html";
	}

	dot++;

	type_len = len - dot;
	type_off = sws_uri_buf + dot;

	switch(type_len)
	{
		case 4:
			if(!strcmp(type_off, "html") || !strcmp(type_off, "HTML"))
			{
				return "text/html";
			}
			if(!strcmp(type_off, "jpeg") || !strcmp(type_off, "JPEG"))
			{
				return "image/jpeg";
			}
			break;
		case 3:
			if(!strcmp(type_off, "htm") || !strcmp(type_off, "HTM"))
			{
				return "text/html";
			}
			if(!strcmp(type_off, "css") || !strcmp(type_off, "CSS"))
			{
				return "text/css";
			}
			if(!strcmp(type_off, "png") || !strcmp(type_off, "PNG"))
			{
				return "image/png";
			}
			if(!strcmp(type_off, "jpg") || !strcmp(type_off, "JPG"))
			{
				return "image/jpeg";
			}
			if(!strcmp(type_off, "gif") || !strcmp(type_off, "GIF"))
			{
				return "image/gif";
			}
			if(!strcmp(type_off, "txt") || !strcmp(type_off, "TXT"))
			{
				return "text/plain";
			}
			break;
		case 2:
			if(!strcmp(type_off, "js") || !strcmp(type_off, "JS"))
			{
				return "text/javascript";
			}
			break;
		default:        
			return "NULL";
			break;
	}

	return NULL;
}

int 
sws_get_send_buf(char *sws_uri_buf, char *sws_send_buf, char *sws_file_buf, int count)
{
	char *mime_type = NULL;
	char *time_now;
	time_t time_s;
	char file_count[10] = {'\0'};
	int send_bytes;

	time_s = time(NULL);
	time_now = ctime(&time_s);
	
	mime_type = sws_get_mime_type(sws_uri_buf);

	sprintf(file_count, "%d", count);

	strcat(sws_send_buf, "HTTP/1.1 200 OK\r\n");
	strcat(sws_send_buf, "Server: sws webServer/Linux\r\n");
	strcat(sws_send_buf, "Date:");
	strcat(sws_send_buf, time_now);
	//strcat(sws_send_buf, "\r\n");
	strcat(sws_send_buf, "Content-Type:");
	strcat(sws_send_buf, mime_type);
	strcat(sws_send_buf, "\r\n");
	strcat(sws_send_buf, "Content-Length:");
	strcat(sws_send_buf, file_count);
	strcat(sws_send_buf, "\r\n");
	strcat(sws_send_buf, "\r\n");
	strcat(sws_send_buf, sws_file_buf);

	send_bytes = strlen(sws_send_buf);

	return send_bytes;
}

int 
sws_make_normal_message(char *sws_uri_buf, char *sws_send_buf, char *sws_file_buf)
{
	int read_count;	
	int send_bytes;

	read_count = sws_get_file_buf(sws_uri_buf, sws_file_buf);
	if(read_count == -1)
	{
		return -1;
	}

	send_bytes = sws_get_send_buf(sws_uri_buf, sws_send_buf, \
			sws_file_buf, read_count);
	
	return read_count;
}

int 
sws_make_error_message(char *sws_send_buf, int errorno)
{
	char *time_now;
	time_t time_s;
	int send_bytes;
	char data_len_s[10] = {'\0'};
	char *response_data = NULL;
	char *response_html_data = NULL;
	int data_len = 0;

	switch(errorno)
	{
		case FILE_NOT_FOUND:
			response_data = "HTTP/1.1 404 File Not Found\r\n";
			response_html_data = "<html><head></head><body> \
				404 File not found<br/> \
				Please check your url,and try it again!\
				</body></html>";

			data_len = strlen(response_html_data);

			time_s = time(NULL);
			time_now = ctime(&time_s);

			sprintf(data_len_s, "%d", data_len);

			strcat(sws_send_buf, response_data);
			strcat(sws_send_buf, "Server: sws webServer/Linux\r\n");
			strcat(sws_send_buf, "Date:");
			strcat(sws_send_buf, time_now);
			strcat(sws_send_buf, "Content-Type:text/html");
			strcat(sws_send_buf, "\r\n");
			strcat(sws_send_buf, "Content-Length:");
			strcat(sws_send_buf, data_len_s);
			strcat(sws_send_buf, "\r\n");
			strcat(sws_send_buf, "\r\n");
			strcat(sws_send_buf, response_html_data);

			break;

		default:
			break;

	}
	send_bytes = strlen(sws_send_buf);

	return send_bytes;


}

void sws_request_to_user(sws_session_t *sws_session)
{
	char sws_uri_buf[URI_SIZE + 1] = {'\0'};
	char sws_send_buf[SEND_BUFFER_SIZE + 1] = {'\0'};
	char sws_file_buf[FILE_MAX_SIZE + 1] = {'\0'};
	int  uri_status = 0;
	int  send_bytes = 0;

	if(sws_get_uri(sws_session->buf, sws_uri_buf) == NULL)
	{
		perror("uri too loong!");

		return;
	}

#if 0
	printf("sws_uri = %s\n", sws_uri_buf);
#endif

	if(access(sws_uri_buf, F_OK) == -1)
	{
		printf("file %s not fond\n", sws_uri_buf);

		uri_status = FILE_NOT_FOUND;
	}
	else if (access(sws_uri_buf, R_OK) == -1) 
	{
		printf("file %s can not read.\n", sws_uri_buf);

		uri_status = FILE_FORBIDEN;
	}
	else uri_status = FILE_OK;

	switch(uri_status)
	{
		case FILE_OK:
			send_bytes = sws_make_normal_message(sws_uri_buf, \
					sws_send_buf, sws_file_buf);
			break;

		case FILE_NOT_FOUND:
			send_bytes = sws_make_error_message(sws_send_buf, \
					FILE_NOT_FOUND);
			break;

		default:
			break;
	}

#if 0
	printf("sws_send_buf = %s\n", sws_send_buf);
#endif

	write(*(sws_session->fd), sws_send_buf, strlen(sws_send_buf));

#if 0
	char sws_buf[1024];

	bzero(sws_buf, sizeof(sws_buf));

	strcat(sws_buf, "HTTP/1.1 200 OK\r\n");
	strcat(sws_buf, "Server: sws webServer/Linux\r\n");
	strcat(sws_buf, "Content-type:text/html");
	strcat(sws_buf, "\r\n\r\n");
	strcat(sws_buf, "HelloWorld!");

	/*printf("sws_buf = %s\n", sws_buf);*/

	write(*(sws_session->fd), sws_buf, strlen(sws_buf));
#endif
}

