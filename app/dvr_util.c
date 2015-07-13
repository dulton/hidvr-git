

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
//#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/route.h>
#include <errno.h>
#include "std.h"
#include "conf.h"
#ifdef _DNVR
#include "dnvr-switch.h"
#endif


#ifndef ETH_NETWORK_DEV
	#define	ETH_NETWORK_DEV "eth0"
#endif
#define PPP_NETWORK_DEV "ppp0"

#define SIN_ADDR(x) (((struct sockaddr_in *) (&(x)))->sin_addr.s_addr)
#define SET_SA_FAMILY(addr, family) \
    memset((char*)&(addr), '\0',   sizeof(addr)); \
    addr.sa_family = (family)
//static int set_ipaddr(char * intf, char *ipaddr)
int set_ipaddr(char * intf, char *ipaddr)

{
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	ASSERT(s >= 0, "s=%d:%s", s, strerror(errno));

	struct ifreq ifr;
	bzero((char *) &ifr, sizeof(ifr));
	strcpy(ifr.ifr_name, intf);

	struct sockaddr_in addr;
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	inet_aton(ipaddr, &addr.sin_addr);//将输入字符转成网络地址

	memcpy((char*) &ifr.ifr_ifru.ifru_addr, (char*) &addr, sizeof(struct sockaddr_in));
	int ret_ioctl = ioctl(s, SIOCSIFADDR, &ifr);
	if(ret_ioctl != 0)
	{
		TRACE_INFO("ret=%d,setip=%s:%s", ret_ioctl, ipaddr, strerror(errno));
	}
//	ASSERT(ret_ioctl == 0, "ret=%d,setip=%s:%s", ret_ioctl, ipaddr, strerror(errno));
	close(s);
	return 0;
}

static int set_netmask(char * intf, char *netmask)
{
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	ASSERT(s >= 0, "s=%d:%s", s, strerror(errno));

	struct ifreq ifr;
	bzero((char *) &ifr, sizeof(ifr));
	strcpy(ifr.ifr_name, intf);

	struct sockaddr_in netmask_addr;
	bzero(&netmask_addr, sizeof(struct sockaddr_in));
	netmask_addr.sin_family = AF_INET;
	inet_aton(netmask, &netmask_addr.sin_addr);

	memcpy((char*) &ifr.ifr_ifru.ifru_netmask, (char*) &netmask_addr,
			sizeof(struct sockaddr_in));
	int ret_ioctl = ioctl(s, SIOCSIFNETMASK, &ifr);
	if(ret_ioctl != 0)
	{
		TRACE_INFO("ret=%d,setmask=%s:%s", ret_ioctl, netmask, strerror(errno));
	}
//	ASSERT(ret_ioctl == 0, "ret=%d,setmask=%s:%s", ret_ioctl, netmask, strerror(errno));
	close(s);
	return 0;
}

static int set_gateway(char * intf, char *gateway)
{
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	ASSERT(s >= 0, "s=%d:%s", s, strerror(errno));

	struct rtentry rt;
	unsigned int dstaddr, gwaddr;

	dstaddr = inet_addr("0.0.0.0");
	gwaddr = inet_addr(gateway);


	memset(&rt, 0, sizeof(rt));
	/*set Destination addr*/
	SET_SA_FAMILY(rt.rt_dst, AF_INET);
	SIN_ADDR(rt.rt_dst) = dstaddr;

	/*set gw addr*/
	SET_SA_FAMILY(rt.rt_gateway, AF_INET);
	SIN_ADDR(rt.rt_gateway) = gwaddr;

	/*set genmask addr*/
	SET_SA_FAMILY(rt.rt_genmask, AF_INET);
	SIN_ADDR(rt.rt_genmask) = 0L;
	rt.rt_dev = intf;
	rt.rt_flags = RTF_GATEWAY;
	int ret_ioctl = ioctl(s, SIOCADDRT, &rt);
	if(ret_ioctl != 0)
	{
		TRACE_INFO("ret=%d,setgateway=%s:%s", ret_ioctl, gateway, strerror(errno));
	}
//	ASSERT(ret_ioctl == 0, "ret=%d,setgateway=%s:%s", ret_ioctl, gateway, strerror(errno));
	close(s);	
	return 0;
}

