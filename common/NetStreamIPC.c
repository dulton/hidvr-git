#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "NetStreamIPC.h"

//typedef struct {
//	void         *pAddr;
//	unsigned long nSize; 
//} _LIST_STRUCT, * _pLIST_STRUCT;

typedef enum {
	PTR_BGN = 0,
	PTR_END,
	PTR_LST
} PTR_ENUM;

int Shm_ID = -1;
typedef struct {
	unsigned char ShmBuf[SHM_BUF_NUM][SHM_BUF_SIZE+MAX_PKG_SIZE];  //缓冲数据区
	unsigned long ShmCnt[SHM_BUF_NUM][PTR_LST];      //数据包计量
	unsigned long ShmPtr[SHM_BUF_NUM][PTR_LST];      //缓冲指针区
//	_LIST_STRUCT  ListBuf[SHM_BUF_NUM][LST_BUF_SIZE]; //列表数据区
//	unsigned long ListPtr[SHM_BUF_NUM][PTR_LST];     //列表指针区
} _SHM_STRUCT, * _pSHM_STRUCT;

volatile _pSHM_STRUCT pComShm = NULL;

int SHM_SrvInit(void)
{
	key_t key = ftok(SHM_KEY_FILE, 0);
	if(key == -1) {
		perror("ftok error");
		return -1;
	}

	Shm_ID = shmget(key, sizeof(*pComShm), IPC_CREAT | 0666);
	if(Shm_ID == -1) {
		perror("shmget error");
		return -1;
	}

	pComShm = (_pSHM_STRUCT)shmat(Shm_ID, NULL, 0);
	if(!pComShm) {
		return -1;
	}

	memset(pComShm, 0, sizeof(*pComShm));

	return 0;
}

int SHM_SrvFin(void)
{
	if(!pComShm) {
		return -1;
	}

	if(shmdt(pComShm) == -1) {
		perror("Shmdt detach error");
		return -1;
	}
	
	pComShm = NULL;

	return 0;
}

int SHM_CliInit(void)
{
	key_t key = ftok(SHM_KEY_FILE, 0);
	if(key == -1) {
		perror("ftok error");
		return -1;
	}

	Shm_ID = shmget(key, sizeof(*pComShm), IPC_CREAT | 0666);
	if(Shm_ID == -1) {
		perror("shmget error");
		return -1;
	}

	pComShm = (_pSHM_STRUCT)shmat(Shm_ID, NULL, 0);
	if(!pComShm) {
		return -1;
	}

    int i;
	for(i = 0; i < SHM_BUF_NUM; i ++) { //Client To Initialize CirBuf Pointer.
        Shm_BufForceEmpty(i);
	}

	return 0;
}

int SHM_CliFin(void)
{
	if(!pComShm) {
		return -1;
	}

	if(shmdt(pComShm) == -1) {
		perror("Shmdt detach error");
		return -1;
	}

	pComShm = NULL;

	return 0;
}

void Shm_BufForceEmpty(int Buf_Ptr)
{
    if(!pComShm || Buf_Ptr >= SHM_BUF_NUM || Buf_Ptr < 0) return;
	pComShm->ShmPtr[Buf_Ptr][PTR_END] = pComShm->ShmPtr[Buf_Ptr][PTR_BGN];
    pComShm->ShmCnt[Buf_Ptr][PTR_END] = pComShm->ShmCnt[Buf_Ptr][PTR_BGN];
}

unsigned long Shm_BufLevel(unsigned int No)
{
	if((!pComShm) ||(No >= SHM_BUF_NUM)) {
		return -1UL;
	}

	unsigned long tmpTgtCntS = pComShm->ShmCnt[No][PTR_BGN];
	unsigned long tmpTgtCntE = pComShm->ShmCnt[No][PTR_END];
	unsigned long XDiff = 0;

	if(tmpTgtCntS >= tmpTgtCntE) {
		XDiff = tmpTgtCntS - tmpTgtCntE;
	}
	else {
		XDiff = -1UL - (tmpTgtCntE - tmpTgtCntS)+1;
	}

	return XDiff;
}

/*
 * int Shm_WritePkg(unsigned int TgtNo, _SHM_PKG_HEAD * InPkgHead)
 * Input:  TgtNo Is Short for Target Number which is the index of
 *         ShmPtr[][] or ShmBuf[][].
 *         InPkgHead Is the buffer to Save Write In Package Data.
 *         Minimum size of this buffer is MAX_PKG_SIZE+sizeof(_SHM_PKG_HEAD).
 * Return: Size of Package Data Saved.
 *         Return value would be 0(No enough Buffer to Write)
 *         or -1(Input Data Is too large) when some errors occur.
 */
