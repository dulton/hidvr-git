#include "tool_for_general.h"

//在配置对应查询名前后加[]，并转换成小写
char *set_conf_format( char *dest )
{
	if(dest == NULL){
		return NULL;
	}
	char *p;

	if( str_insert_str( &p, "[%s]", dest ) == -1 ){
		return NULL;
	}

	string_to_lower( p );

	return p;
}

//读取配置信息path是配置文件路径，confaim是需要查询的配置信息，
//confinfo是返回的信息的指针 ， confnum是需要返回配置信息的数量
int readconf( char *path, char *confaim, char **confinfo, int confnum )
{
	if(path == NULL || confaim == NULL || confinfo == NULL){
		return -1;
	}
	FILE *conf = NULL;
	int ii = 0, conflen = 1024;
	char confbuf[conflen] ;

	/**********open config file**********/
	if( ( conf = fopen( path, "r" ) ) == NULL ){
		perror( "open config file" );
		return -1;
	}

	/**********read config**********/
	while( 1 ){
		if( fgets( confbuf, conflen, conf ) == NULL ){
			perror( "fgets conffile" );
			fclose( conf );
			return -1;
		}
		else if( strstr( confbuf, "[end]" ) != NULL ){
			printf( "please enter a right ddnsserver!\n" );
			fclose( conf );
			return -1;
		}
		else if( *confbuf == '#' ){
			for( ii = 0 ; ii < confnum ; ii++ ){
				if( fgets( confbuf, conflen, conf ) == NULL ){
					fclose( conf );
					return -1;
				}
			}

			continue;
		}
		else if( strstr( confbuf, confaim ) != NULL ){
			for( ii = 0 ; ii < confnum ; ii++ ){
				if( apllyconf( conf, &confinfo[ii] ) == -1 ){
					fclose( conf );
					return -1;
				}
			}

			break;
		}
		else{
			continue;
		}
	}

	fclose( conf );
	return 0;
}

//把对应配置信息copy到对应的字符串指针
int apllyconf( FILE *file, char **info )
{
	if(file == NULL || info == NULL){
		return -1;
	}
	int conflen = 1024;
	char confbuf[conflen];

	if( fgets( confbuf, conflen, file ) == NULL ){
		perror( "fgets conffile" );
		return -1;
	}

	if( *confbuf == '#' ){
		printf( "Thers is a '#' before the conf_info!\n" );
		return -1;
	}

	remove_enter( confbuf );

	*info = ( char * )malloc( ( strlen( confbuf ) + 1 ) * sizeof( char ) );
	strcpy( *info, confbuf );
	//printf("%s\n",*info);
	return 0;
}

//删除读取到的配置文件信息结尾的回车换行
void remove_enter( char *s )
{
	if(s == NULL){
		return;
	}
	char *c;

	if( ( c = strstr( s, "\r\n" ) ) != NULL || ( c = strstr( s, "\n" ) ) != NULL ){
		*c = '\0';
	}

	return;
}

//配置信息中以<>代表GET语句的回车换行，需要用此函数替换读取到的GET语句的<>
void add_enter( char *s )
{
	if(s == NULL){
		return;
	}
	char *ps, *pe;
	ps = s;

	while( ( pe = strstr( ps, "<>" ) ) != NULL ){
		*pe = '\r';
		pe++;
		*pe = '\n';
		ps = pe;
	}

	return;
}

