/*===========================
-- SOCKET用户库头文件
-- 谢福文Kevin
-- <kevin_linux@163.com>
-- v1.00
-- 2018年11月15日
-- 在socket基础库上封装用户使用库
=============================*/
#ifndef  __SOCKUSER_H__
#define  __SOCKUSER_H__

#include "SockBase.h"


//TCP服务器结构
typedef struct
{
	int  iSockFd;        /* socket描述字 */
	int  iLocalPort;     /* 本地端口号 */
	struct sockaddr_in stLocalAddr;  //服务器的本地socket地址
} T_SuTcpServer;

#if 1
//TCP客户端连接结构
typedef struct
{
	int  iSockFd;        /* socket描述字 */
	int  iConnected;     /* 是否已连接 */
	//-------------------
	//形式1:TCP创建时就以客户端形式创建
	int  iSrvPort;       /* 服务器端口号，<=0说明不是这形式的 */
	char tSrvIp[32];     /* 服务器IP地址 */
	struct sockaddr_in stSrvAddr;  //形式1服务器地址
	//-------------------
	//形式2:TCP服务器被连接后生成的TCP客户端
	int  iCliPort;       /* 客户端端口号，<=0说明不是这形式的 */
	char tCliIp[32];     /* 客户端IP地址 */
	struct sockaddr_in stCliAddr;  //形式2客户端地址
	//-------------------
} T_SuTcpClient;
#else
//TCP客户端类型
typedef enum{
	E_TcpCliType_Create = 0,  //形式1:TCP创建时就以客户端形式创建
	E_TcpCliType_Accept  //形式2:TCP服务器被连接后生成的TCP客户端
}E_TcpCliType;
//TCP客户端连接结构
typedef struct
{
	int  iSockFd;        /* socket描述字 */
	int  iConnected;     /* 是否已连接 */
	//int  iClientType;
	E_TcpCliType eClientType;
	union{
		struct{
			//-------------------
			//形式1:TCP创建时就以客户端形式创建
			int  iSrvPort;       /* 服务器端口号，<=0说明不是这形式的 */
			char tSrvIp[32];     /* 服务器IP地址 */
			struct sockaddr_in stSrvAddr;  //形式1服务器地址
		}CreateTypeInfo;
		struct{
			//-------------------
			//形式2:TCP服务器被连接后生成的TCP客户端
			int  iCliPort;       /* 客户端端口号，<=0说明不是这形式的 */
			char tCliIp[32];     /* 客户端IP地址 */
			struct sockaddr_in stCliAddr;  //形式2客户端地址
			//-------------------
		}AcceptTypeInfo;
	}u;
} T_SuTcpClient;
#endif

//UDP客户端连接结构
typedef struct
{
	int  iSockFd;        /* socket描述字 */
	int  iGroupEn;       /* 组播使能，则是组播UDP服务器，不使能，则是普通UDP服务器 */
	int  iLocalPort;     /* 本地端口号,组播使能时为组播端口号 */
	char tGroupIp[32];   /* 组播地址,(组播使能时) */
	struct sockaddr_in stLocalAddr;  //服务器的本地socket地址
} T_SuUdpServer;

//UDP客户端连接结构
typedef struct
{
	int  iSockFd;        /* socket描述字 */
	int  iSrvPort;       /* 端口号 */
	char tSrvIp[32];     /* IP地址 */
	struct sockaddr_in stSrvAddr;
} T_SuUdpClient;


//-------------------------------------------------------------------
#ifndef linux

int winsock_startup( void );
int winsock_cleanup( void );
#endif
//服务器类
//------------------------------
//打开一个UDP socket服务器
int  SockUser_OpenUdpServer(T_SuUdpServer *ptServer, int iLocalPort);
//打开一个组播 socket服务器
int  SockUser_OpenGroupServer(T_SuUdpServer *ptServer, int iGroupPort, char *pGroupIp, int iLoopEn);
//打开一个TCP socket服务器
int  SockUser_OpenTcpServer(T_SuTcpServer *ptServer, int iLocalPort,int iMaxConnect);
//关闭一个UDP socket服务器
int  SockUser_CloseUdpServer(T_SuUdpServer *ptServer);
//关闭一个TCP socket服务器
int  SockUser_CloseTcpServer(T_SuTcpServer *ptServer);

//-------------------------------------------------------------------
//客户端类
//------------------------------
//打开一个UDP socket客户端
int  SockUser_OpenUdpClient(T_SuUdpClient *ptClient, char *pServerIp, int iServerPort);
//打开一个TCP socket客户端
int  SockUser_OpenTcpClient(T_SuTcpClient *ptClient, char *pServerIp, int iServerPort, int iTimeoutMs);
//关闭一个UDP socket客户端
int  SockUser_CloseUdpClient(T_SuUdpClient *ptClient);
//关闭一个TCP socket客户端
int  SockUser_CloseTcpClient(T_SuTcpClient *ptClient);
//-------------------------------------------------------------------

//检测socket读事件
//返回-1表示出错，返回0表示超时，返回1表示socket可读
int SockUser_CheckReadEvent(int iSockfd, int iTimeoutMs);
//检测socket写事件
//返回-1表示出错，返回0表示超时，返回1表示socket可写
int SockUser_CheckWriteEvent(int iSockfd, int iTimeoutMs);

//TCP socket接收一个连接
int  SockUser_TcpAccept(int iSrvSockfd,T_SuTcpClient *ptClient);

//UDP socket接收指定大小的数据，直到接收完成或超时才返回
//出错返回负数，否则返回实际接收的长度，超时时实际接收的长度与最大长度不一样
int  SockUser_UdpReadUntilFullOrTimeout(int iSockfd,char *pDataBuf,int iMaxReadSize,int iTimeoutMs);
//TCP socket接收指定大小的数据，直到接收完成或超时才返回
//出错返回负数，否则返回实际接收的长度，超时时实际接收的长度与最大长度不一样
int  SockUser_TcpReadUntilFullOrTimeout(int iSockfd,char *pDataBuf,int iMaxReadSize,int iTimeoutMs);

//TCP socket发送指定大小的数据，直到发送完成或超时才返回
//出错返回负数，否则返回实际发送的长度，超时时实际发送的长度与最大长度不一样
int  SockUser_TcpSendUntilEndOrTimeout(int iSockfd,char *pDataBuf,int iMaxSendSize,int iTimeoutMs);



#endif

