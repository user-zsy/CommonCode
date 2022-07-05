/*===========================
-- SOCKET�û���Դ�ļ�
-- л����Kevin
-- <kevin_linux@163.com>
-- v1.00
-- 2018��11��15��
-- ��socket�������Ϸ�װ�û�ʹ�ÿ�
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

//��һ��UDP socket������
int  SockUser_OpenUdpServer(T_SuUdpServer *ptServer, int iLocalPort)
{
	int sockfd;
	struct sockaddr_in stLocalAddr;

	//����sockaddr�ṹ�壬�Ա��ط�ʽ
	if(SockBase_ConfigLocalAddr(&stLocalAddr, iLocalPort) < 0)
	{
		return  -1;
	}
	
	//����һ��UDP��ʽ��socket
	sockfd = SockBase_UdpCreate();
	if(sockfd < 0)
	{
		return  -1;
	}

	//socket��
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

//��һ���鲥 socket������
/** Anchel add
 * @brief 
 * 
 * @param ptServer ���
 * @param iGroupPort  ����
 * @param pGroupIp ����
 * @param iLoopEn ����
 * @return int 
 */
int  SockUser_OpenGroupServer(T_SuUdpServer *ptServer, int iGroupPort, char *pGroupIp, int iLoopEn)
{
	int sockfd;
	struct sockaddr_in stLocalAddr;
	
	//����sockaddr�ṹ�壬�Ա��ط�ʽ
	if(SockBase_ConfigAddr(&stLocalAddr, pGroupIp, iGroupPort) < 0)
	{
		return  -1;
	}
	
	//����һ��UDP��ʽ��socket
	sockfd = SockBase_UdpCreate();
	if(sockfd < 0)
	{
		return  -1;
	}

	//����socket�鲥�����Ƿ�ػ����Լ�
	if(SockBase_SetGroupLoop(sockfd, iLoopEn) < 0)
	{
		SockBase_Delete(sockfd);
		return  -1;
	}
	
	//����socket�����鲥��
	if(SockBase_SetJoinGroup(sockfd, pGroupIp) < 0)
	{
		SockBase_Delete(sockfd);
		return  -1;
	}
	
	//socket��
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

//��һ��TCP socket������
int  SockUser_OpenTcpServer(T_SuTcpServer *ptServer, int iLocalPort,int iMaxConnect)
{
	int sockfd;
	struct sockaddr_in stLocalAddr;

	//����sockaddr�ṹ�壬�Ա��ط�ʽ
	if(SockBase_ConfigLocalAddr(&stLocalAddr, iLocalPort) < 0)
	{
		return  -1;
	}
	
	//����һ��Tcp��ʽ��socket
	sockfd = SockBase_TcpCreate();
	if(sockfd < 0)
	{
		return  -1;
	}

	//socket��
	if(SockBase_Bind(sockfd, &stLocalAddr) < 0)
	{
		SockBase_Delete(sockfd);
		return  -1;
	}
	
	//TCP socket����
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

//�ر�һ��UDP socket������
int  SockUser_CloseUdpServer(T_SuUdpServer *ptServer)
{
	//������
	if(ptServer->iSockFd > 0)
	{
		//���鲥���������Ƚ�����鲥�����Ƴ�
		if(ptServer->iGroupEn > 0)
		{
			SockBase_SetRemoveGroup(ptServer->iSockFd, ptServer->tGroupIp);
			ptServer->iGroupEn = 0;
		}
		//ɾ��һ��socket
		SockBase_Delete(ptServer->iSockFd);
	}
	ptServer->iSockFd = -1;

	return  0;
}

//�ر�һ��TCP socket������
int  SockUser_CloseTcpServer(T_SuTcpServer *ptServer)
{
	//������
	if(ptServer->iSockFd > 0)
	{
		//ɾ��һ��socket
		SockBase_Delete(ptServer->iSockFd);
	}
	ptServer->iSockFd = -1;
	
	return  0;
}

//��һ��UDP socket�ͻ���
int  SockUser_OpenUdpClient(T_SuUdpClient *ptClient, char *pServerIp, int iServerPort)
{
	int sockfd;
	struct sockaddr_in stSrvAddr;

	//����sockaddr�ṹ�壬��IP��ʽ
	if(SockBase_ConfigAddr(&stSrvAddr, pServerIp, iServerPort) < 0)
	{
		return  -1;
	}
	
	//����һ��UDP��ʽ��socket
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

//��һ��TCP socket�ͻ���
int  SockUser_OpenTcpClient(T_SuTcpClient *ptClient, char *pServerIp, int iServerPort, int iTimeoutMs)
{
	int sockfd;
	int iConnectFlag = 0;
	struct sockaddr_in stSrvAddr;

	//����sockaddr�ṹ�壬��IP��ʽ
	if(SockBase_ConfigAddr(&stSrvAddr, pServerIp, iServerPort) < 0)
	{
		return  -1;
	}
	
	//����һ��TCP��ʽ��socket
	sockfd = SockBase_TcpCreate();
	if(sockfd < 0)
	{
		return  -1;
	}
	
	//TCP socket����ָ������������������ʽ����ʱ�Զ�����
	//�����ظ�������������0
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

//�ر�һ��UDP socket�ͻ���
int  SockUser_CloseUdpClient(T_SuUdpClient *ptClient)
{
	//������
	if(ptClient->iSockFd > 0)
	{
		//ɾ��һ��socket
		SockBase_Delete(ptClient->iSockFd);
	}
	ptClient->iSockFd = -1;
	
	return  0;
}

//�ر�һ��TCP socket�ͻ���
int  SockUser_CloseTcpClient(T_SuTcpClient *ptClient)
{
	//������
	if(ptClient->iSockFd > 0)
	{
		//ɾ��һ��socket
		SockBase_Delete(ptClient->iSockFd);
	}
	ptClient->iSockFd = -1;
	ptClient->iConnected = 0;
	
	return  0;
}

//���socket���¼�
//����-1��ʾ��������0��ʾ��ʱ������1��ʾsocket�ɶ�
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

//���socketд�¼�
//����-1��ʾ��������0��ʾ��ʱ������1��ʾsocket��д
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

//TCP socket����һ������
int  SockUser_TcpAccept(int iSrvSockfd,T_SuTcpClient *ptClient)
{
	int  iConnectfd;
	struct sockaddr_in tClientAddr;

	//TCP socket����һ������
	if (SockBase_TcpAccept(iSrvSockfd,&iConnectfd,&tClientAddr) < 0)
	{
		//printf("accept error\n");
		return -1;;
	}

	memset(ptClient, 0, sizeof(T_SuTcpClient));
	ptClient->iSockFd = iConnectfd;
	ptClient->iConnected = 1;
	//������ʽ1���޸���ʽ1�Ķ˿ں�Ϊ�Ƿ�
	ptClient->iSrvPort = -1;
	ptClient->tSrvIp[0] = '\0';
	//��ʽ2���޸���ʽ2�Ķ˿ں�
	ptClient->iCliPort = ntohs(tClientAddr.sin_port);
	strcpy(ptClient->tCliIp, SockBase_ntoa(tClientAddr.sin_addr.s_addr));

	memcpy(&ptClient->stCliAddr, &tClientAddr, sizeof(struct sockaddr_in));

	return  0;
}


//UDP socket����ָ����С�����ݣ�ֱ��������ɻ�ʱ�ŷ���
//�����ظ��������򷵻�ʵ�ʽ��յĳ��ȣ���ʱʱʵ�ʽ��յĳ�������󳤶Ȳ�һ��
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
		//UDP socket��ȡ���ݣ�select��ʽ����ʱ����
		//�����ظ�������ʱ����0�����򷵻�ʵ�ʽ��յĳ���
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
		
		//���ζ���ret�ֽ����ݣ��������¶��������ܴ�С
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

//TCP socket����ָ����С�����ݣ�ֱ��������ɻ�ʱ�ŷ���
//�����ظ��������򷵻�ʵ�ʽ��յĳ��ȣ���ʱʱʵ�ʽ��յĳ�������󳤶Ȳ�һ��
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
		//TCP socket��ȡ���ݣ�select��ʽ����ʱ����
		//�����ظ�������ʱ����0�����򷵻�ʵ�ʽ��յĳ���
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
		
		//���ζ���ret�ֽ����ݣ��������¶��������ܴ�С
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

//TCP socket����ָ����С�����ݣ�ֱ��������ɻ�ʱ�ŷ���
//�����ظ��������򷵻�ʵ�ʷ��͵ĳ��ȣ���ʱʱʵ�ʷ��͵ĳ�������󳤶Ȳ�һ��
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
		//TCP socket����ָ����С������
		//�����ظ�������ʱ����0�����򷵻�ʵ�ʷ��͵ĳ���
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
		
		//����ret�ֽ����ݣ��������¶��������ܴ�С
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
//UDP socket����ָ����С������
//�����ظ�������������ʵ�ʷ��͵Ĵ�С
int  SockBase_UdpSend(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iBufSize)
{

}

//TCP socket����ָ����С������
//�����ظ�������������ʵ�ʷ��͵Ĵ�С
int  SockBase_TcpSend(int sockfd,char *pBuffer,int iBufSize)
{

}

//TCP socket����ָ����С������
//�����ظ�������ʱ����0�����򷵻�ʵ�ʷ��͵ĳ���
int  SockBase_TcpSendEx(int sockfd,char *pBuffer,int iBufSize,int iTimeoutMs)
{

}

#endif

