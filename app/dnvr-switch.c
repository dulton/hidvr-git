/* user-switch.c
 *
 * Copyright (C) 2008,JUAN
 *
 * http://www.icplus.com.tw/Data/Datasheet/IP175Cx-DS-R17-20081121.pdf
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Id:
 */

//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sysmacros.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <linux/if_ether.h>
#include <linux/types.h>
#include <linux/sockios.h>
#include <net/if.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/sockios.h>
//#include <linux/mii.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "conf.h"
#include "dnvr-switch.h"

#ifndef ETH_NETWORK_DEV0
#define ETH_NETWORK_DEV0 "eth0"
#endif

#ifndef _DNVR

int SWT_Init(){return -1;}
void SWT_Destroy(){;}
int SWT_Mac_hit_port(int* mac){return -1;}
unsigned long SWT_phy_port_getloss(void){return 0;}

#else //_DNVR

/*
  The switch chip of DNVR(8/16 channel) is IP1725.
  The switch chip of DNVR(4 channel) is IP175D.
  Please note:
    1.It isn't compatible in other case,example 24 channel and 32 channel. 
    2.VLAN config is depend with macro WAN_PORT_NO in DNVR and IP1725.
*/
#if(MAX_CAM_CH >= 8)
	#define SWITCH_IP1725
#endif
#ifdef SWITCH_IP1725
	#define ACL_VLAN_CTRL	0xFA
	#define ACL_DATA		0xFB
	#define VLAN_DATA		0xFC
	#define CPU_PORT_BITMASK (0x1 << 24)  //CPU port is fixed to port 25 in IP1725
#endif
#define PHY_ID_NONE 	0xFFFF
#define SWITCH_VLAN_NONE 0  //Switch no vlan
#define SWITCH_VLAN_HALF 1  //Switch has vlan and need cpu support
#define SWITCH_VLAN_FULL 2  //Switch has vlan and need'nt cpu support
#ifdef _DNVR_VLAN
	#define SWITCH_VLAN_MODE SWITCH_VLAN_HALF
#else
	#define SWITCH_VLAN_MODE SWITCH_VLAN_FULL
#endif

typedef struct _switch_reg {
  unsigned short phy; // phy
  unsigned short mii; // mii
  unsigned short val;
}switch_reg;

#ifdef SWITCH_IP1725
#define SWITCH_CONFIG_REG_NUM 28
switch_reg switch_config[SWITCH_CONFIG_REG_NUM] = {
	{PHY_ID_NONE, 0x56, 0x0}, 
	{PHY_ID_NONE, 0x58, 0x0}, 
	{PHY_ID_NONE, 0x5A, 0x0},  
	{PHY_ID_NONE, 0x70, 0x0}, 
	{PHY_ID_NONE, 0x76, 0x0}, 
	{PHY_ID_NONE, 0x82, 0x0},
	{PHY_ID_NONE, 0x89, 0x0},
	{PHY_ID_NONE, 0x97, 0x0}, 
	{PHY_ID_NONE, 0x98, 0x0},
	{PHY_ID_NONE, 0x99, 0x0},
	{PHY_ID_NONE, 0x9A, 0x0},
	{PHY_ID_NONE, 0x9B, 0x0},
	{PHY_ID_NONE, 0x9C, 0x0},
	{PHY_ID_NONE, 0x9D, 0x0},
	{PHY_ID_NONE, 0x9E, 0x0},
	{PHY_ID_NONE, 0x9F, 0x0},
	{PHY_ID_NONE, 0xA0, 0x0},
	{PHY_ID_NONE, 0xA1, 0x0},
	{PHY_ID_NONE, 0xA2, 0x0},
	{PHY_ID_NONE, (0x9B+((WAN_PORT_NO-1)/3)), 0x0},
	{PHY_ID_NONE, 0xAD, 0x0},
	{PHY_ID_NONE, 0xAE, 0x0},
	{PHY_ID_NONE, 0xAF, 0x0},
	{PHY_ID_NONE, 0xB0, 0x0},
	{PHY_ID_NONE, 0xB1, 0x0},
	{PHY_ID_NONE, 0xB2, 0x0},
	{PHY_ID_NONE, 0xA3, 0x0},
	{PHY_ID_NONE, 0xA3, 0x0},	

};
#else
#define SWITCH_CONFIG_REG_NUM 18
switch_reg switch_config[SWITCH_CONFIG_REG_NUM] = {
	{20, 4, 0xa000},  //force PHY5 speed100 ,full duplex
	{20, 14, 0x0},
	{20, 24, (0 << 2 | 2)},  //LED output mode selection
	{22, 0, ((0x1F << 0) | (0x1 << 5) | (0x0 << 6))},
	{22, 4, 0x1},
	{22, 5, 0x1},
	{22, 6, 0x1},
	{22, 7, 0x1},
	{22, 8, 0x2},
	{22, 9, 0x3},
	{22, 10, 0x7},
	{22, 14, ((0x1 << 0) | (0x1 << 12))},
	{22, 15, ((0x2 << 0) | (0x2 << 12))},
	{22, 16, ((0x3 << 0) | (0x3 << 12))},
	{23, 0, (0x2f << 0) | (0x30 << 8)},
	{23, 1, (0x3f << 0)},
	{23, 16, ((0x3f << 0) | (0x3f << 8))},
	{23, 17, (0x3f << 0)},
};
#endif


