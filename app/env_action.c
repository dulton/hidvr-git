/*
 * env_action.c
 *
 *  Created on: 2011-4-6
 *      Author: root
 */
#include <signal.h>
#include <string.h>
#include <errno.h>

#include "std.h"
#include "bsp.h"
#include "dvr_util.h"
#include "environment.h"
#include "common.h"
#include "osd.h"
#include "gpio.h"
#include "video.h"
#include "audio.h"
#include "conf.h"
#include "sdk/sdk_api.h"
#include "overlay.h"
#include "env_action.h"
#include "smtp.h"
#include "dvr_command.h"


extern Sysenv* g_pstSysEnv;
extern int socket_index_upnpsvr;


#ifndef ETH_NETWORK_DEV
#define ETH_NETWORK_DEV "eth0"
#endif

//#define MINIWEB_BASE_CONF "/tmp/base.conf"
//static void _miniweb_init()
//{
//	char cmd[64];
//
////	sprintf(cmd, "server.port=%d\nserver.document-root=\"/root/dvr_web/www\"\n", g_pstSysEnv->GetHTTPPort(g_pstSysEnv));
////	FILE* fp = fopen(MINIWEB_BASE_CONF, "w+");
////	fwrite(cmd, 1, strlen(cmd), fp);
////	fclose(fp);
////	TRACE_INFO("Written %s", MINIWEB_BASE_CONF);
////	TRACE_INFO("%s", cmd);
////
////	sprintf(cmd, "%s", "lighttpd -f /etc/lighttpd/lighttpd.conf");
////	TRACE_INFO("%s", cmd);
////	system(cmd);
//	sprintf(cmd, "thttpd -d /root/dvr_web/www -p %d", g_pstSysEnv->GetHTTPPort(g_pstSysEnv));
//	TRACE_INFO("%s", cmd);
//	system(cmd);
//}
//
//static void _miniweb_deinit()
//{
//	char cmd[64];
//
//	sprintf(cmd, "kill -SIGKILL `pidof thttpd`; kill -9 `pidof flv.cgi`");
//	TRACE_INFO("%s", cmd);
//	system(cmd);
//}
//
//void XENVACT_miniweb_init()
//{
//	_miniweb_init();
//}
//
//void XENVACT_miniweb_deinit()
//{
//	_miniweb_deinit();
//}

//void XENVACT_miniweb_reload()
//{
//	_miniweb_deinit();
//	_miniweb_init();
//}
//
//static void _avstreamsrv_init()
//{
//	char cmd[64];
//
//	sprintf(cmd, "crond -b; cd /root; ./avstreamsvr & cd /root/dvr_app");
//	TRACE_INFO("%s", cmd);
//	system(cmd);
//}
//
//static void _avstreamsrv_deinit()
//{
//	char cmd[64];
//
//	sprintf(cmd, "kill -9 `pidof crond`; kill -9 `pidof avstreamsvr`");
//	TRACE_INFO("%s", cmd);
//	system(cmd);
//	sleep(3);
//
////	FILE* fp = fopen("/tmp/avstreamsvr.pid", "r");
////	if(fp != NULL)
////	{
////		char pid_buf[16];
////		fgets(pid_buf, 16, fp);
////		fclose(fp);
////		int pid = atoi(pid_buf);
////		TRACE_INFO("found avstreamsvr pid=%s", pid_buf);
////
////		kill(pid, SIGUSR1);
////
////		int i;
////		char path[256];
////		sprintf(path, "/proc/%s/status", pid_buf);
////		for(i = 0; i < 10; i++)
////		{
////			if(!IsFileExist(path))
////			{
////				TRACE_DEBUG("avstreamsvr proc exit");
////				break;
////			}
////			usleep(500*1000);
////		}
////	}
//}
//
//void XENVACT_avstreamsrv_init()
//{
//	return;
//	_avstreamsrv_init();
//}
//
//void XENVACT_avstreamsrv_deinit()
//{
//	return;
//	_avstreamsrv_deinit();
//}
//
//void XENVACT_avstreamsrv_reload()
//{
//	return;
//	_avstreamsrv_deinit();
//	_avstreamsrv_init();
//}

