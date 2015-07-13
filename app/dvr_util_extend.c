#include <asm/types.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/types.h>

#define BUFSIZE 8192

struct route_info{
	u_int dstAddr;
	u_int srcAddr;
	u_int gateWay;
	char ifName[IF_NAMESIZE];
};

int _read_netlink_sock(int sockFd, char *bufPtr, int seqNum, int pId)
{
	struct nlmsghdr *nlHdr;
	int readLen = 0, msgLen = 0;

	do{
	/* Recieve response from the kernel */
		if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0){
			perror("SOCK READ: ");
			return -1;
		}

		nlHdr = (struct nlmsghdr *)bufPtr;

		/* Check if the header is valid */
		if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
		{
			perror("Error in recieved packet");
			return -1;
		}

		/* Check if the its the last message */
		if(nlHdr->nlmsg_type == NLMSG_DONE) {
			break;
		}
		else{
			/* Else move the pointer to buffer appropriately */
			bufPtr += readLen;
			msgLen += readLen;
		}

		/* Check if its a multi part message */
		if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0) {
			/* return if its not */
			break;
		}
	} while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
	return msgLen;
}


/* For printing the routes. */
void _print_route(struct route_info *rtInfo)
{
	char tempBuf[512];

	/* Print Destination address */
	if(rtInfo->dstAddr != 0)
		strcpy(tempBuf, (char *)inet_ntoa(rtInfo->dstAddr));
	else
		sprintf(tempBuf,"*.*.*.*\t");
	fprintf(stdout,"%s\t", tempBuf);

	/* Print Gateway address */
	if(rtInfo->gateWay != 0)
		strcpy(tempBuf, (char *)inet_ntoa(rtInfo->gateWay));
	else
		sprintf(tempBuf,"*.*.*.*\t");
	fprintf(stdout,"%s\t", tempBuf);

	/* Print Interface Name*/
	fprintf(stdout,"%s\t", rtInfo->ifName);

	/* Print Source address */
	if(rtInfo->srcAddr != 0)
		strcpy(tempBuf, (char *)inet_ntoa(rtInfo->srcAddr));
	else
		sprintf(tempBuf,"*.*.*.*\t");
	fprintf(stdout,"%s\n", tempBuf);
}


/* For parsing the route info returned */
void _parse_routes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway)
{
	struct rtmsg *rtMsg;
	struct rtattr *rtAttr;
	int rtLen;
	char *tempBuf = NULL;

	tempBuf = (char *)malloc(100);
	rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);

	/* If the route is not for AF_INET or does not belong to main routing table
	then return. */
	if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
		return;

	/* get the rtattr field */
	rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
	rtLen = RTM_PAYLOAD(nlHdr);
	for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen)){
		switch(rtAttr->rta_type) {
			case RTA_OIF:
				if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
				break;
			case RTA_GATEWAY:
				rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);
				break;
			case RTA_PREFSRC:
				rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);
				break;
			case RTA_DST:
				rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);
				break;
		}
	}	
	//printf("%s\n", (char *)inet_ntoa(rtInfo->dstAddr));

	// ADDED BY BOB - ALSO COMMENTED _print_route

	if (strstr((char *)inet_ntoa(rtInfo->dstAddr), "0.0.0.0"))
		sprintf(gateway, (char *)inet_ntoa(rtInfo->gateWay));
	//_print_route(rtInfo);

	free(tempBuf);
	return;
}


int get_gateway(char *gateway)
{
	struct nlmsghdr *nlMsg;
	struct rtmsg *rtMsg;
	struct route_info *rtInfo;
	char msgBuf[BUFSIZE];
	int sock, len, msgSeq = 0;
	char buff[1024];

	/* Create Socket */
	if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
		perror("Socket Creation: ");

	/* Initialize the buffer */
	memset(msgBuf, 0, BUFSIZE);

	/* point the header and the msg structure pointers into the buffer */
	nlMsg = (struct nlmsghdr *)msgBuf;
	rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);

	/* Fill in the nlmsg header*/
	nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
	nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .
	nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
	nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
	nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.

	/* Send the request */
	if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0){
		printf("Write To Socket Failed...\n");
		return -1;
	}

	/* Read the response */
	if((len = _read_netlink_sock(sock, msgBuf, msgSeq, getpid())) < 0) {
		printf("Read From Socket Failed...\n");
		return -1;
	}
	/* Parse and print the response */
	rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
	// ADDED BY BOB

	/* THIS IS THE NETTSTAT -RL code I commented out the printing here and in parse routes */
	//fprintf(stdout, "Destination\tGateway\tInterface\tSource\n");

	for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len)){
		memset(rtInfo, 0, sizeof(struct route_info));
		_parse_routes(nlMsg, rtInfo,gateway);
	}
	free(rtInfo);
	close(sock);

	return 0;
}

