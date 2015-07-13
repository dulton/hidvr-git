#ifndef __JADAEMON_H__
#define __JADAEMON_H__

typedef enum {
    PM_WEB = 0,  //Web Server Type.
    PM_CLI,      //Client End Type.
//    PM_MOB,      //Mobile Device Access Type.
    PM_NUM       //Sign for Number of Types.
} JA_PM_TYPE;

typedef struct { //UPNP PortMapping
    char gw_IpAddr[64];    //GatWay IP Address.
    int  i_nPorts[PM_NUM]; //Internal Ports to PortMapping.
    char i_IpAddr[64];     //External IP Address.
    int  e_nPorts[PM_NUM]; //External Ports PortMapped.
    char e_IpAddr[64];     //External IP Address.
    int  e_Err;
} _XCHG_UPNP;

typedef struct { //ESee Online
    char MachID[64];   //Machine Serial Number ID.
    char EseeID[64];   //ESee Uniform ID.
    char e_IpAddr[64]; //External IP Address.
} _XCHG_ESEE;

typedef struct { //DHCP Daemon
} _XCHG_DHCP;

typedef struct { //DDNS Daemon
} _XCHG_DDNS;

typedef enum {   //Uniform Type for Exchange.
    XTYPE_NONE = 0,
    XTYPE_DAEMON_UPNP,
    XTYPE_DAEMON_ESEE,
    XTYPE_DAEMON_DHCP,
    XTYPE_DAEMON_DDNS,
    XTYPE_LST
} _XCHG_TYPE;

typedef union {   //As Uniform Parameter.
    _XCHG_UPNP  xUPNP;
    _XCHG_ESEE  xESEE;
    _XCHG_DHCP  xDHCP;
    _XCHG_DDNS  xDDNS;
} _XCHG_DATA;

typedef struct {  //Struct of Data to Transport.
    unsigned int nType;
    _XCHG_DATA   xData;
} _XCHG_STRUCT;

#endif //#ifndef __JADAEMON_H__
