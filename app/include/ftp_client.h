#ifndef	__FTP_H__
#define	__FTP_H__

#define ANONYMOUS_NONE	0
#define ANONYMOUS_USE	1

// error code in FTP module
#define RTN_FTP_OK					0
#define RTN_FTP_NULL				(-1)
#define RTN_FTP_ACTION_FAIL			(-2)
#define RTN_FTP_USER_INCORRECT		(-3)
#define RTN_FTP_PASSWORD_INCORRECT	(-4)
#define RTN_FTP_ADDRESS_INCORRECT	(-5)
#define RTN_FTP_PORT_INCORRECT		(-6)
#define RTN_FTP_UPLOAD_BREAK 		(-7)

#define FTP_MAX_INDEX 999
#define FTP_TEMP_DISK_FOLDER "/root/rec/a1/ftp/"
#define FTP_REC_INTERVAL 30
#define FTP_REPEAT_INTERVAL 15
#define FTP_EXEC_INTERVAL 3
#define FTP_FILE_MIN_SIZE 1 // Unit is MB
#define FTP_REC_NORMAL_INTERVAL 60
#define FTP_REC_DELAY (2 * 60)
#define RECORD_SEARCH_COUNT_MAX 99
#define FTP_FILE_PATH_MAX 256
#define FTP_FILE_NAME_MAX 200
#define FTP_MAX_FILE_COUNT (100)  // Avoid abnormal

typedef struct FTP
{
	int handle;
}FTP_SESSION_t;

typedef struct _FTP_TASK_ITEM FTP_TASK_ITEM;
struct _FTP_TASK_ITEM
{
	unsigned short type;
	unsigned short detail;
	int search_type;
	int search_channels;
	time_t search_begin;
	time_t search_end;
	time_t event_time;
	FTP_TASK_ITEM *next;
	char rec_remote_folder_name[FTP_FILE_PATH_MAX];
	char rec_file_name[FTP_FILE_NAME_MAX];
	char rec_local_full_file_name[FTP_FILE_PATH_MAX];
};


extern FTP_SESSION_t* FTP_init(void);
extern void FTP_destroy(FTP_SESSION_t* ftp);

extern int FTP_login(FTP_SESSION_t* const ftp, const char* server,short port, const char* username, const char* password, unsigned int is_anonymous);
extern void FTP_logout(FTP_SESSION_t* const ftp);
extern int FTP_send(FTP_SESSION_t* const ftp, char* src_full_filename, char* dest_folder, char* dest_filename, unsigned long max_size);
extern int FTP_delete(FTP_SESSION_t* const ftp, char* dest_folder, char* dest_filename);
extern int FTP_keep(FTP_SESSION_t* const ftp);

extern const char* FTP_errordesc(FTP_SESSION_t* const ftp);
extern int FTP_errorcode(FTP_SESSION_t* const ftp);
extern int ftp_make_and_change_folder(int sock_fd, const char* folder_name);
//#define _TEST_FTP
#ifdef _TEST_FTP
int main(int argc, char** argv)
{
	printf("%d-%s\n", FTP_errorcode(NULL), FTP_errordesc(NULL));
	
	FTP_SESSION_t* const ftp = FTP_init();

	int ret = FTP_login(ftp, "192.168.2.188", 21, "huangzhijun", "huangzhijun", ANONYMOUS_NONE);

	ret = FTP_send(ftp, "/tmp/nfs/project/hidvr-svn/app/ftp/test1.avi", "/private/huangzhijun/ftp_test/123/456", "15.24.31-15.25.30-NORMAL-00-01.avi", (10*1024*1024));
	//ret = FTP_send(ftp, "/tmp/test.avi", "/private/huangzhijun/ftp_test/abc", "15.25.31-15.26.30-NORMAL-00-01.avi", (10*1024*1024));

	FTP_logout(ftp);

	FTP_destroy(ftp);
	return 0;
}
#endif

#endif
