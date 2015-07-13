#include "stdinc.h"
#include "nvp_define.h"
#include "onvif.h"
#include "ja_type.h"

/*
	NVP COMMON INTERFACES
*/
inline bool NVP_MD_GET_GRANULARITY(lpNVP_MD_GRANULARITY _this, int row, int column)
{
	int const offset = _this->columnGranularity * row + column;
	int const offsetByte = offset / (sizeof(_this->granularity[0]) * 8);
	int const offsetBit = offset % (sizeof(_this->granularity[0]) * 8);
	return (_this->granularity[offsetByte] & (1<<offsetBit)) ? true : false;
}	

inline void NVP_MD_SET_GRANULARITY(lpNVP_MD_GRANULARITY _this, int row, int column, bool flag)
{
	int const offset = _this->columnGranularity * row + column;
	int const offsetByte = offset / (sizeof(_this->granularity[0]) * 8);
	int const offsetBit = offset % (sizeof(_this->granularity[0]) * 8);

	if(flag){
		_this->granularity[offsetByte] |= (1<<offsetBit);
	}else{
		_this->granularity[offsetByte] &= ~(1<<offsetBit);
	}
}

void NVP_MD_INIT_FROM_GRANULARITY(lpNVP_MD_GRANULARITY grid, lpNVP_MD_REGION region)
{
	struct rect
	{
		int x;
		int y;
		int w;
		int h;
		unsigned int area;
	};

	struct rect rcs[40*40];
	struct rect rc;
	int n = 0;
	int r,c;
	int i, j;
	printf("row : %d column:%d\n", grid->rowGranularity, grid->columnGranularity);
	for(r = 0; r < grid->rowGranularity; r++)
	{
		for( c = 0; c < grid->columnGranularity; c++){
			int right = grid->columnGranularity;
			int bottom = grid->rowGranularity;
			bool flag = NVP_MD_GET_GRANULARITY(grid, r, c);
			//printf("%c  ", flag ? 'T' : 'F');
			if(flag == true){
				for( j = r; j <  bottom; j++){
					i  = c;
					if( NVP_MD_GET_GRANULARITY(grid, j, i) == false){
						//bottom = j + 1;
						break;
					}
					for(; i < right; i++){
						flag = NVP_MD_GET_GRANULARITY(grid, j, i);
						printf("%c	", flag ? 'T' : 'F');
						if( flag == false){
							right = i + 1;
						}
					}
					printf("\r\n");
				}
				rcs[n].x = c;
				rcs[n].y = r;
				rcs[n].w = right - c - 1;
				rcs[n].h = j - r;
				rcs[n].area = rcs[n].w * rcs[n].h;
				n++;
				printf("Get one %03d, %03d, %03d, %03d\n", c ,r , right -c - 1, j -r);

				for( j = c; j <  right; j++){
					i  = r;
					if( NVP_MD_GET_GRANULARITY(grid, i, j) == false){
						break;
					}
					for(; i < bottom; i++){
						flag = NVP_MD_GET_GRANULARITY(grid, i, j);
						printf("%c	", flag ? 'T' : 'F');
						if( flag == false){
							bottom = i + 1;
						}
					}
					printf("\r\n");
				}
				rcs[n].x = c;
				rcs[n].y = r;
				rcs[n].w = j - c;
				rcs[n].h = bottom - r - 1;
				rcs[n].area = rcs[n].w * rcs[n].h;
				n++;
				printf("Get one %03d, %03d, %03d, %03d\n", c ,r , j - c, bottom -r -1);
			}
		}
		printf("\r\n");
	}
	printf("Get Total %d Rects\n", n);
	for( i = 0 ; i < n; i++){
		printf("RECT %03d @(%03d,%03d,%03d,%03d) area:%u\n", i, rcs[i].x, rcs[i].y, rcs[i].w, rcs[i].h, rcs[i].area);
	}
	// sort 
	for(i=0;i<n-2;i++)
	{
		for(j=0;j<n-i-2;j++)   
	    {   
	    	if(rcs[j].area > rcs[j+1].area)   
	      	{ 
				rc = rcs[j];
				rcs[j] = rcs[j+1];
				rcs[j+1] = rc;
	      	}   
		}   
	} 

	for( i = 0 ; i < n; i++){
		printf("RECT %03d @(%03d,%03d,%03d,%03d) area:%u\n", i, rcs[i].x, rcs[i].y, rcs[i].w, rcs[i].h, rcs[i].area);
	}
}

void NVP_MD_set_granularity_block(lpNVP_MD_GRANULARITY grid, int x, int y, int w, int h)
{
	int i,j;
	for( j = y; j < ( y + h); j++){
		for( i = x; i < ( x + w); i++){
			NVP_MD_SET_GRANULARITY(grid, j, i, true);
		}
	}
}

