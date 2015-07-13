#ifndef __DNVR_SWITCH_H__
#define __DNVR_SWITCH_H__

#define DEBUG_SWITCH
#ifdef DEBUG_SWITCH
#define SWT_TRACE(fmt...)	\
	do{printf("\033[1;31mSWITCH->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#else
#define SWITCH_TRACE(fmt...)
#endif

#define SIOCGSWTREG	0x89F1    /* Read IP17XX MII register.	*/
#define SIOCSSWTREG	0x89F2    /* Write IP17XX MII register.	*/
#define SIOCGPORTSTA	0x89F3    /* Get IP17XX ports status.	*/
#define SIOCFINDPORT	0x89F4    /* Get IP17XX ports number.	*/
#define SIOCSWTRESET	0x89F6    /* Reset IP17XX */

#if(MAX_CAM_CH == 4)
	#define LAN_PORT_BITMASK (0x0000001F & ~WAN_PORT_BITMASK)
#elif(MAX_CAM_CH == 8)
	#define LAN_PORT_BITMASK (0x000001FF & ~WAN_PORT_BITMASK)
#elif(MAX_CAM_CH == 16)
	#define LAN_PORT_BITMASK (0x0001FFFF & ~WAN_PORT_BITMASK)
#else
	#define LAN_PORT_BITMASK (0x01FFFFFF & ~WAN_PORT_BITMASK)
#endif

typedef struct ip175c_reg {
  unsigned short p; // phy
  unsigned short m; // mii
} reg;

/* This structure is used in all SIOCxMIIxxx ioctl calls */
struct mii_ioctl_data {
	unsigned short		phy_id;
	unsigned short		reg_num;
	unsigned short		val_in;
	unsigned short		val_out;
};

struct switch_nvr_msg
{
	int port_num;
	int port_state;
	unsigned char mac[6];
	int switch_id;
};

extern int SWT_Init();
extern void SWT_Destroy();
extern int SWT_Mac_hit_port(int* mac);
extern unsigned long SWT_phy_port_getloss(void);
extern int SWT_keep_config();


#endif //__DNVR_SWITCH_H__
