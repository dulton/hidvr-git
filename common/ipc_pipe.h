#ifndef __IPC_PIPE___H__
#define __IPC_PIPE___H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
typedef HANDLE HPIPE;
typedef FILE* HFILE;
#define INVALID_HPIPE	(NULL)
#define INVALID_HFILE	(NULL)
#else
typedef char* HPIPE;
typedef int HFILE;
#define INVALID_HPIPE	(NULL)
#define INVALID_HFILE	(-1)
#endif

#define PIPE_F_RD_ONLY	(0x1)
#define PIPE_F_WR_ONLY	(0x2)
#define PIPE_F_RW		(0x3)

HPIPE IPC_NAMED_PIPE_new(const char *name);
void IPC_NAMED_PIPE_delete(HPIPE handle);

HFILE IPC_NAMED_PIPE_open(const char *name, int flag);
int IPC_NAMED_PIPE_close(HFILE handle);

int IPC_PIPE_write(HFILE handle, void *buf, int size);
int IPC_PIPE_read(HFILE handle, void *buf, int size);
 
#ifdef __cplusplus
}
#endif

#endif
