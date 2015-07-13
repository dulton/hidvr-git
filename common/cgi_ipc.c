/*
 * cgi_ipc.c
 *
 *  Created on: 2011-11-14
 *      Author: root
 */
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <assert.h>

#include "cgi_ipc.h"

int CGI_IPC_semid = -1;

void CGI_IPC_init()
{
	CGI_IPC_semid = semget(ftok(CGI_SEM_PATH, 0), CGI_SEM_COUNT, 0666 | IPC_CREAT);
	semctl(CGI_IPC_semid, POSTER_CGI_INDEX, SETVAL, POSTER_CGI_NUM);
	semctl(CGI_IPC_semid, GW_CGI_INDEX, SETVAL, GW_CGI_NUM);
}

void CGI_IPC_attach()
{
	CGI_IPC_semid = semget(ftok(CGI_SEM_PATH, 0), CGI_SEM_COUNT, 0666 | IPC_CREAT);
}

void CGI_IPC_destory()
{
	CGI_IPC_semid = -1;
}

void CGI_IPC_P(int _sem_index)
{
	assert(CGI_IPC_semid != -1);

	//p²Ù×÷
	struct sembuf op;
	op.sem_num = _sem_index;
	op.sem_op = -1;
	op.sem_flg = SEM_UNDO;
	semop(CGI_IPC_semid, &op, 1);
}

void CGI_IPC_V(int _sem_index)
{
	assert(CGI_IPC_semid != -1);

	//p²Ù×÷
	struct sembuf op;
	op.sem_num = _sem_index;
	op.sem_op = 1;
	op.sem_flg = SEM_UNDO;
	semop(CGI_IPC_semid, &op, 1);
}
