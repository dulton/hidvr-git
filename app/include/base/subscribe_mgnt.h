/******************************************************************************

  Copyright (C), 2014-, GuangZhou JUAN Electronics Co., Ltd.

 ******************************************************************************
  File Name    : subscribe_mgnt.h
  Version       : Initial Draft
  Author        : kejiazhw@163.com(kaga)
  Created       : 2014/11/25
  Last Modified : 2014/11/25
  Description   : subscribe management(pulldown & notify mode)
 	
  History       : 
  1.Date        : 2014/11/21
    	Author      : kaga
 	Modification: Created file	
******************************************************************************/

#ifndef __SUBSCRIBE_MGNT_H__
#define __SUBSCRIBE_MGNT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void *lpSUBSCRIBE_MGNT;
typedef unsigned int stSUBS_ID;
typedef stSUBS_ID stSUBS_GID;
typedef int stSUBS_USER;
#define INVALID_SUBS_ID		(-1)
#define DEFAULT_SUBS_USER	(0)

// event mode
#define SUBSCRIBE_EVENT_MODE_NOTIFY		(0)
#define SUBSCRIBE_EVENT_MODE_PULL		(1)

// event state
#define SUBSCRIBE_EVENT_STATE_INIT		(0)
#define SUBSCRIBE_EVENT_STATE_CHANGED	(1)
#define SUBSCRIBE_EVENT_STATE_DELETE	(2)


typedef enum
{
	S_EVENT_VIDEO_LOSS = 1,
	S_EVENT_MD,
	S_EVENT_ALARM_IN,
	S_EVENT_ALARM_OUT,
	S_EVENT_TOO_BLURRY,
	S_EVENT_TOO_DARK,
	S_EVENT_TOO_BRIGHT,
	S_EVENT_SCENE_CHANGE,
	S_EVENT_PROFILE_CHANGED,
	S_EVENT_VENC_CONF_CHANGED,
	S_EVENT_VSOURCE_CONF_CHANGED,
	S_EVENT_VOUT_CONF_CHANGED,
	S_EVENT_AENC_CONF_CHANGED,
	S_EVENT_ASOURCE_CONF_CHANGED,
	S_EVENT_AOUT_CONF_CHANGED,
	S_EVENT_VAN_CONF_CHANGED,
	S_EVENT_PTZ_CONF_CHANGED,

	S_EVENT_USER1 = 10000,
	S_EVENT_USER2 = 20000,

	S_EVENT_ALL = 60000,
	S_EVENT_GRP_BASE,

	
	S_EVENT_MAX = -1,
	S_EVENT_NR
}enS_EVENT_TYPE;
typedef enS_EVENT_TYPE stSUBS_GEID;
#define SUBS_DEFAULT_GROUP_EVENT		S_EVENT_ALL
#define INVALID_SUBS_EVENT				(-1)

typedef int (*fSUBSCRIBE_EVENT_HOOK)(enS_EVENT_TYPE event, int lparam, void *rParam, void *customCtx);
typedef int (*fSUBSCRIBE_INNER_HOOK)(enS_EVENT_TYPE event, int eventMode, int eventState, int period_s,
	void *customCtx); //client: renew , server: event-notify-hook

/*
* description: subscribe management  new/delete
* parammeter:
*		-- name : module name
*		-- thiz : subscribe management context
*/
lpSUBSCRIBE_MGNT 
SUBSCRIBE_mgnt_client_new(const char *name);
lpSUBSCRIBE_MGNT
SUBSCRIBE_mgnt_server_new(const char *name);
void
SUBSCRIBE_mgnt_delete(lpSUBSCRIBE_MGNT thiz);

/*
* description: push a event to subscribe management
* parammeter:
*		-- thiz : subscribe management context
*		-- event: event type
*		-- lparam & rparam: parameter for event
*/
int 
SUBSCRIBE_mgnt_notify_event(lpSUBSCRIBE_MGNT thiz, enS_EVENT_TYPE event, int lparam, void *rParam);

stSUBS_ID
SUBSCRIBE_mgnt_add_ex(lpSUBSCRIBE_MGNT thiz, stSUBS_USER user,
	enS_EVENT_TYPE event, int eventMode, int period_s,
	fSUBSCRIBE_EVENT_HOOK hook, void *customCtx, int customCtxSize,
	fSUBSCRIBE_INNER_HOOK subscribeHook, 
	fSUBSCRIBE_INNER_HOOK cancelHook, 
	fSUBSCRIBE_INNER_HOOK renewHook, 
	fSUBSCRIBE_INNER_HOOK pulldownHook, 
	void *innerCustomCtx, int innerCustomCtxSize);
stSUBS_ID 
SUBSCRIBE_mgnt_add(lpSUBSCRIBE_MGNT thiz, stSUBS_USER user,
	enS_EVENT_TYPE event, int eventMode, int period_s,
	fSUBSCRIBE_EVENT_HOOK hook, void *customCtx, int customCtxSize);
int 
SUBSCRIBE_mgnt_set_hook(lpSUBSCRIBE_MGNT thiz, stSUBS_ID id,
	fSUBSCRIBE_INNER_HOOK subscribeHook,
	fSUBSCRIBE_INNER_HOOK cancelHook,
	fSUBSCRIBE_INNER_HOOK renewHook,
	void *innerCustomCtx, int innerCustomCtxSize);
int 
SUBSCRIBE_mgnt_set_hook2(lpSUBSCRIBE_MGNT thiz, stSUBS_ID id,
	fSUBSCRIBE_INNER_HOOK subscribeHook,
	fSUBSCRIBE_INNER_HOOK cancelHook,
	fSUBSCRIBE_INNER_HOOK pulldownHook,
	void *innerCustomCtx, int innerCustomCtxSize);
int 
SUBSCRIBE_mgnt_subscribe(lpSUBSCRIBE_MGNT thiz, stSUBS_ID id);
int 
SUBSCRIBE_mgnt_enable_event_state(lpSUBSCRIBE_MGNT thiz, stSUBS_ID id, 
	int enable, int autoDeleteTimeout_s);


int SUBSCRIBE_mgnt_del(lpSUBSCRIBE_MGNT thiz, stSUBS_ID id, int block);
int SUBSCRIBE_mgnt_del_by_user(lpSUBSCRIBE_MGNT thiz, stSUBS_USER user, int block);

int SUBSCRIBE_mgnt_dump(lpSUBSCRIBE_MGNT mgnt);

stSUBS_GEID SUBSCRIBE_mgnt_create_grp_event(lpSUBSCRIBE_MGNT thiz);
int SUBSCRIBE_mgnt_grp_add_event(lpSUBSCRIBE_MGNT thiz, stSUBS_GEID gid, enS_EVENT_TYPE event);
int SUBSCRIBE_mgnt_grp_del_event(lpSUBSCRIBE_MGNT thiz, stSUBS_GEID gid, enS_EVENT_TYPE event);


#ifdef __cplusplus
}
#endif
#endif




