

#ifndef __DVR_UTIL_H__
#define __DVR_UTIL_H__

//extern void get_ip_addr(unsigned char ret_ipaddr[4], unsigned char ret_broadcast[4], unsigned char ret_netmask[4]);
//extern void set_ip_addr(unsigned char ipaddr[4], unsigned char broadcast[4], unsigned char netmask[4]);
//
//extern void get_mac_addr(unsigned char ret_mac[6]);
//extern void set_mac_addr(unsigned char mac_addr[6]);

extern void set_if_property(
		unsigned char _ipaddr[4],
		unsigned char _netmask[4],
		unsigned char _macaddr[6],
		unsigned char _gateway[4]);

#ifdef _DNVR
extern void set_lan_ip_status(int ip_status);
extern int get_lan_ip_status(void);
extern unsigned char* generate_lan_ipaddr(unsigned char _ipaddr[4],unsigned char _lan_ipaddr[4]);
#endif
extern void set_lan_multicast_status(int is_use);
extern int get_lan_multicast_status(void);
extern int add_multicast_route(int op_type);
extern void config_netcard(unsigned char _ipaddr[4]);
extern int set_ipaddr(char * intf, char *ipaddr);
extern int get_ipaddr(char * intf, unsigned char* _ip);



#endif //__DVR_UTIL_H__