int Shm_WritePkg(unsigned int TgtNo, _SHM_PKG_HEAD * InPkgHead)
{
	if((!pComShm) || (TgtNo >= SHM_BUF_NUM) || (!InPkgHead)
	|| (InPkgHead->nDataLen > MAX_PKG_SIZE)) { //大数据包异常数据
		return -1;
	}

	unsigned long tmpTgtPtrS = pComShm->ShmPtr[TgtNo][PTR_BGN];
//	unsigned long tmpTgtPtrE = pComShm->ShmPtr[TgtNo][PTR_END];
	unsigned char*tmpTgtBuf  = pComShm->ShmBuf[TgtNo];

	unsigned long tmpPtrS = tmpTgtPtrS;
	int PkgSize = sizeof(*InPkgHead) + InPkgHead->nDataLen;  //写数据包大小
	int XEndFlg = 0;  //缓冲区尾部截断标志

	tmpPtrS += PkgSize;
	if(tmpPtrS >= SHM_BUF_SIZE) {
		XEndFlg = 1;
		tmpPtrS = 0;  //重定位指针
	}

	if(Shm_BufLevel(TgtNo) > SHM_BUF_SIZE-(sizeof(*InPkgHead)+MAX_PKG_SIZE)) { //缓冲区不足 //FIXME:
		return 0;
	}

/*	if(( XEndFlg && tmpPtrS >= tmpTgtPtrE )
	|| (!XEndFlg
		&& (tmpTgtPtrS <  tmpTgtPtrE)
		&& (tmpPtrS    >= tmpTgtPtrE))) {   //缓冲区不足 //FIXME:
		return 0;
	}*/

	//WriteOperation(ShmBufPtrS~tmpPtrS);
	memcpy(&tmpTgtBuf[tmpTgtPtrS], InPkgHead, PkgSize);
	
	if(tmpPtrS + sizeof(*InPkgHead) >= SHM_BUF_SIZE) {  //跳过不足包头缓冲区
		tmpPtrS = 0;
	}
	pComShm->ShmCnt[TgtNo][PTR_BGN] += PkgSize;
	pComShm->ShmPtr[TgtNo][PTR_BGN]  = tmpPtrS;
	return PkgSize;
}

/*
 * int Shm_ReadPkg(unsigned int SrcNo, _SHM_PKG_HEAD * OutPkgHead)
 * Input:  SrcNo Is Short for Source Number which is the index of
 *         ShmPtr[][] or ShmBuf[][].
 *         OutPkgHead Is the buffer to Save Read Out Package Data.
 *         Minimum size of this buffer is MAX_PKG_SIZE+sizeof(_SHM_PKG_HEAD).
 * Return: Size of Package Data Read.
 *         Return value would be 0(Buffer was checked to be empty)
 *         or -1(Input Data Is too large) when some errors occur.
 */
int Shm_ReadPkg(unsigned int SrcNo, _SHM_PKG_HEAD * OutPkgHead)
{
	if((!pComShm) || (SrcNo >= SHM_BUF_NUM) || (!OutPkgHead)) {
		return -1;
	}

	_SHM_PKG_HEAD tmpPkgHead;

	unsigned long tmpSrcPtrS = pComShm->ShmPtr[SrcNo][PTR_BGN];
	unsigned long tmpSrcPtrE = pComShm->ShmPtr[SrcNo][PTR_END];
	unsigned char*tmpSrcpBuf = (unsigned char*)pComShm->ShmBuf[SrcNo];

	unsigned long tmpPtrE = tmpSrcPtrE;
	if(tmpPtrE + sizeof(tmpPkgHead) >= SHM_BUF_SIZE) {  //跳过不足包头缓冲区
		tmpPtrE = 0;
	}
	if(tmpPtrE == tmpSrcPtrS) { //缓冲区为空
		return 0;
	}

	int PkgSize = 0;  //读数据包大小
	int XEndFlg = 0;  //缓冲区尾部截断标志

	memcpy(&tmpPkgHead, &tmpSrcpBuf[tmpPtrE], sizeof(tmpPkgHead));

	if(tmpPkgHead.nDataLen > MAX_PKG_SIZE) {
		fprintf(stderr, "!!!!!!---------------> tmpPkgHead.nDataLen == %ld\n", tmpPkgHead.nDataLen);
		fprintf(stderr, "!!!!!!---------------> tmpPkgHead.MagicNum == %4S\n", &tmpPkgHead.MagicNum);
		return -1;
	}

	PkgSize = sizeof(tmpPkgHead) + tmpPkgHead.nDataLen;

	tmpPtrE += PkgSize;
	if(tmpPtrE >= SHM_BUF_SIZE) {
		XEndFlg = 1;
		tmpPtrE = 0;
	}

	//ReadOperation(ShmBufPtrE~tmpPtrE);
	memcpy(OutPkgHead, &tmpSrcpBuf[tmpSrcPtrE], PkgSize);
//	printf("Read : %s\n", &ShmSecBuf[ShmBufPtrE+sizeof(*OutPkgHead)]);
	pComShm->ShmCnt[SrcNo][PTR_END] += PkgSize;
	pComShm->ShmPtr[SrcNo][PTR_END]  = tmpPtrE;
	return PkgSize;
}

