#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipc_pipe.h"

#if defined(_WIN32) || defined(_WIN64)
#else
#include <fcntl.h> 
#include <sys/types.h> 
#endif

static void _check_and_delete_file(const char *path)
{
#if defined(_WIN32) || defined(_WIN64)
    DeleteFile(path);
#else
    unlink(path);
#endif
}


HPIPE IPC_NAMED_PIPE_new(const char *name)
{
#if defined(_WIN32) || defined(_WIN64)
	#error "not implement in this platform!!!"
#else
	HPIPE hp = NULL;
	int res = 0;
	
	if (name == NULL) 
		return NULL;

	_check_and_delete_file(name);

	res = mkfifo(name, 0777);	
	if(res != 0)  
	{  
		printf("Could not create fifo %s\n", name);  
		return NULL;
	}

	hp = calloc(1, strlen(name) + 1);
	strcpy(hp, name);

	return hp;
#endif
}


void IPC_NAMED_PIPE_delete(HPIPE handle)
{
#if defined(_WIN32) || defined(_WIN64)
	#error "not implement in this platform!!!"
#else
	if (handle == INVALID_HPIPE)
		return;
	// delete file
	_check_and_delete_file(handle);

	// free resource
	free(handle);
#endif
}


HFILE IPC_NAMED_PIPE_open(const char *name, int flag)
{
#if defined(_WIN32) || defined(_WIN64)
	#error "not implement in this platform!!!"
#else
	HFILE hf = INVALID_HFILE;

	if (flag == PIPE_F_RD_ONLY) {
		flag = O_RDONLY;
	} else if (flag == PIPE_F_WR_ONLY) {
		flag = O_WRONLY;
	} else {
		return hf;
	}

	if ((hf = open(name, flag)) == INVALID_HFILE) {
		printf("Could not open fifo %s\n", name);
		return hf;
	}
	return hf;
#endif
}

int IPC_NAMED_PIPE_close(HFILE handle)
{
#if defined(_WIN32) || defined(_WIN64)
	#error "not implement in this platform!!!"
#else
	if (handle == INVALID_HFILE)
		return -1;
	close(handle);
	return 0;
#endif
}


int IPC_PIPE_write(HFILE handle, void *buf, int size)
{
#if defined(_WIN32) || defined(_WIN64)
	#error "not implement in this platform!!!"
#else
	if (handle == INVALID_HFILE)
		return -1;
	if ((buf == NULL) || (size <= 0))
		return -1;

	return write(handle, buf, size);
#endif
}

int IPC_PIPE_read(HFILE handle, void *buf, int size)
{
#if defined(_WIN32) || defined(_WIN64)
	#error "not implement in this platform!!!"
#else
	if (handle == INVALID_HFILE)
		return -1;
	if ((buf == NULL) || (size <= 0))
		return -1;

	return read(handle, buf, size);
#endif
}

