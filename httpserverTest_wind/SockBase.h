/*===========================
-- SOCKET������ͷ�ļ�
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
#ifndef  __SOCKBASE_H__
#define  __SOCKBASE_H__

#include <sys/types.h>
#include <winsock2.h>

//����һ��UDP��ʽ��socket
//�������ش����õ�socket fd�������ظ���
int  SockBase_UdpCreate(void);
//����һ��TCP��ʽ��socket
//�������ش����õ�socket fd�������ظ���
int  SockBase_TcpCreate(void);
//����һ��ԭʼ�׽���socket

//ɾ��һ��socket
int  SockBase_Delete(int sockfd);


//ת��IP�ַ����ɶ����Ƹ�ʽ
int  SockBase_ConvertIP2Addr(char *pIpString,unsigned int *pHexAddr);
//ת�������ֽ��������IP��ַ��IP�ַ���
int  SockBase_ConvertAddr2IP(unsigned int uiHexAddr,char *pIpString);
//IP�ַ���ת�ɱ����ֽ��������IP��ַ
unsigned int  SockBase_aton(char *pIpString);
//�����ֽ��������IP��ַת����IP�ַ���
char  *SockBase_ntoa(unsigned int uiHexAddr);
//����sockaddr�ṹ�壬�Ա��ط�ʽ
//��������0�������ظ���
int  SockBase_ConfigLocalAddr(struct sockaddr_in *ptSockAddr,int iPort);
//����sockaddr�ṹ�壬��ָ��IP��ʽ
//��������0�������ظ���
int  SockBase_ConfigAddr(struct sockaddr_in *ptSockAddr,char *pIpString,int iPort);
//��ȡ����IP��ַ�����ַ�����ʽ�洢
//��������0�������ظ���
int  SockBase_GetLocalIp(char *ipbuf);

//����һ���鲥��,ָ������ip����һ���鲥����
//��������0�������ظ���
int SockBase_SetIpAddMembership(int sockfd, char *pIp, char *pMulticastIp);
//����socket���ջ�������С
//��������0�������ظ���
int  SockBase_SetRecvBufSize(int sockfd,int iSetSize);
//����socket���ͻ�������С
//��������0�������ظ���
int  SockBase_SetSendBufSize(int sockfd,int iSetSize);
//��ȡsocket���ջ�������С
//��������0�������ظ���
int  SockBase_GetRecvBufSize(int sockfd,int *pGetSize);
//��ȡsocket���ͻ�������С
//��������0�������ظ���
int  SockBase_GetSendBufSize(int sockfd,int *pGetSize);
//����socket���ճ�ʱʱ������λ����
//��������0�������ظ���
int  SockBase_SetRecvTimeout(int sockfd,int iMSec);
//����socket���ͳ�ʱʱ������λ����
//��������0�������ظ���
int  SockBase_SetSendTimeout(int sockfd,int iMSec);
//����TCP socket����Nagle�㷨�Ŀ���״̬
//��������0�������ظ���
int  SockBase_SetTcpNoDelay(int sockfd,int iEnable);
//����socket�󶨶�Ӧ������
//��������0�������ظ���

//����socket��ַ���õĿ���״̬
//��������0�������ظ���
int  SockBase_SetReuseAddr(int sockfd,int iReuseEn);
//����socket�鲥�����Ƿ�ػ����Լ�
//��������0�������ظ���
int  SockBase_SetGroupLoop(int sockfd,int iLoopEn);
//����socket�����鲥��
//��������0�������ظ���
int  SockBase_SetJoinGroup(int sockfd, char *pMulticastIp);
//����socket�Ƴ��鲥��
//��������0�������ظ���
int  SockBase_SetRemoveGroup(int sockfd, char *pMulticastIp);
//socket��
//��������0�������ظ���
int  SockBase_Bind(int sockfd,struct sockaddr_in *ptSockAddr);
//TCP socket����
//��������0�������ظ���
int  SockBase_TcpListen(int sockfd,int iMaxConnect);
//TCP socket����һ������
//��������0�������ظ���
int  SockBase_TcpAccept(int sockfd,int *pConnectfd,struct sockaddr_in *ptClientAddr);
//TCP socket����ָ��������
//��������0�������ظ���
int  SockBase_TcpConnect(int sockfd,struct sockaddr_in *ptServerAddr);
//TCP socket����ָ������������������ʽ����ʱ�Զ�����
//�����ظ�������������0
int  SockBase_TcpConnectEx(int sockfd,struct sockaddr_in *ptServerAddr,int timeoutMs);
//UDP socket����ָ����С������
//�����ظ�������������ʵ�ʷ��͵Ĵ�С
int  SockBase_UdpSend(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iBufSize);
//TCP socket����ָ����С������
//�����ظ�������������ʵ�ʷ��͵Ĵ�С
int  SockBase_TcpSend(int sockfd,char *pBuffer,int iBufSize);
//TCP socket����ָ����С������
//�����ظ�������ʱ����0�����򷵻�ʵ�ʷ��͵ĳ���
int  SockBase_TcpSendEx(int sockfd,char *pBuffer,int iBufSize,int iTimeoutMs);
//UDP socket��ȡ���ݣ�block������ȡ
//�����ظ�������������ʵ�ʽ��յĳ���
int  SockBase_UdpRead(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iMaxBufSize);
//UDP socket��ȡ���ݣ�select��ʽ����ʱ����
//�����ظ�������ʱ����0�����򷵻�ʵ�ʽ��յĳ���
int  SockBase_UdpReadEx(int sockfd,struct sockaddr_in *ptSockAddr,char *pBuffer,int iMaxBufSize,int iTimeoutMs);
//TCP socket��ȡ���ݣ�block������ȡ
//�����ظ�������������ʵ�ʽ��յĳ���
int  SockBase_TcpRead(int sockfd,char *pBuffer,int iMaxBufSize);
//TCP socket��ȡ���ݣ�select��ʽ����ʱ����
//�����ظ�������ʱ����0�����򷵻�ʵ�ʽ��յĳ���
int  SockBase_TcpReadEx(int sockfd,char *pBuffer,int iMaxBufSize,int iTimeoutMs);

//��ȡ������Ӧ��IP��ַ�����ַ�����ʽ�洢
int  SockBase_GetDomainIp(char *pIpBuf, char *pcDomainStr);

#ifdef linux
//�������ش����õ�socket fd�������ظ���
int  SockBase_RawCreate(void);
//����socketΪ��������ʽ
int  SockBase_SetNonBlock(int sockfd);
//����socketΪ������ʽ
int  SockBase_SetBlock(int sockfd);
int  SockBase_SetBind2Dev(int sockfd,char *pIfName);
#endif
#endif
