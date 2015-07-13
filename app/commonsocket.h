

int  InitSockEnv(void);

int  CreateUDSocket(const char * SocketName, int InOrOut);

int  SendSocket(int Sock, char * Buffer, int Size);

int  RecvSocket(int Sock, char * Buffer, int Size, int TimeOut);
