/*===========================
-- SOCKET������Դ�ļ�
-- л����<kevin_linux@163.com>
-- v1.00
-- 2013��5��8��
-- socket������
-- v1.10
-- 2018��11��8��
-- ����RAWԭʼ�׽��ֲ��ֽӿ�
-- �����鲥�׽��ֲ��ֽӿ�
-- ���ӵ�ַaton ntoa�ӿ�
=============================*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h> //TCP_NODELAY
#include <arpa/inet.h>
#include <linux/if_ether.h> //ETH_P_ALL
#include <net/if.h>
#include "SockBase.h"
#include <sys/errno.h>

//����һ��UDP��ʽ��socket
//�������ش����õ�socket fd�������ظ���
int  SockBase_UdpCreate(void)
{
	int sockfd;

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd < 0)
	{
		//printf("socket create error\n");
		return  -1;
	}

	return sockfd;
}

//����һ��TCP��ʽ��socket
//�������ش����õ�socket fd�������ظ���
int  SockBase_TcpCreate(void)
{
	int sockfd;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
		//printf("socket create error\n");
		return  -1;
	}

	return sockfd;
}

//����һ��ԭʼ�׽���socket
//�������ش����õ�socket fd�������ظ���
int  SockBase_RawCreate(void)
{
	int sockfd;

	/*
	ETH_P_IP   0x800  ֻ���շ�������mac��ip���͵�����֡
	ETH_P_ARP  0x806  ֻ���ܷ�������mac��arp���͵�����֡
	ETH_P_RARP 0x8035 ֻ���ܷ�������mac��rarp���͵�����֡
	ETH_P_ALL  0x3    ���շ�������mac����������ip arp rarp������֡, ���մӱ����������������͵�����֡
	
	//sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP|ETH_P_ARP));
	*/
	
	sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sockfd < 0)
	{
		//printf("socket create error\n");
		return  -1;
	}

	return sockfd;
}

//ɾ��һ��socket
int  SockBase_Delete(int sockfd)
{
	if(sockfd > 0)
	{
		close(sockfd);
	}

	return  0;
}

//����socketΪ��������ʽ
int  SockBase_SetNonBlock(int sockfd)
{
	int flag;

	flag = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);

	return 0;
}

//����socketΪ������ʽ
int  SockBase_SetBlock(int sockfd)
{
	int flag;

	flag = fcntl(sockfd, F_GETFL, 0) & (~O_NONBLOCK);
	fcntl(sockfd, F_SETFL, flag);

	return 0;
}

//ת��IP�ַ����ɱ����ֽ�������Ƹ�ʽ
//��������0�������ظ���
int  SockBase_ConvertIP2Addr(char *pIpString,unsigned int *pHexAddr)
{
	int  ret;
	unsigned int uiNetHexAddr;

	//IP�ַ���ת�������ֽ������еĶ����Ƶ�ַ
	ret = inet_pton(AF_INET, pIpString, &uiNetHexAddr);
	if(ret <= 0)
	{
		// if(ret == 0)
		// {
			// printf("invalid network address\n");
		// }
		// else
		// {
			// printf("inet_pton error\n");
		// }
		return  -1;
	}
	
	//�����ֽ���ת�ɱ����ֽ������
	*pHexAddr = htonl(uiNetHexAddr);
	
	return  0;
}

//ת�������ֽ��������IP��ַ��IP�ַ���
//��������0�������ظ���
int  SockBase_ConvertAddr2IP(unsigned int uiHexAddr,char *pIpString)
{
	//int  ret;
	unsigned int uiNetHexAddr;
	
	//�����ֽ�����ת�������ֽ���
	uiNetHexAddr = htonl(uiHexAddr);
	
	if(inet_ntop(AF_INET, (const void *)&uiNetHexAddr, pIpString, INET_ADDRSTRLEN) == NULL)
	{
		// printf("invalid network address\n");
		// printf("inet_ntop error\n");
		return  -1;
	}

	return  0;
}