int XENVACT_set_ip_running = 0;
void XENVACT_set_ip()
{
	if(XENVACT_set_ip_running == 1)
	{
		TRACE_DEBUG("Ignore %s running.", __FUNCTION__);
		return;
	}
	XENVACT_set_ip_running = 1;
	unsigned char bDHCPUse;
	unsigned char u8MACAddr[MAC_ADDR_LEN];
	unsigned char u8IPAddr[IP_ADDR_LEN];
	unsigned char u8SubMask[IP_ADDR_LEN];
	unsigned char u8Gateway[IP_ADDR_LEN];
	unsigned char u8DNSAddr[IP_ADDR_LEN];

	bDHCPUse = g_pstSysEnv->IsDHCPUse(g_pstSysEnv);
	g_pstSysEnv->GetMACAddr(g_pstSysEnv, u8MACAddr);
	g_pstSysEnv->GetIPAddr(g_pstSysEnv, u8IPAddr);
	g_pstSysEnv->GetSubMask(g_pstSysEnv, u8SubMask);
	g_pstSysEnv->GetGateway(g_pstSysEnv, u8Gateway);
	g_pstSysEnv->GetDNSAddr(g_pstSysEnv, u8DNSAddr);

	#ifdef _DNVR
		TRACE_DEBUG("config netcard!");
		config_netcard(u8IPAddr);
	#endif
	
	TRACE_DEBUG("setup mac address!");
	set_if_property(NULL, NULL, u8MACAddr, NULL);

	char cmd_buf[100];
//	sprintf(cmd_buf, "ifconfig eth0 hw ether %x:%x:%x:%x:%x:%x",
//			u8MACAddr[0], u8MACAddr[1], u8MACAddr[2],
//			u8MACAddr[3], u8MACAddr[4], u8MACAddr[5]);
//	TRACE_DEBUG("%s", cmd_buf);
//	system(cmd_buf);

	if(bDHCPUse == 1)
	{
		TRACE_DEBUG("use dhcp ip!");
		#ifdef _DNVR
			sprintf(cmd_buf, "udhcpc -n -q -i %s &", ETH_NETWORK_DEV);
		#else
			sprintf(cmd_buf, "udhcpc -n -q &");
		#endif
		TRACE_DEBUG("%s", cmd_buf);
		system(cmd_buf);
	}
	else
	{
		TRACE_DEBUG("use static ip!");

		set_if_property(u8IPAddr, u8SubMask, NULL, u8Gateway);

		sprintf(cmd_buf, "nameserver %d.%d.%d.%d\nnameserver 8.8.8.8\n", u8DNSAddr[0], u8DNSAddr[1], u8DNSAddr[2], u8DNSAddr[3]);
		FILE* fp = fopen("/tmp/resolv-sys.conf", "w+");
		ASSERT(fp != NULL, "fp is NULL");
		int ret = fwrite(cmd_buf, 1, strlen(cmd_buf), fp);
		ASSERT(ret == strlen(cmd_buf), "ret = %d:%s", ret, strerror(errno));
		fclose(fp);
		system("cat /tmp/resolv-*.conf > /tmp/resolv.conf");
	}
	#ifdef _DNVR
		set_lan_ip_status(1);
		TRACE_DEBUG("Add multicast route!");
		add_multicast_route(3);   //re-add multicast route after re-config network
	#endif

	system("route add -net 224.0.0.0 netmask 224.0.0.0 eth0;route -e");

	TRACE_DEBUG("setup ip finished!");
	XENVACT_set_ip_running = 0;
}

#define DIAL_SET_FILE "/tmp/dial-set"
#define DIAL_OPT_FILE "/tmp/dial-opt"
#define DIAL_DNS_FILE "/tmp/resolv-ppp.conf"

int XENVACT_PPPSetReady(char * PPPSetFile)
{
    FILE *fd;
    char buf[2*1024];

    if(!PPPSetFile) {
        return -1;
    }

    unlink(PPPSetFile);

    fd = fopen(PPPSetFile, "wb+");
    if(fd != NULL) {
        snprintf(buf, sizeof(buf)-1, "#!/bin/sh\n\n"
                    "TEL=\"%s\"\n"
                    "APN=\"%s\"\n"
                    "PIN=\"%s\"\n"
                    "USR=\"%s\"\n"
                    "PWD=\"%s\"\n"
                    "export TEL APN PIN USR PWD\n\n",
                    g_pstSysEnv->stAttr.stContent.stNetwork.sz3GDIALNum,
                    g_pstSysEnv->stAttr.stContent.stNetwork.sz3GAPN,
                    g_pstSysEnv->stAttr.stContent.stNetwork.sz3GPIN,
                    g_pstSysEnv->stAttr.stContent.stNetwork.sz3GUser,
                    g_pstSysEnv->stAttr.stContent.stNetwork.sz3GPassword);
        fwrite(buf, strlen(buf), 1, fd);
        fclose(fd);

        return 0;
    }

    return -1;
}

