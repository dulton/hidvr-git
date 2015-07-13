#ifndef __tool_for_ddns_smtp__
#define __tool_for_ddns_smtp__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <regex.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

typedef struct
{
    unsigned short id;
    unsigned short flags;
    unsigned short questNum;
    unsigned short answerNum;
    unsigned short authorNum;
    unsigned short additionNum;
}DNS_requset_pack1;

typedef struct
{
    unsigned short type;
    unsigned short res_class;
}DNS_requset_pack2;

typedef struct
{
	unsigned short name;
	unsigned short type;
	unsigned short classes;
	unsigned int   ttl;
	unsigned short length;
}DNS_response_answer;

#define DNS_SERVER 		"8.8.8.8"
#define DNS_IP  		0x0001
#define DNS_NAME 		0x0005
#define REGEX_FIND_IP 	"([0-9]{1,3}\\.){3}[0-9]{1,3}"

#define REGEX_EAMIL		"([A-Za-z0-9]+[-_]?[A-Za-z0-9]+[\\.])*([A-Za-z0-9]+)+"\
						"@([A-Za-z0-9]+[-_]?[A-Za-z0-9]+[\\.])+([A-Za-z0-9]+)+"

/*
char *fun
	NULL for error
int fun
	-1 for error
	0 for success
*/

char* set_conf_format( char *dest );

int readconf( char *path, char *confaim, char **confinfo, int confnum );
/*
example:
	char *path = "/etc/smtp.conf"
	char *confaim = @163.com;
	char *confinfo[2] ;
	int confnum = sizeof(p)/sizeof(p[0]);

smtp.conf format:
	[@163.com]
	smtp.163.com	-> confinfo[0]
	1				-> confinfo[1]

*/
int apllyconf( FILE *file, char **info );

void add_enter( char *s );

void remove_enter( char *s );
//The function above are for read config file

int findkeyword( char *regex_line, char *source, char **dest );

int free_set_NULL( char **p );

void hold_time( int peroid );

int str_insert_str( char** dest, ... );
/*
example:
	char *p1, *p2="test";
	str_insert_str( &p1, "%s", p2);
warning:
	this only support %s
*/

int base64Encode_by_len( char *intext, char **output, int len );

int get_email_date( char result[128] );

int genRandomString( char string[], int length, unsigned int seed );

int Read_file( char *Path, char *openmode, int Encode_base64, char **dest );

int Save_file( char *path, char *openmode, void *dest, size_t count );

int check_email_format( char *email );

int check_ip_format( char *ip );

void string_to_lower( char *str );

void string_to_upper( char *str );

int string_insert_len( char **str, int *allocated_len, char *val, int len, int position );

int string_replace_str( char **str, char *val, char *replaceval );

char *my_new_string( char *source );

int my_get_ipn( char *host );

int run_cmd(char *cmd , int close_child_fd);

int run_cmd_rw( char *cmd_raw, char *type, char *aim, int len );

size_t WriteData(const void *buffer, size_t size, size_t count, FILE* stream);

size_t ReadData(void *buffer, size_t size, size_t count, FILE *stream);
#ifdef __cplusplus
};
#endif

#endif