inline void NVP_MD_INIT_TO_GRANULARITY(lpNVP_MD_GRANULARITY grid, lpNVP_MD_REGION region)
{
	
}


/*
	NVP NULL INTERFACES
*/

int NVPSearch() { return -1;}
int NVPLogin	(lpNVP_ARGS args) { return -1;}
int NVPLogout	(lpNVP_ARGS args) { return -1;}
int NVPSetEventHook(lpNVP_ARGS args, fNVPEventHook hook, void *custom){return -1;}

int NVPGetSystemVersion	(lpNVP_ARGS args, char *version) { return -1;}
int NVPGetProtocalVersion			(lpNVP_ARGS args, char *version) { return  -1;}

int NVPGetDeviceInfo				(lpNVP_ARGS args, lpNVP_DEV_INFO info) { return -1; }
int NVPGetRtspUri					(lpNVP_ARGS args, lpNVP_RTSP_STREAM rtsps) { return -1; }
int NVPGetSystemDateTime			(lpNVP_ARGS args, lpNVP_SYS_TIME dt) { return -1; }
int NVPSetSystemDateTime			(lpNVP_ARGS args, lpNVP_SYS_TIME dt ) { return -1; }

int NVPGetVideoEncoderConfigs		(lpNVP_ARGS args, lpNVP_VENC_CONFIGS venc) { return -1; } // get all stream configurations
int NVPGetVideoEncoderConfig		(lpNVP_ARGS args, lpNVP_VENC_CONFIG venc) { return -1; }	// get one stream configuration
int NVPSetVideoEncoderConfig		(lpNVP_ARGS args, lpNVP_VENC_CONFIG venc) { return -1; }
int NVPGetVideoEncoderConfigOption(lpNVP_ARGS args, lpNVP_VENC_OPTIONS option) { return -1; }

int NVPGetColorConfig				(lpNVP_ARGS args, lpNVP_COLOR_CONFIG color) { return -1; }
int NVPSetColorConfig				(lpNVP_ARGS args, lpNVP_COLOR_CONFIG color) { return -1; }
int NVPSetHue						(lpNVP_ARGS args, lpNVP_COLOR_CONFIG color) { return -1; }
int NVPSetSharpness 				(lpNVP_ARGS args, lpNVP_COLOR_CONFIG color) { return -1; }
int NVPSetContrast					(lpNVP_ARGS args, lpNVP_COLOR_CONFIG color) { return -1; }
int NVPSetBrightness				(lpNVP_ARGS args, lpNVP_COLOR_CONFIG color) { return -1; }
int NVPSetSaturation				(lpNVP_ARGS args, lpNVP_COLOR_CONFIG color) { return -1; }

int NVPSetNetworkInterface			(lpNVP_ARGS args, lpNVP_ETHER_CONFIG ether) { return -1; }
int NVPGetNetworkInterface			(lpNVP_ARGS args, lpNVP_ETHER_CONFIG ether) { return -1; }

int NVPGetTitleOverlay				(lpNVP_ARGS args, lpNVP_TITLE_OVERLAY overlay) { return -1; }
int NVPGetDateTimeOverlay			(lpNVP_ARGS args, lpNVP_TIME_OVERLAY overlay) { return -1; }
int NVPSetTitleOverlay				(lpNVP_ARGS args, lpNVP_TITLE_OVERLAY overlay) { return -1; }
int NVPSetDateTimeOverlay			(lpNVP_ARGS args, lpNVP_TIME_OVERLAY overlay) { return -1; }

int NVPGetDigitalInputConfig		(lpNVP_ARGS args, lpNVP_INPUT_CONFIG input) { return -1; }
int NVPSetDigitalInputConfig		(lpNVP_ARGS args, lpNVP_INPUT_CONFIG input) { return -1; }

int NVPSubscribeEvent 			(lpNVP_ARGS args, lpNVP_SUBSCRIBE data) { return -1; }
int NVPCancelEvent				(lpNVP_ARGS args, lpNVP_SUBSCRIBE data) { return -1; }
int NVPGetEventStatus					(lpNVP_ARGS args, lpNVP_EVENT status) { return -1; }

int NVPGetMDConfig					(lpNVP_ARGS args, lpNVP_MD_CONFIG md) { return -1; }
int NVPSetMDConfig					(lpNVP_ARGS args, lpNVP_MD_CONFIG md) { return -1; }

int NVPControlPTZ					(lpNVP_ARGS args, lpNVP_PTZ_CONTROL ptz) { return -1; }	
int NVPReboot							(lpNVP_ARGS args, void *data) { return -1; }

