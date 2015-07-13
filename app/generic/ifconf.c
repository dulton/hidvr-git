
#include "ifconf.h"

char* ifconf_ipv4_ntoa(ifconf_ipv4_addr_t addr)
{
	return inet_ntoa(addr.in_addr);
}

ifconf_ipv4_addr_t ifconf_ipv4_aton(const char* addr)
{
	ifconf_ipv4_addr_t ip4_addr;
	inet_aton(addr, &ip4_addr.in_addr);
	return ip4_addr;
}

char* ifconf_hw_ntoa(ifconf_hw_addr_t addr)
{
	static char str_hw[sizeof("00:01:02:03:04:05")];
	sprintf(str_hw, "%02X:%02X:%02X:%02X:%02X:%02X",
		addr.s_b1, addr.s_b2, addr.s_b3, addr.s_b4, addr.s_b5, addr.s_b6);
	return str_hw;
}

ifconf_hw_addr_t ifconf_hw_aton(const char* addr)
{
	ifconf_hw_addr_t hw_addr;
	uint32_t hw1, hw2, hw3, hw4, hw5, hw6;
	memset(&hw_addr, 0, sizeof(hw_addr));
	sscanf(addr, "%X:%X:%X:%X:%X:%X", &hw1, &hw2, &hw3, &hw4, &hw5, &hw6);
	hw_addr.s_b1 = hw1;
	hw_addr.s_b2 = hw2;
	hw_addr.s_b3 = hw3;
	hw_addr.s_b4 = hw4;
	hw_addr.s_b5 = hw5;
	hw_addr.s_b6 = hw6;
	return hw_addr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

static int __ifconf_ioctl(int sock, int cmd, void* arg)
{
	if(0 == ioctl(sock, cmd, arg)){	
		return 0;
	}
	perror("ifconf_ioctl");
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// route
static int _ifconf_add_route(int sock, const char* rt_name, ifconf_route_t* if_route)
{
	int ret = 0;
	struct rtentry rt_entry;
	struct  sockaddr_in* p_addr = NULL;

	memset(&rt_entry, 0, sizeof(struct rtentry));
	// config
	rt_entry.rt_dev = rt_name; 
	rt_entry.rt_metric = 0;   
	rt_entry.rt_window = 0;   
	rt_entry.rt_flags = RTF_UP;
	
	// dest address
	p_addr = (struct sockaddr_in *)(&rt_entry.rt_dst);
	p_addr->sin_family = AF_INET;   
	p_addr->sin_addr.s_addr = if_route->dest.s_addr;

	// gateway
	//if(if_route->gateway.s_addr){
		p_addr = (struct sockaddr_in *)(&rt_entry.rt_gateway);
		p_addr->sin_family = AF_INET;   
		p_addr->sin_addr.s_addr = if_route->gateway.s_addr;
		rt_entry.rt_flags |= RTF_GATEWAY;
	//}

	// netmask
	p_addr = (struct sockaddr_in *)(&rt_entry.rt_genmask);
	p_addr->sin_family = AF_INET;   
	p_addr->sin_addr.s_addr = if_route->netmask.s_addr;

	// ioctl
	ret = ioctl(sock, SIOCADDRT, &rt_entry); // FIXME: always return -1
	if(0 != ret){
		if(errno == -ENETUNREACH){
			printf("---------------------- wahahahahah -----------------------\r\n");
		}
		perror("ioctl");
		return -1;
	}
	return 0;
}

static int _ifconf_delete_route(int sock, const char* rt_name, ifconf_route_t* if_route)
{
	struct rtentry rt_entry;
	struct  sockaddr_in* p_addr = NULL;

	memset(&rt_entry, 0, sizeof(struct rtentry));
	// config
	rt_entry.rt_dev = rt_name;
	
	// dest address
	p_addr = (struct sockaddr_in *)(&rt_entry.rt_dst);
	p_addr->sin_family = AF_INET;   
	p_addr->sin_addr.s_addr = if_route->dest.s_addr;

	// gateway
	p_addr = (struct sockaddr_in *)(&rt_entry.rt_gateway);
	p_addr->sin_family = AF_INET;   
	p_addr->sin_addr.s_addr = if_route->gateway.s_addr;

	// netmask
	p_addr = (struct sockaddr_in *)(&rt_entry.rt_genmask);
	p_addr->sin_family = AF_INET;   
	p_addr->sin_addr.s_addr = if_route->netmask.s_addr;

	// ioctl
	__ifconf_ioctl(sock, SIOCDELRT, &rt_entry); // FIXME: always return -1
	return -1;
}

static int _ifconf_get_route(int sock, const char* rt_name, ifconf_route_t* if_route)
{
	FILE* fid_rt = NULL;
	char buf_rt[256];
	
	fid_rt = fopen("/proc/net/route", "r");
	assert(fid_rt);
	
	// skip the title line 
	fgets(buf_rt, sizeof(buf_rt), fid_rt);
	// analyse by line
	while (fgets(buf_rt, sizeof(buf_rt), fid_rt)) {
		// Iface	Destination	Gateway 	Flags	RefCnt	Use	Metric	Mask		MTU	Window	IRTT 
		// eth0	00000000	0101A8C0	0003	0	0	0	00FFFFFF	0	0	0
		char name[IFNAMSIZ];
		uint32_t destination, gateway, flags, refcnt, use, metric, mask, mtu, window, irtt;
		// only check the valid line
		if(11 != sscanf(buf_rt, "%s%X%X%X%d%d%d%X%d%d%d",
			name, &destination, &gateway, &flags, &refcnt, &use, &metric, &mask,&mtu, &window, &irtt)){
			continue;
		}
		if(0 == strncmp(name, rt_name, strlen(rt_name))){
			if(if_route->dest.s_addr != destination){
				continue;
			}
			if_route->gateway.s_addr = gateway;
			if_route->netmask.s_addr = mask;
			break;
		}
	}
	
	fclose(fid_rt);
	fid_rt = NULL;
	return 0;
} 


int ifconf_add_route(const char* rt_name, ifconf_route_t* if_route)
{
	int ret = 0;
	int sock = 0;
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	ret = _ifconf_add_route(sock, rt_name, if_route);
	close(sock);
	return ret;
}

int ifconf_delete_route(const char* rt_name, ifconf_route_t* if_route)
{
	int ret = 0;
	int sock = 0;
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	ret = _ifconf_delete_route(sock, rt_name, if_route);
	close(sock);
	return ret;
}

int ifconf_set_route(const char* rt_name, ifconf_route_t* if_route)
{
	return ifconf_add_route(rt_name, if_route);
}

int ifconf_get_route(const char* rt_name, ifconf_route_t* if_route)
{
	int ret = 0;
	int sock = 0;
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	ret = _ifconf_get_route(sock, rt_name, if_route);
	close(sock);
	return ret;
} 

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// interface



static int _ifconf_ioctl(int sock, const char if_name[IFNAMSIZ], int type, void* arg)
{
	int i = 0;
	struct ifreq if_req;
	struct sockaddr_in* addr_in = (struct sockaddr_in*)(&if_req.ifr_addr);	
	memset(&if_req, 0, sizeof(struct ifreq));
	strcpy(if_req.ifr_name, if_name);
	switch(type)
	{
	case SIOCGIFADDR:
	case SIOCGIFNETMASK:
	case SIOCGIFBRDADDR:
		{
			ifconf_ipv4_addr_t* const ipv4_addr = (ifconf_ipv4_addr_t*)arg;
			if(0 == __ifconf_ioctl(sock, type, &if_req)){
				if(ipv4_addr){
					ipv4_addr->s_addr = addr_in->sin_addr.s_addr;
				}
				return 0;
			}
		}
		break;

	case SIOCGIFHWADDR:
		{
			ifconf_hw_addr_t* const hw_addr = (ifconf_hw_addr_t*)arg;
			if(0 == __ifconf_ioctl(sock, type, (void*)&if_req)){
				if(hw_addr){
					for(i = 0; i < sizeof(hw_addr->s_b) / sizeof(hw_addr->s_b[0]); ++i){
						hw_addr->s_b[i] = if_req.ifr_hwaddr.sa_data[i];
					}	
				}
				return 0;
			}
		}
		break;

	case SIOCGIFMTU:
		{
			int* const mtu = (int*)arg;
			if(0 == __ifconf_ioctl(sock, type, (void*)&if_req)){
				if(mtu){
					*mtu = if_req.ifr_ifru.ifru_mtu;
				}
				return 0;
			}
		}
		break;
		
	case SIOCSIFADDR:
	case SIOCSIFNETMASK:
	case SIOCSIFBRDADDR:
		{
			ifconf_ipv4_addr_t* const ipv4_addr = (ifconf_ipv4_addr_t*)arg;
			if(ipv4_addr){
				addr_in->sin_family = AF_INET;
				addr_in->sin_addr.s_addr = ipv4_addr->s_addr;
				if(0 == __ifconf_ioctl(sock, type, &if_req)){
					return 0;
				}
			}
		}
		break;

	case SIOCSIFHWADDR:
		{
			ifconf_hw_addr_t* const hw_addr = (ifconf_hw_addr_t*)arg;
			if(hw_addr){
				if_req.ifr_hwaddr.sa_family = ARPHRD_ETHER;
				for(i = 0; i < sizeof(hw_addr->s_b) / sizeof(hw_addr->s_b[0]); ++i){
					if_req.ifr_hwaddr.sa_data[i] = hw_addr->s_b[i];
				}
				if(0 == __ifconf_ioctl(sock, type, &if_req)){
					return 0;
				}
			}
		}
		break;

	case SIOCSIFMTU:
		{
			int* const mtu = (int*)arg;
			if(mtu){
				if_req.ifr_ifru.ifru_mtu = *mtu;
				if(0 == __ifconf_ioctl(sock, type, (void*)&if_req)){
					return 0;
				}
			}
		}
		break;
		
	default:
		;
	}
	return -1;
}

static int _ifconf_get_ipaddr(int sock, const char if_name[IFNAMSIZ], ifconf_ipv4_addr_t* ret_addr)
{
	return _ifconf_ioctl(sock, if_name, SIOCGIFADDR, ret_addr);
}

static int _ifconf_get_netmask(int sock, const char if_name[IFNAMSIZ], ifconf_ipv4_addr_t* ret_addr)
{
	return _ifconf_ioctl(sock, if_name, SIOCGIFNETMASK, ret_addr);
}

static int _ifconf_get_broadcast(int sock, const char if_name[IFNAMSIZ], ifconf_ipv4_addr_t* ret_addr)
{
	return _ifconf_ioctl(sock, if_name, SIOCGIFBRDADDR, ret_addr);
}

static int _ifconf_get_hwaddr(int sock, const char if_name[IFNAMSIZ], ifconf_hw_addr_t* ret_addr)
{
	return _ifconf_ioctl(sock, if_name, SIOCGIFHWADDR, ret_addr);
}

static int _ifconf_get_mtu(int sock, const char if_name[IFNAMSIZ], uint32_t* ret_mtu)
{
	return _ifconf_ioctl(sock, if_name, SIOCGIFMTU, ret_mtu);
}

static int _ifconf_set_ipaddr(int sock, const char if_name[IFNAMSIZ], ifconf_ipv4_addr_t* addr)
{
	return _ifconf_ioctl(sock, if_name, SIOCSIFADDR, addr);
}

static int _ifconf_set_netmask(int sock, const char if_name[IFNAMSIZ], ifconf_ipv4_addr_t* addr)
{
	return _ifconf_ioctl(sock, if_name, SIOCSIFNETMASK, addr);
}

static int _ifconf_set_broadcast(int sock, const char if_name[IFNAMSIZ], ifconf_ipv4_addr_t* addr)
{
	return _ifconf_ioctl(sock, if_name, SIOCSIFBRDADDR, addr);
}

static int _ifconf_set_hwaddr(int sock, const char if_name[IFNAMSIZ], ifconf_hw_addr_t* addr)
{
	return _ifconf_ioctl(sock, if_name, SIOCSIFHWADDR, addr);
}

static int _ifconf_set_mtu(int sock, const char if_name[IFNAMSIZ], uint32_t* mtu)
{
	return _ifconf_ioctl(sock, if_name, SIOCSIFMTU, mtu);
}

int ifconf_get_interface(const char if_name[IFNAMSIZ], ifconf_interface_t* ifr)
{
	int ret = 0;
	int sock = 0;
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	assert(sock > 0);

	if(ifr){
		ifconf_route_t if_route;
		// ip address
		ret = _ifconf_get_ipaddr(sock, if_name, &ifr->ipaddr);
		// netmask
		ret = _ifconf_get_netmask(sock, if_name, &ifr->netmask);
		// gateway found in routE
		if_route.dest.s_addr = 0;
		ret = _ifconf_get_route(sock, if_name, &if_route);
		ifr->gateway = if_route.gateway;
		// broadcast
		ret = _ifconf_get_broadcast(sock, if_name, &ifr->broadcast);
		// hwaddr
		ret = _ifconf_get_hwaddr(sock, if_name, &ifr->hwaddr);
		// mtu
		ret = _ifconf_get_mtu(sock, if_name, &ifr->mtu);
	}
	close(sock);
	return 0;
}

int ifconf_set_interface(const char if_name[IFNAMSIZ], ifconf_interface_t* ifr)
{
	int ret = 0;
	int sock = 0;
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	assert(sock > 0);

	if(ifr){
		// ip address
		if(ifr->ipaddr.s_addr){
			ret = _ifconf_set_ipaddr(sock, if_name, &ifr->ipaddr);
			assert(0 == ret);
		}
		// netmask
		if(ifr->netmask.s_addr){
			ret = _ifconf_set_netmask(sock, if_name, &ifr->netmask);
			assert(0 == ret);
		}
		// gateway found in route
		if(ifr->gateway.s_addr){
			ifconf_route_t if_route;
			if_route.dest.s_addr = 0;
			if_route.gateway = ifr->gateway;
			if_route.netmask.s_addr = 0;
			ret = _ifconf_add_route(sock, if_name, &if_route);
		}
		// broadcast
		if(ifr->broadcast.s_addr){
			ret = _ifconf_set_broadcast(sock, if_name, &ifr->broadcast);
			assert(0 == ret);
		}
		// hwaddr
		if(ifr->hwaddr.s_addr){
			ret = _ifconf_set_hwaddr(sock, if_name, &ifr->hwaddr);
			assert(0 == ret);
		}
		// mtu
		if(ifr->mtu){
			ret = _ifconf_set_mtu(sock, if_name, &ifr->mtu);
			assert(0 == ret);
		}
	}
	close(sock);
	return 0;
}   

int ifconf_get_dns(ifconf_dns_t* ret_dns)
{
	// FIXME: to be completed	
	return 0;
}

int ifconf_set_dns(ifconf_dns_t* dns)
{
	FILE* fid = fopen("/etc/resolv.conf", "w");
	if(fid){
		char str_dns[128];
		sprintf(str_dns, "nameserver %s\r\n", ifconf_ipv4_ntoa(dns->preferred));
		fputs(str_dns, fid);
		sprintf(str_dns, "nameserver %s\r\n", ifconf_ipv4_ntoa(dns->alternate));
		fputs(str_dns, fid);
		// extra dns
		sprintf(str_dns, "nameserver %s\r\n", "8.8.8.8");
		fputs(str_dns, fid);
		sprintf(str_dns, "nameserver %s\r\n", "8.8.4.4");
		fputs(str_dns, fid);
		sprintf(str_dns, "nameserver %s\r\n", "165.98.1.1");
		fputs(str_dns, fid);
		fclose(fid);
		return 0;
	}
	return -1;
}


#if 0
int main(int argc, char** argv)
{
	ifconf_interface_t ifconf_ifr;
	ifconf_hw_addr_t hwaddr;

	
	ifconf_get_interface("eth0", &ifconf_ifr);

	
	printf("IP address = %s\r\n", ifconf_ipv4_ntoa(ifconf_ifr.ipaddr));
	printf("Mask = %s\r\n", ifconf_ipv4_ntoa(ifconf_ifr.netmask));
	printf("Gateway = %s\r\n", ifconf_ipv4_ntoa(ifconf_ifr.gateway));
	printf("Broadcast = %s\r\n", ifconf_ipv4_ntoa(ifconf_ifr.broadcast));
	hwaddr = ifconf_ifr.hwaddr;
	printf("Hardware = %02X:%02X:%02X:%02X:%02X:%02X\r\n",
		hwaddr.s_b[0], hwaddr.s_b[1], hwaddr.s_b[2],
		hwaddr.s_b[3], hwaddr.s_b[4], hwaddr.s_b[5]);
	printf("MTU = %d\r\n", ifconf_ifr.mtu);

	printf("======================================================\r\n");
	printf("= configuration\r\n");
	printf("======================================================\r\n");
	printf("\r\n");

	ifconf_ifr.ipaddr = ifconf_ipv4_aton("192.168.1.47");
	ifconf_ifr.netmask = ifconf_ipv4_aton("255.255.0.0");
	ifconf_ifr.gateway = ifconf_ipv4_aton("192.168.1.1");
	ifconf_ifr.hwaddr = ifconf_hw_aton("00:11:22:33:44:55");
	ifconf_ifr.mtu = 1100;

	ifconf_set_interface("eth0", &ifconf_ifr);


	ifconf_get_interface("eth0", &ifconf_ifr);

	
	printf("IP address = %s\r\n", ifconf_ipv4_ntoa(ifconf_ifr.ipaddr));
	printf("Mask = %s\r\n", ifconf_ipv4_ntoa(ifconf_ifr.netmask));
	printf("Gateway = %s\r\n", ifconf_ipv4_ntoa(ifconf_ifr.gateway));
	printf("Broadcast = %s\r\n", ifconf_ipv4_ntoa(ifconf_ifr.broadcast));
	hwaddr = ifconf_ifr.hwaddr;
	printf("Hardware = %02X:%02X:%02X:%02X:%02X:%02X\r\n",
		hwaddr.s_b[0], hwaddr.s_b[1], hwaddr.s_b[2],
		hwaddr.s_b[3], hwaddr.s_b[4], hwaddr.s_b[5]);
	printf("MTU = %d\r\n", ifconf_ifr.mtu);



	ifconf_route_t ifconf_rt;
	memset(&ifconf_rt, 0, sizeof(ifconf_rt));
	ifconf_rt.dest = ifconf_ipv4_aton("192.168.1.10");
	ifconf_rt.gateway = ifconf_ipv4_aton("255.255.255.0");
	ifconf_add_route("eth0", &ifconf_rt);

	system("route -e");
//	sleep(3);

	ifconf_delete_route("eth0", &ifconf_rt);
	system("route -e");
//	sleep(3);

	return 0;
}
#endif

