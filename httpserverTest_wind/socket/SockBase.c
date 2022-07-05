/*===========================
-- SOCKET基础库源文件
-- 谢福文<kevin_linux@163.com>
-- v1.00
-- 2013年5月8日
-- socket基础库
-- v1.10
-- 2018年11月8日
-- 增加RAW原始套接字部分接口
-- 增加组播套接字部分接口
-- 增加地址aton ntoa接口
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

//创建一个UDP方式的socket
//正常返回创建好的socket fd，出错返回负数
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

//创建一个TCP方式的socket
//正常返回创建好的socket fd，出错返回负数
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

//创建一个原始套接字socket
//正常返回创建好的socket fd，出错返回负数
int  SockBase_RawCreate(void)
{
	int sockfd;

	/*
	ETH_P_IP   0x800  只接收发往本机mac的ip类型的数据帧
	ETH_P_ARP  0x806  只接受发往本机mac的arp类型的数据帧
	ETH_P_RARP 0x8035 只接受发往本机mac的rarp类型的数据帧
	ETH_P_ALL  0x3    接收发往本机mac的所有类型ip arp rarp的数据帧, 接收从本机发出的所有类型的数据帧
	
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

//删除一个socket
int  SockBase_Delete(int sockfd)
{
	if(sockfd > 0)
	{
		close(sockfd);
	}

	return  0;
}

//设置socket为非阻塞方式
int  SockBase_SetNonBlock(int sockfd)
{
	int flag;

	flag = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);

	return 0;
}

//设置socket为阻塞方式
int  SockBase_SetBlock(int sockfd)
{
	int flag;

	flag = fcntl(sockfd, F_GETFL, 0) & (~O_NONBLOCK);
	fcntl(sockfd, F_SETFL, flag);

	return 0;
}

//转换IP字符串成本地字节序二进制格式
//正常返回0，出错返回负数
int  SockBase_ConvertIP2Addr(char *pIpString,unsigned int *pHexAddr)
{
	int  ret;
	unsigned int uiNetHexAddr;

	//IP字符串转成网络字节序排列的二进制地址
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
	
	//网络字节序转成本地字节序输出
	*pHexAddr = htonl(uiNetHexAddr);
	
	return  0;
}

//转换本地字节序二进制IP地址成IP字符串
//正常返回0，出错返回负数
int  SockBase_ConvertAddr2IP(unsigned int uiHexAddr,char *pIpString)
{
	//int  ret;
	unsigned int uiNetHexAddr;
	
	//本地字节序先转成网络字节序
	uiNetHexAddr = htonl(uiHexAddr);
	
	if(inet_ntop(AF_INET, (const void *)&uiNetHexAddr, pIpString, INET_ADDRSTRLEN) == NULL)
	{
		// printf("invalid network address\n");
		// printf("inet_ntop error\n");
		return  -1;
	}

	return  0;
}

//IP字符串转成本地字节序二进制IP地址
//eg: "127.0.0.1" 转出来是 0x7f000001
unsigned int  SockBase_aton(char *pIpString)
{
	struct in_addr tAddr;

	//inet_addr()转出来的二进制IP是网络字节序排列的
	tAddr.s_addr = inet_addr(pIpString);

	//转成本地字节序输出
	return  htonl(tAddr.s_addr);
}

//本地字节序二进制IP地址转换成IP字符串
//eg: 0x7f000001 转出来是 "127.0.0.1"
char  *SockBase_ntoa(unsigned int uiHexAddr)
{
	struct in_addr tAddr;

	//本地字节序先转成网络字节序
	tAddr.s_addr = htonl(uiHexAddr);

	//再去转成IP字符串
	return inet_ntoa(tAddr);
}

//IP检查接口
static int __SockBase_ip_check(char *ip)
{
	unsigned int uiAddr;
	
	return  SockBase_ConvertIP2Addr(ip, &uiAddr);
}

//PORT检查接口
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

//配置sockaddr结构体，以本地方式
//正常返回0，出错返回负数
int  SockBase_ConfigLocalAddr(struct sockaddr_in *ptSockAddr,int iPort)
{
	assert(ptSockAddr);

	if(__SockBase_port_check(iPort) < 0)
	{
		return  -1;
	}

	memset(ptSockAddr,0,sizeof(struct sockaddr_in));

	ptSockAddr->sin_family = AF_INET;
	ptSockAddr->sin_port = htons(iPort);	/* 配置端口 */
	ptSockAddr->sin_addr.s_addr = htonl(INADDR_ANY);	/* IP地址 */

	return  0;
}

//配置sockaddr结构体，以指定IP方式
//正常返回0，出错返回负数
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
	ptSockAddr->sin_port = htons(iPort);	/* 配置端口 */
	ptSockAddr->sin_addr.s_addr = inet_addr(pIpString);	/* IP地址 */

	return  0;
}

