/*===========================
-- SOCKET用户库源文件
-- 谢福文Kevin
-- <kevin_linux@163.com>
-- v1.00
-- 2018年11月15日
-- 在socket基础库上封装用户使用库
=============================*/
#include <stdio.h>
#include <string.h>

#ifdef linux
#include <unistd.h>
#endif
#include "SockBase.h"
#include "SockUser.h"


#ifndef linux
int winsock_startup( void )
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD( 2, 2 );
 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                 */
		return err;
	}
 
    return 0; 
}

int winsock_cleanup( void )
{
    return WSACleanup( );
}
#endif

//打开一个UDP socket服务器
int  SockUser_OpenUdpServer(T_SuUdpServer *ptServer, int iLocalPort)
{
	int sockfd;
	struct sockaddr_in stLocalAddr;

	//配置sockaddr结构体，以本地方式
	if(SockBase_ConfigLocalAddr(&stLocalAddr, iLocalPort) < 0)
	{
		return  -1;
	}
	
	//创建一个UDP方式的socket
	sockfd = SockBase_UdpCreate();
	if(sockfd < 0)
	{
		return  -1;
	}

	//socket绑定
	if(SockBase_Bind(sockfd, &stLocalAddr) < 0)
	{
		SockBase_Delete(sockfd);
		return  -1;
	}
	
	memset(ptServer, 0, sizeof(T_SuUdpServer));
	ptServer->iSockFd = sockfd;
	ptServer->iLocalPort = iLocalPort;
	memcpy(&ptServer->stLocalAddr, &stLocalAddr, sizeof(struct sockaddr_in));
	
	return 0;
}

//打开一个组播 socket服务器
/** Anchel add
 * @brief 
 * 
 * @param ptServer 输出
 * @param iGroupPort  输入
 * @param pGroupIp 输入
 * @param iLoopEn 输入
 * @return int 
 */
int  SockUser_OpenGroupServer(T_SuUdpServer *ptServer, int iGroupPort, char *pGroupIp, int iLoopEn)
{
	int sockfd;
	struct sockaddr_in stLocalAddr;
	
	//配置sockaddr结构体，以本地方式
	if(SockBase_ConfigAddr(&stLocalAddr, pGroupIp, iGroupPort) < 0)
	{
		return  -1;
	}
	
	//创建一个UDP方式的socket
	sockfd = SockBase_UdpCreate();
	if(sockfd < 0)
	{
		return  -1;
	}

	//设置socket组播数据是否回环给自己
	if(SockBase_SetGroupLoop(sockfd, iLoopEn) < 0)
	{
		SockBase_Delete(sockfd);
		return  -1;
	}
	
	//设置socket加入组播组
	if(SockBase_SetJoinGroup(sockfd, pGroupIp) < 0)
	{
		SockBase_Delete(sockfd);
		return  -1;
	}
	
	//socket绑定
	if(SockBase_Bind(sockfd, &stLocalAddr) < 0)
	{
		SockBase_SetRemoveGroup(sockfd, pGroupIp);
		SockBase_Delete(sockfd);
		return  -1;
	}
	
	memset(ptServer, 0, sizeof(T_SuUdpServer));
	ptServer->iSockFd = sockfd;
	ptServer->iGroupEn = 1;
	ptServer->iLocalPort = iGroupPort;
	strcpy(ptServer->tGroupIp, pGroupIp);
	memcpy(&ptServer->stLocalAddr, &stLocalAddr, sizeof(struct sockaddr_in));
	
	return 0;
}