//IP�ַ���ת�ɱ����ֽ��������IP��ַ
//eg: "127.0.0.1" ת������ 0x7f000001
unsigned int  SockBase_aton(char *pIpString)
{
	struct in_addr tAddr;

	//inet_addr()ת�����Ķ�����IP�������ֽ������е�
	tAddr.s_addr = inet_addr(pIpString);

	//ת�ɱ����ֽ������
	return  htonl(tAddr.s_addr);
}

//�����ֽ��������IP��ַת����IP�ַ���
//eg: 0x7f000001 ת������ "127.0.0.1"
char  *SockBase_ntoa(unsigned int uiHexAddr)
{
	struct in_addr tAddr;

	//�����ֽ�����ת�������ֽ���
	tAddr.s_addr = htonl(uiHexAddr);

	//��ȥת��IP�ַ���
	return inet_ntoa(tAddr);
}

//IP���ӿ�
static int __SockBase_ip_check(char *ip)
{
	unsigned int uiAddr;
	
	return  SockBase_ConvertIP2Addr(ip, &uiAddr);
}

//PORT���ӿ�
static int __SockBase_port_check(int iPort)
{
	//if(iPort <= 0 || iPort > 65535)
	if(iPort < 0 || iPort > 65535)
	{
		//printf("invalid network iPort\n");
		return  -1;
	}

	return  0;
}

//����sockaddr�ṹ�壬�Ա��ط�ʽ
//��������0�������ظ���
int  SockBase_ConfigLocalAddr(struct sockaddr_in *ptSockAddr,int iPort)
{
	assert(ptSockAddr);

	if(__SockBase_port_check(iPort) < 0)
	{
		return  -1;
	}

	memset(ptSockAddr,0,sizeof(struct sockaddr_in));

	ptSockAddr->sin_family = AF_INET;
	ptSockAddr->sin_port = htons(iPort);	/* ���ö˿� */
	ptSockAddr->sin_addr.s_addr = htonl(INADDR_ANY);	/* IP��ַ */

	return  0;
}

//����sockaddr�ṹ�壬��ָ��IP��ʽ
//��������0�������ظ���
int  SockBase_ConfigAddr(struct sockaddr_in *ptSockAddr,char *pIpString,int iPort)
{
	assert(ptSockAddr);
	assert(pIpString);

	if(__SockBase_ip_check(pIpString) < 0)
	{
		return  -1;
	}
	if(__SockBase_port_check(iPort) < 0)
	{
		return  -1;
	}

	memset(ptSockAddr,0,sizeof(struct sockaddr_in));

	ptSockAddr->sin_family = AF_INET;
	ptSockAddr->sin_port = htons(iPort);	/* ���ö˿� */
	ptSockAddr->sin_addr.s_addr = inet_addr(pIpString);	/* IP��ַ */

	return  0;
}

//��ȡ����IP��ַ�����ַ�����ʽ�洢
//��������0�������ظ���
int  SockBase_GetLocalIp(char *pIpBuf)
{
	struct hostent *ptHost;
	char  *pIpStr;
	char namebuf[128];

	if(pIpBuf == NULL)
	{
		return  -1;
	}

	/* ��������� */
	if(gethostname(namebuf,sizeof(namebuf)) != 0)
	{
		return  -1;
	}

	/* ��������ṹ */
	ptHost = gethostbyname(namebuf); 
	if(ptHost == NULL)
	{
		return  -1;
	}

	/* ת��IP��ַΪ�ַ�����ʽ */
	//pIpStr = inet_ntoa(*((in_addr *)ptHost->h_addr));
	//pIpStr = inet_ntoa(*((in_addr *)ptHost->h_addr_list[0]));
	pIpStr = inet_ntoa(*((struct in_addr *)ptHost->h_addr_list[0]));
	if(pIpStr == NULL)
	{
		return  -1;
	}

	/* ��������������� */
	strcpy(pIpBuf,pIpStr);

	return  0;
}

