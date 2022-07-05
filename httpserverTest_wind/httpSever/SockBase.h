/*===========================
-- SOCKET基础库头文件
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
#ifndef  __SOCKBASE_H__
#define  __SOCKBASE_H__

#include <sys/types.h>
#include <winsock2.h>

//创建一个UDP方式的socket
//正常返回创建好的socket fd，出错返回负数
int  SockBase_UdpCreate(void);
//创建一个TCP方式的socket
//正常返回创建好的socket fd，出错返回负数
int  SockBase_TcpCreate(void);
//创建一个原始套接字socket

//删除一个socket
int  SockBase_Delete(int sockfd);


//转换IP字符串成二进制格式
int  SockBase_ConvertIP2Addr(char *pIpString,unsigned int *pHexAddr);
//转换本地字节序二进制IP地址成IP字符串
int  SockBase_ConvertAddr2IP(unsigned int uiHexAddr,char *pIpString);
//IP字符串转成本地字节序二进制IP地址
unsigned int  SockBase_aton(char *pIpString);
//本地字节序二进制IP地址转换成IP字符串
char  *SockBase_ntoa(unsigned int uiHexAddr);
//配置sockaddr结构体，以本地方式
//正常返回0，出错返回负数
int  SockBase_ConfigLocalAddr(struct sockaddr_in *ptSockAddr,int iPort);
//配置sockaddr结构体，以指定IP方式
//正常返回0，出错返回负数
int  SockBase_ConfigAddr(struct sockaddr_in *ptSockAddr,char *pIpString,int iPort);
//获取本地IP地址，以字符串方式存储
//正常返回0，出错返回负数
int  SockBase_GetLocalIp(char *ipbuf);

//加入一个组播组,指定具体ip加入一个组播组里
//正常返回0，出错返回负数
int SockBase_SetIpAddMembership(int sockfd, char *pIp, char *pMulticastIp);
//设置socket接收缓冲区大小
//正常返回0，出错返回负数
int  SockBase_SetRecvBufSize(int sockfd,int iSetSize);
//设置socket发送缓冲区大小
//正常返回0，出错返回负数
int  SockBase_SetSendBufSize(int sockfd,int iSetSize);
//获取socket接收缓冲区大小
//正常返回0，出错返回负数
int  SockBase_GetRecvBufSize(int sockfd,int *pGetSize);
//获取socket发送缓冲区大小
//正常返回0，出错返回负数
int  SockBase_GetSendBufSize(int sockfd,int *pGetSize);
//设置socket接收超时时长，单位毫秒
//正常返回0，出错返回负数
int  SockBase_SetRecvTimeout(int sockfd,int iMSec);
//设置socket发送超时时长，单位毫秒
//正常返回0，出错返回负数
int  SockBase_SetSendTimeout(int sockfd,int iMSec);
//设置TCP socket禁用Nagle算法的开关状态
//正常返回0，出错返回负数
int  SockBase_SetTcpNoDelay(int sockfd,int iEnable);
//设置socket绑定对应的网卡
//正常返回0，出错返回负数

//设置socket地址复用的开关状态
//正常返回0，出错返回负数
int  SockBase_SetReuseAddr(int sockfd,int iReuseEn);
//设置socket组播数据是否回环给自己
//正常返回0，出错返回负数
int  SockBase_SetGroupLoop(int sockfd,int iLoopEn);
//设置socket加入组播组
//正常返回0，出错返回负数
int  SockBase_SetJoinGroup(int sockfd, char *pMulticastIp);
//设置socket移出组播组
//正常返回0，出错返回负数
int  SockBase_SetRemoveGroup(int sockfd, char *pMulticastIp);
//socket绑定
//正常返回0，出错返回负数
int  SockBase_Bind(int sockfd,struct sockaddr_in *ptSockAddr);
//TCP socket监听
//正常返回0，出错返回负数
int  SockBase_TcpListen(int sockfd,int iMaxConnect);
//TCP socket接收一个连接
//正常返回0，出错返回负数
int  SockBase_TcpAccept(int sockfd,int *pConnectfd,struct sockaddr_in *ptClientAddr);
//TCP socket连接指定服务器
//正常返回0，出错返回负数
int  SockBase_TcpConnect(int sockfd,struct sockaddr_in *ptServerAddr);
//TCP socket连接指定服务器，非阻塞方式，超时自动返回
//出错返回负数，正常返回0
int  SockBase_TcpConnectEx(int sockfd,struct sockaddr_in *ptServerAddr,int timeoutMs);
//UDP socket发送指定大小的数据
//出错返回负数，正常返回实际发送的大小
int  SockBase_UdpSend(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iBufSize);
//TCP socket发送指定大小的数据
//出错返回负数，正常返回实际发送的大小
int  SockBase_TcpSend(int sockfd,char *pBuffer,int iBufSize);
//TCP socket发送指定大小的数据
//出错返回负数，超时返回0，否则返回实际发送的长度
int  SockBase_TcpSendEx(int sockfd,char *pBuffer,int iBufSize,int iTimeoutMs);
//UDP socket读取数据，block阻塞读取
//出错返回负数，正常返回实际接收的长度
int  SockBase_UdpRead(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iMaxBufSize);
//UDP socket读取数据，select方式，超时返回
//出错返回负数，超时返回0，否则返回实际接收的长度
int  SockBase_UdpReadEx(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iMaxBufSize,int iTimeoutMs);
//TCP socket读取数据，block阻塞读取
//出错返回负数，正常返回实际接收的长度
int  SockBase_TcpRead(int sockfd,char *pBuffer,int iMaxBufSize);
//TCP socket读取数据，select方式，超时返回
//出错返回负数，超时返回0，否则返回实际接收的长度
int  SockBase_TcpReadEx(int sockfd,char *pBuffer,int iMaxBufSize,int iTimeoutMs);

//获取域名对应的IP地址，以字符串方式存储
int  SockBase_GetDomainIp(char *pIpBuf, char *pcDomainStr);

#ifdef linux
//正常返回创建好的socket fd，出错返回负数
int  SockBase_RawCreate(void);
//设置socket为非阻塞方式
int  SockBase_SetNonBlock(int sockfd);
//设置socket为阻塞方式
int  SockBase_SetBlock(int sockfd);
int  SockBase_SetBind2Dev(int sockfd,char *pIfName);
#endif
#endif