//打开一个TCP socket服务器
int  SockUser_OpenTcpServer(T_SuTcpServer *ptServer, int iLocalPort,int iMaxConnect)
{
	int sockfd;
	struct sockaddr_in stLocalAddr;

	//配置sockaddr结构体，以本地方式
	if(SockBase_ConfigLocalAddr(&stLocalAddr, iLocalPort) < 0)
	{
		return  -1;
	}
	
	//创建一个Tcp方式的socket
	sockfd = SockBase_TcpCreate();
	if(sockfd < 0)
	{
		return  -1;
	}

	//socket绑定
	if(SockBase_Bind(sockfd, &stLocalAddr) < 0)
	{
		SockBase_Delete(sockfd);
		return  -1;
	}
	
	//TCP socket监听
	if(SockBase_TcpListen(sockfd, iMaxConnect) < 0)
	{
		SockBase_Delete(sockfd);
		return  -1;
	}
	
	memset(ptServer, 0, sizeof(T_SuTcpServer));
	ptServer->iSockFd = sockfd;
	ptServer->iLocalPort = iLocalPort;
	memcpy(&ptServer->stLocalAddr, &stLocalAddr, sizeof(struct sockaddr_in));
	
	return 0;
}

//关闭一个UDP socket服务器
int  SockUser_CloseUdpServer(T_SuUdpServer *ptServer)
{
	//创建过
	if(ptServer->iSockFd > 0)
	{
		//是组播服务器，先将其从组播组中移除
		if(ptServer->iGroupEn > 0)
		{
			SockBase_SetRemoveGroup(ptServer->iSockFd, ptServer->tGroupIp);
			ptServer->iGroupEn = 0;
		}
		//删除一个socket
		SockBase_Delete(ptServer->iSockFd);
	}
	ptServer->iSockFd = -1;

	return  0;
}

//关闭一个TCP socket服务器
int  SockUser_CloseTcpServer(T_SuTcpServer *ptServer)
{
	//创建过
	if(ptServer->iSockFd > 0)
	{
		//删除一个socket
		SockBase_Delete(ptServer->iSockFd);
	}
	ptServer->iSockFd = -1;
	
	return  0;
}

//打开一个UDP socket客户端
int  SockUser_OpenUdpClient(T_SuUdpClient *ptClient, char *pServerIp, int iServerPort)
{
	int sockfd;
	struct sockaddr_in stSrvAddr;

	//配置sockaddr结构体，以IP方式
	if(SockBase_ConfigAddr(&stSrvAddr, pServerIp, iServerPort) < 0)
	{
		return  -1;
	}
	
	//创建一个UDP方式的socket
	sockfd = SockBase_UdpCreate();
	if(sockfd < 0)
	{
		return  -1;
	}

	memset(ptClient, 0, sizeof(T_SuUdpClient));
	ptClient->iSockFd = sockfd;
	ptClient->iSrvPort = iServerPort;
	strcpy(ptClient->tSrvIp, pServerIp);
	memcpy(&ptClient->stSrvAddr, &stSrvAddr, sizeof(struct sockaddr_in));
	
	return 0;
}

//打开一个TCP socket客户端
int  SockUser_OpenTcpClient(T_SuTcpClient *ptClient, char *pServerIp, int iServerPort, int iTimeoutMs)
{
	int sockfd;
	int iConnectFlag = 0;
	struct sockaddr_in stSrvAddr;

	//配置sockaddr结构体，以IP方式
	if(SockBase_ConfigAddr(&stSrvAddr, pServerIp, iServerPort) < 0)
	{
		return  -1;
	}
	
	//创建一个TCP方式的socket
	sockfd = SockBase_TcpCreate();
	if(sockfd < 0)
	{
		return  -1;
	}
	
	//TCP socket连接指定服务器，非阻塞方式，超时自动返回
	//出错返回负数，正常返回0
	if(SockBase_TcpConnectEx(sockfd, &stSrvAddr, iTimeoutMs) < 0)
	{
		//SockBase_Delete(sockfd);
		//return  -1;
		iConnectFlag = 0;
	}
	else
	{
		iConnectFlag = 1;
	}

	memset(ptClient, 0, sizeof(T_SuTcpClient));
	ptClient->iSockFd = sockfd;
	ptClient->iConnected = iConnectFlag;
	ptClient->iSrvPort = iServerPort;
	strcpy(ptClient->tSrvIp, pServerIp);
	memcpy(&ptClient->stSrvAddr, &stSrvAddr, sizeof(struct sockaddr_in));
	
	return 0;
}