//获取本地IP地址，以字符串方式存储
//正常返回0，出错返回负数
int  SockBase_GetLocalIp(char *pIpBuf)
{
	struct hostent *ptHost;
	char  *pIpStr;
	char namebuf[128];

	if(pIpBuf == NULL)
	{
		return  -1;
	}

	/* 获得主机名 */
	if(gethostname(namebuf,sizeof(namebuf)) != 0)
	{
		return  -1;
	}

	/* 获得主机结构 */
	ptHost = gethostbyname(namebuf); 
	if(ptHost == NULL)
	{
		return  -1;
	}

	/* 转换IP地址为字符串格式 */
	//pIpStr = inet_ntoa(*((in_addr *)ptHost->h_addr));
	//pIpStr = inet_ntoa(*((in_addr *)ptHost->h_addr_list[0]));
	pIpStr = inet_ntoa(*((struct in_addr *)ptHost->h_addr_list[0]));
	if(pIpStr == NULL)
	{
		return  -1;
	}

	/* 拷贝到输出缓存里 */
	strcpy(pIpBuf,pIpStr);

	return  0;
}

//获取域名对应的IP地址，以字符串方式存储
//正常返回0，出错返回负数
int  SockBase_GetDomainIp(char *pIpBuf, char *pcDomainStr)
{
	struct hostent *ptHost;
	char  *pIpStr;

	if(!pIpBuf || !pcDomainStr)
	{
		return  -1;
	}

    pIpBuf[0] = '\0';
	/* 获得主机结构 */
	ptHost = gethostbyname(pcDomainStr); 
	if(ptHost == NULL)
	{
		return  -1;
	}

	/* 转换IP地址为字符串格式 */
	//pIpStr = inet_ntoa(*((in_addr *)ptHost->h_addr));
	//pIpStr = inet_ntoa(*((in_addr *)ptHost->h_addr_list[0]));
	pIpStr = inet_ntoa(*((struct in_addr *)ptHost->h_addr_list[0]));
	if(pIpStr == NULL)
	{
		return  -1;
	}

	/* 拷贝到输出缓存里 */
	strcpy(pIpBuf, pIpStr);

	return  0;
}


//加入一个组播组,指定具体ip加入一个组播组里
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

//设置socket接收缓冲区大小
//正常返回0，出错返回负数
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

//设置socket发送缓冲区大小
//正常返回0，出错返回负数
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

//获取socket接收缓冲区大小
//正常返回0，出错返回负数
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

//获取socket发送缓冲区大小
//正常返回0，出错返回负数
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

//设置socket接收超时时长，单位毫秒
//正常返回0，出错返回负数
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

//设置socket发送超时时长，单位毫秒
//正常返回0，出错返回负数
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

//设置TCP socket禁用Nagle算法的开关状态
//正常返回0，出错返回负数
int  SockBase_SetTcpNoDelay(int sockfd,int iEnable)
{
	int tmpEn = !!iEnable;

	if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,(void*)&tmpEn, sizeof(int)) < 0) {
		//printf("() error\n");
		return -1;
	}

	return 0;
}

//设置socket绑定对应的网卡
//正常返回0，出错返回负数
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

//设置socket地址复用的开关状态
//正常返回0，出错返回负数
int  SockBase_SetReuseAddr(int sockfd,int iReuseEn)
{
	int tmpEn = !!iReuseEn;
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&tmpEn, sizeof(int)) < 0) {
		//printf("() error\n");
		return -1;
	}

	return 0;
}

//设置socket组播数据是否回环给自己
//正常返回0，出错返回负数
int  SockBase_SetGroupLoop(int sockfd,int iLoopEn)
{
	int tmpEn = !!iLoopEn;
	
	if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, (void*)&tmpEn, sizeof(int)) < 0) {
		//printf("() error\n");
		return -1;
	}

	return 0;
}

//设置socket加入组播组
//正常返回0，出错返回负数
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

	mreq.imr_multiaddr.s_addr = uiMulticastIp; //组播IP
	mreq.imr_interface.s_addr = htonl(INADDR_ANY); /*网络接口为默认*/

	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) < 0)
	{
		//printf("IP_ADD_MEMBERSHIP\n");
		return -1;
	}

	return iRtn;
}

//设置socket移出组播组
//正常返回0，出错返回负数
int SockBase_SetRemoveGroup(int sockfd, char *pMulticastIp)
{
	struct ip_mreq mreq; 
	unsigned int uiMulticastIp;

	if(SockBase_ConvertIP2Addr(pMulticastIp, &uiMulticastIp) < 0)
	{
		//printf("IP check error:%s\n",pMulticastIp);
		return -1;
	}

	mreq.imr_multiaddr.s_addr = uiMulticastIp; //组播IP
	mreq.imr_interface.s_addr = htonl(INADDR_ANY); /*网络接口为默认*/

	if (setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) < 0)
	{
		printf("setsockopt():IP_DROP_MEMBERSHIP");
		return -1;
	}

	return 0;
}