//base64编码
int base64Encode_by_len( char *intext, char **output, int len )
{
	if(intext == NULL || output == NULL){
		return -1;
	}
	unsigned char ibuf[3];
	unsigned char obuf[4];
	static char table64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int i = 0, j = 0,  k = 0, inputparts = 0, out_len = 0 ;

	out_len = ( len / 3 + 1 ) * 4;

	if( ( *output = ( char * ) malloc( 2 * out_len / 76 + out_len + 10 * sizeof( char ) ) ) == NULL ){
		return -1;
	}

	char *p = *output;

	for( j = 0; j < len; ){
		for( i = inputparts = 0; i < 3; i++ ){
			if( j < len ){
				inputparts++;
				ibuf[i] = intext[j];
				j++;
			}
			else{
				ibuf[i] = 0;
			}
		}

		obuf [0] = ( ibuf [0] & 0xFC ) >> 2;
		obuf [1] = ( ( ibuf [0] & 0x03 ) << 4 ) | ( ( ibuf [1] & 0xF0 ) >> 4 );
		obuf [2] = ( ( ibuf [1] & 0x0F ) << 2 ) | ( ( ibuf [2] & 0xC0 ) >> 6 );
		obuf [3] = ibuf [2] & 0x3F;

		switch( inputparts ){
			case 1: // only one byte read
				sprintf( p, "%c%c==",
						 table64[obuf[0]],
						 table64[obuf[1]] );
				break;

			case 2: // two bytes read
				sprintf( p, "%c%c%c=",
						 table64[obuf[0]],
						 table64[obuf[1]],
						 table64[obuf[2]] );
				break;

			default:
				sprintf( p, "%c%c%c%c",
						 table64[obuf[0]],
						 table64[obuf[1]],
						 table64[obuf[2]],
						 table64[obuf[3]] );
				break;
		}

		p += 4;
		k += 4;

		if( k >= 76 ){
			memcpy( p, "\r\n", 2 * sizeof( char ) );
			p += 2;
			k = 0;
		}//every 76 char add an enter
	}

	*p = '\0';
	return 0;
}

//获取邮件的时间，时区信息
int get_email_date( char result[128] )
{
	time_t clock = time( NULL );
	struct tm *timeptr ;
	int i = 0 ;

	char *wday_name[7] ={
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
	};

	char *mon_name[12] ={
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
	};

	char *time_zone[24] ={
		"-1100", "-1000", "-0900", "-0800", "-0700", "-0600",
		"-0500", "-0400", "-0300", "-0200", "-0100", "0000",
		"0100", "0200", "0300", "0400", "0500", "0600",
		"0700", "0800", "0900", "1000", "1100", "1200",
	};

	timeptr = gmtime( &clock );
	i = timeptr->tm_mday * 24 + timeptr->tm_hour;
	timeptr = localtime( &clock );
	i = timeptr->tm_mday * 24 + timeptr->tm_hour - i + 11;

	sprintf( result, "%s, %d %s %d %d:%d:%d %s",
			 wday_name[timeptr->tm_wday], timeptr->tm_mday,
			 mon_name[timeptr->tm_mon], 1900 + timeptr->tm_year,
			 timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec, time_zone[i] );

	return 0;
}

//以0.1秒为间隔，暂停peroid秒
void hold_time( int peroid )
{
	int i = 0;
	int times = peroid * 10;

	while( i < times ){
		usleep( 100000 );
		i++;
	}

	return;
}

//free 申请的空间并置为NULL
int free_set_NULL( char **p )
{
	if( *p == NULL ){
		return 0;
	}

	free( *p );
	*p = NULL;
	return 0;
}

//以正则表达式查找相关信息，regex_line是正则表达式 ，source是待查询的内容 ，dest是返回查询到的信息
int findkeyword( char *regex_line, char *source, char **dest )
{
	if(regex_line == NULL || source == NULL || dest == NULL || *dest != NULL){
		return -1;
	}
	regex_t reg;
	size_t nmatch = 1;
	regmatch_t matchptr[1];
	int dest_len;

	//把指定的正则表达式编译成一种特定的数据格式
	//REG_EXTENDED以功能更加强大的扩展正则表达式的方式进行匹配
	//REG_ICASE为匹配字母时忽略大小写
	if( regcomp( &reg, regex_line, REG_EXTENDED | REG_ICASE ) != 0 ){
		printf( "This is a wrong regex!\n" );
		regfree( &reg );
		return -1;
	}

	// 匹配待查询的内容
	if( ( regexec( &reg, source, nmatch, matchptr, 0 ) != 0 ) ){
		//printf("Can't find any keyword!\n");
		regfree( &reg );
		*dest = NULL;
		return -1;
	}

	//copy查询到的信息
	dest_len = matchptr[0].rm_eo - matchptr[0].rm_so;
	*dest = ( char * )realloc( *dest, ( dest_len + 10 ) * sizeof( char ) );
	char *dest_end;

	source += matchptr[0].rm_so;
	memcpy( *dest, source, dest_len );
	dest_end = *dest + dest_len;
	*dest_end = '\0';

	//printf("find result :%s\n",*dest);
	regfree( &reg );

	return 0;
}