//��ȡ������Ӧ��IP��ַ�����ַ�����ʽ�洢
//��������0�������ظ���
int  SockBase_GetDomainIp(char *pIpBuf, char *pcDomainStr)
{
	struct hostent *ptHost;
	char  *pIpStr;

	if(!pIpBuf || !pcDomainStr)
	{
		return  -1;
	}

    pIpBuf[0] = '\0';
	/* ��������ṹ */
	ptHost = gethostbyname(pcDomainStr); 
	if(ptHost == NULL)
	{
		return  -1;
	}

	/* ת��IP��ַΪ�ַ�����ʽ */
	//pIpStr = inet_ntoa(*((in_addr *)ptHost->h_addr));
	//pIpStr = inet_ntoa(*((in_addr *)ptHost->h_addr_list[0]));
	pIpStr = inet_ntoa(*((struct in_addr *)ptHost->h_addr_list[0]));
	if(pIpStr == NULL)
	{
		return  -1;
	}

	/* ��������������� */
	strcpy(pIpBuf, pIpStr);

	return  0;
}


//����һ���鲥��,ָ������ip����һ���鲥����
int SockBase_SetIpAddMembership(int sockfd, char *pIp, char *pMulticastIp)
{
	struct ip_mreq tMreq;
	int size = sizeof(tMreq);
	
	memset(&tMreq, 0, sizeof(struct ip_mreq));
	tMreq.imr_multiaddr.s_addr = inet_addr(pMulticastIp);
    tMreq.imr_interface.s_addr = inet_addr(pIp);//htonl(INADDR_ANY);
	if(0 > setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&tMreq, size))
	{
		return -1;
	}

	return 0;
}

//����socket���ջ�������С
//��������0�������ظ���
int  SockBase_SetRecvBufSize(int sockfd,int iSetSize) 
{
	//socklen_t tmp = iSetSize;
	int tmp = iSetSize;

	if (setsockopt(sockfd, SOL_SOCKET,SO_RCVBUF,(char*)&tmp,sizeof(int)) < 0) {
		//printf("SockBase_SetRecvBufSize() error\n");
		return -1;
	}

	return 0;
}

//����socket���ͻ�������С
//��������0�������ظ���
int  SockBase_SetSendBufSize(int sockfd,int iSetSize) 
{
	//socklen_t tmp = iSetSize;
	int tmp = iSetSize;
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF,(char*)&tmp, sizeof(int)) < 0) {
		//printf("SockBase_SetSendBufSize() error\n");
		return -1;
	}

	return 0;
}

//��ȡsocket���ջ�������С
//��������0�������ظ���
int  SockBase_GetRecvBufSize(int sockfd,int *pGetSize) 
{
	socklen_t tmpsize;
	socklen_t len = sizeof(socklen_t);
	
	if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF,(char*)&tmpsize,&len) < 0) {
		//printf("SockBase_SetSendBufSize() error\n");
		return -1;
	}

	*pGetSize = tmpsize;

	return 0;
}

//��ȡsocket���ͻ�������С
//��������0�������ظ���
int  SockBase_GetSendBufSize(int sockfd,int *pGetSize) 
{
	socklen_t tmpsize;
	socklen_t len = sizeof(socklen_t);
	
	if (getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF,(char*)&tmpsize,&len) < 0) {
		//printf("SockBase_SetSendBufSize() error\n");
		return -1;
	}

	*pGetSize = tmpsize;

	return 0;
}

//����socket���ճ�ʱʱ������λ����
//��������0�������ظ���
int  SockBase_SetRecvTimeout(int sockfd,int iMSec) 
{
	struct timeval t;
	
	t.tv_sec = iMSec/1000;
	t.tv_usec = (iMSec%1000)*1000;

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(char*)&t, sizeof(struct timeval)) < 0) {
		//printf("SockBase_SetRecvTimeout() error\n");
		return -1;
	}

	return 0;
}

//����socket���ͳ�ʱʱ������λ����
//��������0�������ظ���
int  SockBase_SetSendTimeout(int sockfd,int iMSec) 
{
	struct timeval t;
	
	t.tv_sec = iMSec/1000;
	t.tv_usec = (iMSec%1000)*1000;

	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO,(char*)&t, sizeof(struct timeval)) < 0) {
		//printf("SockBase_SetSendTimeout() error\n");
		return -1;
	}

	return 0;
}

//����TCP socket����Nagle�㷨�Ŀ���״̬
//��������0�������ظ���
int  SockBase_SetTcpNoDelay(int sockfd,int iEnable)
{
	int tmpEn = !!iEnable;

	if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,(void*)&tmpEn, sizeof(int)) < 0) {
		//printf("() error\n");
		return -1;
	}

	return 0;
}