int NVPGetJpegImage 				(lpNVP_ARGS args, void *data) { return -1; }


stNVP_INTERFACE gNVPNULL =
{
	NULL,
	NVPSetEventHook,

	NVPGetSystemVersion,
	NVPGetProtocalVersion,
	NVPSearch,
	NVPLogin,
	NVPLogout,
	NVPGetDeviceInfo,
	NVPGetRtspUri,
	NVPGetSystemDateTime,
	NVPSetSystemDateTime,
	NVPGetVideoEncoderConfigs,
	NVPGetVideoEncoderConfig,
	NVPSetVideoEncoderConfig,
	NVPGetVideoEncoderConfigOption,
	NVPGetColorConfig,
	NVPSetColorConfig,
	NVPSetHue,
	NVPSetSharpness,
	NVPSetContrast,
	NVPSetBrightness,
	NVPSetSaturation,

	NVPGetNetworkInterface,
	NVPSetNetworkInterface,

	NVPGetTitleOverlay,
	NVPSetTitleOverlay,
	NVPGetDateTimeOverlay,
	NVPSetDateTimeOverlay,
	
	NVPSubscribeEvent,
	NVPCancelEvent,
	NVPGetEventStatus,

	NVPGetDigitalInputConfig,
	NVPSetDigitalInputConfig,

	NVPGetMDConfig,
	NVPSetMDConfig,

	NVPControlPTZ,

	NVPReboot,
	NVPGetJpegImage,
};


void NVP_delete(lpNVP_INTERFACE iface)
{
	if(iface)
	{
		if(iface->private_ctx){
			free(iface->private_ctx);
		}
		iface->private_ctx = NULL;
		free(iface);
	}
}

const char *g_sz_h264_profile[5] = {"baseline", "main", "extended", "high" };
inline void NVP_dump_venc_options(lpNVP_VENC_OPTIONS options)
{
//#ifdef NVP_TEST
	int i;
	printf("<<%d>> resolution-nr : %d", options->index+1, options->resolution_nr);
	for( i = 0; i < options->resolution_nr; i++){
		printf(" %dx%d",options->resolution[i].width, options->resolution[i].height);
	}
	printf("\r\n");
	printf("encode profile-nr : %d", options->enc_profile_nr);
	for( i = 0; i < options->enc_profile_nr; i++){
		printf(" %s ", g_sz_h264_profile[options->enc_profile[i]]);
	}
	printf("\r\n");
	printf("\tfps[%d,%d] bps[%d,%d] gov[%d,%d] interval[%d,%d] quality[%d,%d]\n",
		options->enc_fps.min,options->enc_fps.max,
		options->enc_bps.min,options->enc_bps.max,
		options->enc_gov.min,options->enc_gov.max,
		options->enc_interval.min,options->enc_interval.max,
		options->enc_quality.min,options->enc_quality.max);
//#endif
}

inline void NVP_dump_venc_profile(lpNVP_VENC_CONFIG profile)
{
//#ifdef NVP_TEST
	const char *szQuantMode[3] = {"auto", "cbr", "vbr"};
	printf("\nprofile %d(name:%s token:%s)  encoder(name:%s token:%s)\n"
		"\t%dx%d encoding:%s(%s) %s:bps:%d fps:%d gov:%d quality:%d interval:%d\n",
		profile->index,
		profile->name,profile->token,
		profile->enc_name,profile->enc_token,
		profile->width,profile->height,
		(profile->enc_type == NVP_VENC_H264) ? "h264" : "mpeg4",
		(profile->enc_profile <  NVP_H264_PROFILE_NR) ? g_sz_h264_profile[profile->enc_profile] : "unknown",
		(profile->quant_mode < NVP_QUANT_NR) ? szQuantMode[profile->quant_mode] : "unknown",
		profile->enc_bps,profile->enc_fps,
		profile->enc_gov,profile->enc_quality,
		profile->enc_interval);
//#endif
}

inline void NVP_dump_device_info(lpNVP_DEV_INFO info)
{
//#ifdef NVP_TEST
	printf("\nDeviceInfo(brand:%s model:%s name:%s)  SW(version:%s at %s)\n"
		"\tHW:%s HWID:%s sn:%s\n",
		info->manufacturer, info->model, info->devname,
		info->sw_version, info->sw_builddate,
		info->hw_version, info->hwid, info->sn);
//#endif
}

