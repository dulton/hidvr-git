#ifndef __APP_NET_DATA_FILE_HEADER__
#define __APP_NET_DATA_FILE_HEADER__

enum {
    CMD_REQ_IFRAME = 1,   //Request I KeyFrame
    CMD_MOB_BGN,          //Begin Mobile Channel
    CMD_MOB_END,          //End   Mobile Channel
    CMD_MOB_ATR,          //Set   Mobile Channel Attribute
    CMD_PTZ_CTL,          //Ptz Control
};

typedef struct _tagA2NCmd{
	int		nHead;
	int		nCmd;
    int     nLen;
} A2NCmd, *lpA2NCmd;

typedef struct {
    unsigned int nChannel;
} Req_Iframe_t;

typedef struct {
    unsigned int nChannel;
} Mob_Bgn_t;

typedef struct {
    unsigned int nChannel;
} Mob_End_t;

typedef struct {
    unsigned long Frms;
    unsigned long Gops;
    unsigned long Kbps;
} Mob_Atr_t;

typedef struct {
    unsigned int nChn;
    unsigned int nCmd;
    unsigned int nArg;
} Ptz_Ctl_t;

#endif