//关闭一个UDP socket客户端
int  SockUser_CloseUdpClient(T_SuUdpClient *ptClient)
{
	//创建过
	if(ptClient->iSockFd > 0)
	{
		//删除一个socket
		SockBase_Delete(ptClient->iSockFd);
	}
	ptClient->iSockFd = -1;
	
	return  0;
}

//关闭一个TCP socket客户端
int  SockUser_CloseTcpClient(T_SuTcpClient *ptClient)
{
	//创建过
	if(ptClient->iSockFd > 0)
	{
		//删除一个socket
		SockBase_Delete(ptClient->iSockFd);
	}
	ptClient->iSockFd = -1;
	ptClient->iConnected = 0;
	
	return  0;
}

//检测socket读事件
//返回-1表示出错，返回0表示超时，返回1表示socket可读
int SockUser_CheckReadEvent(int iSockfd, int iTimeoutMs)
{
	int                 ret = 0;
	fd_set              fds_rd;
	struct timeval      tv;

	tv.tv_sec = iTimeoutMs /1000;
	tv.tv_usec = (iTimeoutMs %1000) *1000;
	
	FD_ZERO(&fds_rd);
	FD_SET(iSockfd,&fds_rd);
	
	ret = select(iSockfd+1,&fds_rd,(fd_set *)0,(fd_set *)0,&tv);
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
		if(FD_ISSET(iSockfd,&fds_rd))
		{
			ret = 1;
		}
	}
	
	return  ret;
}

//检测socket写事件
//返回-1表示出错，返回0表示超时，返回1表示socket可写
int SockUser_CheckWriteEvent(int iSockfd, int iTimeoutMs)
{
	int                 ret = 0;
	fd_set              fds_wr;
	struct timeval      tv;

	tv.tv_sec = iTimeoutMs /1000;
	tv.tv_usec = (iTimeoutMs %1000) *1000;
	
	FD_ZERO(&fds_wr);
	FD_SET(iSockfd,&fds_wr);
	
	ret = select(iSockfd+1,(fd_set *)0,&fds_wr,(fd_set *)0,&tv);
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
		if(FD_ISSET(iSockfd,&fds_wr))
		{
			ret = 1;
		}
	}
	
	return  ret;
}

//TCP socket接收一个连接
int  SockUser_TcpAccept(int iSrvSockfd,T_SuTcpClient *ptClient)
{
	int  iConnectfd;
	struct sockaddr_in tClientAddr;

	//TCP socket接收一个连接
	if (SockBase_TcpAccept(iSrvSockfd,&iConnectfd,&tClientAddr) < 0)
	{
		//printf("accept error\n");
		return -1;;
	}

	memset(ptClient, 0, sizeof(T_SuTcpClient));
	ptClient->iSockFd = iConnectfd;
	ptClient->iConnected = 1;
	//不是形式1，修改形式1的端口号为非法
	ptClient->iSrvPort = -1;
	ptClient->tSrvIp[0] = '\0';
	//形式2，修改形式2的端口号
	ptClient->iCliPort = ntohs(tClientAddr.sin_port);
	strcpy(ptClient->tCliIp, SockBase_ntoa(tClientAddr.sin_addr.s_addr));

	memcpy(&ptClient->stCliAddr, &tClientAddr, sizeof(struct sockaddr_in));

	return  0;
}


//UDP socket接收指定大小的数据，直到接收完成或超时才返回
//出错返回负数，否则返回实际接收的长度，超时时实际接收的长度与最大长度不一样
int  SockUser_UdpReadUntilFullOrTimeout(int iSockfd,char *pDataBuf,int iMaxReadSize,int iTimeoutMs)
{
	int  ret = 0;
	int iCurSize = 0;
	struct sockaddr_in tPeerAddr;
	
	if(iSockfd <= 0 || !pDataBuf || iMaxReadSize <= 0 || iTimeoutMs <= 0)
	{
		printf("%s %d:param error\n",__FUNCTION__,__LINE__);
		return  -1;
	}

	while(iCurSize < iMaxReadSize)
	{
		//UDP socket读取数据，select方式，超时返回
		//出错返回负数，超时返回0，否则返回实际接收的长度
		ret = SockBase_UdpReadEx(iSockfd, &tPeerAddr, pDataBuf + iCurSize, iMaxReadSize - iCurSize, iTimeoutMs);
		if (ret < 0)
		{
			//printf("SockBase_UdpReadEx error\n");
			break;
		}
		else if (ret == 0)
		{
			//printf("SockBase_UdpReadEx timeout\n");
			break;
		}
		
		//本次读到ret字节数据，更新最新读到数据总大小
		iCurSize += ret;
	}
	
	if(ret < 0)
	{
		return  -1;
	}
	else
	{
		return  iCurSize;
	}
}