inline void NVP_dump_ether_info(lpNVP_ETHER_CONFIG ether)
{
//#ifdef NVP_TEST
	printf("\nNetwork:%s dhcp:%s ip:%d.%d.%d.%d netmask:%d.%d.%d.%d gateway:%d.%d.%d.%d\n"
		"\tmac %02x:%02x:%02x:%02x:%02x:%02x\n"
		"\tdns1:%d.%d.%d.%d dns2:%d.%d.%d.%d\n"
		"\thttp_port:%d rtsp_port:%d\n",
		ether->token,
		ether->dhcp ? "true" : "false",
		ether->ip[0],ether->ip[1],ether->ip[2],ether->ip[3],
		ether->netmask[0],ether->netmask[1],ether->netmask[2],ether->netmask[3],
		ether->gateway[0],ether->gateway[1],ether->gateway[2],ether->gateway[3],
		ether->mac[0],ether->mac[1],ether->mac[2],ether->mac[3],ether->mac[4],ether->mac[5],
		ether->dns1[0],ether->dns1[1],ether->dns1[2],ether->dns1[3],
		ether->dns2[0],ether->dns2[1],ether->dns2[2],ether->dns2[3],
		ether->http_port, ether->rtsp_port);
//#endif
}



static lpNVP_INTERFACE gNVP = NULL;

int nvp_trace_param(char *format/*support %d%u%s%f%c*/, ...)
{
	int i;
	int index = 0;
	int n = 0;
	char *pkey = NULL;
	void *pvalue = NULL;
	char key[32][128];
	void* value[32];
	
	va_list args;
	va_start(args, format);
	do{
		int remind = index % 2;
		n = index/2;
		if((remind) == 0){
			pkey = va_arg(args, char *);
			if(pkey != NULL)
				strcpy(key[n], pkey);
			else
				break;
		}else if((remind) == 1){
			pvalue = va_arg(args, void *);
			if(pvalue == NULL )	break;
			else
				value[n] = pvalue;
		}
		index++;
	}while(1);


	printf("please input %d Param ", n);
	for(i = 0 ; i < n; i++){
		printf("%s ", key[i]);
	}
	printf("\r\n>");
	
	for(i = 0 ; i < n; i++){
		if(format[i*2+1] == 'd'){
			scanf("%d", (int *)value[i]);
		}
		else if(format[i*2 + 1] == 'u'){
			scanf("%u", (unsigned int *)value[i]);
		}else if(format[i*2 +1] == 's'){
			scanf("%[^\r\n]", (char *)value[i]);
		}else if(format[i*2 +1] == 'c'){
			scanf("%c", (char *)value[i]);
		}else if(format[i*2 +1] == 'f'){
			scanf("%f", (float *)value[i]);
		}else{
			printf("%d unknow format:%c\n", i, format[i*2 +1]);
			return -1;
		}
		//scanf("%*[\ ]");
	}
	
	printf("Get %d Parram ", n);
	for(i = 0 ; i < n; i++){
		if(format[i*2+1] == 'd'){
			printf("%s:%d", key[i], *((int *)value[i]));
		}
		else if(format[i*2 + 1] == 'u'){
			printf("%s:%u", key[i], *((unsigned int *)value[i]));
		}else if(format[i*2 +1] == 's'){
			printf("%s:%s", key[i], (char *)value[i]);
		}else if(format[i*2 +1] == 'c'){
			printf("%s:%c", key[i], *((char *)value[i]));
		}else if(format[i*2 +1] == 'f'){
			printf("%s:%f", key[i], *((float *)value[i]));
		}else{
			printf("%d unknow format:%c!!!!\n", i, format[i*2 +1]);
			return -1;
		}
		printf("  ");
	}
	printf("\r\n");
	
	va_end(args);
	return n;
}

void my_event_hook(int event, unsigned int l, unsigned int r, void *custom)
{
	printf("$$$$ event  <<  %2d (%u, %u) >>  hook $$$$$\n", event, l , r);
}
static int onvif_search_hook(char *bindip, unsigned char *ip,unsigned short port,
	char *name, char *location, char *firmware, void *customCtx)
{
	printf("add ipc %d.%d.%d.%d:%d %s %s @%s\n",ip[0],ip[1],ip[2],ip[3],port,
		name, firmware, location);
	return 0;
}

void f_found_local_ip(const char *szip)
{
	gNVP->Search(ONVIF_DEV_NVT, true, 0, onvif_search_hook, szip);
}