//获取指定长度的随机字符串
int genRandomString( char string[], int length, unsigned int seed )
{
	if(string == NULL){
		return -1;
	}
	int flag, i;
	srand( seed );

	for( i = 0; i < length; i++ ){
		flag = rand() % 3;

		switch( flag ){
			case 0:
				string[i] = 'A' + rand() % 26;
				break;

			case 1:
				string[i] = 'a' + rand() % 26;
				break;

			case 2:
				string[i] = '0' + rand() % 10;
				break;

			default:
				string[i] = 'x';
				break;
		}
	}

	string[length] = '\0';
	return 0;
}

//实现asprintf的函数，只能插入%s
int str_insert_str( char **dest, ... )
{
	if(dest == NULL){
		return -1;
	}
	va_list parameter;
	va_start( parameter, dest );

	//第二个字符串，即输出格式
	char *par = va_arg( parameter, char * );
	int i = 0 ;
	char *p = par;

	//how many %s
	while( p - par < strlen( par ) ){
		p = strstr( p, "%s" );

		if( p == NULL ){
			break;
		}
		else{
			p += strlen( "%s" );
			i++;
		}
	}

	//cut the aim str and read the parameter
	int str_len = 0, j = 0;
	char **p1 = NULL, ** p2 = NULL, ** p3 = NULL ;

	//before %s
	if( ( p1 = ( char ** )malloc( ( i + 1 ) *  sizeof( char * ) ) ) == NULL ){
		printf( "Error in malloc for the p1\n" );
		return -1;
	}

	//after %s
	if( ( p2 = ( char ** )malloc( i *  sizeof( char * ) ) ) == NULL ){
		printf( "Error in malloc for the p2\n" );
		return -1;
	}

	//parameter
	if( ( p3 = ( char ** )malloc( i *  sizeof( char * ) ) ) == NULL ){
		printf( "Error in malloc for the p3\n" );
		return -1;
	}

	p = par;

	for( j = 0; j < i ; j++ ){
		p = strstr( p, "%s" );
		p1[j] = p;
		p += strlen( "%s" );
		p2[j] = p;
		p3[j] = va_arg( parameter, char * );

		if( p3[j] != NULL ){
			str_len += strlen( p3[j] );
		}
		else{
			str_len += 1;
		}
	}

	va_end( parameter );

	//malloc dest and insert str
	str_len += strlen( par );

	if( ( *dest = ( char * )malloc( ( str_len + 10 ) * sizeof( char ) ) ) == NULL ){
		printf( "Error in malloc for the dest\n" );
		return -1;
	}

	p = *dest;

	memcpy( p, par, p1[0] - par );
	p += p1[0] - par;
	p1[j] = par += strlen( par );

	for( j = 0 ; j < i ; j++ ){
		if( p3[j] != NULL ){
			memcpy( p, p3[j], strlen( p3[j] )*sizeof( char ) );
			p += strlen( p3[j] );
		}
		else{
			memcpy( p, "?", sizeof( char ) );
			p += sizeof( char );
		}

		memcpy( p, p2[j], ( p1[j + 1] - p2[j] )*sizeof( char ) );
		p += p1[j + 1] - p2[j];
	}

	memcpy( p, "\0", sizeof( char ) );
	free( p1 );
	free( p2 );
	free( p3 );
	return 0;
}