//����socket�󶨶�Ӧ������
//��������0�������ظ���
int  SockBase_SetBind2Dev(int sockfd,char *pIfName)
{
	struct ifreq stIf;

	strcpy(stIf.ifr_name, pIfName);
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&stIf, sizeof(stIf))  < 0) {
		//perror("SO_BINDTODEVICE failed");
		//printf("() error\n");
		return -1;
	}

	return 0;
}

//����socket��ַ���õĿ���״̬
//��������0�������ظ���
int  SockBase_SetReuseAddr(int sockfd,int iReuseEn)
{
	int tmpEn = !!iReuseEn;
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&tmpEn, sizeof(int)) < 0) {
		//printf("() error\n");
		return -1;
	}

	return 0;
}

//����socket�鲥�����Ƿ�ػ����Լ�
//��������0�������ظ���
int  SockBase_SetGroupLoop(int sockfd,int iLoopEn)
{
	int tmpEn = !!iLoopEn;
	
	if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, (void*)&tmpEn, sizeof(int)) < 0) {
		//printf("() error\n");
		return -1;
	}

	return 0;
}

//����socket�����鲥��
//��������0�������ظ���
int  SockBase_SetJoinGroup(int sockfd, char *pMulticastIp)
{
	int   iRtn = 0;
	struct ip_mreq mreq; 
	unsigned int uiMulticastIp;

	if(SockBase_ConvertIP2Addr(pMulticastIp, &uiMulticastIp) < 0)
	{
		//printf("IP check error:%s\n",pMulticastIp);
		return -1;
	}

	mreq.imr_multiaddr.s_addr = uiMulticastIp; //�鲥IP
	mreq.imr_interface.s_addr = htonl(INADDR_ANY); /*����ӿ�ΪĬ��*/

	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) < 0)
	{
		//printf("IP_ADD_MEMBERSHIP\n");
		return -1;
	}

	return iRtn;
}

//����socket�Ƴ��鲥��
//��������0�������ظ���
int SockBase_SetRemoveGroup(int sockfd, char *pMulticastIp)
{
	struct ip_mreq mreq; 
	unsigned int uiMulticastIp;

	if(SockBase_ConvertIP2Addr(pMulticastIp, &uiMulticastIp) < 0)
	{
		//printf("IP check error:%s\n",pMulticastIp);
		return -1;
	}

	mreq.imr_multiaddr.s_addr = uiMulticastIp; //�鲥IP
	mreq.imr_interface.s_addr = htonl(INADDR_ANY); /*����ӿ�ΪĬ��*/

	if (setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) < 0)
	{
		printf("setsockopt():IP_DROP_MEMBERSHIP");
		return -1;
	}

	return 0;
}


//socket��
//��������0�������ظ���
int  SockBase_Bind(int sockfd,struct sockaddr_in *ptSockAddr)
{
	SockBase_SetReuseAddr(sockfd, 1);

	if (bind(sockfd,(struct sockaddr *)ptSockAddr,sizeof(struct sockaddr)) < 0)
	{
		//printf("failed to bind\n");
		return  -1;
	}

	return  0;
}

//TCP socket����
//��������0�������ظ���
int  SockBase_TcpListen(int sockfd,int iMaxConnect)
{
	if (listen(sockfd, iMaxConnect) < 0)
	{
		//printf("listen error\n");
		return  -1;
	}

	return  0;
}

//TCP socket����һ������
//��������0�������ظ���
int  SockBase_TcpAccept(int sockfd,int *pConnectfd,struct sockaddr_in *ptClientAddr)
{
	int  ret;
	int  addrsize;
	struct sockaddr_in tClientAddr;

	//BUG�޸���20150607
	//��ַ��С��ֵ��ͬʱ���������װ���ӵĿͻ��˵�ַ�Ľṹ�壬��������accept��������
	addrsize = sizeof(struct sockaddr_in);
	memset(&tClientAddr,0,sizeof(struct sockaddr_in));
	
	ret = accept(sockfd, (struct sockaddr *)&tClientAddr,(socklen_t *)&addrsize);
	if (ret < 0)
	{
		//printf("accept error\n");
		return -1;;
	}

	*pConnectfd = ret;
	memcpy(ptClientAddr, &tClientAddr,sizeof(struct sockaddr_in));

	return  0;
}

