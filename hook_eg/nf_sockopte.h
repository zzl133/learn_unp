/*
 * file:nf_sockopte.h 
 * sockopt extern header file
 *
 */

#ifndef _NF_SOCKOPTE_H__
#define _NF_SOCKOPTE_H__

/*cmd 命令定义
 *SOE_BANDIP:IP地址发送禁止命令
 *SOE_BANDPORT:端口禁止命令
 *SOE_BANDPING:ping禁止
 */
#define SOE_BANDIP   0x6001
#define SOE_BANDPORT 0x6002
#define SOE_BANDPING 0x6003

/*禁止端口结构*/
typedef struct nf_bandport
{
	/*band protocol, TCP?UDP*/
	unsigned short protocol;
	
	/*band port*/
	unsigned short port;
};

/*与用户交互的数据结构*/
typedef struct band_status
{
	/*IP发送禁止，IP地址，当为0时，未设置*/
	unsigned int bind_ip;

	/*端口禁止,当协议和端口均未0时，未设置*/
	nf_bandport band_port;

	/*是否允许ping回显响应，为0时响应，为1时禁止*/
	unsigned char band_ping;
}band status;;

#endif //_NF_SOCKOPTE_H__