//从文本读取文件，并根据Encode_base64_or_not，判断是否进行BASE64编码
int Read_file( char *Path, char *openmode, int Encode_base64, char **dest )
{
	if(Path == NULL || openmode == NULL || dest == NULL){
		return -1;
	}
	FILE *fp;
	char *file;
	unsigned int length;
	struct stat FileStat;
	if(stat(Path,&FileStat)!=0){
		perror("Problem getting information");
		switch(errno){
			case ENOENT:
				printf("File:%s not found.\n",Path);
				break;
			case EINVAL:
				printf("Invalid parameter to stat.\n");
				break;
			default:
				printf("Unexpected error in_stat.\n");
				break;
		}
	}else{
		length = FileStat.st_size;
	}

	if( ( fp = fopen( Path, openmode ) ) == NULL ){
		printf( "Can not open " );
		return -1;
	}
/*
	if( fseek( fp, 0, SEEK_END ) == -1 ){
		printf( "fseek error code:%d \n error message: %s\n", errno, strerror( errno ) );
		fclose( fp );
		return -1;
	}

	int length = ftell( fp );

	if( fseek( fp, 0, SEEK_SET ) == -1 ){
		printf( "fseek error code:%d \n error message: %s\n", errno, strerror( errno ) );
		fclose( fp );
		return -1;
	}
*/
	if( ( file = ( char * )malloc( ( length + 10 ) * sizeof( char ) ) ) == NULL ){
		printf( "Error in malloc for " );
		fclose( fp );
		return -1;
	}

	if( ReadData( file, sizeof( char ), length, fp ) != length ){
		printf( "Can read the complete " );
		fclose( fp );
		free_set_NULL( &file );
		return -1;
	}

	fclose( fp );
	file[length] = '\0';

	if( Encode_base64 == 1 ){
		base64Encode_by_len( file, dest, length );
		free_set_NULL( &file );
	}
	else{
		*dest = file;
	}

	return length;
}

//openmode：r只读、r+可读写、rb+二进制打开、rw+读写打开
//w只写，清0、w+读写，清0、wb只写二进制、wb+读写二进制
//a只写，不清零，EOF符号保留、a+，不清零，EOF符号不保留，ab+二进制读写打开，不清零
int Save_file( char *Path, char *openmode, void *dest, size_t count )
{
	if(Path == NULL || openmode == NULL || dest == NULL){
		return -1;
	}
	FILE *fp;

	if( ( fp = fopen( Path, openmode ) ) == NULL ){
		printf( "Can not open" );
		return -1;
	}

	int ret = 0, total_size = 0, SaveBuff = 1024 * 64, left = SaveBuff;
	unsigned char inpBuf[SaveBuff], *data = (unsigned char*)dest;
	unsigned char *p = data;
	if(count < SaveBuff){
		left = count;
	}
	while(1){
		memcpy(inpBuf, p, left);
		ret = WriteData(inpBuf, sizeof(unsigned char), left * sizeof(unsigned char), fp);
		if(ret == -1){
			break;
		}

		total_size += ret;
		memset(inpBuf, 0, SaveBuff);
		p += ret;
		left = count - total_size;

		if( left > SaveBuff){
			left = SaveBuff;
		}
		else if(left == 0){
			break;
		}
	}
	fclose( fp );
	return total_size;
}

//用正则表达式判断邮件格式是否正确
int check_email_format( char *email )
{
	if(email == NULL){
		return -1;
	}
	char *res = NULL;
	int ret = 0;

	do{
		if( (ret = findkeyword( REGEX_EAMIL, email, &res )) == -1 ){
			printf( "The E-mail address is wrong!\n" );
			break;
		}

		if(strcmp(email, res)){
			printf( "The is not only an E-mail address!\n" );
			ret = -1;
			break;
		}
	}while(0);
	free_set_NULL( &res );

	return ret;
}