//TCP socket����ָ��������
//��������0�������ظ���
int  SockBase_TcpConnect(int sockfd,struct sockaddr_in *ptServerAddr)
{
	int  ret;

	ret = connect(sockfd, (struct sockaddr *)ptServerAddr, sizeof(struct sockaddr));
	if (ret < 0)
	{
		//printf("connect error\n");
		return -1;;
	}

	return  0;
}

//TCP socket����ָ������������������ʽ����ʱ�Զ�����
//�����ظ�������������0
static int  __SockBase_tcpconnect_ex(int sockfd,struct sockaddr_in *ptServerAddr,int iTimeoutMs)
{
	int  ret;
	int  error;
	//int  len;
	int iFlag = 0;
	socklen_t len;
	fd_set   fds_wd;
	struct timeval  tv;
	
	//������sockfdΪ������ģʽ
	iFlag = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, iFlag|O_NONBLOCK);

	ret = connect(sockfd, (struct sockaddr *)ptServerAddr, sizeof(struct sockaddr));
	if (ret == 0)
	{
		//printf("tcp connect success\n");
	}
	else
	{
		//����ʧ��
		if(errno != EINPROGRESS)
		{
			goto errout;
		}
		else
		{
			FD_ZERO(&fds_wd);
			FD_SET((unsigned)sockfd,&fds_wd);

			tv.tv_sec = iTimeoutMs /1000;
			tv.tv_usec = (iTimeoutMs %1000) *1000;

			ret = select(sockfd+1,(fd_set *)0,&fds_wd,(fd_set *)0,&tv);
			if(ret < 0)	/* ������-1 */
			{
			//	printf("tcp socket select error\n");
				goto errout;
			}
			else if(ret == 0)	/* ��ʱ����0 */
			{
			//	printf("tcp socket select timeout\n");
				goto errout;
			}
			else	/* �ȴ���socket�׽ӿ�״̬�иı� */
			{
				if(FD_ISSET(sockfd,&fds_wd))
				{
					len=sizeof(error);
					ret = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *)&error, &len);  
					if(ret < 0){
					//	printf("getsockopt error:%s\n",strerror(error));
						goto errout;
					}
					if(error != 0){ 
					//	printf("getsockopt get error:%s\n",strerror(error)); 
						goto errout;
					}

				//	printf("getsockopt success\n");
				}
			}
		}
	}

	//��ԭԭģʽ
	fcntl(sockfd, F_SETFL, iFlag);
	
	return 0;

errout:
	//��ԭԭģʽ
	fcntl(sockfd, F_SETFL, iFlag);
	
	return -1;
}

//TCP socket����ָ������������������ʽ����ʱ�Զ�����
//�����ظ�������������0
int  SockBase_TcpConnectEx(int sockfd,struct sockaddr_in *ptServerAddr,int timeoutMs)
{
	int  ret;
	
	//��������ʽ���ӷ���������������������ڣ�� timeoutMs�����᷵��
	ret = __SockBase_tcpconnect_ex(sockfd,ptServerAddr,timeoutMs);
	if (ret < 0)
	{
		return -1;;
	}

	return  0;
}

//UDP socket����ָ����С������
//�����ظ�������������ʵ�ʷ��͵Ĵ�С
int  SockBase_UdpSend(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iBufSize)
{
	int  ret;

	assert(pBuffer);

	ret = sendto(sockfd,pBuffer,iBufSize,0, (struct sockaddr *)ptSockAddr,sizeof(struct sockaddr));
	//if(ret != iBufSize)
	if(ret <= 0)	
	{
		return  -1;
	}

	return  ret;
}

