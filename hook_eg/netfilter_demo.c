#include <linux/netfilter_ipv4.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/ip.h>  //IP头
#include <net/tcp.h>   //TCP头
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include "nf_sockopte.h"

/*版权*/
MODULE_LICENSE("GPL");
/*NF初始化状态宏*/
#define NF_SUCCESS 0;
#define NF_FAILUER 1;
/*初始化绑定状态*/
band_status b_status;
/*快速绑定操作宏*/
/*判断是否禁止TCP端口*/
#define IS_BANDPORT_TCP(status) (status.band_port.port != 0 && status.band_port.protocol == IPPROTO_TCP)
/*判断是否禁止UDP端口*/
#define IS_BANDPORT_UDP(status) (status.band_port.port != 0 && status.band_port.protocol == IPPROTO_UDP)
/*判断是否禁止ping*/
#define IS_BANDPING(status) (status.band_ping)
/*判断是否禁止IP*/
#define IS_BANDIP(status) (status.band_ip)
/*nf sock 选项扩展操作*/
static int nf_sockopt_set(struct sock *sock,
						  int cmd,
						  void __user *user,
						  unsigned int len)
{
	int ret = 0;
	struct band_status status;

	/*权限检查*/
	if(!capable(CAP_NET_ADMIN)) //没有足够权限
	{
		ret = -EPERM;
		goto ERROR:
	}

	/*从用户空间复制数据*/
	ret = copy_from_user(&status, user, len);
	if(ret != 0)
	{
		ret = -EINVAL;
		goto ERROR;
	}

	/*命令类型*/
	switch(cmd)
	{
		case SOE_BANDIP: //禁止IP协议
			if(IS_BANDIP(status))
				b_status.band_ip = status.band_ip;
			else
				b_status.band_ip = 0;

			break;
		case SOE_BANDPORT:
			if(IS_BANDPORT_TCP(status))
			{
				b_status.band_port.protocol = IPPROTO_TCP;
				b_status.band_port.port = status.band_port.port;
			}
			else if(IS_BANDPORT_UDP(status))
			{
				b_status.band_port.protocol = IPPROTO_UDP;
				b_status.band_port.port = status.band_port.port;
			}
			else
			{
				b_status.band_port.protocol = 0;
				b_status.band_port.port = 0;
			}
			break;
		case SOE_BANDPING:
			if(IS_BANDPING(status))
			{
				b_status.band_ping = 1;
			}
			else
			{
				b_status.band_ping = 0;
			}
			break;
		default:
			ret = -EINVAL;
			break;

	}
ERROR:
	return ret;
}

/*nf sock 操作扩展命令操作*/
static int nf_sockopt_get(struct sock *sock,
		                  cmd,
						  void __user *user,
						  unsigned int len)
{
	int ret = 0;

	/*权限检查*/
	if(!capable(CAP_NET_ADMIN)) //没有足够权限
	{
		ret = -EPERM;
		goto ERROR:
	}

	/*将数据从内核空间复制到用户空间*/
	switch(cmd)
	{
		case SOE_BANDIP:
		case SOE_BANDPORT:
		case SOE_BANDPING:
			/*复制数据*/
			ret = copy_to_user(user, &b_status, len);
			if(ret != 0)
			{
				ret = -EINVAL;
				goto ERROR;
			}
			break;
		default:
			ret = -EINVAL;
			break;
	}

ERROR:
	return ret;

}

/*在LOCAL_OUT上挂钩子*/
static unsigned int nf_hook_out(unsigned int hooknum,
								struct sk_buff **skb,
								const struct net_device *in,
								const struct net_device *out,
								int (*okfn)(struct sk_buff*))
{
	struct sk_buff *sk = *skb;
	struct iphdr *iph = ip_hdr(sk);

	if(IS_BANDIP(b_status))                               //判断是否禁止IP协议
	{
		if(b_status.band_ip == iph->saddr)                //IP地址符合
		{
			return NF_DORP;                               //丢弃网络报文
		}
	}

	return NF_ACCEPT;
}