//字符串转换为小写
void string_to_lower( char *str )
{
	if(str == NULL){
		return;
	}
	for( ; *str != '\0'; str++ ){
		*str = tolower( *str );
	}
	return;
}

//字符串转换为大写
void string_to_upper( char *str )
{
	if(str == NULL){
		return;
	}
	for( ; *str != '\0'; str++ ){
		*str = toupper( *str );
	}
	return;
}

//在malloc分配内存的指针的指定位置添加内容，str为目标字串，allocated_len是原始分配长度，val是需要添加内容
//len是添加内容的长度，position是指定添加内容的位置（为负数时默认添加到目标字串的结尾）。
int string_insert_len( char **str, int *allocated_len, char *val, int len, int position )
{
	if(str == NULL || *str == NULL || allocated_len == NULL || val == NULL){
		return -1;
	}
	int now_len = strlen( *str );

	if( *allocated_len < 0 ){
		return -1;
	}

	//len = -1, insert the whole string
	if( len == 0 ){
		return 0;
	}
	else if( len < 0 ){
		len = strlen( val );
	}

	//position = -1, insert at the end of the string
	if( position < 0 ){
		position = now_len;
	}

	if( now_len + len >= *allocated_len ){
		*str = ( char * )realloc( *str, ( now_len + len + 64 ) * sizeof( char ) );
		*allocated_len = now_len + len + 64;

		if( *str == NULL ){
			return -1;
		}
	}

	if( position < now_len ){
		memmove( *str + position + len, *str + position, now_len - position + 1 );
		memmove( *str + position, val, len );
	}
	else{
		strcpy( *str + now_len, val );
	}

	return 0;
}

int string_replace_str( char **str, char *val, char *replaceval )
{
	if(str == NULL || *str == NULL || replaceval == NULL || val == NULL){
		return -1;
	}
	char *val_start = strstr( *str, val );

	if( val_start == NULL ){
		return -1;
	}

	char *val_end = val_start + strlen( val );
	int position = val_start - *str;
	int malloclen = strlen( *str );
	memmove( val_start, val_end, strlen( val_end ) + 1 );

	if( string_insert_len( str, &malloclen, replaceval, strlen( replaceval ), position ) != 0 ){
		return -1;
	}

	return 0;
}


//复制source的全部内容到一个未分配空间的指针
char *my_new_string( char *source )
{
	if(source == NULL){
		return NULL;
	}

	int len = strlen( source );

	if( len <= 0 ){
		return NULL;
	}

	char *dest = ( char * )malloc( ( len + 1 ) * sizeof( char ) );

	if( dest == NULL ){
		return NULL;
	}

	strcpy( dest, source );

	return dest;
}

int check_ip_format( char *ip )
{
	if(ip == NULL){
		return -1;
	}
	char *res = NULL;
	int ret = 0;

	do{
		if( (ret = findkeyword( REGEX_FIND_IP, ip, &res )) == -1 ){
			break;
		}
		if( strcmp( ip, res ) ){
			printf( "The is not only an ip address!\n" );
			ret = -1;
			break;
		}
	}while(0);
	free_set_NULL( &res );

	return ret;
}

