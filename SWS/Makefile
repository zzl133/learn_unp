OBJS = sws.o sws_work_process.o sws_http_request.o

CC = gcc

all: sws_server

sws_server: $(OBJS)
	$(CC) -o ./sws_server $(OBJS)

clean:
	rm -rf *.o
	rm -rf sws_server
