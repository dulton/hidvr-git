#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <stddef.h>

//Must Invoke Be Other Function Calling;
int  InitSockEnv(void)
{
    //Ignore Default System Signal that Forcing Program Exit;
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, 0);

    return !0;
}

//Create UNIX Domain Socket By Named File;
int  CreateUDSocket(const char * SocketName, int InOrOut)
{
    int  tmpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (tmpSock < 0) {
		perror("create unix sock failed");
		return -1;
	}

    if(!InOrOut) {
		int  BlockMode = 0;
        struct sockaddr_in SockAddr;

        memset(&SockAddr, 0, sizeof(SockAddr));
        SockAddr.sin_family     = AF_INET;
        SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        SockAddr.sin_port        = htons(55555);

        if (bind(tmpSock,
                (struct sockaddr *)&SockAddr,
                sizeof(SockAddr))  < 0) {
            perror("UDSocket Bind Error");
            close(tmpSock);
            return -1;
        }
#if 0
		BlockMode  = fcntl(tmpSock, F_GETFL);
		BlockMode |= O_NONBLOCK;
		fcntl(tmpSock, F_SETFL, BlockMode);
#endif
    }

    return tmpSock;
}

int  SendSocket(int Sock, char * Buffer, int Size)
{
    struct sockaddr_in SockAddr;

    memset(&SockAddr, 0, sizeof(SockAddr));
    SockAddr.sin_family     = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &SockAddr.sin_addr);
    SockAddr.sin_port        = htons(55555);

    return sendto(Sock, Buffer, Size, 0,
        (struct sockaddr *)&SockAddr, sizeof(SockAddr));
}

int  RecvSocket(int Sock, char * Buffer, int Size, int TimeOut)
{
    struct timeval SetTO;
	fd_set SetRead;

    int nRecvSize;

    socklen_t          RcvAddrLen;
    struct sockaddr_in RcvAddr;

    memset(&RcvAddr, 0, sizeof(RcvAddr));
	RcvAddrLen = sizeof(RcvAddr);
#if 1
    SetTO.tv_sec  = TimeOut;
    SetTO.tv_usec = 0;

    FD_ZERO(&SetRead);
	FD_SET(Sock, &SetRead);

    nRecvSize = select(Sock+1, &SetRead, NULL, NULL, &SetTO);
    if(nRecvSize == 0) {// || errno == EINTR) { //Anything Failed
		perror("select");
		errno = 0;
		fprintf(stderr, "\nTimeOut1 : %d\n", nRecvSize);
        return 0;
	}

	if (!FD_ISSET(Sock, &SetRead)) {
		fprintf(stderr, "\nTimeOut2 : %d\n", nRecvSize);
        return -1;
    }
#endif

    nRecvSize = recvfrom(Sock, Buffer, Size, 0, (struct sockaddr *)&RcvAddr, &RcvAddrLen);
#if 0
	if(nRecvSize < 0) {
		perror("recvfrom");
	}
#endif
    return nRecvSize;
}