static int g_swt_sock = -1;

static void _swt_age_time_set(unsigned short sec);
static int _swt_print_router_config(void);
static int _swt_set_router_config(void);
static void phy_mii_write_reg(int phy_id, int reg_num, unsigned short val);
static unsigned short phy_mii_read_reg(int phy_id, int reg_num);
static int _swt_check_config(void);

static int phy_dev_open(void)
{
	g_swt_sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (g_swt_sock < 0){
		return -errno;
	}
	
	return 0;
}


static void phy_dev_close(void)
{
	if(g_swt_sock < 0)
		return;
	
	close(g_swt_sock);
	g_swt_sock = -1;
}


static int phy_mii_write(reg mii, unsigned short value)
{
	//fprintf(stderr, "       -> Setting to 0x%04X\n",value);
	struct mii_ioctl_data *data;
	struct ifreq iwr;
	int err = 0;

	if (g_swt_sock < 0){
		return -errno;
	}
	
	(void) strncpy (iwr.ifr_name, ETH_NETWORK_DEV0, sizeof (ETH_NETWORK_DEV0));
	data = (struct mii_ioctl_data *) &iwr.ifr_data;
	data->phy_id = mii.p;
	data->reg_num = mii.m;
	data->val_in = value;
	if (ioctl (g_swt_sock, SIOCSSWTREG, &iwr) == -1) {
		fprintf(stderr,"Cannot set PHY %d %d to %X",mii.p,mii.m,value);
		perror("");
		err = -errno;
	}

	return err;
}

static unsigned short phy_mii_read(reg mii)
{
	struct mii_ioctl_data *data;
	struct ifreq iwr;

	if (g_swt_sock < 0){
		return -1;
	}
	(void) strncpy (iwr.ifr_name, ETH_NETWORK_DEV0, sizeof (ETH_NETWORK_DEV0));
	data = (struct mii_ioctl_data *) &iwr.ifr_data;
	data->phy_id = mii.p;
	data->reg_num = mii.m;
	data->val_out = 0;
	if (ioctl (g_swt_sock, SIOCGSWTREG, &iwr) == -1) {
		fprintf(stderr,"Cannot get PHY %d %d",mii.p,mii.m);
		perror("");
	}

	//fprintf(stderr, "PHY %d %d is currently 0x%04X\n",mii.p,mii.m,data->val_out);
	return data->val_out;
}


static unsigned long phy_port_getloss(void)
{
	struct switch_nvr_msg *swtnvr_msg;
	struct ifreq iwr;

	if (g_swt_sock < 0){
		return 0;
	}
	
	(void) strncpy (iwr.ifr_name, ETH_NETWORK_DEV0, sizeof (ETH_NETWORK_DEV0));
	swtnvr_msg = (struct switch_nvr_msg *) &iwr.ifr_data;
	swtnvr_msg->port_state = 0;
	swtnvr_msg->port_num = 0;
	if (ioctl (g_swt_sock, SIOCGPORTSTA, &iwr) == -1) {
		fprintf(stderr,"Cannot getloss!\n");
		perror("");
	}

	//fprintf(stderr, "ports current status: 0x%04X\n",data->val_out);
	return swtnvr_msg->port_state;
}


