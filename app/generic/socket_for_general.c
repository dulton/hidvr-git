#include "socket_for_general.h"

//根据host获取网络字节序的IP地址
int get_ipn( char *host )
{
	if(host == NULL){
		return -1;
	}
	struct addrinfo *answer, hint;
	struct in_addr ipn;
	int ret;
	memset( &hint, 0, sizeof( hint ) );
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	ret = getaddrinfo( host, NULL, &hint, &answer );

	if( ret != 0 ){
		printf( "getaddrinfo: %s\n", gai_strerror( ret ) );
		return -1;
	}

	ipn = ( ( struct sockaddr_in * )( answer->ai_addr ) )->sin_addr;

	if( ( char * )inet_ntoa( ipn ) == NULL ){
		printf( "Get a wrong ip from the host!\n" );
	}

	//printf("IP: %s\n",(char*)inet_ntoa( ipn ));
	freeaddrinfo( answer );
	return ipn.s_addr;
}

int CreateSocket( int Send_system_buf, int Recv_system_buf, struct timeval Stimeout, struct timeval Rtimeout, struct linger *sLinger )
{
	int sockfd;

	if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) <= 2 ){
		perror( "socket error!" );
		return -1;
	}

	if( sLinger != NULL ){
		//set connect opt
		if( setsockopt( sockfd, SOL_SOCKET, SO_LINGER, ( const char * )sLinger, sizeof( struct linger ) ) ){
			perror( "set connect opt" );
			return -1;
		}
	}

	//set send opt
	if( setsockopt( sockfd, SOL_SOCKET, SO_SNDBUF, ( const char * )&Send_system_buf, sizeof( int ) ) ){
		perror( "set send buf" );
		return -1;
	}

	if( setsockopt( sockfd, SOL_SOCKET, SO_SNDTIMEO, ( char * )&Stimeout, sizeof( struct timeval ) ) ){
		perror( "set send timeout" );
		return -1;
	}

	//set recv opt
	if( setsockopt( sockfd, SOL_SOCKET, SO_RCVBUF, ( const char * )&Recv_system_buf, sizeof( int ) ) ){
		perror( "set recv buf" );
		return -1;
	}

	if( setsockopt( sockfd, SOL_SOCKET, SO_RCVTIMEO, ( char * )&Rtimeout, sizeof( struct timeval ) ) ){
		perror( "set recv timeout" );
		return -1;
	}

	//printf("%d\n",sockfd);
	return sockfd;
}


int ConnectSock( int sockfd, unsigned long ipn, int port )
{
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons( port );
	server_addr.sin_addr.s_addr = ipn;

	if( connect( sockfd, ( struct sockaddr * )&server_addr, sizeof( server_addr ) ) != 0 ){
		perror( "Connect error!" );
		return -1;
	}

	else{
		return 0;
	}
}


int senddata( int sockfd, char *msg, int Max_send_trytimes, int msglen )
{
	if(msg == NULL){
		return -1;
	}
	int size_send = 0, total_send = 0, sendtimes = 0;

	while( 1 ){
		if( ( size_send = send( sockfd, msg + total_send, msglen - total_send, 0 ) ) == -1 ){
			perror( "send data" );

			if( errno == EWOULDBLOCK || errno == EINTR || errno == EAGAIN ){
				if( sendtimes < Max_send_trytimes ){
					sendtimes++;
					continue ;
				}

				return -1;
			}

			else{
				return -1;
			}
		}

		else{
			total_send += size_send;

			if( total_send == strlen( msg ) ){
				break;
			}
		}
	}

	return size_send;
}


int recvdata( int sockfd, char **data, int Max_recv_trytimes, int bufsize )
{
	if(data == NULL){
		return -1;
	}
	int size_recv = 0, total_size = 0,  recvtimes = 0, recvcount = 0;
	char *buf = ( char * ) malloc( bufsize * sizeof( char ) );
	*data = ( char * ) malloc( bufsize * sizeof( char ) );
	//Receive a reply from the server

	while( 1 ){
		memset( buf, 0, bufsize );

		if( ( size_recv = recv( sockfd, buf, bufsize, 0 ) ) == -1 ){
			if( ( total_size > 0 ) && ( total_size < bufsize ) ){
				break;
			}

			perror( "receive data" );

			if( errno == EWOULDBLOCK || errno == EINTR || errno == EAGAIN ){
				if( recvtimes < Max_recv_trytimes ){
					++recvtimes;
					continue;
				}

				recvtimes = 0;
				free( buf );
				return -1;
			}

			else{
				free( buf );
				return -1;
			}
		}

		else if( size_recv == 0 ){
			if( total_size > 0 ){
				//printf("Peer shutdown or receive over ... \n");
				break;
			}
		}

		else{
			memcpy( *data + total_size, buf, size_recv );
			total_size += size_recv;

			if( total_size > 0 && total_size < bufsize ){
				break;
			}

			recvcount++;

			if( ( *data = ( char * )realloc( *data, ( total_size + bufsize + 1 ) * sizeof( char ) ) ) == NULL ){
				printf( "realloc for recv data error...\n" );
				free( buf );
				return -1;
			}
		}
	}

	free( buf );
	*( *data + total_size ) = '\0';
	return total_size;
}