int nvp_test(int argc, char *argv[])
{
	int i = 0;
	stNVP_ARGS args;
 	stNVP_VENC_CONFIGS profiles;
	stNVP_ETHER_CONFIG ether;
	stNVP_VENC_OPTIONS options;
	stNVP_VENC_CONFIG profile;
	stNVP_DEV_INFO devinfo;
	stNVP_SYS_TIME stime;
	stNVP_COLOR_CONFIG color;
	stNVP_PTZ_CONTROL ptz_control;
	stNVP_EVENT event;
	stNVP_INPUT_CONFIGS inputs;
	stNVP_INPUT_CONFIG input;
	stNVP_MD_CONFIG md;
	stNVP_TIME_OVERLAY time_overlay;
	stNVP_TITLE_OVERLAY title_overlay;
	stNVP_INPUT_STATUS	io_status;
	stNVP_MD_CONFIG md_config;
	stNVP_SUBSCRIBE subscribe;
	
	char user[32] = "admin";
	char pwd[32] = "";
	char ip[20] = "192.168.1.84";
	unsigned short port =80;
	const char *usage=\
		"/******************NVP module test engin********************/\r\n"
		"/****************** -d : discovery     *********************/\r\n"
		"/****************** -devinfo : get device info  ************/\r\n"
		"/****************** -getrtsp : get rtsp url  ***************/\r\n"
		"/****************** -getvenc : get venc profiles	*********/\r\n"
		"/****************** -setupmain : setup main stream profile */\r\n"
		"/****************** -setupsub : setup sub stream profile   */\r\n"
		"/****************** -gtime : get time	*********************/\r\n"
		"/****************** -stime : set time	*********************/\r\n"
		"/****************** -vencopt : get venc options ************/\r\n"
		"/****************** -setvenc : set venc profiles ***********/\r\n"
		"/****************** -getether : get network	*************/\r\n"
		"/****************** -setether : set network	*************/\r\n"
		"/****************** -getcolor : get color		*************/\r\n"
		"/****************** -setcolor : set color		*************/\r\n"
		"/****************** -sethue : set hue			*************/\r\n"
		"/****************** -setcon : set contrast		*************/\r\n"
		"/****************** -setsat : set saturation	*************/\r\n"
		"/****************** -setbri : set brightness	*************/\r\n"
		"/****************** -setsha : set sharpness	*************/\r\n"
		"/****************** -reboot : reboot			*************/\r\n"
		"/****************** -ptz :ptz move/iris/zoom/focus...     * /\r\n"
		"/****************** -ptz2:ptz preset/tour.. 			  ***/\r\n"
		"/****************** -gettitleoverlay : get title overlay   */\r\n"
		"/****************** -settitleoverlay : set title overlay   */\r\n"
		"/****************** -gettimeoverlay : get time overlay     */\r\n"
		"/****************** -settimeoverlay : set time overlay     */\r\n"
		"/****************** -subscribe : subcribe event			    */\r\n"
		"/****************** -cancel : cancel event			    */\r\n"
		"/****************** -setmd : set md config			    */\r\n"
		"/****************** -getmd : get md config			    */\r\n"
		"/****************** -mdstatus : get md status			    */\r\n"
		"/****************** -iostatus : get io input status   	    */\r\n";
		
	if(argc < 2){
		printf(usage);
		return 0;
	}
	if(argc >= 4){
		strcpy(ip, argv[2]);
		port = atoi(argv[3]);
	}
	if(argc >= 5){
		strcpy(user, argv[4]);
		strcpy(pwd, "");
	}
	if( argc >= 6){
		strcpy(pwd, argv[5]);
	}
	
	
	memset(&devinfo, 0, sizeof(devinfo));
	memset(&profile, 0, sizeof(profile));
	memset(&profiles, 0, sizeof(profiles));
	memset(&options, 0, sizeof(options));
	memset(&ether, 0, sizeof(ether));
	memset(&stime, 0, sizeof(stime));
	memset(&color, 0, sizeof(color));
	memset(&ptz_control, 0, sizeof(ptz_control));
	memset(&title_overlay, 0, sizeof(title_overlay));
	memset(&time_overlay, 0, sizeof(time_overlay));
	memset(&input, 0, sizeof(input));
	memset(&inputs, 0, sizeof(inputs));
	memset(&md, 0, sizeof(md));
	memset(&event, 0, sizeof(event));
	memset(&io_status, 0, sizeof(io_status));
	memset(&md_config, 0, sizeof(md_config));
	memset(&subscribe, 0, sizeof(subscribe));
	
#if defined(ONVIF_TEST)
	gNVP = NVP_ONVIF_new();
#elif defined(ZAVIO_TEST)
	gNVP = NVP_ZAVIO_new();
#elif defined(HICHIP_TEST)
	gNVP = NVP_HICHIPV110_new();
#else
	//#error "No Module Select to Compile!!!!!!!!!!!!!!!!!!!\n"
#endif

	NVP_INIT_ARGS2(args, gNVP, ip, port, user, pwd, 0);
	
	if(strcmp(argv[1],"-devinfo")==0){
		gNVP->GetDeviceInfo(&args, &devinfo);
	} else if(strcmp(argv[1],"-devinfo2")==0){		
		//extern int ONVIF_MULTICAST_get_devinfo(lpNVP_DEV_INFO info);
		//ONVIF_MULTICAST_get_devinfo(&devinfo);
	}else if(strcmp(argv[1],"-gtime")==0){	
		stNVP_SYS_TIME dt;
		gNVP->GetSystemDateTime(&args, &dt);
	}else if(strcmp(argv[1],"-stime2")==0){
		gNVP->SetSystemDateTime(&args, NULL);
	}else if(strcmp(argv[1],"-stime")==0){
		time_t t_now, ptime;
		struct tm ptm;
		int tzone;

		nvp_trace_param("%d", "time_zone_offset", &tzone, NULL);
		stime.tzone = tzone;
		
		time(&t_now);
		localtime_r(&t_now, &ptm);
		stime.gm_time.date.year = ptm.tm_year;
		stime.gm_time.date.month = ptm.tm_mon;
		stime.gm_time.date.day = ptm.tm_mday;
		stime.gm_time.time.hour = ptm.tm_hour;
		stime.gm_time.time.minute = ptm.tm_min;
		stime.gm_time.time.second = ptm.tm_sec;
		stime.daylightsaving = false;
		stime.ntp_enable = false;
		gNVP->SetSystemDateTime(&args, &stime);
		gNVP->GetSystemDateTime(&args, &stime);
		ptm.tm_year = stime.local_time.date.year;
		ptm.tm_mon = stime.local_time.date.month;
		ptm.tm_mday = stime.local_time.date.day;
		ptm.tm_hour = stime.local_time.time.hour;
		ptm.tm_min = stime.local_time.time.minute;
		ptm.tm_sec = stime.local_time.time.second;
		ptime = mktime(&ptm);
		if (abs(ptime - t_now) > 5) {
			printf("dtime: %d-%d-%d %d:%d:%d\n", stime.gm_time.date.year + 1900, 
				stime.gm_time.date.month + 1, stime.gm_time.date.day,
				stime.gm_time.time.hour, stime.gm_time.time.minute, stime.gm_time.time.second);
			printf("ptime :%lu sync:%lu diff:%lu\n", ptime, t_now, abs(ptime - t_now) );
			memset(&stime, 0, sizeof(stime));
			stime.tzone = tzone;
			time(&t_now);
			gmtime_r(&t_now, &ptm);
			stime.gm_time.date.year = ptm.tm_year;
			stime.gm_time.date.month = ptm.tm_mon;
			stime.gm_time.date.day = ptm.tm_mday;
			stime.gm_time.time.hour = ptm.tm_hour;
			stime.gm_time.time.minute = ptm.tm_min;
			stime.gm_time.time.second = ptm.tm_sec;
			stime.daylightsaving = false;
			stime.ntp_enable = false;
			//gNVP->SetSystemDateTime(&args, &stime);
			//nvc_session->hocks->GetSystemDateTime(&args, &stime);
		} 
	}else if(strcmp(argv[1],"-vencopt")==0){
		gNVP->GetVideoEncoderConfigs(&args, &profiles);
		for( i = 0; i < profiles.nr; i++){
			options.index = i;
			strcpy(options.token, profiles.entry[i].token);
			strcpy(options.enc_token, profiles.entry[i].enc_token);
			gNVP->GetVideoEncoderConfigOption(&args, &options);
		}
	}else if(strcmp(argv[1],"-d")==0){
		system("route add -net 224.0.0.0 netmask 224.0.0.0 eth0");
		SOCK_getallhostip(f_found_local_ip);
		//gNVP->Search(ONVIF_DEV_NVT, onvif_search_hook, "192.168.1.81");
		system("route del -net 224.0.0.0 netmask 224.0.0.0 eth0");
	}else if(strcmp(argv[1],"-getrtsp")==0){	
		stNVP_RTSP_STREAM rtsps={0,1,};
		gNVP->GetRtspUri(&args, &rtsps);
	}else if(strcmp(argv[1],"-getvenc")==0){		
		gNVP->GetVideoEncoderConfigs(&args, &profiles);	
	}else if(strcmp(argv[1],"-setvenc")==0){
		gNVP->GetVideoEncoderConfigs(&args, &profiles);
		// set main stream
		int mainindex = 0;
		int subindex = 1;
		options.index = mainindex;
		strcpy(options.token, profiles.entry[mainindex].token);
		strcpy(options.enc_token, profiles.entry[mainindex].enc_token);
		gNVP->GetVideoEncoderConfigOption(&args, &options);
		profiles.entry[mainindex].enc_type = NVP_VENC_H264;
		profiles.entry[mainindex].width = 1280;
		profiles.entry[mainindex].height = 720;
		profiles.entry[mainindex].enc_fps = 25;
		profiles.entry[mainindex].enc_bps = 1024*3;
		profiles.entry[mainindex].quant_mode = NVP_QUANT_CBR;
		if(gNVP->SetVideoEncoderConfig(&args, &profiles.entry[mainindex]) == 0){
			//return 0;
			// set sub stream
			if(profiles.nr > 1){
				subindex = profiles.nr - 1;
				options.index = subindex;
				strcpy(options.token, profiles.entry[subindex].token);
				strcpy(options.enc_token, profiles.entry[subindex].enc_token);
				gNVP->GetVideoEncoderConfigOption(&args, &options);
				profiles.entry[subindex].enc_type = NVP_VENC_H264;
				profiles.entry[subindex].width = 352;
				profiles.entry[subindex].height = 288;
				profiles.entry[subindex].enc_fps = 25;
				profiles.entry[subindex].enc_bps = 512;
				profiles.entry[mainindex].quant_mode = NVP_QUANT_CBR;

				gNVP->SetVideoEncoderConfig(&args, &profiles.entry[subindex]);
			}
		}
	}else if(strcmp(argv[1],"-getether")==0){		
		gNVP->GetNetworkInterface(&args, &ether);
	}else if(strcmp(argv[1],"-setether")==0){
		char _szip[20];
		char _szmask[20];
		strcpy(_szip, ip);
		strcpy(_szmask, "255.255.0.0");
		if(argc >= 7){
			strcpy(_szip, argv[6]);
		}
		if( argc >= 8){
			strcpy(_szmask, argv[7]);
		}
		ether.dhcp = false;
		NVP_IP_INIT_FROM_STRING(ether.ip, _szip);
		NVP_IP_INIT_FROM_STRING(ether.netmask,_szmask);
		NVP_IP_INIT_FROM_STRING(ether.gateway,"192.168.1.1");
		NVP_IP_INIT_FROM_STRING(ether.dns1,"192.168.1.1");
		NVP_IP_INIT_FROM_STRING(ether.dns2,"192.168.2.1");
		NVP_MAC_INIT(ether.mac, 0, 0x1B, 0xFE, 0x04, 0x39, 0x34);
		int ret = gNVP->SetNetworkInterface(&args, &ether);
		printf("set ret:%d\n", ret);
	}else if(strcmp(argv[1],"-getimage")==0){		
		gNVP->GetVideoEncoderConfigs(&args, &profiles);
		gNVP->GetColorConfig(&args, &color);
	}else if(strcmp(argv[1],"-setimage")==0){
		gNVP->GetVideoEncoderConfigs(&args, &profiles);
		//gNVP->GetImageConfig(&args, &image);
		color.contrast = 0.5;
		color.hue = 0.5;
		color.sharpness = 0.5;
		color.saturation = 0.5;
		color.brightness = 0.5;
		gNVP->SetColorConfig(&args, &color);
	}else if(strcmp(argv[1],"-sethue")==0){		
		gNVP->GetVideoEncoderConfigs(&args, &profiles);
		//image.hue = 0.5;
		nvp_trace_param("%f", "hue", &color.hue, NULL);
		gNVP->SetHue(&args, &color);
	}else if(strcmp(argv[1],"-setbri")==0){ 	
		gNVP->GetVideoEncoderConfigs(&args, &profiles);
		//image.brightness = 0.5;
		nvp_trace_param("%f", "brightness", &color.brightness, NULL);
		gNVP->SetBrightness(&args, &color);
	}else if(strcmp(argv[1],"-setcon")==0){ 	
		gNVP->GetVideoEncoderConfigs(&args, &profiles);
		//image.contrast = 0.5;
		nvp_trace_param("%f", "contrast", &color.contrast, NULL);
		gNVP->SetContrast(&args, &color);
	}else if(strcmp(argv[1],"-setsat")==0){ 	
		gNVP->GetVideoEncoderConfigs(&args, &profiles);
		//image.saturation = 0.5;
		nvp_trace_param("%f", "saturation", &color.saturation, NULL);
		gNVP->SetSaturation(&args, &color);
	}else if(strcmp(argv[1],"-setsha")==0){ 	
		gNVP->GetVideoEncoderConfigs(&args, &profiles);
		//image.sharpness = 0.5;
		nvp_trace_param("%f", "sharpness", &color.sharpness, NULL);
		gNVP->SetSharpness(&args, &color);
	}else if(strcmp(argv[1],"-gettitleoverlay")==0){ 	
		gNVP->GetTitleOverlay(&args, &title_overlay);
	}else if(strcmp(argv[1],"-settitleoverlay")==0){
		nvp_trace_param("%d%s", "enable", &title_overlay.enable, "title", title_overlay.alias, NULL);
		gNVP->SetTitleOverlay(&args, &title_overlay);
	}else if(strcmp(argv[1],"-gettimeoverlay")==0){	
		gNVP->GetDateTimeOverlay(&args, &time_overlay);
	}else if(strcmp(argv[1],"-settimeoverlay")==0){
		nvp_trace_param("%d", "enable", &time_overlay.enable, NULL);
		gNVP->SetDateTimeOverlay(&args, &time_overlay);
	}else if(strcmp(argv[1],"-ptz")==0){
		ptz_control.speed = 0.9;
		printf("0:up 1:down 2:left 3:right 4:left-up 5:right-up 6:left-down 7:right-down 8:autopan 9: iris-open\n"
				"9:iris-open 10:iris-close 11:zoom-in 12:zoom-out 13:focus-far 14:focus-near 15 stop\n"
				"16:wipper-on 17:wipper-off 18:light-on 19:light-off 20:power-on 21:power-off\n");
		nvp_trace_param("%d%f", "command", &ptz_control.cmd,"speed(float 0~1.0)",  &ptz_control.speed, NULL);
		gNVP->ControlPTZ(&args, &ptz_control);
	}else if(strcmp(argv[1],"-ptz2")==0){
		ptz_control.speed = 0.9;
		printf("22:goto-preset 23:set-preset 24:clear-preset 25:start tour\n");
		nvp_trace_param("%d%d", "command", &ptz_control.cmd, "index", &ptz_control.index, NULL);
		gNVP->ControlPTZ(&args, &ptz_control);
	}else if(strcmp(argv[1],"-reboot")==0){
		gNVP->Reboot(&args, NULL);
	}else if(strcmp(argv[1],"-mdstatus")==0){
		do{
			stNVP_EVENT event;
			event.type = JA_EVENT_MD;
			gNVP->GetEventStatus(&args, &event);
			printf("Get MD STATUS : %d !!!!!!!!!!!!!!\n", event.status);
			sleep(1);
		}while(1);
	}else if(strcmp(argv[1],"-iostatus")==0){
		do{
			stNVP_EVENT event;
			event.index = 0;
			event.type = JA_EVENT_SENSOR_IN;
			gNVP->GetEventStatus(&args, &event);
			printf("Get IO STATUS : %d !!!!!!!!!!!!!!\n", event.status);
			sleep(1);
		}while(1);
	}else if(strcmp(argv[1],"-subscribe")==0){
		subscribe.event = NVP_EVENT_ALL;
		subscribe.keeplive_time = 10;
		subscribe.hook = my_event_hook;
		subscribe.hook_custom = NULL;
		gNVP->SubscribeEvent(&args, &subscribe);
		for ( ; ; ) {
			char ch = getchar();
			if (ch == 'r' ) {
				gNVP->SubscribeEvent(&args, &subscribe);
			}else if ( ch == 'c'){
				gNVP->CancelEvent(&args, &subscribe);
			}else if (ch == 'x'){
#if defined(ONVIF_TEST)				
				NVP_ONVIF_delete(gNVP);
				gNVP = NVP_ONVIF_new();
#endif
				gNVP->SubscribeEvent(&args, &subscribe);
			}
		}
	}else if(strcmp(argv[1],"-cancel")==0){
		gNVP->CancelEvent(&args, NULL);
	}else if(strcmp(argv[1],"-setmd")==0){
		md_config.enable = true;
		md_config.type = NVP_MD_TYPE_REGION;
		md_config.region.ch[0].enable = true;
		md_config.region.ch[0].index = 0;
		strcpy(md_config.region.ch[0].name, "M1");
		md_config.region.ch[0].nX = 0.0;
		md_config.region.ch[0].nY = 0.0;
		md_config.region.ch[0].nWidth = 1.0;
		md_config.region.ch[0].nHeight = 1.0;
		md_config.region.ch[0].sensitivity = 0.4;
		md_config.region.ch[0].threshold = 0.7;
		gNVP->SetMDConfig(&args, &md_config);
	}else if(strcmp(argv[1], "toregion") == 0){
		stNVP_MD_GRANULARITY grid;
		stNVP_MD_REGION region;
		memset(&grid, 0, sizeof(grid));
		memset(&region, 0, sizeof(region));
		grid.rowGranularity = 24;
		grid.columnGranularity = 32;
		NVP_MD_set_granularity_block(&grid, 0, 0, 5, 2);
		NVP_MD_set_granularity_block(&grid, 0, 0, 3, 5);
		NVP_MD_INIT_FROM_GRANULARITY(&grid, &region);
	}else{
		printf(usage);
	}
	return 0;

}

#ifdef NVP_TEST
int main(int argc, char *argv[])
{
	return main_client(argc, argv);
}
#endif