unsigned long SWT_phy_port_getloss(void)
{
	return phy_port_getloss();
}


int SWT_Mac_hit_port(int* mac)
{
	struct switch_nvr_msg *swtnvr_msg;
	struct ifreq iwr;

	if (g_swt_sock < 0){
		return -1;
	}
	
	(void) strncpy (iwr.ifr_name, ETH_NETWORK_DEV0, sizeof (ETH_NETWORK_DEV0));
	swtnvr_msg = (struct switch_nvr_msg *)&iwr.ifr_data;
	//printf("mac_port:%02x,%02x,%02x,%02x,%02x,%02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	//memcpy(swtnvr_msg->mac, mac, 6);
	swtnvr_msg->mac[0] = mac[0];
	swtnvr_msg->mac[1] = mac[1];
	swtnvr_msg->mac[2] = mac[2];
	swtnvr_msg->mac[3] = mac[3];
	swtnvr_msg->mac[4] = mac[4];
	swtnvr_msg->mac[5] = mac[5];

	swtnvr_msg->port_num = -1;

	if (ioctl(g_swt_sock, SIOCFINDPORT, &iwr) == -1){
		printf("SWT_Mac_hit_port fail\n");
	}

	printf("currently port is %d.\n",swtnvr_msg->port_num);
	return swtnvr_msg->port_num;
}

static int _swt_save_config(void)
{
	int i;
	
	for(i = 0; i < SWITCH_CONFIG_REG_NUM; i++)
	{
		switch_config[i].val =  phy_mii_read_reg(switch_config[i].phy, switch_config[i].mii);	
	}
	return 0;
}

int SWT_Init(void)
{
	printf("DNVR Switch(D007):WAN port is %d, VLAN mode is %d\n", WAN_PORT_NO,SWITCH_VLAN_MODE);
	phy_dev_close();
	phy_dev_open();
	
	_swt_set_router_config();
	_swt_print_router_config(); 
	_swt_age_time_set(30);
	_swt_save_config();

	return 0;
}


void SWT_Destroy(void)
{
	phy_dev_close();
}

static int _swt_check_config(void)
{
	int i;
	unsigned short reg_val;
	
	for(i = 0; i < SWITCH_CONFIG_REG_NUM; i++)
	{
		reg_val =  phy_mii_read_reg(switch_config[i].phy, switch_config[i].mii);	
		if(reg_val != switch_config[i].val)
		{
			printf("Switch register[%d(0x%04x),%d(0x%04x)] different:current=0x%04x,save=0x%04x\n", switch_config[i].phy, switch_config[i].phy,
				switch_config[i].mii, switch_config[i].mii, reg_val, switch_config[i].val);
			return 1;
		}
	}	
	return 0;
}

int SWT_keep_config(void)
{
	int i;

	for(i=0; i < 3; i++)
	{
		printf("Switch check config %d\n", i);
		if(_swt_check_config() == 0)
		{
			return 0;
		}	
		sleep(1);
	}
	_swt_print_router_config();
	printf("Switch print before restore\n");

	struct ifreq iwr;
	(void) strncpy (iwr.ifr_name, ETH_NETWORK_DEV0, sizeof (ETH_NETWORK_DEV0));
	
	//re-config
	if (ioctl(g_swt_sock, SIOCSWTRESET, &iwr) == -1){
		printf("Switch restore fail\n");
	}
	SWT_Destroy();
	SWT_Init();

	printf("Switch restore completed\n");
	return 1;
}


static void phy_mii_print_reg(int phy_id, int reg_num)
{
    reg sw_reg;
	unsigned short val;
	
	sw_reg.p = phy_id;
	sw_reg.m = reg_num;    
    val = phy_mii_read(sw_reg);

	#ifndef SWITCH_IP1725
	printf("phy_id=%02d,reg_num=%02d,val=0x%08x\n",phy_id,reg_num,val);
	#else
	printf("reg_num=0x%02x,val=0x%08x\n",reg_num,val);
	#endif
}


static void phy_mii_write_reg(int phy_id, int reg_num, unsigned short val)
{
	int ret;
	reg sw_reg;

	sw_reg.p = phy_id;
	sw_reg.m = reg_num;
	ret = phy_mii_write(sw_reg,val);
	if (ret)
		printf("write fail(%d):phy_id=%02d,reg_num=%02d,val=0x%08x\n",ret,phy_id,reg_num,val);
}