//由域名获取 网络字节序的IP地址，有超时控制
int Make_DNS_request( const char *dns_name, unsigned char *buf, int *len );
int get_preferred_dns( char **dns );
int my_get_ipn( char *host )
{
	int ipn = -1;
	if(host == NULL){
		return -1;
	}
	if(check_ip_format(host) ==0){
		struct in_addr addr;
		inet_aton(host, &addr);
		ipn = addr.s_addr;
		return ipn;
	}

	int ipn_sockfd;
	struct timeval Stimeout ={ 5, 0 };
	struct timeval Rtimeout ={ 10, 0 };

	//创建UDP socket，并限定超时发送，接收时间
	ipn_sockfd = socket( AF_INET, SOCK_DGRAM, 0 );

	if( ipn_sockfd < 0 ){
		perror( "create socket failed" );
		close( ipn_sockfd );
		return -1;
	}

	if( setsockopt( ipn_sockfd, SOL_SOCKET, SO_SNDTIMEO, ( char * )&Stimeout, sizeof( struct timeval ) ) ){
		perror( "set send timeout" );
		close( ipn_sockfd );
		return -1;
	}

	if( setsockopt( ipn_sockfd, SOL_SOCKET, SO_RCVTIMEO, ( char * )&Rtimeout, sizeof( struct timeval ) ) ){
		perror( "set recv timeout" );
		close( ipn_sockfd );
		return -1;
	}

	int msglen = 0, ret = 0;
	unsigned char sbuf[1024];
	char *dns = NULL ;

	//构建DNS请求包
	Make_DNS_request( host, sbuf, &msglen );

	struct sockaddr_in dest;
	socklen_t len = sizeof( struct sockaddr_in );
	memset( &dest, 0, sizeof( dest ) );
	dest.sin_family = AF_INET;
	dest.sin_port = htons( 53 );

	//在/etc/resolv.conf里面获取DNS,没找到则用默认的DNS_SERVER
	ret = get_preferred_dns( &dns );

	if( ret == 0 ){
		dest.sin_addr.s_addr = inet_addr( dns );
		free_set_NULL( &dns );
	}
	else{
		dest.sin_addr.s_addr = inet_addr( DNS_SERVER );
	}

	ret = sendto( ipn_sockfd, sbuf, msglen, 0,
				  ( struct sockaddr * )&dest, len ); //发送DNS请求包

	//printf("%d\n",ret);
	if( ret != msglen ){
		perror( "sendto" );
		close( ipn_sockfd );
		return -1;
	}

	char rbuf[1024];
	struct sockaddr_in socket_info;

	//接收DNS返回信息
	ret = recvfrom( ipn_sockfd, rbuf, sizeof( rbuf ), 0,
					( struct sockaddr * )&socket_info, &len );

	if( ret == -1 ){
		perror( "recvfrom" );
		close( ipn_sockfd );
		return -1;
	}

	char *ptr = rbuf;
	int i = 0;
	unsigned short type, datalen;

	//因为请求包和回应包前面部分的结构相同，可用同一结构获取
	DNS_requset_pack1 res_req;
	//从回应包中获取服务器应答数
	memcpy( &res_req, ptr, sizeof( res_req ) );
	//指针跳过请求包
	ptr += msglen;

	for( i = 0 ; i < ntohs( res_req.answerNum ) ; i++ ){ //根据应答数遍历应答结果

		ptr += 2; 	// move ptr to Type
		type = htons( *( ( unsigned short * )ptr ) );
		ptr += 8;	// move ptr to Data lenth
		datalen = ntohs( *( ( unsigned short * )ptr ) );
		ptr += 2; 	// move ptr to Data

		if( type == DNS_NAME ){ //判断是否为域名别名
			ptr += datalen;
		}
		else if( type == DNS_IP ){ //判断是否为域名对应IP，是则取第一个，并返回该IP地址的网络字节序
			ipn = *( ( unsigned int * )ptr );
			ptr += datalen;
			break;
		}
		else{
			break;
		}
	}

	close( ipn_sockfd );
	return ipn;
}

