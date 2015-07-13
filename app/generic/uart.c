


#include "uart.h"

#define DEBUG_UART
#ifdef DEBUG_UART
#define UART_TRACE(fmt...) \
	do{printf("\033[1;31mUART->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#else
#define UART_TRACE(fmt...)
#endif


typedef struct UART_ATTR
{
	int fid;
	char device[32];
}UART_ATTR_t;

static int uart_set_baud(struct UART* const thiz, int baud)
{
	UART_ATTR_t* const uart_attr = (UART_ATTR_t*)(&thiz[1]);
	
	struct termios options = {0};
	if(0 != tcgetattr(uart_attr->fid, &options)){
		perror("tcgetattr");
		return -1;
	}
	if(baud <= 300){
		baud = B300;
	}else if(baud <= 1200){
		baud = B1200;
	}else if(baud <= 2400){
		baud = B2400;
	}else if(baud <= 4800){
		baud = B4800;
	}else if(baud <= 9600){
		baud = B9600;
	}else if(baud <= 19200){
		baud = B19200;
	}else if(baud <= 38400){
		baud = B38400;
	}else if(baud <= 57600){
		baud = B57600;
	}else if(baud <= 115200){
		baud = B115200;
	}else{
		baud = B4800;
	}
	//UART_TRACE("Set baudrate: %d", baud);
	cfsetispeed(&options, baud);
	cfsetospeed(&options, baud);
	tcflush(uart_attr->fid, TCIOFLUSH);
	if(0 !=  tcsetattr(uart_attr->fid, TCSANOW, &options)){
		perror("tcsetattr");
		return -1;
	}
	tcflush(uart_attr->fid, TCIOFLUSH);
	return thiz->get_baud(thiz);
}

static int uart_get_baud(struct UART* const thiz)
{
	UART_ATTR_t* const uart_attr = (UART_ATTR_t*)(&thiz[1]);
	struct termios options = {0};
	if(0 != tcgetattr(uart_attr->fid, &options)){
		perror("tcgetattr");
		return -1;
	}
	switch(cfgetispeed(&options))
	{
	case B300:
		return 300;
	case B1200:
		return 1200;
	case B2400:
		return 2400;
	case B4800:
		return 4800;
	case B9600:
		return 9600;
	case B19200:
		return 19200;
	case B38400:
		return 38400;
	case B57600:
		return 57600;
	case B115200:
		return 115200;
	}
	return -1;
}

static int uart_set_databit(struct UART* const thiz, int databit)
{
	UART_ATTR_t* const uart_attr = (UART_ATTR_t*)(&thiz[1]);
	struct termios options = {0};
	if(databit != 5 && databit != 6 && databit != 7 && databit != 8){
		errno = EINVAL;
		return -1;
	}

	if(0 != tcgetattr(uart_attr->fid, &options)){
		perror("tcgetattr");
		return -1;
	}

	options.c_cflag |= CLOCAL | CREAD;
	options.c_cflag &= ~CSIZE;
	switch(databit)
	{
	case 5:
		
		options.c_cflag |= CS5;
		break;
	case 6:
		options.c_cflag |= CS6;
		break;
	case 7:
		options.c_cflag |= CS7;
		break;

	case 8:
	default:
		databit = 8;
		options.c_cflag |= CS8;
		break; 
	}
	
	tcflush(uart_attr->fid, TCIOFLUSH);
	if(0 !=  tcsetattr(uart_attr->fid, TCSANOW, &options)){
		perror("tcsetattr");
		return -1;
	}
	return 0;
}

static int uart_get_databit(struct UART* const thiz)
{
	UART_ATTR_t* const uart_attr = (UART_ATTR_t*)(&thiz[1]);
	struct termios options = {0};
	if(0 != tcgetattr(uart_attr->fid, &options)){
		perror("tcgetattr");
		return -1;
	}
	switch(options.c_cflag & CSIZE)
	{
	case CS5:
		return 5;
	case CS6:
		return 6;
	case CS7:
		return 7;
	case CS8:
		return 8;
	default:
		break;
	}
	return 0;
}

static int uart_set_stopbit(struct UART* const thiz, int stopbit)
{
	UART_ATTR_t* const uart_attr = (UART_ATTR_t*)(&thiz[1]);
	struct termios options = {0};
	if(stopbit != 1 && stopbit != 2){
		errno = EINVAL;
		return -1;
	}
	// get uart attr
	if(0 != tcgetattr(uart_attr->fid, &options)){
		perror("tcgetattr");
		return -1;
	}
	switch(stopbit)
	{
	case 2:
		
		options.c_cflag |= CSTOPB;
		break;

	case 1:
	default:
		 stopbit = 1;
		options.c_cflag &= ~CSTOPB;
		break;
	}
	
	tcflush(uart_attr->fid, TCIOFLUSH);
	if(0 !=  tcsetattr(uart_attr->fid, TCSANOW, &options)){
		perror("tcsetattr");
		return -1;
	}
	return 0;
}

static int uart_get_stopbit(struct UART* const thiz)
{
	UART_ATTR_t* const uart_attr = (UART_ATTR_t*)(&thiz[1]);
	struct termios options = {0};
	// get uart attr
	if(0 != tcgetattr(uart_attr->fid, &options)){
		perror("tcgetattr");
		return -1;
	}
	if(options.c_cflag & CSTOPB)
	{
		return 2;
	}
	else
	{
		return 1;
	}
	return 0;
}

static int uart_set_parity(struct UART* const thiz, int parity)
{
	UART_ATTR_t* const uart_attr = (UART_ATTR_t*)(&thiz[1]);
	struct termios options = {0};
	// get uart attr
	if(0 != tcgetattr(uart_attr->fid, &options)){
		perror("tcgetattr");
		return -1;
	}
	switch(parity)
	{
	// odd
	case UART_PARITY_ODD:
		options.c_cflag |= PARENB;
		options.c_cflag |= PARODD;
		options.c_iflag |= INPCK | ISTRIP;
		break;
	// even
	case UART_PARITY_EVEN:
		options.c_cflag |= PARENB;
		options.c_cflag &= ~PARODD;
		options.c_iflag |= INPCK | ISTRIP;
		break;
	// none
	case UART_PARITY_NONE:
	default:
		options.c_cflag &= ~PARENB;
		break;
	}

	tcflush(uart_attr->fid, TCIOFLUSH);
	if(0 !=  tcsetattr(uart_attr->fid, TCSANOW, &options)){
		perror("tcsetattr");
		return -1;
	}
	//tcflush(uart_attr->fid, TCIOFLUSH);
	return 0;
}

static int uart_get_parity(struct UART* const thiz)
{
	UART_ATTR_t* const uart_attr = (UART_ATTR_t*)(&thiz[1]);
	struct termios options = {0};
	if(0 != tcgetattr(uart_attr->fid, &options))
	{
		perror("tcgetattr");
		return 'N';
	}
	if(options.c_cflag & PARENB){
		if(options.c_cflag & PARODD){
			return 'O';
		}else{
			return 'E';
		}
	}else{
		return 'N';
	}
	return 'N';
}


static ssize_t uart_write(struct UART* const thiz, const void* bytes, ssize_t len)
{
	UART_ATTR_t* const uart_attr = (UART_ATTR_t*)(&thiz[1]);
	return write(uart_attr->fid, bytes, len);
}

static ssize_t uart_read(struct UART* const thiz, void* ret_bytes, ssize_t len)
{
	UART_ATTR_t* const uart_attr = (UART_ATTR_t*)(&thiz[1]);
	return read(uart_attr->fid, ret_bytes, len);
}

UART_t* UART_open(const char* device)
{
	UART_t* uart = calloc(sizeof(UART_t) + sizeof(UART_ATTR_t), 1);
	UART_ATTR_t* const uart_attr = (UART_ATTR_t*)(&uart[1]);

	uart_attr->fid = open(device, O_RDWR);
	if(uart_attr->fid < 0){
		free(uart);
		return NULL;
	}

	strncpy(uart_attr->device, device, sizeof(uart_attr->device));

	uart->set_baud = uart_set_baud;
	uart->get_baud = uart_get_baud;
	uart->set_databit = uart_set_databit;
	uart->get_databit = uart_get_databit;
	uart->set_stopbit = uart_set_stopbit;
	uart->get_stopbit = uart_get_stopbit;
	uart->set_parity = uart_set_parity;
	uart->get_parity = uart_get_parity;
	uart->write = uart_write;
	uart->read = uart_read;
	return uart;

}

void UART_close(UART_t* uart)
{
	if(uart){
		UART_ATTR_t* uart_attr = (UART_ATTR_t*)(&uart[1]);
		close(uart_attr->fid);
		//
		free(uart);
	}
}

//#define TEST_UART
#ifdef TEST_UART
int main(int argc, char** argv)
{
	UART_t* uart = UART_open("/dev/ttyAMA0");
	if(uart){
		printf("UART start test, press any keys to continue.");
		getchar();
		printf("Set baudrate 115200\r\n");
		printf("Set databit 8\r\n");
		printf("Set stopbit 1\r\n");
		printf("Set parity NONE\r\n");
		uart->set_baud(uart, 115200);
		uart->set_databit(uart, 8);
		uart->set_stopbit(uart, 1);
		uart->set_parity(uart, UART_PARITY_NONE);
		getchar();
		uart->write(uart, (unsigned char*)"Hello\r\n", strlen("Hello\r\n"));
		printf("Press any keys to send \"Merry Christmas\"");
		getchar();
		uart->write(uart, (unsigned char*)"Merry Christmas\r\n.", strlen("Merry Christmas\r\n"));
		printf("Press any keys to send \"Goodbye\".");
		getchar();
		uart->write(uart, (unsigned char*)"Goodbye\r\n", strlen("Goodbye\r\n"));
		printf("Press any keys to finish testing");
		getchar();
		UART_close(uart);
		uart = NULL;
	}
	return 0;
}
#endif