char * PPP_OPTS_String = \
"debug\n" \
"logfile /tmp/pppd.log\n" \
"nodetach\n" \
"idle 0\n" \
"lock\n" \
"/dev/ttyUSB0\n" \
"115200\n" \
"asyncmap 0\n" \
"lcp-echo-failure 4\n" \
"lcp-echo-interval 30\n" \
"crtscts\n" \
"show-password\n" \
"noauth\n" \
"novj\n" \
"novjccomp\n" \
"noccp\n" \
"noipdefault\n" \
"defaultroute\n" \
"usepeerdns\n" \
"ipcp-accept-local\n" \
"ipcp-accept-remote\n" \
"connect '/sbin/chat -s -v -f /tmp/dial-cmd'\n";
//"user %s\n"
//"password %s\n"

int XENVACT_PPPOptReady(char * PPPOptFile)
{
    FILE *fd;
    char buf[2*1024];
	
    char * usr = g_pstSysEnv->stAttr.stContent.stNetwork.sz3GUser;
    char * pwd = g_pstSysEnv->stAttr.stContent.stNetwork.sz3GPassword;

    if(!PPPOptFile) {
        return -1;
    }

    unlink(PPPOptFile);

    fd = fopen(PPPOptFile, "wb+");
    if(fd != NULL) {
        snprintf(buf, sizeof(buf)-1,
                    "%s",
                    PPP_OPTS_String);
        fwrite(buf, strlen(buf), 1, fd);
        if(strlen(usr)) {
            snprintf(buf, sizeof(buf)-1,
                    "user %s\n""password %s\n",
                    usr,
                    strlen(pwd) ? pwd : "null");
            fwrite(buf, strlen(buf), 1, fd);
        }
        fclose(fd);

        return 0;
    }

    return -1;
}

static int XENVACT_set_ppp_running = 0;

void* XENVACT_PPP_Proc(void* arg)
{
    if(!XENVACT_set_ppp_running) {
        pthread_exit(NULL);
    }

    int  countdown;
    char cmd_buf[100];
    TRACE_DEBUG("use ppp0 ip!");

    sprintf(cmd_buf, "sh /etc/ppp/ppp-bgn &");
    TRACE_DEBUG("%s", cmd_buf);
    system(cmd_buf);

    countdown = 40;
    do {
        sleep(1);

        if( 0 == ppp_check_active()) {
            sleep(1);
            ppp_set_route_by_ip();

            system("cat /tmp/resolv-*.conf > /tmp/resolv.conf");
            break;
        }
    } while(countdown --);

	TRACE_DEBUG("setup ppp0 finished!");
	XENVACT_set_ppp_running = 0;

	pthread_exit(NULL);
}

void XENVACT_PPP_Bgn(void)
{
	if(XENVACT_set_ppp_running == 1)
	{
		TRACE_DEBUG("Ignore %s running.", __FUNCTION__);
		return;
	}
	XENVACT_set_ppp_running = 1;
	unsigned char bPPPUse = g_pstSysEnv->Is3GUse(g_pstSysEnv);

    if(0 != XENVACT_PPPSetReady(DIAL_SET_FILE)
    || 0 != XENVACT_PPPOptReady(DIAL_OPT_FILE)) {
        return;
    }

	if(bPPPUse == 1) {
        pthread_t PPPPid = 0;
        if(0 != pthread_create(&PPPPid, NULL, XENVACT_PPP_Proc, NULL)) {
            XENVACT_set_ppp_running = 0;
        }
	}
}

void XENVACT_PPP_End(void)
{
	if(XENVACT_set_ppp_running == 1)
	{
		TRACE_DEBUG("Ignore %s running.", __FUNCTION__);
		return;
	}
	XENVACT_set_ppp_running = 1;
	unsigned char bPPPUse = g_pstSysEnv->Is3GUse(g_pstSysEnv);

    char cmd_buf[100];
    TRACE_DEBUG("Exit ppp0!");

    sprintf(cmd_buf, "sh /etc/ppp/ppp-end");
    TRACE_DEBUG("%s", cmd_buf);
    system(cmd_buf);

	TRACE_DEBUG("Exit ppp0 finished!");
	XENVACT_set_ppp_running = 0;
}