//TCP socket����ָ����С������
//�����ظ�������������ʵ�ʷ��͵Ĵ�С
int  SockBase_TcpSend(int sockfd,char *pBuffer,int iBufSize)
{
	int  ret;

	assert(pBuffer);

	//ret = send(sockfd,pBuffer,iBufSize,0);
	ret = send(sockfd,pBuffer,iBufSize,MSG_NOSIGNAL);
	//ret = send(sockfd,pBuffer,iBufSize,MSG_NOSIGNAL | MSG_DONTWAIT);
	//if(ret != iBufSize)
	if(ret <= 0)
	{
		return  -1;
	}

	return  ret;
}

//TCP��ʽsocket����ѯ�Ƿ��д�ٷ��ͣ�����������
//�����ѷ��͵Ĵ�С����ʱ����0�������ظ���
static int  __SockBase_tcp_select_send(int sockfd,char *pBuffer,int iBufSize,int iTimeoutMs)
{
	int                 ret = 0;
	int                 send_size;
	fd_set              fds_rd;
	struct timeval      tv;

	tv.tv_sec = iTimeoutMs /1000;//Anchel comment
	//tv.tv_sec = 1;//Anchel add
	tv.tv_usec = (iTimeoutMs %1000) *1000;
	//tv.tv_usec = (iTimeoutMs %1000) *3000;//Anchel add
	FD_ZERO(&fds_rd);
	FD_SET(sockfd,&fds_rd);

	//����Ƿ��д��ʵ���Ƿ��ͻ���������û��λ��
	ret = select(sockfd+1,(fd_set *)0,&fds_rd,(fd_set *)0,&tv);
	if(ret < 0)
	{
		return  -1;
	}
	else if(ret == 0)
	{
		return  0;
	}
	else
	{
		if(FD_ISSET(sockfd,&fds_rd))
		{
			//send_size = send(sockfd,pBuffer,iBufSize,0);
			send_size = send(sockfd,pBuffer,iBufSize,MSG_NOSIGNAL | MSG_DONTWAIT);
			if(send_size <= 0)
			{
				ret = -2;
			}
			else
			{
				ret = send_size;
			}
		}
	}
	
	return  ret;
}

//TCP socket����ָ����С������
//�����ظ�������ʱ����0�����򷵻�ʵ�ʷ��͵ĳ���
int  SockBase_TcpSendEx(int sockfd,char *pBuffer,int iBufSize,int iTimeoutMs)
{
	int  ret;

	assert(pBuffer);

	ret = __SockBase_tcp_select_send(sockfd,pBuffer,iBufSize,iTimeoutMs);

	return  ret;
}

//UDP socket��ȡ���ݣ�block������ȡ
//�����ظ�������������ʵ�ʽ��յĳ���
int  SockBase_UdpRead(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iMaxBufSize)
{
	int   recv_size;
	socklen_t   addrsize;

	assert(ptSockAddr);
	assert(pBuffer);

	addrsize = sizeof(struct sockaddr);
	
	recv_size = recvfrom(sockfd,pBuffer,iMaxBufSize, 0, (struct sockaddr *)ptSockAddr, &addrsize);
	if(recv_size <= 0)
	{
		return -1;
	}

	return  recv_size;
}

//TCP socket��ȡ���ݣ�block������ȡ
//�����ظ�������������ʵ�ʽ��յĳ���
int  SockBase_TcpRead(int sockfd,char *pBuffer,int iMaxBufSize)
{
	int   recv_size;

	assert(pBuffer);

	recv_size = recv(sockfd,pBuffer,iMaxBufSize,0);
	//recv_size = recv(sockfd,pBuffer,iMaxBufSize,MSG_DONTWAIT);
	if(recv_size <= 0)
	{
		return -1;
	}

	return  recv_size;
}