static unsigned short phy_mii_read_reg(int phy_id, int reg_num)
{
    reg sw_reg;
	unsigned short val;
	
	sw_reg.p = phy_id;
	sw_reg.m = reg_num;    
    val = phy_mii_read(sw_reg);

	return val;
}


#ifndef SWITCH_IP1725
#define AGE_TIME_UNIT_ONE_SEC	1
#define  AGE_TIME_UNIT_OFFSET	5
static void _swt_age_time_set(unsigned short sec)
{
	reg sw_reg = {20,14};
	unsigned short val;

	/*AGE_TIME_VLE,14[4:0]
	  Age Time Value
	  5¡¯h00: no aging
	*/
	if(sec > 0x1F)
		sec = 0x1F;
	else if(sec <= 0)
		sec = 1;
	
	val = phy_mii_read(sw_reg);
	val &= ~(0x3 << AGE_TIME_UNIT_OFFSET);
	val |= AGE_TIME_UNIT_ONE_SEC << AGE_TIME_UNIT_OFFSET | sec;
	phy_mii_write(sw_reg, val);

	val = phy_mii_read(sw_reg);
	printf("---IP175D age time unit:0x%x, age time value:0x%x \n",val&0x60,val&0x1f);
}

int _swt_print_router_config(void)
{
	phy_mii_print_reg(20, 13);
	phy_mii_print_reg(21, 18);
	phy_mii_print_reg(22, 0);
	phy_mii_print_reg(22, 4);
	phy_mii_print_reg(22, 5);
	phy_mii_print_reg(22, 6);
	phy_mii_print_reg(22, 7);
	phy_mii_print_reg(22, 8);
	phy_mii_print_reg(22, 9);
	phy_mii_print_reg(22, 10);
	phy_mii_print_reg(22, 14);
	phy_mii_print_reg(22, 15);
	phy_mii_print_reg(22, 16);
	phy_mii_print_reg(22, 17);
	phy_mii_print_reg(22, 18);
	phy_mii_print_reg(23, 0);
	phy_mii_print_reg(23, 1);
	phy_mii_print_reg(23, 2);
	phy_mii_print_reg(23, 8);
	phy_mii_print_reg(23, 9);
	phy_mii_print_reg(23, 10);	
	phy_mii_print_reg(23, 16);
	phy_mii_print_reg(23, 17);
	phy_mii_print_reg(23, 18);

	return 0;
}

#ifdef _DNVR_VLAN
int _swt_set_router_config(void)
{
	unsigned short val;

	//val = 0x1;
	//phy_mii_write_reg(20, 13, val);
	val = (0x1F << 0) | (0x1 << 5) | (0x0 << 6);
	phy_mii_write_reg(22, 0, val);
	val = 0x1;
	phy_mii_write_reg(22, 4, val);
	val = 0x1;
	phy_mii_write_reg(22, 5, val);
	val = 0x1;
	phy_mii_write_reg(22, 6, val);
	val = 0x1;
	phy_mii_write_reg(22, 7, val);
	val = 0x2;
	phy_mii_write_reg(22, 8, val);
	val = 0x1;
	phy_mii_write_reg(22, 9, val);
	val = 0x3;
	phy_mii_write_reg(22, 10, val);
	val = (0x1 << 0) | (0x1 << 12);
	phy_mii_write_reg(22, 14, val);
	val = (0x2 << 0) | (0x2 << 12);
	phy_mii_write_reg(22, 15, val);
	val = (0x2f << 0) | (0x30 << 8);
	phy_mii_write_reg(23, 0, val);
	val = (0x1 << 5) | (0x1 << 13);
	phy_mii_write_reg(23, 8, val);
	val = (0x1f << 0) | (0x1f << 8);
	phy_mii_write_reg(23, 16, val);		

	printf("router(IP175D) config with VLAN(CPU Port) done!\n");
	return 0;
}
#else
int _swt_set_router_config(void)
{
	unsigned short val;

	//val = 0x1;
	//phy_mii_write_reg(20, 13, val);
	val = (0x1F << 0) | (0x1 << 5) | (0x0 << 6);
	phy_mii_write_reg(22, 0, val);
	val = 0x1;
	phy_mii_write_reg(22, 4, val);
	val = 0x1;
	phy_mii_write_reg(22, 5, val);
	val = 0x1;
	phy_mii_write_reg(22, 6, val);
	val = 0x1;
	phy_mii_write_reg(22, 7, val);
	val = 0x2;
	phy_mii_write_reg(22, 8, val);
	val = 0x3;  //0x1
	phy_mii_write_reg(22, 9, val);
	val = 0x7;  //0x3
	phy_mii_write_reg(22, 10, val);
	val = (0x1 << 0) | (0x1 << 12);
	phy_mii_write_reg(22, 14, val);
	val = (0x2 << 0) | (0x2 << 12);
	phy_mii_write_reg(22, 15, val);
	val = (0x3 << 0) | (0x3 << 12);
	phy_mii_write_reg(22, 16, val);
	val = (0x2f << 0) | (0x30 << 8);
	phy_mii_write_reg(23, 0, val);
	val = (0x3f << 0) ;
	phy_mii_write_reg(23, 1, val);
	//val = (0x1 << 5) | (0x1 << 13);
	//phy_mii_write_reg(23, 8, val);
	//val = (0x1f << 0) | (0x1f << 8);
	val = (0x3f << 0) | (0x3f << 8);
	phy_mii_write_reg(23, 16, val);	
	val = (0x3f << 0);
	phy_mii_write_reg(23, 17, val);	

	printf("router(IP175D) config without VLAN(CPU Port) done!\n");
	return 0;
}

