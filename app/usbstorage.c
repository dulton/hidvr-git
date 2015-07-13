

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include "usbstorage.h"

#define DEBUG_USB
#ifdef DEBUG_USB
#define USBSTORAGE_TRACE(fmt...)	\
	do{printf("\033[1;35mUSBSTORAGE->[%s]:%d ", __FILE__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#else
#define USBSTORAGE_TRACE(fmt...)
#endif

typedef struct USBStorage
{
	int nDevCnt;
	char szDevNode[MAX_USB_STORAGE][USB_NODE_LEN];
	char szDevName[MAX_USB_STORAGE][USB_NAME_LEN];
	int nDevBlkCnt[MAX_USB_STORAGE];
	unsigned char bDevBlk[MAX_USB_STORAGE][MAX_BLOCK_CNT];
	void (*Clear)(struct USBStorage* thiz);
	void (*Print)(struct USBStorage* thiz);
	int (*GetCnt)(struct USBStorage* thiz);
	int (*GetName)(struct USBStorage* thiz, int index, char* ret_name);
	int (*GetSize)(struct USBStorage* thiz, int index);
	int (*Mount)(struct USBStorage* thiz, char* dev);
	int (*MountAll)(struct USBStorage* thiz);
	int (*Umount)(struct USBStorage* thiz, const char* dev);
	int (*UmountAll)(struct USBStorage* thiz);
}USBStorage;

static void usbstorage_Clear(USBStorage* thiz)
{
	thiz->nDevCnt = 0;
	memset(thiz->szDevNode, 0, sizeof(thiz->szDevNode));
	memset(thiz->szDevName, 0, sizeof(thiz->szDevName));
	memset(thiz->nDevBlkCnt, 0, sizeof(thiz->nDevBlkCnt));
	memset(thiz->bDevBlk, 0, sizeof(thiz->bDevBlk));
}

static void usbstorage_Print(USBStorage* thiz)
{
	int i = 0, ii = 0;
	if(!thiz->nDevCnt){
		thiz->GetCnt(thiz);
	}
	printf("USB-Storge count %d, All is blew\r\n", thiz->nDevCnt);
	for(i = 0; i < thiz->nDevCnt; ++i){
		printf("%s: name=\"%s\" block(%d)=", thiz->szDevNode[i], thiz->szDevName[i], thiz->nDevBlkCnt[i]);
		for(ii = 0; ii < MAX_BLOCK_CNT; ++ii){
			printf("%c", thiz->bDevBlk[i][ii] ? 'o' : 'x');
		}
		printf("\r\n");
	}
}

static int usbstorage_GetCnt(USBStorage* thiz)
{
	int i = 0, ii = 0;
	DIR* dirPath = NULL;
	char szPath[100] = {""};
	char szDev[100] = {""};
	char u8Buf[100] = {""};
	FILE* f = NULL;
	for(i = 0, thiz->nDevCnt = 0; i < MAX_USB_STORAGE; ++i){
		sprintf(szDev, "sd%c", i + 'a');
		sprintf(szPath, "/sys/block/%s", szDev);
		//USBSTORAGE_TRACE("check path \"%s\"", szPath);
		if((dirPath = opendir(szPath))){
			closedir(dirPath);	// close dir immediately
			strcat(szPath, "/device/vendor");
			//USBSTORAGE_TRACE("check file \"%s\"", szPath);
			f = fopen(szPath, "r");
			assert(f);
			fgets(u8Buf, sizeof(u8Buf), f);
			fclose(f);
			if(strncmp(u8Buf, "ATA", 3) != 0){
				char szVendor[50] = {""};
				char szModel[50] = {""};
				char szSize[50] = {""};
				// is a usb device then check every blocks
				strcpy(thiz->szDevNode[thiz->nDevCnt], szDev);
				for(ii = 0, thiz->nDevBlkCnt[thiz->nDevCnt] = 0; ii < MAX_BLOCK_CNT; ++ii){
					if(0 == ii){
						++thiz->nDevBlkCnt[thiz->nDevCnt];
						thiz->bDevBlk[thiz->nDevCnt][ii] = 1;
					}else{
						sprintf(szPath, "/sys/block/%s/%s%c", szDev, szDev, ii > 0 ? '0' + ii : '\0');
						//USBSTORAGE_TRACE("check path \"%s\"", szPath);
						if((dirPath = opendir(szPath))){
							closedir(dirPath);
							// check valid block
							++thiz->nDevBlkCnt[thiz->nDevCnt];
							thiz->bDevBlk[thiz->nDevCnt][ii] = 1;

							//printf("o");
						}else{
							thiz->bDevBlk[thiz->nDevCnt][ii] = 0;
							//printf("x");
						}
					}
				}
				// get name = vender + model
				// vendor
				sprintf(szPath, "/sys/block/%s/device/vendor", szDev);
				//USBSTORAGE_TRACE("check file \"%s\"", szPath);
				f = fopen(szPath, "r");
				fgets(szVendor, sizeof(szVendor) - 1, f);
				fclose(f);
				// model
				sprintf(szPath, "/sys/block/%s/device/model", szDev);
				//USBSTORAGE_TRACE("check file \"%s\"", szPath);
				f = fopen(szPath, "r");
				fgets(szModel, sizeof(szModel) - 1, f);
				fclose(f);
				// merge name
				strcpy(thiz->szDevName[thiz->nDevCnt], szVendor);
				*strpbrk(thiz->szDevName[thiz->nDevCnt], "\r\n") = '\0';
				strcat(thiz->szDevName[thiz->nDevCnt], " ");
				strcat(thiz->szDevName[thiz->nDevCnt], szModel);
				*strpbrk(thiz->szDevName[thiz->nDevCnt], "\r\n") = '\0';
				USBSTORAGE_TRACE("Check Device %s", thiz->szDevName[thiz->nDevCnt]);
				// size
				sprintf(szPath, "/sys/block/%s/size", szDev);
				f = fopen(szPath, "r");
				fgets(szSize, sizeof(szSize) - 1, f);
				fclose(f);
				//
				thiz->nDevCnt++;
			}else{
				USBSTORAGE_TRACE("Skip Device %s", szDev);
			}
		}
	}
	return thiz->nDevCnt;
}


static int usbstorage_GetName(USBStorage* thiz, int index, char* ret_name)
{
	int ret = -1;
	if(index >= 0 && index < thiz->nDevCnt && ret_name != NULL)
	{
		strcpy(ret_name, thiz->szDevName[index]);
		ret = 0;
	}
	return ret;
}

static int usbstorage_GetSize(USBStorage* thiz, int index)
{
	int ret = -1;
	if(index >= 0 && index < thiz->nDevCnt)
	{
		//ret = thiz->fDevSize[index];
	}
	return ret;
}

inline static int _usbstorage_Mount(const char* dev, const char* target)
{
	int nRet = 0;
	if(0 != (nRet = mkdir(target, 0))){
		//USBSTORAGE_TRACE("mkdir error: %s", strerror(errno));
//		return -1;
	}
	if(0 != (nRet = mount(dev, target, "vfat", 0, "iocharset=gb2312"))){
		//USBSTORAGE_TRACE("mount %s error: %s", dev, strerror(errno));
		rmdir(target);
		return -1;
	}
	return 0;
}

static int usbstorage_Mount(USBStorage* thiz, char* dev)
{
	int i = 0, ii = 0;
	char device[50] = {""};
	char target[50] = {""};
	dev[3] = '\0'; // remove number
	if(!thiz->nDevCnt){
		if(0 == thiz->GetCnt(thiz)){
			return -1;
		}
	}
	for(i = 0; i < thiz->nDevCnt; ++i){
		if(!strcasecmp(dev, thiz->szDevNode[i])){
			int nBlkCnt = thiz->nDevBlkCnt[i];
			//USBSTORAGE_TRACE("mount %s block=%d", thiz->szDevName[i], thiz->nDevBlkCnt[i]);
			for(ii = 0; ii < MAX_BLOCK_CNT && nBlkCnt > 0; ++ii){
				if(thiz->bDevBlk[i][ii]){
					sprintf(device, "/dev/%s%c", thiz->szDevNode[i], ii > 0 ? '0' + ii : '\0');
					sprintf(target, "/media/%s%c", thiz->szDevNode[i], ii > 0 ? '0' + ii : '\0');
					USBSTORAGE_TRACE("mount \"%s\"", dev);
					_usbstorage_Mount(device, target);
					--nBlkCnt;
				}
			}
			return 0;
		}
	}
	return -1;
}

static int usbstorage_MountAll(USBStorage* thiz)
{
	int i = 0, nRet = 0;
	if(!thiz->nDevCnt){
		if(0 == thiz->GetCnt(thiz)){
			return -1;
		}
	}
	for(i = 0; i < thiz->nDevCnt; i++){
		thiz->Mount(thiz, thiz->szDevNode[i]);
		nRet++;
	}
	return nRet;
}

inline static int _usbstorage_Umount(const char* target)
{
	int nRet = 0;
	if(0 != (nRet = umount(target))){
		//USBSTORAGE_TRACE("umount error: %s", strerror(errno));
		goto _usbstorage_Umount_exit1;
	}
	if(0 != (nRet = rmdir(target))){
		//USBSTORAGE_TRACE("rmdir error: %s", strerror(errno));
//		goto _usbstorage_Umount_exit1;
	}
	return 0;
_usbstorage_Umount_exit1:
	return -1;
}

static int usbstorage_Umount(USBStorage* thiz, const char* dev)
{
	int i = 0;
	char szTarget[50];
	DIR* dirPath =NULL;
	for(i = 0; i < MAX_BLOCK_CNT; ++i){
		sprintf(szTarget, "/media/%s%c", dev, i > 0 ? '0' + i : '\0');
		if((dirPath = opendir(szTarget))){
			closedir(dirPath);
			USBSTORAGE_TRACE("umount block \"%s\"", szTarget);
			_usbstorage_Umount(szTarget);
		}
	}
	return 0;
}

static int usbstorage_UmountAll(USBStorage* thiz)
{
	int i, nRet = 0;
	char szDev[50] = {""};
	for(i = 0; i < MAX_USB_STORAGE; ++i){
		sprintf(szDev, "sd%c", 'a' + i);
		nRet += thiz->Umount(thiz, szDev);
	}
	return nRet;
}

static USBStorage s_stUSBStorage;
int USBSTORAGE_Init()
{
	// interfaces
	s_stUSBStorage.Clear = usbstorage_Clear;
	s_stUSBStorage.Print = usbstorage_Print;
	s_stUSBStorage.GetCnt = usbstorage_GetCnt;
	s_stUSBStorage.GetName = usbstorage_GetName;
	s_stUSBStorage.GetSize = usbstorage_GetSize;
	s_stUSBStorage.Mount = usbstorage_Mount;
	s_stUSBStorage.MountAll = usbstorage_MountAll;
	s_stUSBStorage.Umount = usbstorage_Umount;
	s_stUSBStorage.UmountAll = usbstorage_UmountAll;

	// init
	s_stUSBStorage.Clear(&s_stUSBStorage);
	mkdir(USB_MOUNT_ROOT, 0);
	return 0;
}

void USBSTORAGE_Destroy()
{
	s_stUSBStorage.UmountAll(&s_stUSBStorage);
	s_stUSBStorage.Clear(&s_stUSBStorage);
	rmdir(USB_MOUNT_ROOT);
}

void USBSTORAGE_Print()
{
	return s_stUSBStorage.Print(&s_stUSBStorage);
}

int USBSTORAGE_GetCnt()
{
	return s_stUSBStorage.GetCnt(&s_stUSBStorage);
}

int USBSTORAGE_Mount(char* dev)
{
	return s_stUSBStorage.Mount(&s_stUSBStorage, dev);
}

int USBSTORAGE_MountAll()
{
	return s_stUSBStorage.MountAll(&s_stUSBStorage);
}

int USBSTORAGE_Umount(const char* dev)
{
	return s_stUSBStorage.Umount(&s_stUSBStorage, dev);
}

int USBSTORAGE_UmountAll()
{
	return s_stUSBStorage.UmountAll(&s_stUSBStorage);
}


//#define USB_STORAGE_TEST
//#define USB_UDEV_AUTOMOUNT
#if defined(USB_STORAGE_TEST)
int main(int argc, char** argv)
{
	USBSTORAGE_Init();
	printf("USB-storage count = %d\r\n", USBSTORAGE_GetCnt());
	USBSTORAGE_Print();
	USBSTORAGE_MountAll();
	system("du "USB_MOUNT_ROOT);
	getchar();
	USBSTORAGE_UmountAll();
	system("du "USB_MOUNT_ROOT);
	USBSTORAGE_Destroy();
	return 0;
}
#elif defined(USB_UDEV_AUTOMOUNT)
static void Help(char** argv)
{
	printf("usage:\r\n");
	printf("\t%s -m [dev]\r\n", *argv);
	printf("\t%s -u [dev]\r\n", *argv);
}

int main(int argc, char** argv)
{
	if(argc != 2 && argc != 3){
		Help(argv);
		return 1;
	}
	USBSTORAGE_Init();
	if(!strcasecmp(argv[1], "-m")){
		if(2 == argc){
			// mount all
			USBSTORAGE_MountAll();
		}else{
			USBSTORAGE_Mount(argv[2]);
		}
	}else if(!strcasecmp(argv[1], "-u")){
		if(2 == argc){
			// umount all
			USBSTORAGE_UmountAll();
		}else{
			USBSTORAGE_Umount(argv[2]);
		}
	}
	USBSTORAGE_Destroy();
	return 0;
}
#endif