static int XENVACT_set_pppoe_running = 0;
static int XENVACT_pppoe_thead_keep = 0;
static pthread_t XENVACT_pppoe_thread_id = 0;
void XENVACT_PPPoE_Config(void)
{
	char cmd_buf[255];
	char user_buf[50];
	char pwd_buf[50];
	
    TRACE_DEBUG("config ppp0(pppoe)...");
	g_pstSysEnv->GetPPPoEUser(g_pstSysEnv,user_buf);
	g_pstSysEnv->GetPPPoEPwd(g_pstSysEnv,pwd_buf);
    sprintf(cmd_buf, "pppoe-config %s %s %s",user_buf,pwd_buf,ETH_NETWORK_DEV);
    TRACE_DEBUG("%s", cmd_buf);
    system(cmd_buf);

	TRACE_DEBUG("config ppp0(pppoe) finished!");

}


void* XENVACT_PPPoE_Proc(void* arg)
{
    if(!XENVACT_set_pppoe_running) {
        pthread_exit(NULL);
    }

    int  countdown,i;
    char cmd_buf[100];
	char gateway[255]={0};
	char ppp_ip[32]={0};
	unsigned char u8SaveGW[IP_ADDR_LEN];
	unsigned char SaveGWString[32];
	int is_ip_notify = 0;
	
    TRACE_DEBUG("ppp0(pppoe) proc...");

    sprintf(cmd_buf, "pppoe-start &");
    TRACE_DEBUG("%s", cmd_buf);
    system(cmd_buf);

	sprintf(cmd_buf, "rm -f /tmp/pppoe.up");
	TRACE_DEBUG("%s", cmd_buf);
	system(cmd_buf);

    countdown = 40;
    while(XENVACT_pppoe_thead_keep)
	{
        sleep(1);
		//FIXME:UPNP_SERVER also use MCASTADDR("239.255.255.250") temporarily and need to avoid confilct.
		if( is_ip_notify != 0 )
		{
			if( get_lan_multicast_status() )
				continue;

			is_ip_notify = 0;
			countdown = 0;
			//FIXME:app2upnp_send_upnp is perhaps invalid
			#if 0
			TRACE_DEBUG("app2upnp_send_upnp,socket=%d",socket_index_upnpsvr);
			app2upnp_send_upnp(socket_index_upnpsvr);
			#else
        	sprintf(cmd_buf, "kill -9 `pidof upnp_server`");
        	TRACE_DEBUG("%s", cmd_buf);
        	system(cmd_buf);
			sleep(1);
        	sprintf(cmd_buf, "/root/upnp_server &");
        	TRACE_DEBUG("%s", cmd_buf);
        	system(cmd_buf);
			#endif
			continue;
		}
		
		if(countdown <= 0)
		{
			i = 0;
			while(XENVACT_pppoe_thead_keep && (i++ <= 60))
				sleep(1);  
			if(!XENVACT_pppoe_thead_keep)
				break;
		}
		else
			countdown--;

		if( 0 != ppp_get_ipaddr(ppp_ip) )
		{
			if(access("/tmp/pppoe.up", 0) == 0)
			{
				sprintf(cmd_buf, "rm -f /tmp/pppoe.up");
				TRACE_DEBUG("%s", cmd_buf);
				system(cmd_buf);
			}

			sleep(1);
			g_pstSysEnv->GetGateway(g_pstSysEnv, u8SaveGW);
			sprintf(SaveGWString,"%d.%d.%d.%d",u8SaveGW[0],u8SaveGW[1],u8SaveGW[2],u8SaveGW[3]);
			if( 0 == get_gateway(gateway) )
			{
				TRACE_DEBUG("get gateway %s",gateway);
				if( 0 == strcmp(SaveGWString,gateway) )
					continue;
			}
			set_if_property(NULL, NULL, NULL, u8SaveGW);	
			TRACE_DEBUG("update gateway to %s",SaveGWString);

			continue;
		}
		
		sleep(1);
		if( 0 == get_gateway(gateway) )
		{
			TRACE_DEBUG("get gateway %s",gateway);
			if( 0 == strcmp(ppp_ip,gateway) )
				continue;
			else
			{
	            sprintf(cmd_buf, "route del default");
	            TRACE_DEBUG("%s", cmd_buf);
	            system(cmd_buf);				
			}			
		}

		sleep(1);
		ppp_set_route_by_ip();

		TRACE_DEBUG("update gateway to %s",ppp_ip);
		if(access("/tmp/pppoe.up", 0) != 0)
		{
			sprintf(cmd_buf, "touch /tmp/pppoe.up");
			TRACE_DEBUG("%s", cmd_buf);
			system(cmd_buf);
		}
	
		is_ip_notify = 1;
    }

	TRACE_DEBUG("ppp0(pppoe) proc finished!");
	XENVACT_set_pppoe_running = 0;

	pthread_exit(NULL);
}