//构建DNS请求包
int Make_DNS_request( const char *dns_name, unsigned char *buf, int *len )
{
	if(dns_name == NULL || buf == NULL){
		return -1;
	}
	char tmp[63];
	char *pos = NULL;
	unsigned char namebuf[63];
	unsigned char *ptr = namebuf;
	int n, namelen = 0;
	DNS_requset_pack1 pDnsHdr1;
	DNS_requset_pack2 pDnsHdr2;

	//过滤请求查询名信息
	strcpy( tmp, dns_name );
	pos = strstr( tmp, "://" );

	//删除查询名前面的HTTP:// FTP://等协议信息
	if( pos != NULL ){
		pos = pos + strlen("://");
		pos = strtok(pos, "/");
	}else{
		pos = strtok(tmp, "/");
	}

	if( strstr( pos, "." ) == NULL ){
		return -1;
	}

	//printf("%s\n",pos);
	while( 1 ){
		n = strlen( pos ) - ( strstr( pos, "." ) ? strlen( strstr( pos, "." ) ) : 0 );
		*ptr ++ = ( unsigned char )n;
		memcpy( ptr, pos, n );
		namelen += n + 1;
		ptr += n;

		if( !strstr( pos, "." ) ){
			*ptr = ( unsigned char )0;
			ptr ++;
			namelen += 1;
			break;
		}

		pos += n + 1;
	}

	pDnsHdr1.id = htons( 0xff00 );		//标示ID
	pDnsHdr1.flags = htons( 0x0100 );		//标志
	pDnsHdr1.questNum = htons( 1 );		//问题数
	pDnsHdr1.answerNum = 0;				//回答数
	pDnsHdr1.authorNum = 0;				//授权信息
	pDnsHdr1.additionNum = 0;			//额外信息
	pDnsHdr2.type = htons( 1 );			//DNS查询类型，A类型，即获取IPv4地址
	pDnsHdr2.res_class = htons( 1 );			//响应类
	/*DNS其他查询类型
	CNAME	5		规范名称（别名记录）
	PTR		12		指针记录（反向解析记录），从IP获取NAME
	HINFO	13		主机信息（硬件地址、CPU、系统等）
	MX		15		邮件交换记录
	SRV		33		服务记录
	IXFR	251		增量区域传输记录
	AXFR	252		对区域转换的请求（标准的区域传输记录）
	*或ANY	255		对所有记录的请求
	*/

	//打包DNS请求包，将查询名包含进去，并获取请求包长度
	*len = 0;
	memcpy( buf, &pDnsHdr1, sizeof( DNS_requset_pack1 ) );

	*len = sizeof( DNS_requset_pack1 );
	memcpy( buf + *len, namebuf, namelen );

	*len = sizeof( DNS_requset_pack1 ) + namelen;
	memcpy( buf + *len, &pDnsHdr2, sizeof( DNS_requset_pack2 ) );

	*len = sizeof( DNS_requset_pack1 ) + namelen + sizeof( DNS_requset_pack2 );

	return 0;
}

//在系统文件/etc/resolv.conf里面寻找优先匹配的DNS地址
int get_preferred_dns( char **dns )
{
	if(dns == NULL){
		return -1;
	}
	char *res = NULL, *dns_path = "/etc/resolv.conf", *source = NULL;
	int ret = 0, dns_pack_len;
	dns_pack_len = Read_file( dns_path, "r", 0, &source );

	if( source == NULL ){
		printf( "Can't find /etc/resolv.conf !\n" );
		return -1;
	}

	do{
		if( findkeyword( REGEX_FIND_IP, source, &res ) == -1 ){
			printf( "Can't get the DNS!\n" );
			ret = -1;
			break;
		}

		*dns = my_new_string( res );

		if( dns == NULL ){
			ret = -1;
			break;
		}

	}
	while( 0 );

	free_set_NULL( &source );
	free_set_NULL( &res );
	return ret;
}

int run_cmd_closefd( char *cmdstring );
int run_cmd(char *cmd , int close_child_fd)
{
	if(cmd == NULL){
		return -1;
	}
	int ret = 0;
	unsigned char cmdres, sigres;
	sig_t old_handler;

	old_handler = signal( SIGCHLD, SIG_DFL );
	if( close_child_fd ){
		ret = run_cmd_closefd( cmd );
	}
	else{
		ret = system( cmd );
	}

	signal( SIGCHLD, old_handler );

	sigres = ret & 0x00FF;
	cmdres = ret >> 8 & 0x00FF;
	if( sigres != 0 ){
		printf( "Command is stop by some signal,signal ID:%d\n", sigres );
		return ret;
	}
	else if( cmdres == 127 ){
		printf( "Command not found!\n" );
		return ret;
	}
	else if( ret == -1 ){
		perror( "system error:" );
		return ret;
	}
	else{
		return 0;
	}
}

