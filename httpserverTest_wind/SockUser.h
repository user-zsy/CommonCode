/*===========================
-- SOCKET�û���ͷ�ļ�
-- л����Kevin
-- <kevin_linux@163.com>
-- v1.00
-- 2018��11��15��
-- ��socket�������Ϸ�װ�û�ʹ�ÿ�
=============================*/
#ifndef  __SOCKUSER_H__
#define  __SOCKUSER_H__

#include "SockBase.h"


//TCP�������ṹ
typedef struct
{
	int  iSockFd;        /* socket������ */
	int  iLocalPort;     /* ���ض˿ں� */
	struct sockaddr_in stLocalAddr;  //�������ı���socket��ַ
} T_SuTcpServer;

#if 1
//TCP�ͻ������ӽṹ
typedef struct
{
	int  iSockFd;        /* socket������ */
	int  iConnected;     /* �Ƿ������� */
	//-------------------
	//��ʽ1:TCP����ʱ���Կͻ�����ʽ����
	int  iSrvPort;       /* �������˿ںţ�<=0˵����������ʽ�� */
	char tSrvIp[32];     /* ������IP��ַ */
	struct sockaddr_in stSrvAddr;  //��ʽ1��������ַ
	//-------------------
	//��ʽ2:TCP�����������Ӻ����ɵ�TCP�ͻ���
	int  iCliPort;       /* �ͻ��˶˿ںţ�<=0˵����������ʽ�� */
	char tCliIp[32];     /* �ͻ���IP��ַ */
	struct sockaddr_in stCliAddr;  //��ʽ2�ͻ��˵�ַ
	//-------------------
} T_SuTcpClient;
#else
//TCP�ͻ�������
typedef enum{
	E_TcpCliType_Create = 0,  //��ʽ1:TCP����ʱ���Կͻ�����ʽ����
	E_TcpCliType_Accept  //��ʽ2:TCP�����������Ӻ����ɵ�TCP�ͻ���
}E_TcpCliType;
//TCP�ͻ������ӽṹ
typedef struct
{
	int  iSockFd;        /* socket������ */
	int  iConnected;     /* �Ƿ������� */
	//int  iClientType;
	E_TcpCliType eClientType;
	union{
		struct{
			//-------------------
			//��ʽ1:TCP����ʱ���Կͻ�����ʽ����
			int  iSrvPort;       /* �������˿ںţ�<=0˵����������ʽ�� */
			char tSrvIp[32];     /* ������IP��ַ */
			struct sockaddr_in stSrvAddr;  //��ʽ1��������ַ
		}CreateTypeInfo;
		struct{
			//-------------------
			//��ʽ2:TCP�����������Ӻ����ɵ�TCP�ͻ���
			int  iCliPort;       /* �ͻ��˶˿ںţ�<=0˵����������ʽ�� */
			char tCliIp[32];     /* �ͻ���IP��ַ */
			struct sockaddr_in stCliAddr;  //��ʽ2�ͻ��˵�ַ
			//-------------------
		}AcceptTypeInfo;
	}u;
} T_SuTcpClient;
#endif

//UDP�ͻ������ӽṹ
typedef struct
{
	int  iSockFd;        /* socket������ */
	int  iGroupEn;       /* �鲥ʹ�ܣ������鲥UDP����������ʹ�ܣ�������ͨUDP������ */
	int  iLocalPort;     /* ���ض˿ں�,�鲥ʹ��ʱΪ�鲥�˿ں� */
	char tGroupIp[32];   /* �鲥��ַ,(�鲥ʹ��ʱ) */
	struct sockaddr_in stLocalAddr;  //�������ı���socket��ַ
} T_SuUdpServer;

//UDP�ͻ������ӽṹ
typedef struct
{
	int  iSockFd;        /* socket������ */
	int  iSrvPort;       /* �˿ں� */
	char tSrvIp[32];     /* IP��ַ */
	struct sockaddr_in stSrvAddr;
} T_SuUdpClient;


//-------------------------------------------------------------------
#ifndef linux

int winsock_startup( void );
int winsock_cleanup( void );
#endif
//��������
//------------------------------
//��һ��UDP socket������
int  SockUser_OpenUdpServer(T_SuUdpServer *ptServer, int iLocalPort);
//��һ���鲥 socket������
int  SockUser_OpenGroupServer(T_SuUdpServer *ptServer, int iGroupPort, char *pGroupIp, int iLoopEn);
//��һ��TCP socket������
int  SockUser_OpenTcpServer(T_SuTcpServer *ptServer, int iLocalPort,int iMaxConnect);
//�ر�һ��UDP socket������
int  SockUser_CloseUdpServer(T_SuUdpServer *ptServer);
//�ر�һ��TCP socket������
int  SockUser_CloseTcpServer(T_SuTcpServer *ptServer);

//-------------------------------------------------------------------
//�ͻ�����
//------------------------------
//��һ��UDP socket�ͻ���
int  SockUser_OpenUdpClient(T_SuUdpClient *ptClient, char *pServerIp, int iServerPort);
//��һ��TCP socket�ͻ���
int  SockUser_OpenTcpClient(T_SuTcpClient *ptClient, char *pServerIp, int iServerPort, int iTimeoutMs);
//�ر�һ��UDP socket�ͻ���
int  SockUser_CloseUdpClient(T_SuUdpClient *ptClient);
//�ر�һ��TCP socket�ͻ���
int  SockUser_CloseTcpClient(T_SuTcpClient *ptClient);
//-------------------------------------------------------------------

//���socket���¼�
//����-1��ʾ��������0��ʾ��ʱ������1��ʾsocket�ɶ�
int SockUser_CheckReadEvent(int iSockfd, int iTimeoutMs);
//���socketд�¼�
//����-1��ʾ��������0��ʾ��ʱ������1��ʾsocket��д
int SockUser_CheckWriteEvent(int iSockfd, int iTimeoutMs);

//TCP socket����һ������
int  SockUser_TcpAccept(int iSrvSockfd,T_SuTcpClient *ptClient);

//UDP socket����ָ����С�����ݣ�ֱ��������ɻ�ʱ�ŷ���
//�����ظ��������򷵻�ʵ�ʽ��յĳ��ȣ���ʱʱʵ�ʽ��յĳ�������󳤶Ȳ�һ��
int  SockUser_UdpReadUntilFullOrTimeout(int iSockfd,char *pDataBuf,int iMaxReadSize,int iTimeoutMs);
//TCP socket����ָ����С�����ݣ�ֱ��������ɻ�ʱ�ŷ���
//�����ظ��������򷵻�ʵ�ʽ��յĳ��ȣ���ʱʱʵ�ʽ��յĳ�������󳤶Ȳ�һ��
int  SockUser_TcpReadUntilFullOrTimeout(int iSockfd,char *pDataBuf,int iMaxReadSize,int iTimeoutMs);

//TCP socket����ָ����С�����ݣ�ֱ��������ɻ�ʱ�ŷ���
//�����ظ��������򷵻�ʵ�ʷ��͵ĳ��ȣ���ʱʱʵ�ʷ��͵ĳ�������󳤶Ȳ�һ��
int  SockUser_TcpSendUntilEndOrTimeout(int iSockfd,char *pDataBuf,int iMaxSendSize,int iTimeoutMs);



#endif