void XENVACT_PPPoE_Start(void)
{
	if(XENVACT_set_pppoe_running == 1)
	{
		TRACE_DEBUG("Ignore %s running.", __FUNCTION__);
		return;
	}
	char cmd_buf[100];
	unsigned char bPPPoEUse = g_pstSysEnv->IsPPPoE(g_pstSysEnv);
	
	XENVACT_set_pppoe_running = 1;
    
    TRACE_DEBUG("start ppp0(pppoe)...");

	XENVACT_PPPoE_Config();
	if(bPPPoEUse)
	{
		XENVACT_pppoe_thead_keep = 1;
        if(0 != pthread_create(&XENVACT_pppoe_thread_id, NULL, XENVACT_PPPoE_Proc, NULL)) {
            XENVACT_set_pppoe_running = 0;
        }
	}
	else
		XENVACT_set_pppoe_running = 0;

	TRACE_DEBUG("start ppp0(pppoe) finished!");
}


void XENVACT_PPPoE_Stop(void)
{
    char cmd_buf[100];
    TRACE_DEBUG("stop ppp0(pppoe)...");

	XENVACT_pppoe_thead_keep = 0;
	if( XENVACT_pppoe_thread_id != 0)
	{
		pthread_join(XENVACT_pppoe_thread_id, NULL);
		XENVACT_pppoe_thread_id = 0;
	}

    sprintf(cmd_buf, "pppoe-stop");
    TRACE_DEBUG("%s", cmd_buf);
    system(cmd_buf);

	sprintf(cmd_buf, "rm -f /tmp/pppoe.up");
	TRACE_DEBUG("%s", cmd_buf);
	system(cmd_buf);

	char gateway[255]={0};
	unsigned char u8SaveGW[IP_ADDR_LEN];
	unsigned char SaveGWString[32];
	g_pstSysEnv->GetGateway(g_pstSysEnv, u8SaveGW);
	
	//restore default gateway
	if( 0 == get_gateway(gateway) )
	{
		TRACE_DEBUG("gateway is %s",gateway);
		sprintf(SaveGWString,"%d.%d.%d.%d",u8SaveGW[0],u8SaveGW[1],u8SaveGW[2],u8SaveGW[3]);
		if( 0 != strcmp(SaveGWString,gateway) )
		{
			sprintf(cmd_buf, "route del default");
			TRACE_DEBUG("%s", cmd_buf);
			system(cmd_buf);
			set_if_property(NULL, NULL, NULL, u8SaveGW);
		}
	}
	else
		set_if_property(NULL, NULL, NULL, u8SaveGW);	


	TRACE_DEBUG("stop ppp0(pppoe) finished!");
}

static int _read_parament_file(const char* pathFile, SMTP_INFO *info)
{
	FILE* fp = NULL;
	int rlt = 0;
	fp = fopen(pathFile, "rb");

	if(fp == NULL)
	{
		rlt = -1;
		return rlt;
	}
	memset(info, 0, sizeof(SMTP_INFO));	
	int read_count = fread(info, sizeof(SMTP_INFO), 1, fp);
	if(read_count != 1)
	{
		rlt = -1;
		TRACE_DEBUG("Read fail for SMTP parament file");
	}

	fclose(fp);
	fp = NULL;
	return rlt;
}

void OSD_RELOAD(unsigned char bTime, unsigned char bTitle)
{
	int i = 0;
	char szTitle[100] = {""};
	if(bTime){
		switch(g_pstSysEnv->GetDateFmt(g_pstSysEnv))
		{
		default:
		case YYYY_MM_DD:
			OSD_SetDateFormat("yyyymmdd", NULL);
			break;
		case MM_DD_YYYY:
			OSD_SetDateFormat("mmddyyyy", NULL);
			break;
		case DD_MM_YYYY:
			OSD_SetDateFormat("ddmmyyyy", NULL);
			break;
		}
	}
	if(bTitle){
		for(i = 0; i < MAX_CAM_CH; ++i){
			g_pstSysEnv->GetTitle(g_pstSysEnv, i, szTitle);
			OSD_SetTitle(i, szTitle);
		}
	}
	OSD_Refresh(2);
	OSD_SetAlphaScale(g_pstSysEnv->GetAlpha(g_pstSysEnv));
}