static int set_macaddr(char * intf, unsigned char* mac)//not a string
{
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	ASSERT(s >= 0, "s=%d:%s", s, strerror(errno));

	struct ifreq ifr;
	ifr.ifr_addr.sa_family = ARPHRD_ETHER;
	strncpy(ifr.ifr_name, intf, IFNAMSIZ - 1);
	memcpy((unsigned char *) ifr.ifr_hwaddr.sa_data, mac, 6);

	int ret_ioctl = ioctl(s, SIOCSIFHWADDR, &ifr);
	if(ret_ioctl != 0)
	{
		TRACE_INFO("ret=%d,setmac=%x:%x:%x:%x:%x:%x:%s", ret_ioctl,
				mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
				strerror(errno));
	}
//	ASSERT(ret_ioctl == 0, "ret=%d,setmac=%x:%x:%x:%x:%x:%x:%s", ret_ioctl,
//			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
//			strerror(errno));
	close(s);
	return 0;
}

#define IF_UPDOWN_DOWN (2)
#define IF_UPDOWN_UP (3)
static int set_if_updown(char * intf, int flag)
{
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	ASSERT(s >= 0, "s=%d:%s", s, strerror(errno));

	struct ifreq ifr;
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, intf, IFNAMSIZ - 1);

	int ret_ioctl;

	ret_ioctl = ioctl(s, SIOCGIFFLAGS, &ifr);
	ASSERT(ret_ioctl == 0, "ret=%d:%s", ret_ioctl, strerror(errno));
	if (flag == IF_UPDOWN_DOWN)
	{
		ifr.ifr_flags &= ~IFF_UP;
	}
	else if (flag == IF_UPDOWN_UP)
	{
		ifr.ifr_flags |= IFF_UP;
	}

	ret_ioctl = ioctl(s, SIOCSIFFLAGS, &ifr);
	ASSERT(ret_ioctl == 0, "ret=%d:%s", ret_ioctl, strerror(errno));
	close(s);
	return 0;
}

//static int get_ipaddr(char * intf, unsigned char* _ip)
int get_ipaddr(char * intf, unsigned char* _ip)

{
	int ret;
	int fsd;

	struct ifreq req;
	struct sockaddr_in *sin;

    fsd = socket( PF_INET, SOCK_DGRAM, 0 );
    if(fsd < 0) {
        return -1;
    }

	memset(&req, 0, sizeof(struct ifreq));
	strncpy(req.ifr_name, intf, IFNAMSIZ);

	sin = (struct sockaddr_in *)&req.ifr_addr;
	ret = ioctl( fsd, SIOCGIFADDR, &req);

	close( fsd );

	if(ret != 0) {
		return -1;
	}

	int i;
	for(i = 0; i < 4; i++) {
		*(_ip+i) = (sin->sin_addr.s_addr >> (i*8)) & 0xff;
	}
	
	return 0;
}

void set_if_property(
		unsigned char _ipaddr[4],
		unsigned char _netmask[4],
		unsigned char _macaddr[6],
		unsigned char _gateway[4])
{

	if(_ipaddr)
	{
		char ipaddr[64];
		sprintf(ipaddr, "%d.%d.%d.%d", _ipaddr[0], _ipaddr[1], _ipaddr[2], _ipaddr[3]);
		set_ipaddr(ETH_NETWORK_DEV, ipaddr);
	}
	if(_netmask)
	{
		char netmask[32];
		sprintf(netmask, "%d.%d.%d.%d", _netmask[0], _netmask[1], _netmask[2], _netmask[3]);
		set_netmask(ETH_NETWORK_DEV, netmask);
	}
	if(_macaddr)
	{
//		char macaddr[32];
//		sprintf(macaddr, "%x:%x:%x:%x:%x%x", _macaddr[0], _macaddr[1], _macaddr[2], _macaddr[3], _macaddr[4], _macaddr[5]);
		#ifdef _DNVR
			#ifdef _DNVR_VLAN
				set_if_updown(ETH_NETWORK_DEV0, IF_UPDOWN_DOWN);
				set_macaddr(ETH_NETWORK_DEV0, _macaddr);
				set_macaddr(ETH_NETWORK_DEV1, _macaddr);
				set_macaddr(ETH_NETWORK_DEV2, _macaddr);
				set_if_updown(ETH_NETWORK_DEV0, IF_UPDOWN_UP);
			#else
				set_if_updown(ETH_NETWORK_DEV, IF_UPDOWN_DOWN);
				set_macaddr(ETH_NETWORK_DEV, _macaddr);
				set_if_updown(ETH_NETWORK_DEV, IF_UPDOWN_UP);
			#endif
		#else
			set_if_updown(ETH_NETWORK_DEV, IF_UPDOWN_DOWN);
			set_macaddr(ETH_NETWORK_DEV, _macaddr);
			set_if_updown(ETH_NETWORK_DEV, IF_UPDOWN_UP);
		#endif
	}
	if(_gateway)
	{
		char gateway[32];
		sprintf(gateway, "%d.%d.%d.%d", _gateway[0], _gateway[1], _gateway[2], _gateway[3]);
		set_gateway(ETH_NETWORK_DEV, gateway);
	}
}