//UDP socket��ȡ���ݣ�select��ʽ����ʱ����
//�����ظ�������ʱ����0�����򷵻�ʵ�ʽ��յĳ���
static int  __SockBase_udp_select_read(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iMaxBufSize,int iTimeoutMs)
{
	int                 ret = 0;
	int                 addrsize;
	int                 recv_size;
	fd_set              fds_rd;
	struct timeval      tv;

	memset(ptSockAddr,0,sizeof(struct sockaddr_in));
	addrsize = sizeof(struct sockaddr);

	tv.tv_sec = iTimeoutMs /1000;
	tv.tv_usec = (iTimeoutMs %1000) *1000;
	FD_ZERO(&fds_rd);
	FD_SET(sockfd,&fds_rd);
	
	ret = select(sockfd+1,&fds_rd,(fd_set *)0,(fd_set *)0,&tv);
	if(ret < 0)
	{
		return  -1;
	}
	else if(ret == 0)
	{
		return  0;
	}
	else
	{
		if(FD_ISSET(sockfd,&fds_rd))
		{
			recv_size = recvfrom(sockfd,pBuffer,iMaxBufSize,0,
								 (struct sockaddr *)ptSockAddr,(socklen_t *)&addrsize);
			if(recv_size <= 0)
			{
				ret = -2;
			}
			else
			{
				ret = recv_size;
			}
		}
	}
	
	return  ret;
}

//TCP socket��ȡ���ݣ�select��ʽ����ʱ����
//�����ظ�������ʱ����0�����򷵻�ʵ�ʽ��յĳ���
static int  __SockBase_tcp_select_read(int sockfd,char *pBuffer,int iMaxBufSize,int iTimeoutMs)
{
	int                 ret = 0;
	int                 recv_size;
	fd_set              fds_rd;
	struct timeval      tv;

	tv.tv_sec = iTimeoutMs /1000; //Anchel comment
	tv.tv_usec = (iTimeoutMs %1000) *1000;
	
	tv.tv_sec = 0;//anchel add
	tv.tv_usec = 10000;//Anchel comment

	FD_ZERO(&fds_rd);
	FD_SET(sockfd,&fds_rd);
	
	ret = select(sockfd+1,&fds_rd,(fd_set *)0,(fd_set *)0,&tv);
	if(ret < 0)
	{
		return  -1;
	}
	else if(ret == 0)
	{
		return  0;
	}
	else
	{
		if(FD_ISSET(sockfd,&fds_rd))
		{
			//recv_size = recv(sockfd,pBuffer,iMaxBufSize,0);
			recv_size = recv(sockfd,pBuffer,iMaxBufSize,MSG_DONTWAIT);
			if(recv_size <= 0)
			{
				ret = -2;
			}
			else
			{
				ret = recv_size;
			}
		}
	}
	
	return  ret;
}

//UDP socket��ȡ���ݣ�select��ʽ����ʱ����
//�����ظ�������ʱ����0�����򷵻�ʵ�ʽ��յĳ���
int  SockBase_UdpReadEx(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iMaxBufSize,int iTimeoutMs)
{
	assert(ptSockAddr);
	assert(pBuffer);

	return  __SockBase_udp_select_read(sockfd,ptSockAddr,pBuffer,iMaxBufSize,iTimeoutMs);
}

//TCP socket��ȡ���ݣ�select��ʽ����ʱ����
//�����ظ�������ʱ����0�����򷵻�ʵ�ʽ��յĳ���
int  SockBase_TcpReadEx(int sockfd,char *pBuffer,int iMaxBufSize,int iTimeoutMs)
{
	assert(pBuffer);

	return  __SockBase_tcp_select_read(sockfd,pBuffer,iMaxBufSize,iTimeoutMs);
}

// int main()
// {
	// int ret = 0;
	// unsigned int uiHexAddr;
	// char tTmpBuf[32];
	
	// strcpy(tTmpBuf,"127.0.555.2");
	// uiHexAddr = SockBase_aton(tTmpBuf);
	
	// printf("SockBase_aton: 127.0.1.2 --> %#x\n", uiHexAddr);
	
	// ret = SockBase_ConvertIP2Addr(tTmpBuf, &uiHexAddr);
	// printf("SockBase_ConvertIP2Addr:ret=%d, \"127.0.1.2\" --> %#x\n", ret, uiHexAddr);
	
	// uiHexAddr = 0x01020304;
	// printf("SockBase_ntoa: 0x01020304 --> %s\n", SockBase_ntoa(uiHexAddr));
	
	// ret = SockBase_ConvertAddr2IP(uiHexAddr, tTmpBuf);
	// printf("SockBase_ConvertAddr2IP:ret=%d, 0x01020304 --> %s\n", ret, tTmpBuf);
	
	
	// return 0;
// }