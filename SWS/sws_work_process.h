
#ifndef __SWS_WORK_PROCESS_H__
#define __SWS_WORK_PROCESS_H__

#include <netinet/in.h>

extern int sws_setnonblocking(int listen_fd);

extern int sws_work_process(int *listen_fd);

#endif /*__SWS_WORK_PROCESS_H__*/
