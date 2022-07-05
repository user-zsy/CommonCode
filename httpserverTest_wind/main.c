#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
 #include <winsock2.h>
#include <windows.h>
#include <time.h> //what time is it now
#include "SockBase.h"
#include "SockUser.h"
#include "cJSON.h"
//#include "longtime.h"
#include <time.h>
#include "base64.h"
#define HTTP_MAX_CLIENTNUM (5)
#define HTTP_SERVER_PORT (8080)
#define HTTP_MAX_DATABUFSIZE (10*1024*1024) //10M
#define HTTP_DATA_PATH "POST /test_data_path"// 这里的path实际为/test_data_path
#include "char_conversion.h"
#define HTTP_HDR_JSON \
"HTTP/1.1 %s\r\n\
Access-Control-Allow-Origin: *\r\n\
Connection: close\r\n\
Content-Type: application/json\r\n\
Content-Length: %d\r\n\r\n%s"
#define snprintf _snprintf
typedef struct tagT_HttpServerInfo
{
	T_SuTcpServer 	tTcpServer;     //TCP服务器
	T_SuTcpClient 	tTcpClient[HTTP_MAX_CLIENTNUM];     //客户端连接
}T_HttpServerInfo;

static int save_img(char *src, char *file_name, int img_size)
{
	int ret;
	FILE *fp=fopen(file_name, "wb");
	if(!fp)
	{
		return -1;
	}	
	ret = fwrite(src, 1, img_size, fp);
	fclose(fp);
	fp = NULL;//20210419
	return ret;
}
double what_time_is_it_now()
{
#ifdef linux
    struct timeval time;
	
    if (gettimeofday(&time,NULL)){
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
#else
	SYSTEMTIME timeNow;
	GetLocalTime(&timeNow);
	return (double)timeNow.wSecond+(double)timeNow.wMilliseconds*.001;

#endif
}


void HttptestProtocolMode1Deal(int iSockfd, char *pcBody)
{
	cJSON *json = NULL;
	cJSON *jsonData = NULL;
	cJSON *jsonCarObj = NULL;
	cJSON *jsonPicObj = NULL;
	cJSON *jsonpicArry = NULL;
	cJSON *jsonItem = NULL;
	cJSON *jsonTmpItem = NULL;
	char *pcCarObjStr = NULL;
	char acCmdStr[64] = {0};
	char acDeviceId[64] = {0};
	char acSn[64] = {0};
	int iTimestamp = 0;
	int iDeviceType = -1;
	int arraySize = 0;
	int iIndex = 0;

	if(NULL == pcBody)
	{
		return;
	}
	json = cJSON_Parse((const char *)pcBody);
	if(NULL == json)
	{
		printf("[%s][%d] cJSON_Parse fail iSockfd(%d) error.\n",__FUNCTION__, __LINE__, iSockfd);
		return;
	}
	jsonItem = cJSON_GetObjectItem(json, "cmd");
	if(NULL != jsonItem)
	{
		if((cJSON_String == jsonItem->type) && (strlen(jsonItem->valuestring) < sizeof(acCmdStr)))
		{
			snprintf(acCmdStr, sizeof(acCmdStr), "%s", jsonItem->valuestring);
		}
	}
	printf("[%s][%d] cmd:%s\n",__FUNCTION__, __LINE__, acCmdStr);
	
	jsonItem = cJSON_GetObjectItem(json, "deviceId");
	if(NULL != jsonItem)
	{
		if((cJSON_String == jsonItem->type) && (strlen(jsonItem->valuestring) < sizeof(acDeviceId)))
		{
			snprintf(acDeviceId, sizeof(acDeviceId), "%s", jsonItem->valuestring);
		}
	}
	printf("[%s][%d] deviceId:%s\n",__FUNCTION__, __LINE__, acDeviceId);
	
	jsonItem = cJSON_GetObjectItem(json, "sn");
	if(NULL != jsonItem)
	{
		if((cJSON_String == jsonItem->type) && (strlen(jsonItem->valuestring) < sizeof(acSn)))
		{
			snprintf(acSn, sizeof(acSn), "%s", jsonItem->valuestring);
		}
	}
	printf("[%s][%d] sn:%s\n",__FUNCTION__, __LINE__, acSn);

	jsonItem = cJSON_GetObjectItem(json, "deviceType");
	if(NULL != jsonItem && cJSON_Number == jsonItem->type)
	{
		iDeviceType = jsonItem->valueint;
	}
	printf("[%s][%d] deviceType:%d\n",__FUNCTION__, __LINE__, iDeviceType);
	
	jsonItem = cJSON_GetObjectItem(json, "timestamp");
	if(NULL != jsonItem && cJSON_Number == jsonItem->type)
	{
		iTimestamp = jsonItem->valueint;
	}
	printf("[%s][%d] timestamp:%d\n",__FUNCTION__, __LINE__, iTimestamp);
	jsonData = cJSON_GetObjectItem(json, "body");
	if((NULL != jsonData) && (cJSON_Object == jsonData->type))
	{
		//如果是过路车
		if(0 == strcmp(acCmdStr, "passingCarSnap"))
		{
			jsonCarObj = cJSON_GetObjectItem(jsonData, "passingCarInfo");
			if((NULL != jsonCarObj) && (cJSON_Object == jsonCarObj->type))
			{
				pcCarObjStr = cJSON_PrintUnformatted(jsonCarObj);
				printf("[%s][%d] passingCarInfo:\n%s\n",__FUNCTION__, __LINE__, pcCarObjStr);
			}
						
			jsonPicObj = cJSON_GetObjectItem(jsonData, "picData");
			if((NULL != jsonPicObj) && (cJSON_Object == jsonPicObj->type))
			{
				jsonItem = cJSON_GetObjectItem(jsonPicObj, "picNum");
				if(NULL != jsonItem && cJSON_Number == jsonItem->type)
				{
					printf("[%s][%d] picNum:%d\n",__FUNCTION__, __LINE__, jsonItem->valueint);
				}
				jsonpicArry = cJSON_GetObjectItem(jsonPicObj, "picArry");
				if(NULL != jsonpicArry && cJSON_Array == jsonpicArry->type)
				{
					arraySize = cJSON_GetArraySize(jsonpicArry);
					printf("[%s][%d] arraySize:%d\n",__FUNCTION__, __LINE__, arraySize);
					for(iIndex = 0; iIndex < arraySize; iIndex++)
					{
						jsonTmpItem = cJSON_GetArrayItem(jsonpicArry, iIndex);
						jsonItem = cJSON_GetObjectItem(jsonTmpItem, "picType");
						if(NULL != jsonItem && cJSON_Number == jsonItem->type)
						{
							printf("[%s][%d] picType:%d\n",__FUNCTION__, __LINE__, jsonItem->valueint);
						}
						jsonItem = cJSON_GetObjectItem(jsonTmpItem, "picFileLen");
					
						if(NULL != jsonItem && cJSON_Number == jsonItem->type)
						{
							printf("[%s][%d] picFileLen:%d\n",__FUNCTION__, __LINE__, jsonItem->valueint);
						}
					//picFile 为图片base64成员，解析此成员然后base64解码
					}
				}
			}
		}
		else if(0 == strcmp(acCmdStr, "cycleUpload"))
		{		
			jsonCarObj = cJSON_GetObjectItem(jsonData, "picData");
			if((jsonCarObj)&&(jsonItem->type==cJSON_Object));
			{
              cJSON* picNUM_upload=cJSON_GetObjectItem(jsonCarObj,"picNum");
			  cJSON* picArry_uploadx=NULL;
			  cJSON* picdata=NULL;
			  if((picNUM_upload)&&(picNUM_upload->type==cJSON_Number))
			  {
				  printf("[%s][%d] picNum:%d\n",__FUNCTION__, __LINE__, picNUM_upload->valueint);
			  }
			 // cJSON* picArry_upload=cJSON_GetObjectItem(jsonCarObj,"picArry");
			   
				 picArry_uploadx = cJSON_GetObjectItem(jsonCarObj, "picArry");
				if(NULL != picArry_uploadx && cJSON_Array == picArry_uploadx->type)
				{
					int arraySize_upload = cJSON_GetArraySize(picArry_uploadx);
					int picsize;
					char* picbuf;
					printf("[%s][%d] arraySize:%d\n",__FUNCTION__, __LINE__, arraySize_upload);
					for(iIndex = 0; iIndex < arraySize_upload; iIndex++)
					{
						jsonTmpItem = cJSON_GetArrayItem(picArry_uploadx, iIndex);
						jsonItem = cJSON_GetObjectItem(jsonTmpItem , "picType");

						if(NULL != jsonItem && cJSON_Number == jsonItem->type)
						{
							printf("[%s][%d] picType:%d\n",__FUNCTION__, __LINE__, jsonItem->valueint);
						}
						jsonItem = cJSON_GetObjectItem(jsonTmpItem , "picFileLen");
						picsize=jsonItem->valueint;
						if(NULL != jsonItem && cJSON_Number == jsonItem->type)
						{
							printf("[%s][%d] picFileLen:%d\n",__FUNCTION__, __LINE__, jsonItem->valueint);
						}
						jsonItem = cJSON_GetObjectItem(jsonTmpItem , "picFile");
						picbuf=(char*)malloc(picsize);
						picsize=base64_decode(jsonItem->valuestring,picsize,(unsigned char*)picbuf);
						printf("[%s][%d] picsize:%d\n",__FUNCTION__, __LINE__, picsize);
						save_img(picbuf,"1.jpg",picsize);
					//picFile 为图片base64成员，解析此成员然后base64解码
					}
				}

#if 0
				pcCarObjStr = cJSON_PrintUnformatted(jsonCarObj);
				printf("[%s][%d] cycleUpload:\n%s\n",__FUNCTION__, __LINE__, pcCarObjStr);
#endif
			}

		}
	}
	if(NULL != pcCarObjStr)
	{
		free(pcCarObjStr);
		pcCarObjStr = NULL;
	}
	if(NULL != json)
	{
		cJSON_DeleteO(json);
	}

}

void HttpSrvRsp200Msg(int iSockfd, int iSuc, char *pcMsg)
{
	char acJson[1024] = {0};
	char acSendBuf[1536] = {0};
	if(NULL == pcMsg)
	{
		return;
	}
	//json格式包体
	//sprintf(acJson, "{\"msg\":\"%s\",\"result\":1,\"success\":%s}", pcMsg, iSuc? "true":"false");

	//anchel add
	sprintf(acJson, "{\"cmd\":\"passingCarSnap\",\"deviceId\":\"123456789001\",\"deviceType\":\"0\",%s",
	"\"sn\":\"123456789023\",\"timestamp\":\"12345144\",\"response\":1,\"reson\":\"faile\"}");
	
	//http协议应答
	sprintf(acSendBuf, HTTP_HDR_JSON, "200 OK", (int)strlen(acJson), acJson);
	printf("acJson len:%d, acSendBuf len:%d\n", strlen(acJson), strlen(acSendBuf));

	SockUser_TcpSendUntilEndOrTimeout(iSockfd, acSendBuf, strlen(acSendBuf), 1000);

	//断开连接
#ifdef linux
	usleep(100*1000);
#else
	Sleep(100);
#endif
}

void HttpSrvRsp404Msg(int iSockfd, char *pcMsg)
{
	char acJson[128] = {0};
	char acSendBuf[256] = {0};
	
	//json格式包体
	sprintf(acJson, "{\"msg\":\"%s\",\"result\":0}", pcMsg);
	//http协议应答
	sprintf(acSendBuf, HTTP_HDR_JSON, "404 NOT FOUND", (int)strlen(acJson), acJson);
	SockUser_TcpSendUntilEndOrTimeout(iSockfd, acSendBuf, strlen(acSendBuf), 1000);

	//断开连接
	#ifdef linux
	usleep(100*1000);
    #else
	Sleep(100);
	#endif
}

/*-****************************************************************** 
CommHttpSrvProtoAux_RecvHttpHead:接收判断http头信息，并将有效数据存入pDstBuf.
-------------------------- 
参数iSockFd[in]:socket号
参数pDstBuf[inout]:待存有效数据的buf
参数piDstLen[inout]:有效数据的长度
参数piTotalLen[inout]:包体的总长度
-------------------------- 
返回 0：成功
返回 -1：失败

*******************************************************************-*/ 
int HttpSrv_RecvHttpHead(int iSockFd, char *pDstBuf, int *piDstLen, int *piTotalLen)
{
	int iRcvLen = 0;
	int iContentLen = 0;
	char *pcChr = NULL;
	char *pcStr = NULL;
	char *pcBody = NULL;
	//char acHttpBuf[1024] = {0};//anchel comment
	char acHttpBuf[1024*4] = {0};//anchel add 

	iRcvLen = SockBase_TcpReadEx(iSockFd, acHttpBuf, sizeof(acHttpBuf), 1000);
	if(iRcvLen <= 0 )
	{
		printf("[%s][%d] sockfd(%d) error errno %d iRcvLen %d\n",__FUNCTION__, __LINE__, iSockFd, errno, iRcvLen);
		return -1;
	}

	if(0 != strncmp(acHttpBuf, HTTP_DATA_PATH, strlen(HTTP_DATA_PATH)))
	{
		printf("[%s][%d] tHttpBuf(%s) PATH error.\n",__FUNCTION__, __LINE__, acHttpBuf);
		return -1;
	}


	//定位包体
	pcBody = strstr(acHttpBuf, "\r\n\r\n");
	if(NULL == pcBody)
	{
		return -1;
	}
	*pcBody = '\0';

	//解析包头	
	pcStr = strtok((char*)acHttpBuf, "\r\n");
    while(pcStr != NULL)
    {
        //解析参数
        pcChr = strchr(pcStr, ':');
        if (NULL == pcChr)
        {
            pcStr = strtok(NULL, "\r\n");
            continue;
        }

        *pcChr++ = '\0';
        if(0 == strncmp(pcStr, "Content-Type", strlen("Content-Type")))
        {
        	//校验类型 不是json 本服务无法解析
        	if(NULL == strstr(pcChr, "application/json"))
			{
        		printf("[%s][%d] not application/json acHttpBuf %s\n",__FUNCTION__, __LINE__, acHttpBuf);
        		return -1;
        	}
        }
#ifdef linux
        else if(0 == strncasecmp(pcStr, "Content-Length", strlen("Content-Length")))
		{
        	iContentLen = atoi(pcChr);
		}
#else
		else if(0 == stricmp(pcStr, "Content-Length"))
		{
        	iContentLen = atoi(pcChr);
		}

#endif
        pcStr = strtok(NULL, "\r\n");
    }

	if(0 == iContentLen)
	{
		printf("[%s][%d] content_length %d error \n",__FUNCTION__, __LINE__, iContentLen);
		return -1;	//包头解析失败 未获取长度
	}

	//获取包体部分数据
	pcBody += 4;//跳过/r/n/r/n

	*piDstLen = iRcvLen - (pcBody - acHttpBuf);
	memcpy(pDstBuf, pcBody, *piDstLen);
	*piTotalLen = iContentLen;

	printf("[%s][%d] curRecvLen %d total file len %d \n",__FUNCTION__, __LINE__, *piDstLen, *piTotalLen);
	return 0;
}

void *HttpRequestDealTask(void *pParam)
{
	T_SuTcpClient *ptTcpClient = NULL;
	int iRet = 0;
	int iRemain = 0;
	int iTotalLen = 0;
	int iCurLen = 0;
	int iRcvLen = 0;
	int NeedRcvLen = 0;
	int iIndex = 0;
	char acRecvBuf[4096] = {0};
	char *pcHttpDataBuf = NULL;
	
	// double start_time = 0.0, stop_time = 0.0;
	// start_time = what_time_is_it_now();
	// printf("[%s]new thread start %lf\n",TIMESTAMP0(), start_time);
	//pthread_detach(pthread_self());
	if(NULL == pParam)
	{
		printf("[%s][%d] input param is null. pthread exit \n",__FUNCTION__, __LINE__);
		return NULL;
	}
	ptTcpClient = (T_SuTcpClient *)pParam;
	printf("[%s][%d] pthread run %s:%d\n", __FUNCTION__, __LINE__, 
		inet_ntoa(ptTcpClient->stCliAddr.sin_addr), 
		ntohs(ptTcpClient->stCliAddr.sin_port));

	NeedRcvLen = sizeof(acRecvBuf);
	pcHttpDataBuf = (char *)malloc(1*HTTP_MAX_DATABUFSIZE);
	if(NULL == pcHttpDataBuf)
	{
		printf("[%s][%d]port malloc %dB error.\n",__FUNCTION__,__LINE__, HTTP_MAX_DATABUFSIZE);
		goto ClientExit;
	}
	iRemain = 0;
	iCurLen = 0;

	//接收http头 并将有效数据存入目标tRecvBuf.
	iRet = HttpSrv_RecvHttpHead(ptTcpClient->iSockFd, acRecvBuf, &iRemain, &iTotalLen);
	if(iRet < 0)
	{
		HttpSrvRsp404Msg(ptTcpClient->iSockFd, "test data");
		goto ClientExit;	
	}
	if(iTotalLen > HTTP_MAX_DATABUFSIZE)
	{
		HttpSrvRsp404Msg(ptTcpClient->iSockFd, "test data");//此处应该回复具体错误原因,测试代码简单回复
		printf("[%s][%d]  The received data is too large. error.\n", __FUNCTION__, __LINE__);
		goto ClientExit;
	}

	while(1)
	{
		//TCP socket读取数据，select方式，超时返回
		//出错返回负数，超时返回0，否则返回实际接收的长度
		iRcvLen = SockBase_TcpReadEx(ptTcpClient->iSockFd, &acRecvBuf[iRemain], NeedRcvLen - iRemain, 1000);
		if(iRcvLen < 0)
		{
			printf("[%s][%d] sockfd(%d) error errno %d iRcvLen %d\n", __FUNCTION__, __LINE__, 
				ptTcpClient->iSockFd, errno, iRcvLen);
			break;
		}
		else if(iRcvLen == 0)
		{
			//printf("[%s][%d] sockfd(%d) timeout.\n",__FUNCTION__, __LINE__, ptTcpClient->iSockFd);
			//break;
		}

		//数据量不足
		iRemain += iRcvLen;
		if(iRemain < NeedRcvLen && iRemain < iTotalLen - iCurLen)
		{
			continue;
		}
		
		//数据接收完毕
		if((iCurLen + iRemain) >= iTotalLen)
		{
			//拷贝剩余数据
			memcpy(pcHttpDataBuf + iCurLen, acRecvBuf, iRemain);
			iCurLen += iRemain;
			//执行http 包体解析处理 按照具体协议去解析处理，本样例是json数据
			HttptestProtocolMode1Deal(ptTcpClient->iSockFd, pcHttpDataBuf);
			//pcHttpDataBuf[800] = '\0';//printf("pcHttpDataBuf:\n%s\n", pcHttpDataBuf);
			HttpSrvRsp200Msg(ptTcpClient->iSockFd, 1, "test data");
			break;
		}
		else
		{
			//拷贝数据
			memcpy(pcHttpDataBuf + iCurLen, acRecvBuf, iRemain);
			iCurLen += iRemain;
		}
		//数据不够继续接收,清零操作
		iRemain = 0;
	}
	// stop_time = what_time_is_it_now();
	// printf("new thread cost: %lfms\n", stop_time-start_time);
	// printf("[%s]new thread exit\n",TIMESTAMP0());

ClientExit:
	if(NULL != pcHttpDataBuf)
	{
		free(pcHttpDataBuf);
		pcHttpDataBuf = NULL;
	}
	printf("[%s][%d] pthread exit %s:%d\n\n", __FUNCTION__, __LINE__,
				inet_ntoa(ptTcpClient->stCliAddr.sin_addr), 
				ntohs(ptTcpClient->stCliAddr.sin_port));
	SockUser_CloseTcpClient(ptTcpClient);

	return NULL;
}


int main (void)
{
	DWORD threadId;		//客户端线程id
	int iRet = 0;
	int iIndex = 0;
	T_HttpServerInfo tHttpServerInfo;   //服务器信息
	T_SuTcpClient tNewTcpClient = {0};     //客户端连接
	winsock_startup();
	memset(&tHttpServerInfo, 0x0, sizeof(T_HttpServerInfo));
	//创建tcp服务器
	iRet = SockUser_OpenTcpServer(&tHttpServerInfo.tTcpServer, HTTP_SERVER_PORT, HTTP_MAX_CLIENTNUM);
	if(iRet != 0)
	{
		printf("[%s][%d]port %d tcp server create error.\n",__FUNCTION__,__LINE__, HTTP_SERVER_PORT);
		return  -1;
	}
	
	while(1)
	{
		//检测socket读事件
		//返回-1表示出错，返回0表示超时，返回1表示socket可读
		iRet = SockUser_CheckReadEvent(tHttpServerInfo.tTcpServer.iSockFd, 2000);
		if(iRet < 0)
		{
			printf("[%s][%d]  server error.\n",__FUNCTION__,__LINE__);
			break;
		}
		else if(iRet == 0)
		{
			//printf("[%s][%d]  server timeout.\n",__FUNCTION__,__LINE__);
			continue;
		}

		//TCP socket接收一个连接
		iRet = SockUser_TcpAccept(tHttpServerInfo.tTcpServer.iSockFd, &tNewTcpClient);
		if(iRet < 0)
		{
			printf("[%s][%d]  server accept error.\n", __FUNCTION__, __LINE__);
			continue;
		}
		//查找数组寻找可用的客户端buf
		for(iIndex = 0; iIndex < HTTP_MAX_CLIENTNUM; iIndex++)
		{
			if(0 >= tHttpServerInfo.tTcpClient[iIndex].iSockFd 
				&& 0 == tHttpServerInfo.tTcpClient[iIndex].iConnected)
			{
				printf("[%s][%d]  new Client[%d] is found. success\n", __FUNCTION__, __LINE__, iIndex);
				break;//找到可用客户端buf
			}
		}
		//超过设定的最大连接数
		if(HTTP_MAX_CLIENTNUM <= iIndex)
		{
			printf("Accept Connection reject connect %s:%d\n", 
				inet_ntoa(tNewTcpClient.stCliAddr.sin_addr), 
				ntohs(tNewTcpClient.stCliAddr.sin_port));
			SockUser_CloseTcpClient(&tNewTcpClient);
			continue;
		}
		memcpy((void *)(&tHttpServerInfo.tTcpClient[iIndex]), &tNewTcpClient, sizeof(T_SuTcpClient));
		//创建一个客户线程
		if( CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HttpRequestDealTask, (void *)(&tHttpServerInfo.tTcpClient[iIndex]) ,0,&threadId)==NULL)
		{
			printf("[%s][%d]  pthread_create iIndex(%d) error.\n", __FUNCTION__, __LINE__, iIndex);
			SockUser_CloseTcpClient(&tHttpServerInfo.tTcpClient[iIndex]);
		}
	}

	iRet = SockUser_CloseTcpServer(&tHttpServerInfo.tTcpServer);
	if(iRet != 0)
	{
		printf("[%s][%d] tcp server close error.\n",__FUNCTION__,__LINE__);
		return -1;
	}
	winsock_cleanup();
	return 0;
}