//TCP socket接收指定大小的数据，直到接收完成或超时才返回
//出错返回负数，否则返回实际接收的长度，超时时实际接收的长度与最大长度不一样
int  SockUser_TcpReadUntilFullOrTimeout(int iSockfd,char *pDataBuf,int iMaxReadSize,int iTimeoutMs)
{
	int  ret = 0;
	int iCurSize = 0;
	
	if(iSockfd <= 0 || !pDataBuf || iMaxReadSize <= 0 || iTimeoutMs <= 0)
	{
		printf("%s %d:param error\n",__FUNCTION__,__LINE__);
		return  -1;
	}

	while(iCurSize < iMaxReadSize)
	{
		//TCP socket读取数据，select方式，超时返回
		//出错返回负数，超时返回0，否则返回实际接收的长度
		ret = SockBase_TcpReadEx(iSockfd, pDataBuf + iCurSize, iMaxReadSize - iCurSize, iTimeoutMs);
		if (ret < 0)
		{
			//printf("SockBase_TcpReadEx error\n");
			break;
		}
		else if (ret == 0)
		{
			//printf("SockBase_TcpReadEx timeout\n");
			break;
		}
		
		//本次读到ret字节数据，更新最新读到数据总大小
		iCurSize += ret;
	}
	
	if(ret < 0)
	{
		return  -1;
	}
	else
	{
		return  iCurSize;
	}
}

//TCP socket发送指定大小的数据，直到发送完成或超时才返回
//出错返回负数，否则返回实际发送的长度，超时时实际发送的长度与最大长度不一样
int  SockUser_TcpSendUntilEndOrTimeout(int iSockfd,char *pDataBuf,int iMaxSendSize,int iTimeoutMs)
{
	int  ret = 0;
	int iCurSize = 0;
	
	if(iSockfd <= 0 || !pDataBuf || iMaxSendSize <= 0 || iTimeoutMs <= 0)
	{
		printf("%s %d:param error\n",__FUNCTION__,__LINE__);
		return  -1;
	}

	while(iCurSize < iMaxSendSize)
	{
		//TCP socket发送指定大小的数据
		//出错返回负数，超时返回0，否则返回实际发送的长度
		ret = SockBase_TcpSendEx(iSockfd, pDataBuf + iCurSize, iMaxSendSize - iCurSize, iTimeoutMs);
		if (ret < 0)
		{
			//printf("SockBase_TcpSendEx error\n");
			break;
		}
		else if (ret == 0)
		{
			//printf("SockBase_TcpSendEx timeout\n");
			break;
		}
		
		//本次ret字节数据，更新最新读到数据总大小
		iCurSize += ret;
	}
	
	if(ret < 0)
	{
		return  -1;
	}
	else
	{
		return  iCurSize;
	}
}



#if 0
//UDP socket发送指定大小的数据
//出错返回负数，正常返回实际发送的大小
int  SockBase_UdpSend(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iBufSize)
{

}

//TCP socket发送指定大小的数据
//出错返回负数，正常返回实际发送的大小
int  SockBase_TcpSend(int sockfd,char *pBuffer,int iBufSize)
{

}

//TCP socket发送指定大小的数据
//出错返回负数，超时返回0，否则返回实际发送的长度
int  SockBase_TcpSendEx(int sockfd,char *pBuffer,int iBufSize,int iTimeoutMs)
{

}

#endif

