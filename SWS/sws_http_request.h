
#ifndef __SWS_HTTP_REQUEST_H__
#define __SWS_HTTP_REQUEST_H__

#include "sws.h"

#define    URI_SIZE               128
#define    FILE_OK                200
#define    FILE_FORBIDEN          403            
#define    FILE_NOT_FOUND         404            
#define    UNALLOW_METHOD         405            
#define    FILE_TOO_LARGE         413            
#define    URI_TOO_LONG           414            
#define    UNSUPPORT_MIME_TYPE    415
#define    UNSUPPORT_HTTP_VERSION 505
#define    FILE_MAX_SIZE          1048576        
#define    SEND_BUFFER_SIZE       1050000

extern void sws_request_to_user(sws_session_t *sws_session);

#endif /*__SWS_HTTP_REQUEST_H__*/
