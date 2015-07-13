#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

struct MSG_QUEUE;
typedef struct MSG_QUEUE stMSG_QUEUE, *lpMSG_QUEUE;

struct MSG_DATA;

extern int MSGQ_init();
extern void MSGQ_deinit();

extern lpMSG_QUEUE MSGQ_new();
extern void MSGQ_delete(lpMSG_QUEUE mq);

extern int MSGQ_set_msg(lpMSG_QUEUE mq, int msg_type, int (*msg_hook)());

extern int MSGQ_send_msg(lpMSG_QUEUE mq, int msg_type, void *pdata);
extern int MSGQ_post_msg(lpMSG_QUEUE mq, int msg_type, void *pdata);
extern int MSGQ_recv_msg(lpMSG_QUEUE mq, int *msg_type, void *pdata);
extern int MSGQ_recv_msg2(lpMSG_QUEUE mq, int msg_type, void *pdata);


#ifdef __cplusplus
}
#endif
#endif






