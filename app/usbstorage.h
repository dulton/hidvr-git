

#ifndef __USBSTORAGE_H__
#define __USBSTORAGE_H__

#define USB_NAME_LEN (50)
#define USB_NODE_LEN (20)
#define MAX_USB_STORAGE (10)
#define MAX_BLOCK_CNT (10)
#define USB_MOUNT_ROOT "/media/"

extern int USBSTORAGE_Init();
extern void USBSTORAGE_Destroy();
extern void USBSTORAGE_Print();
extern int USBSTORAGE_GetCnt();
extern int USBSTORAGE_Mount(char* dev);
extern int USBSTORAGE_MountAll();
extern int USBSTORAGE_Umount(const char* dev);
extern int USBSTORAGE_UmountAll();

#endif	//__USBSTORAGE_H__