#endif

#else  //SWITCH_IP1725
static void _swt_age_time_set(unsigned short sec)
{
	unsigned short val;
	
	/*Reg Addr. 89h,default value 15'hA
	  Aging timer threshold setting
	  bit[14:0] : aging timer, the aging time is about :
	  (mg_aging_time + 1) x 27.6 sec. (+/-3.8%)
	*/
	if(sec > 0x7FFF)
		sec = 0x7FFF;
	val = sec / 27.6;
	val = val & 0x7FFF;
	if(val > 1)
		val = val - 1;
	else
		val = 0;
	phy_mii_write_reg(PHY_ID_NONE, 0x89, val);	
	printf("---IP1725 age time value:(0x%x)*27.6 sec\n",val+1);
}


int _swt_print_router_config(void)
{
	unsigned short i,val;

	phy_mii_print_reg(PHY_ID_NONE, 0x02);
	phy_mii_print_reg(PHY_ID_NONE, 0x47);
	phy_mii_print_reg(PHY_ID_NONE, 0x48);
	phy_mii_print_reg(PHY_ID_NONE, 0x56);
	phy_mii_print_reg(PHY_ID_NONE, 0x58);
	phy_mii_print_reg(PHY_ID_NONE, 0x5A);
	phy_mii_print_reg(PHY_ID_NONE, 0x70);
	phy_mii_print_reg(PHY_ID_NONE, 0x76);
	phy_mii_print_reg(PHY_ID_NONE, 0x82);
	phy_mii_print_reg(PHY_ID_NONE, 0x89);
	phy_mii_print_reg(PHY_ID_NONE, 0x8A);
	phy_mii_print_reg(PHY_ID_NONE, 0x8B);
	phy_mii_print_reg(PHY_ID_NONE, 0x95);
	phy_mii_print_reg(PHY_ID_NONE, 0x96);
	phy_mii_print_reg(PHY_ID_NONE, 0x97);
	phy_mii_print_reg(PHY_ID_NONE, 0x98);
	phy_mii_print_reg(PHY_ID_NONE, 0x99);
	phy_mii_print_reg(PHY_ID_NONE, 0x9A);
	for(i=0x9B;i<=0xA3;i++)
		phy_mii_print_reg(PHY_ID_NONE, i);	
	for(i=0xA4;i<=0xA8;i++)
		phy_mii_print_reg(PHY_ID_NONE, i);
	phy_mii_print_reg(PHY_ID_NONE, 0xA9);
	phy_mii_print_reg(PHY_ID_NONE, 0xAA);
	phy_mii_print_reg(PHY_ID_NONE, 0xAB);
	phy_mii_print_reg(PHY_ID_NONE, 0xAC);
	for(i=0xAD;i<=0xB2;i++)
		phy_mii_print_reg(PHY_ID_NONE, i);
	for(i=0;i<=31;i++)
	{		
		phy_mii_write_reg(PHY_ID_NONE,ACL_VLAN_CTRL,((i & 0x1f) | 0x8200));
		val = phy_mii_read_reg(PHY_ID_NONE, VLAN_DATA);
		printf("entry %d vlan_id:0x%x\n",i,val);
	}
}