int ppp_check_active(void)
{
	char ipaddr[32];
	memset(ipaddr, 0, sizeof(ipaddr));

	return get_ipaddr(PPP_NETWORK_DEV, ipaddr);
}

int ppp_set_route_by_ip(void)
{
	char ipaddr[32];
	memset(ipaddr, 0, sizeof(ipaddr));
	if(0 == get_ipaddr(PPP_NETWORK_DEV, ipaddr)) {
		char gateway[32];
		sprintf(gateway, "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
		set_gateway(PPP_NETWORK_DEV, gateway);

		return 0;
	}

    return -1;
}

int ppp_get_ipaddr(unsigned char* _ip)
{
	char ipaddr[32];
	int rlt = 0;

	rlt = get_ipaddr(PPP_NETWORK_DEV, ipaddr);
	if(rlt ==  0)
	{
		sprintf(_ip, "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
	}
	
	return rlt;
}


//void get_if_property(
//		unsigned char ret_ipaddr[4],
//		unsigned char ret_netmask[4],
//		unsigned char ret_macaddr[6],
//		unsigned char ret_gateway[4])/*未实现*/
//{
//	struct ifreq ifreq_st;
//	struct sockaddr_in* addr = (struct sockaddr_in*)&ifreq_st.ifr_addr;
//	unsigned char ip[4];
//	unsigned char netmask[4];
//	unsigned char macaddr[6];
//	unsigned char gateway[4];
//
//	int sfd = -1;
//	sfd = socket(PF_INET, SOCK_DGRAM, 0);
//	strcpy(ifreq_st.ifr_name, ETH_NETWORK_DEV);
//
//	// ip address
//	ioctl(sfd,SIOCGIFADDR,&ifreq_st);
//	*(unsigned int*)ip = addr->sin_addr.s_addr;//uint32
//	printf("ip addr: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
//
//	// mac address
//	ioctl(sfd,SIOCGIFHWADDR,&ifreq_st);
//	memcpy(macaddr, ifreq_st.ifr_hwaddr.sa_data, sizeof(macaddr));//uint8[6]
//	printf("mac addr: %x:%x:%x:%x:%x:%x\r\n",
//			macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
//
//	// netmask
//	ioctl(sfd,SIOCGIFNETMASK,&ifreq_st);
//	*(unsigned int*)netmask = addr->sin_addr.s_addr;//uint32
//	printf("netmask: %d.%d.%d.%d\r\n", netmask[0], netmask[1], netmask[2], netmask[3]);
//
//	// gateway
//	printf("gateway: %d.%d.%d.%d\r\n", gateway[0], gateway[1], gateway[2], gateway[3]);
//
//	// close socket
//	close(sfd); sfd = -1;
//	// return
//	if(ret_ipaddr){
//		memcpy(ret_ipaddr, ip, sizeof(ip));
//	}
//	if(ret_netmask){
//		memcpy(ret_netmask, netmask, sizeof(netmask));
//	}
//	if(ret_macaddr){
//		memcpy(ret_macaddr, macaddr, sizeof(macaddr));
//	}
//	if(ret_gateway){
//		memcpy(ret_gateway, gateway, sizeof(gateway));
//	}
//}

#ifdef _DNVR
/*
ifconfig eth0 mtu 1448 up
vconfig add eth0 1
vconfig add eth0 2
ifconfig eth0.1 192.168.135.1 mtu 1448 up
ifconfig eth0.2 192.168.1.72 mtu 1448 up
route add -net 224.0.0.0 netmask 224.0.0.0 eth0.1
*/
static int lan_ip_status = 0;
void set_lan_ip_status(int ip_status)
{
	lan_ip_status = ip_status;
}

int get_lan_ip_status(void)
{
	return lan_ip_status;
}

unsigned char* generate_lan_ipaddr(unsigned char _ipaddr[4],unsigned char _lan_ipaddr[4])
{
	if(_ipaddr[0] == 193)
		_lan_ipaddr[0] = 194;
	else
		_lan_ipaddr[0] = 193;
	if(_ipaddr[1] == 169)
		_lan_ipaddr[1] = 170;
	else
		_lan_ipaddr[1] = 169;	
	if(_ipaddr[2] == 100)
		_lan_ipaddr[2] = 101;
	else
		_lan_ipaddr[2] = 100;
	_lan_ipaddr[3] = 10;

	return _lan_ipaddr;
}

static int current_lan_multicast_status = 0;  //0--free,1--using
void set_lan_multicast_status(int is_use)
{
	current_lan_multicast_status=is_use;
}
int get_lan_multicast_status(void)
{
	return current_lan_multicast_status;
}

#define UNKNOW_MULTICAST_TYPE 0
#define LAN_MULTICAST_TYPE 1
#define WAN_MULTICAST_TYPE 2
static int current_lan_multicast_type = UNKNOW_MULTICAST_TYPE;  //0--unknow,1--lan,2--wan
/*
	op_type: bit0=1,LAN;bit0=0,WAN;bit1=1,force excute
*/
int add_multicast_route(int op_type)
{
	#ifndef _DNVR_VLAN
		return 0;
	#endif
	int ret = 0;
	char cmd_buf[200];
	
	if((op_type & 0x1) == 1)
	{
		set_lan_multicast_status(1);
		if( (current_lan_multicast_type == LAN_MULTICAST_TYPE) && !(op_type & 0x2) )
			return 0;
		else
		{
			current_lan_multicast_type = LAN_MULTICAST_TYPE;
			memset(cmd_buf,0,sizeof(cmd_buf));
			sprintf(cmd_buf, "route;route del -net 224.0.0.0 netmask 224.0.0.0");
			TRACE_DEBUG("%s", cmd_buf);
			system(cmd_buf);
			memset(cmd_buf,0,sizeof(cmd_buf));
			sprintf(cmd_buf, "route add -net 224.0.0.0 netmask 224.0.0.0 eth0.1;route");
			TRACE_DEBUG("%s", cmd_buf);
			system(cmd_buf);			
		}
	}
	else
	{
		if(get_lan_multicast_status() == 1)
			return 1;
		if( (current_lan_multicast_type == WAN_MULTICAST_TYPE) && !(op_type & 0x2) )
			return 0;
		else
		{
			current_lan_multicast_type = WAN_MULTICAST_TYPE;
			memset(cmd_buf,0,sizeof(cmd_buf));
			sprintf(cmd_buf, "route;route del -net 224.0.0.0 netmask 224.0.0.0");
			TRACE_DEBUG("%s", cmd_buf);
			system(cmd_buf);
			memset(cmd_buf,0,sizeof(cmd_buf));
			sprintf(cmd_buf, "route add -net 224.0.0.0 netmask 224.0.0.0 eth0.2;route");
			TRACE_DEBUG("%s", cmd_buf);
			system(cmd_buf);	
		}
	}

	return ret;
}

void config_netcard(unsigned char _ipaddr[4])
{
	char cmd_buf[255];
	char ipaddr[64];
	unsigned char lan_ipaddr[4];

	SWT_Init();
	usleep(200*1000);
	#ifdef _DNVR_VLAN
		//Config VLAN
		memset(cmd_buf,0,sizeof(cmd_buf));
		sprintf(cmd_buf, "ifconfig eth0 mtu 1448 up;vconfig add eth0 1;vconfig add eth0 2;ifconfig eth0.1 mtu 1448 up;\
			ifconfig eth0.2 mtu 1448 up");
		TRACE_DEBUG("%s", cmd_buf);
		system(cmd_buf);

		//Config LAN IP
		generate_lan_ipaddr(_ipaddr,lan_ipaddr);	
		sprintf(ipaddr, "%d.%d.%d.%d", lan_ipaddr[0], lan_ipaddr[1], lan_ipaddr[2], lan_ipaddr[3]);
		TRACE_DEBUG("Lan IP 1 is %s", ipaddr);
		set_ipaddr(ETH_NETWORK_DEV1, ipaddr);
	#else
		//Config physical device
		memset(cmd_buf,0,sizeof(cmd_buf));
		sprintf(cmd_buf, "ifconfig eth0 up");
		TRACE_DEBUG("%s", cmd_buf);
		system(cmd_buf);

		//Config virtual device
		generate_lan_ipaddr(_ipaddr,lan_ipaddr);	
		sprintf(ipaddr, "%d.%d.%d.%d", lan_ipaddr[0], lan_ipaddr[1], lan_ipaddr[2], lan_ipaddr[3]);
		TRACE_DEBUG("Lan IP 1 is %s", ipaddr);
		memset(cmd_buf,0,sizeof(cmd_buf));
		sprintf(cmd_buf, "ifconfig eth0:1 %s up",ipaddr);
		TRACE_DEBUG("%s", cmd_buf);
		system(cmd_buf);		
	#endif

}
#else
void set_lan_multicast_status(int op_type){}
int get_lan_multicast_status(void){return 0;}
int add_multicast_route(int op_type){return 0;}
void config_netcard(unsigned char _ipaddr[4])
{
	TRACE_DEBUG("%s:do nothing!",__FUNCTION__);
}
#endif