int run_cmd_rw( char *cmd_raw, char *type, char *aim, int len )
{
	if(type == NULL){
		return -1;
	}
	char *cmd = NULL;
	int ret = 0, cmd_len = 0;

	if ( ( type[0] != 'r' && type[0] != 'w' ) && type[1] != 0 ){
		printf( "type just can be \"r\" or \"w\"!\n" );
		return -1;
	}

	if( cmd_raw != NULL ){
		cmd_len = strlen(cmd_raw); 
	}
	else{
		printf( "Empty cmd string!\n" );
		return -1;
	}

	cmd = (char*)malloc( ( cmd_len + 256 ) * sizeof(char) );
	if( cmd == NULL ){
		perror( "ERROR in malloc for cmd string:" );
		return -1;
	}

	//将错误输出定向到标准输出
	ret = snprintf( cmd, ( cmd_len + 256 ) * sizeof(char), "%s 2>&1", cmd_raw );
	if( ret <= 0 ){
		free( cmd );
		return - 1;
	}

	FILE *stream = popen( cmd, type ); //建立管道,将标准（输出|输入）定义到管道（写|读）端
	free( cmd );

	if ( !stream ){
		return -1;
	}

	if( aim != NULL ){
		if( *type == 'r' ){
			memset( aim, 0, len );
			ret = ReadData( aim, sizeof(char), len, stream );
			if( ret > 0 ){
				if( !feof( stream ) ){
					printf( "Data stream is not totally read! Please set abigger buffer!\n" ); 
				}
				*( aim += ret ) = '\0';
			}
		}
		else{
			ret = WriteData( aim, sizeof(char), len, stream );
			if( ret > 0 && ret != len ){
				printf( "Data stream is not totally written in!\n" );
			}
		}

		if ( ferror( stream ) ){
			ret = -1;
			printf( "Data stream operation failed!\n" ); 
		}
	}

	int pclose_ret = pclose( stream ); //关闭管道
	if( pclose_ret == -1 ){
		perror( "Error in pclose:" );
		return -1;
	}

	return ret;
}

int run_cmd_closefd( char *cmdstring )
{
	pid_t pid;
	int status;

	if( cmdstring == NULL ){
		return -1;
	}

	if( ( pid = fork() ) < 0 ){
		status = -1;
	}
	else if( pid == 0 ){
		int i = 3;
		for( i = 3; i < sysconf(_SC_OPEN_MAX); i++){
			close(i);
		}
		execl( "/bin/sh", "sh", "-c", cmdstring, ( char * )0 );
		exit( 127 ); //子进程正常执行则不会执行此语句
	}
	else{
		while( waitpid( pid, &status, 0 ) < 0 ){
			if( errno != EINTR ){
				status = -1;
				break;
			}
		}
	}

	return status;
}

size_t WriteData(const void *buffer, size_t size, size_t count, FILE* stream)
{
	if(buffer == NULL || stream == NULL){
		return 0;
	}
	size_t ret = fwrite(buffer, size, count, stream);
	if(ret < count){
		if(feof(stream) == 0){
			perror("fwrite");
			return -1;
		}
	}
	return ret;
}

size_t ReadData(void *buffer, size_t size, size_t count, FILE *stream)
{
	if(buffer == NULL || stream == NULL){
		return 0;
	}
	size_t ret = fread(buffer, size, count, stream);
	if(ret < count){
		if(feof(stream) == 0){
			perror("fread");
			return -1;
		}
	}
	return ret;
}