int _swt_set_router_config(void)
{
	unsigned short val,idx;

	/*Reg Addr. 47h, R/W, default value is 16'h0
	  When tag insertion is needed, the inserting PVID/PRI is according to source port or output port. 1bit per port for port 01~port 16
	  bit[15:0] : 0 : base on source port
	              1 : base on output port
	*/

	/* Reg. Addr. 54h, R/W, default value is 9'h23
	LED mode settings
	bit[1:0] : LED mode
	11 : direct mode (L/A) (default)
	10 : 3-bit for serial mode bi-color (C/F, L/A, Spd)
	01 : 3-bit for serial mode (C/F, L/A, Spd)
	00 : 2-bit for serial mode (Spd, L/A )
	bit[3:2] : LED blink time
	00 : 40 ms
	01 : 80 ms
	10 :120 ms
	11 :160 ms
	bit[5:4] : LED clock rate
	00 : 781 KHz
	01 : 2.5 MHz
	10 : 5 MHz
	11 : 10 MHz
	bit[6] : LED Col blink disable
	0 : will blink when Col
	1 : don¡¯t blink when Col
	bit[7] : LED act (Tx/Rx) blink disable
	0 : will blink when act
	1 : don¡¯t blink when act
	bit[8] : LED port number control in fiber mode
	0 : normal
	1 : 19 ports mode
	*/
	val = 0x20;  // 2-bit mode
	phy_mii_write_reg(PHY_ID_NONE, 0x54, val);

	
	/*Reg Addr. 48h, R/W, default value is 10'h0
	  When tag insertion is needed, the inserting PVID/PRI is according to source port or output port. 1bit per port for port 17~port 25
	  bit[8:0] : 0 : base on source port
	            1 : base on output port
	  When priority tag is modified, the PRI bit in tag is modified or not
	  bit[9] : 0 : do not modified
	           1 : modified by the mg_ins_tag_cfg setting
	*/

	/*Reg Addr. 70h, R/W, 2'h2
	  CPU mode setting
	  bit[0] : port 25 linked to CPU
	  bit[1] : CPU interface RvMII mode
	*/
	val = 0x3;
	phy_mii_write_reg(PHY_ID_NONE, 0x70, val);


	/*Reg Addr. 82h, R/W, default value is 16'h1
	  bit[0] : hash algorithm selection,0:direct map base,1:CRC map base
	  bit[1] : address table aging function disable,0:aging enable,1:aging disable
	  bit[6] : pass all IPv4 packet when block broadcast frame to CPU port,0:disable pass,1:enable pass
	  bit[7] : VLAN type,0:port based VLAN,1:802.1Q tag based VLAN
	  bit[8] : unicast packet stride across VLAN
	  bit[11] : select the tag/untag method,0:base on Ports (ref. reg 0x97~0x9A),1:base on VIDs (ref. reg 0xFA and 0xFC)	
	  bit[12] : modify tag in VID_TAG mode for CPU port,0:do not modify tag,1:modify tag by setting register 0xFA and 0xFC
	*/
	val = phy_mii_read_reg(PHY_ID_NONE, 0x82);
	#if(SWITCH_VLAN_MODE==SWITCH_VLAN_NONE)
		val &= ~(0x1 << 7);
		phy_mii_write_reg(PHY_ID_NONE, 0x82, val);
		printf("router(IP1725) config without VLAN done!\n");
		return 0;
	#else
		val |= (0x1 << 7);
		val &= ~( (0x1 << 11) | (0x1 << 12)  );
		phy_mii_write_reg(PHY_ID_NONE, 0x82, val);
	#endif

	/*Reg Addr. 95h, R/W, default value is 16'h0
	  Indicates which port is router port for port 01~port 16, 1 bit per port
	  bit[15:0] : 0 : this port is a normal port
	              1 : this port is a router port
	*/
	
	/*Reg Addr. 96h, R/W, default value is 9'h0
	  Indicates which port is router port for port 17~port 25, 1 bit per port
	  bit[8:0] : 0 : this port is a normal port
	           1 : this port is a router port
	*/
	
	/*Reg Addr. 97h	R/W,	default value is 16'h0
	  VLAN output port add tag operation configuration for port 01~port 16, 1 bit per port
	  bit[15:0] : output frame add tag
	    0 : disable
	    1 : add VLAN tag to output frame
	*/
	#if(SWITCH_VLAN_MODE==SWITCH_VLAN_HALF)
		val = CPU_PORT_BITMASK & 0xFFFF;
		phy_mii_write_reg(PHY_ID_NONE, 0x97, val);
	#endif
	
	/*Reg Addr. 98h, R/W, default value is 9'h0
	  VLAN output port add tag operation configuration for port 17~port 25, 1 bit per port
	  bit[8:0] : output frame add tag
	    0 : disable
	    1 : add VLAN tag to output frame
	*/
	#if(SWITCH_VLAN_MODE==SWITCH_VLAN_HALF)
		val = (CPU_PORT_BITMASK >> 16) & 0x1FF;
		phy_mii_write_reg(PHY_ID_NONE, 0x98, val);
	#endif
	
	/*Reg Addr. 99h, R/W, default value is 16'h0
	  VLAN output port remove tag operation configuration for port 01~port 16, 1 bit per port
	  bit[15:0] : output frame remove tag
	    0 : disable
	    1 : remove VLAN tag to output frame
	*/
	#if(SWITCH_VLAN_MODE==SWITCH_VLAN_HALF)
		val = (~CPU_PORT_BITMASK) & 0xFFFF;  
		phy_mii_write_reg(PHY_ID_NONE, 0x99, val);
	#else
		val = (CPU_PORT_BITMASK | LAN_PORT_BITMASK | WAN_PORT_BITMASK) & 0xFFFF;
		phy_mii_write_reg(PHY_ID_NONE, 0x99, val);
	#endif
	
	//Reg Addr. 9Ah, R/W, default value is 9'h0
	//VLAN output port remove tag operation configuration for port 17~port 25, 1 bit per port
	//bit[8:0] : output frame remove tag
	//  0 : disable
	//  1 : remove VLAN tag to output frame
	#if(SWITCH_VLAN_MODE == SWITCH_VLAN_HALF)
		val = (~CPU_PORT_BITMASK >> 16) & 0x1FF;  
		phy_mii_write_reg(PHY_ID_NONE, 0x9A, val);
	#else
		val = ((CPU_PORT_BITMASK | LAN_PORT_BITMASK | WAN_PORT_BITMASK) >> 16) & 0x1FF;
		phy_mii_write_reg(PHY_ID_NONE, 0x9A, val);
	#endif

	/*Reg Addr. 9Bh, R/W, default value is 15'h0
	  PVID index setting for port 01~port 03
	  bit[4:0] : PVID configuration for port 01
	  bit[9:5] : PVID configuration for port 02
	  bit[14:10] : PVID configuration for port 03
	*/
	//LAN port PVID index is all entry 0
	val = (0x0 << 0) | (0x0 << 5) | (0x0 << 10);
	for(idx=0x9B;idx<=0xA2;idx++)
		phy_mii_write_reg(PHY_ID_NONE, idx, val);
	//WAN port PVID index is entry 1
	val = phy_mii_read_reg(PHY_ID_NONE, 0x9B+((WAN_PORT_NO-1)/3));
	val &= ~(0x1F << (5*((WAN_PORT_NO-1)%3)));
	val |= 0x1 << (5*((WAN_PORT_NO-1)%3));
	phy_mii_write_reg(PHY_ID_NONE, 0x9B+((WAN_PORT_NO-1)/3), val);

	/*Reg Addr. A3h, R/W, default value is 5'h0
	  PVID index setting for port 25
	  bit[4:0] : PVID configuration for port 25
	*/
	//CPU port(port 25) PVID index is entry 2
	val = (0x2 << 0);
	phy_mii_write_reg(PHY_ID_NONE, 0xA3, val);

	/*Reg Addr. A9h, R/W, default value is 16'h0
	  Force the incoming packet use PVID as 802.1Q VLAN check for port 01~port 16
	  bit[15:0] : 0 : don¡¯t force
	             1 : force to use PVID
	*/
	
	/*Reg Addr. AAh, R/W, default value is 9'h0
	  Force the incoming packet use PVID as 802.1Q VLAN check for port 17~port 25
	  bit[8:0] : 0 : don¡¯t force
	             1 : force to use PVID
	*/
	
	/*Reg Addr. ABh,R/W,16'hFFFF
	  Check if the input port is in the VLAN group for port 01~port 16 (ingress check)
	  bit[15:0] : 0 : don¡¯t check if the input port is in the group
	              1 : check if the input port is in the group. If not, drop.
	*/
	
	/*Reg Addr. ACh, R/W, 9'h1FF
	  Check if the input port is in the VLAN group for port 17~port 25 (ingress check)
	  bit[8:0] : 0 : don¡¯t check if the input port is in the group
	             1 : check if the input port is in the group. If not, drop.
	*/

	/*Reg Addr. ADh, R/W, 16'hFFFF
	  VLAN member configuration for VLAN table entry 0 or port 01 port base VLAN setting.
	  bit[15:0] : 0 : not in VLAN group
	              1 : in VLAN group
	  Reg Addr. AEh, R/W, 9'h1FF
	  VLAN member configuration for VLAN table entry 0 or port 01 port base VLAN setting.
	  bit[8:0] : 0 : not in VLAN group
	             1 : in VLAN group
	*/
	//entry 0 VLAN member:CPU+LAN
	val = (CPU_PORT_BITMASK | LAN_PORT_BITMASK) & 0xFFFF;
	phy_mii_write_reg(PHY_ID_NONE, 0xAD, val);
	val = ((CPU_PORT_BITMASK | LAN_PORT_BITMASK) >> 16) & 0x1FF;
	phy_mii_write_reg(PHY_ID_NONE, 0xAE, val);
	//entry 1 VLAN member:CPU+WAN
	val = (CPU_PORT_BITMASK | WAN_PORT_BITMASK) & 0xFFFF;
	phy_mii_write_reg(PHY_ID_NONE, 0xAF, val);
	val = ((CPU_PORT_BITMASK | WAN_PORT_BITMASK) >> 16) & 0x1FF;
	phy_mii_write_reg(PHY_ID_NONE, 0xB0, val);
	//entry 2 member:ALL=CPU+WAN+LAN
	val = (CPU_PORT_BITMASK | LAN_PORT_BITMASK | WAN_PORT_BITMASK) & 0xFFFF;
	phy_mii_write_reg(PHY_ID_NONE, 0xB1, val);
	val = ((CPU_PORT_BITMASK | LAN_PORT_BITMASK | WAN_PORT_BITMASK) >> 16) & 0x1FF;
	phy_mii_write_reg(PHY_ID_NONE, 0xB2, val);
	for(idx=3;idx<=31;idx++)
	{
		val = 0;
		phy_mii_write_reg(PHY_ID_NONE, (idx*2)+0xAD, val);
		phy_mii_write_reg(PHY_ID_NONE, (idx*2+1)+0xAD, val);
	}

	//VLAN entry 
	//VLAN entry 0 VID=1
	idx = 0;
	phy_mii_write_reg(PHY_ID_NONE, VLAN_DATA,0x1);
	phy_mii_write_reg(PHY_ID_NONE, ACL_VLAN_CTRL, (0xC200 | (idx & 0x1f)));
	//VLAN entry 1 VID=2
	idx = 1;
	phy_mii_write_reg(PHY_ID_NONE, VLAN_DATA, 0x2);
	phy_mii_write_reg(PHY_ID_NONE, ACL_VLAN_CTRL, (0xC200 | (idx & 0x1f)));
	//VLAN entry 2 VID=3
	idx = 2;
	phy_mii_write_reg(PHY_ID_NONE, VLAN_DATA, 0x3);
	phy_mii_write_reg(PHY_ID_NONE, ACL_VLAN_CTRL, (0xC200 | (idx & 0x1f)));

	printf("router(IP1725) config 49 done!\n");
	return 0;
}

#endif  //SWITCH_IP1725

#endif  //_DNVR
