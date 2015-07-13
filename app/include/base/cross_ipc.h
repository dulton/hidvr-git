#ifndef __CROSS_IPC_HEAD_FILE__
#define __CROSS_IPC_HEAD_FILE__
#include "stdinc.h"


#ifdef __cplusplus
extern "C" {
#endif

void ipc_pipe_new();
void ipc_pipe_delete();
void ipc_pipe_write();
void ipc_pipe_read();

void ipc_named_pipe_new();
void ipc_named_pipe_delete();
void ipc_named_pipe_write();
void ipc_named_pipe_read();

void *ipc_event_new();
void ipc_event_delete(void *handle);
int ipc_event_post(void *handle);
int ipc_event_wait(void *handle, unsigned long timeout_ms);

void ipc_sem_new();
void ipc_sem_delete();
void ipc_sem_post();
void ipc_sem_wait();

#ifdef __cplusplus
}
#endif


#endif