/*
 * unsigned char * Shm_Request(unsigned int TgtNo, _SHM_PKG_HEAD * InPkgHead)
 * Input:  TgtNo Is Short for Target Number which is the index of
 *         ShmPtr[][] or ShmBuf[][].
 *         InPkgHead Is the Information Media to Post In for Buffer Requestion.
 *         Request Buffer Size Should Be Set in InPkgHead->nDataLen.
 *         Maximum size of this buffer is MAX_PKG_SIZE+sizeof(_SHM_PKG_HEAD).
 * Return: Start Pointer for Request Buffer.
 *         Return of NULL Means Requestion Failed.
 */
unsigned char * Shm_Request(unsigned int TgtNo, _SHM_PKG_HEAD * InPkgHead)
{
	if((!pComShm) || (TgtNo >= SHM_BUF_NUM) || (!InPkgHead)
	|| (InPkgHead->nDataLen > MAX_PKG_SIZE)) { //大数据包异常数据
		return NULL;
	}

	unsigned long tmpTgtPtrS = pComShm->ShmPtr[TgtNo][PTR_BGN];
//	unsigned long tmpTgtPtrE = pComShm->ShmPtr[TgtNo][PTR_END];
	unsigned char*tmpTgtBuf  = pComShm->ShmBuf[TgtNo];

	unsigned long tmpPtrS = tmpTgtPtrS;
	int PkgSize = sizeof(*InPkgHead) + InPkgHead->nDataLen;  //写数据包大小
	int XEndFlg = 0;  //缓冲区尾部截断标志

	if(Shm_BufLevel(TgtNo) > SHM_BUF_SIZE-(sizeof(*InPkgHead)+MAX_PKG_SIZE)) { //缓冲区不足 //FIXME:
		return NULL;
	}
	
	tmpPtrS += PkgSize;
	if(tmpPtrS >= SHM_BUF_SIZE) {
		XEndFlg = 1;
		tmpPtrS = 0;  //重定位指针
	}

	return (unsigned char *)&tmpTgtBuf[tmpTgtPtrS];
}

/*
 * int Shm_Commit(unsigned int TgtNo, _SHM_PKG_HEAD * InPkgHead)
 * Input:  TgtNo Is Short for Target Number which is the index of
 *         ShmPtr[][] or ShmBuf[][].
 *         InPkgHead Is the Information Media to Post In for Buffer Requestion.
 *         Request Buffer Size Should Be Set in InPkgHead->nDataLen.
 *         Maximum size of this buffer is MAX_PKG_SIZE+sizeof(_SHM_PKG_HEAD).
 * Return: Return of 0 Means Commit Failed,
 *         Other Return Value Means Success Operation.
 */
int Shm_Commit(unsigned int TgtNo, _SHM_PKG_HEAD * InPkgHead)
{
	if((!pComShm) || (TgtNo >= SHM_BUF_NUM) || (!InPkgHead)
	|| (InPkgHead->nDataLen > MAX_PKG_SIZE)) { //大数据包异常数据
		return 0;
	}

	unsigned long tmpPtrS = pComShm->ShmPtr[TgtNo][PTR_BGN];
	int PkgSize = sizeof(*InPkgHead) + InPkgHead->nDataLen;  //写数据包大小

	tmpPtrS += PkgSize;
	if(tmpPtrS >= SHM_BUF_SIZE) {
		tmpPtrS = 0;  //重定位指针
	}

	if(tmpPtrS + sizeof(*InPkgHead) >= SHM_BUF_SIZE) {  //跳过不足包头缓冲区
		tmpPtrS = 0;
	}

	pComShm->ShmCnt[TgtNo][PTR_BGN] += PkgSize;
	pComShm->ShmPtr[TgtNo][PTR_BGN]  = tmpPtrS;
	return !0;
}