//socket绑定
//正常返回0，出错返回负数
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

//TCP socket监听
//正常返回0，出错返回负数
int  SockBase_TcpListen(int sockfd,int iMaxConnect)
{
	if (listen(sockfd, iMaxConnect) < 0)
	{
		//printf("listen error\n");
		return  -1;
	}

	return  0;
}

//TCP socket接收一个连接
//正常返回0，出错返回负数
int  SockBase_TcpAccept(int sockfd,int *pConnectfd,struct sockaddr_in *ptClientAddr)
{
	int  ret;
	int  addrsize;
	struct sockaddr_in tClientAddr;

	//BUG修复，20150607
	//地址大小填值，同时先清空用来装连接的客户端地址的结构体，否则会出现accept出错问题
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

//TCP socket连接指定服务器
//正常返回0，出错返回负数
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

//TCP socket连接指定服务器，非阻塞方式，超时自动返回
//出错返回负数，正常返回0
static int  __SockBase_tcpconnect_ex(int sockfd,struct sockaddr_in *ptServerAddr,int iTimeoutMs)
{
	int  ret;
	int  error;
	//int  len;
	int iFlag = 0;
	socklen_t len;
	fd_set   fds_wd;
	struct timeval  tv;
	
	//先设置sockfd为非阻塞模式
	iFlag = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, iFlag|O_NONBLOCK);

	ret = connect(sockfd, (struct sockaddr *)ptServerAddr, sizeof(struct sockaddr));
	if (ret == 0)
	{
		//printf("tcp connect success\n");
	}
	else
	{
		//连接失败
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
			if(ret < 0)	/* 出错返回-1 */
			{
			//	printf("tcp socket select error\n");
				goto errout;
			}
			else if(ret == 0)	/* 超时返回0 */
			{
			//	printf("tcp socket select timeout\n");
				goto errout;
			}
			else	/* 等待的socket套接口状态有改变 */
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

	//还原原模式
	fcntl(sockfd, F_SETFL, iFlag);
	
	return 0;

errout:
	//还原原模式
	fcntl(sockfd, F_SETFL, iFlag);
	
	return -1;
}

//TCP socket连接指定服务器，非阻塞方式，超时自动返回
//出错返回负数，正常返回0
int  SockBase_TcpConnectEx(int sockfd,struct sockaddr_in *ptServerAddr,int timeoutMs)
{
	int  ret;
	
	//非阻塞方式连接服务器，如果服务器不存在，最长 timeoutMs毫秒后会返回
	ret = __SockBase_tcpconnect_ex(sockfd,ptServerAddr,timeoutMs);
	if (ret < 0)
	{
		return -1;;
	}

	return  0;
}

//UDP socket发送指定大小的数据
//出错返回负数，正常返回实际发送的大小
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

//TCP socket发送指定大小的数据
//出错返回负数，正常返回实际发送的大小
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

//TCP方式socket，查询是否可写再发送，防阻塞卡死
//正常已发送的大小，超时返回0，出错返回负数
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

	//检查是否可写，实际是发送缓冲区还有没有位置
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

//TCP socket发送指定大小的数据
//出错返回负数，超时返回0，否则返回实际发送的长度
int  SockBase_TcpSendEx(int sockfd,char *pBuffer,int iBufSize,int iTimeoutMs)
{
	int  ret;

	assert(pBuffer);

	ret = __SockBase_tcp_select_send(sockfd,pBuffer,iBufSize,iTimeoutMs);

	return  ret;
}

//UDP socket读取数据，block阻塞读取
//出错返回负数，正常返回实际接收的长度
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

//TCP socket读取数据，block阻塞读取
//出错返回负数，正常返回实际接收的长度
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

//UDP socket读取数据，select方式，超时返回
//出错返回负数，超时返回0，否则返回实际接收的长度
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

//TCP socket读取数据，select方式，超时返回
//出错返回负数，超时返回0，否则返回实际接收的长度
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

//UDP socket读取数据，select方式，超时返回
//出错返回负数，超时返回0，否则返回实际接收的长度
int  SockBase_UdpReadEx(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iMaxBufSize,int iTimeoutMs)
{
	assert(ptSockAddr);
	assert(pBuffer);

	return  __SockBase_udp_select_read(sockfd,ptSockAddr,pBuffer,iMaxBufSize,iTimeoutMs);
}

//TCP socket读取数据，select方式，超时返回
//出错返回负数，超时返回0，否则返回实际接收的长度
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