void OSD_INIT(int nVGAW, int nVGAH, int nCVBSW, int nCVBSH)
{
	OSD_Init(nVGAW, nVGAH, nCVBSW, nCVBSH);
	OSD_RELOAD(TRUE, TRUE);
}

void OVERLAY_INIT()
{
	int i = 0, ii = 0;
	char szOverlayTitle[32] = {""};
	char szDateFmt[20] = {""};

	switch(g_pstSysEnv->GetDateFmt(g_pstSysEnv))
	{
	default:
	case YYYY_MM_DD:
		strcpy(szDateFmt, "yyyymmdd");
		break;
	case MM_DD_YYYY:
		strcpy(szDateFmt, "mmddyyyy");
		break;
	case DD_MM_YYYY:
		strcpy(szDateFmt, "ddmmyyyy");
		break;
	}
	//OVERLAY_Init(szDateFmt, NULL); // ��������OSD
	for(i = 0; i < MAX_CAM_CH; ++i){
		for(ii = 0; ii < 2; ++ii){
			SDK_ENC_VIDEO_OVERLAY_CANVAS_t* canvas = sdk_enc->get_overlay_canvas(i, ii, "title");
			g_pstSysEnv->GetTitle(g_pstSysEnv, i, szOverlayTitle);
			OVERLAY_put_text(canvas, 0, 0, 0 == i ? OVERLAY_FONT_SIZE_20 : OVERLAY_FONT_SIZE_16, szOverlayTitle, 0);
			sdk_enc->update_overlay(i, ii, "title");
		}
	}
}

void OVERLAY_RELOAD()
{
	int i = 0, ii = 0;
	char szOverlayTitle[32] = {""};
	char szDateFmt[20] = {""};
	if (sdk_enc) {
		for(i = 0; i < MAX_CAM_CH; ++i){
			for(ii = 0; ii < 2; ++ii){
				SDK_ENC_VIDEO_OVERLAY_CANVAS_t* canvas = sdk_enc->get_overlay_canvas(i, ii, "title");
				if(canvas != NULL)
				{
					canvas->erase_rect(canvas, 0, 0, 0, 0);
					g_pstSysEnv->GetTitle(g_pstSysEnv, i, szOverlayTitle);
					OVERLAY_put_text(canvas, 0, 0, 0 == i ? OVERLAY_FONT_SIZE_20 : OVERLAY_FONT_SIZE_16, szOverlayTitle, 0);
					sdk_enc->update_overlay(i, ii, "title");
				}
			}
		}
	}
}

void GPIO_RELOAD()
{
	if(g_pstSysEnv->IsKeyBuzzer(g_pstSysEnv)){
		GPIO_EnableKeypadBuzzer();
	}else{
		GPIO_DisableKeypadBuzzer();
	}
	GPIO_SetIrdaId(g_pstSysEnv->GetDVRId(g_pstSysEnv));
}

void GPIO_INIT()
{
	GPIO_Init();
	GPIO_RELOAD();
}


void VIDEO_RELOAD()
{
	int i = 0;
	for(i = 0; i < MAX_CAM_CH; ++i){
		int const nHue = g_pstSysEnv->GetHue(g_pstSysEnv, i);
		int const nBrightness = g_pstSysEnv->GetLuminance(g_pstSysEnv, i);
		int const nContrast = g_pstSysEnv->GetContrast(g_pstSysEnv, i);
		int const nSaturation = g_pstSysEnv->GetSaturation(g_pstSysEnv, i);
		VIDEO_SetColor(i, nHue, nBrightness, nContrast, nSaturation);
	}
	VIDEO_ResetAutoSwitch();
}

void VIDEO_INIT()
{
	VIDEO_Init();
	return VIDEO_RELOAD();
}

static void _change_volumn(int _volumn)
{
	AUDIO_SetVolume(_volumn);
}

void XENVACT_init_volumn()
{
	_change_volumn(3);
}

void XENVACT_change_volumn(int _volumn)
{
	ASSERT(_volumn >= 0 && _volumn <= 7, "volumn = %d", _volumn);
	_change_volumn(_volumn);
}