unsigned char * Shm_Release(unsigned int SrcNo, _SHM_PKG_HEAD * OutPkgHead)
{
	if((!pComShm) || (SrcNo >= SHM_BUF_NUM) || (!OutPkgHead)) {
		return 0;
	}

	_SHM_PKG_HEAD tmpPkgHead;

	unsigned long tmpSrcPtrS = pComShm->ShmPtr[SrcNo][PTR_BGN];
	unsigned long tmpSrcPtrE = pComShm->ShmPtr[SrcNo][PTR_END];
	unsigned char*tmpSrcpBuf = (unsigned char*)pComShm->ShmBuf[SrcNo];

	unsigned long tmpPtrE = tmpSrcPtrE;
	if(tmpPtrE + sizeof(tmpPkgHead) >= SHM_BUF_SIZE) {  //跳过不足包头缓冲区
		tmpPtrE = 0;
	}
	if(tmpPtrE == tmpSrcPtrS) { //缓冲区为空
		return 0;
	}

	int PkgSize = 0;  //读数据包大小
	int XEndFlg = 0;  //缓冲区尾部截断标志

	memcpy(&tmpPkgHead, &tmpSrcpBuf[tmpPtrE], sizeof(tmpPkgHead));

	if(tmpPkgHead.nDataLen > MAX_PKG_SIZE) {
		fprintf(stderr, "!!!!!!---------------> tmpPkgHead.nDataLen == %ld\n", tmpPkgHead.nDataLen);
		fprintf(stderr, "!!!!!!---------------> tmpPkgHead.MagicNum == %4S\n", &tmpPkgHead.MagicNum);
		return 0;
	}

    return &tmpSrcpBuf[tmpPtrE];

	PkgSize = sizeof(tmpPkgHead) + tmpPkgHead.nDataLen;

	tmpPtrE += PkgSize;
	if(tmpPtrE >= SHM_BUF_SIZE) {
		XEndFlg = 1;
		tmpPtrE = 0;
	}
    
    return &tmpSrcpBuf[tmpSrcPtrE];
}

int Shm_Update(unsigned int SrcNo, _SHM_PKG_HEAD * OutPkgHead)
{
	if((!pComShm) || (SrcNo >= SHM_BUF_NUM) || (!OutPkgHead)) {
		return 0;
	}

    _SHM_PKG_HEAD tmpPkgHead;

	unsigned long tmpSrcPtrS = pComShm->ShmPtr[SrcNo][PTR_BGN];
	unsigned long tmpSrcPtrE = pComShm->ShmPtr[SrcNo][PTR_END];
	unsigned char*tmpSrcpBuf = (unsigned char*)pComShm->ShmBuf[SrcNo];

	unsigned long tmpPtrE = tmpSrcPtrE;
	if(tmpPtrE + sizeof(tmpPkgHead) >= SHM_BUF_SIZE) {  //跳过不足包头缓冲区
		tmpPtrE = 0;
	}
	if(tmpPtrE == tmpSrcPtrS) { //缓冲区为空
		return 0;
	}

	int PkgSize = 0;  //读数据包大小
	int XEndFlg = 0;  //缓冲区尾部截断标志

	memcpy(&tmpPkgHead, &tmpSrcpBuf[tmpPtrE], sizeof(tmpPkgHead));

	if(tmpPkgHead.nDataLen > MAX_PKG_SIZE) {
		fprintf(stderr, "!!!!!!---------------> tmpPkgHead.nDataLen == %ld\n", tmpPkgHead.nDataLen);
		fprintf(stderr, "!!!!!!---------------> tmpPkgHead.MagicNum == %4S\n", &tmpPkgHead.MagicNum);
		return -1;
	}

	PkgSize = sizeof(tmpPkgHead) + tmpPkgHead.nDataLen;

	tmpPtrE += PkgSize;
	if(tmpPtrE >= SHM_BUF_SIZE) {
		XEndFlg = 1;
		tmpPtrE = 0;
	}

	pComShm->ShmCnt[SrcNo][PTR_END] += PkgSize;
	pComShm->ShmPtr[SrcNo][PTR_END]  = tmpPtrE;
	return !0;
}
