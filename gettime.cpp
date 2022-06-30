
#if 0
//要包含这头文件<time.h>
//且在编译链接时需加上 -lrt ;因为在librt中实现了clock_gettime函数。
unsigned long GetTickCount()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);

}


typedef unsigned long  DWORD
#include <time.h>
DWORD GetTickCount(void)
{
    struct timeval tv;
    int ret=gettimeofday(&tv,NULL);
    if(ret==-1)
    {
        return 0;
    }
    return (DWORD)(tv.tvsec*1000+tv.tv.tv_usec/1000);
}

void getlocaltime(char* outtime)
{
    struct tm *pstm;
    struct timeval tv;
    gettimeofday(&tv);
    pstm=localtime(&tv.tv_sec);
    sprintf(puttime,sprintf(outtime,"%d-%02d-%02dT%02d:%02d:%02d:%03d",pstm->tm_year,pstm->tm_mon,pstm->tm_day,
   pstm->tm_hour,pstm->tm_min,pstm->tm_sec);
}
#endif
#include <stdio.h>
#include <Windows.h>
#include <winbase.h>
/********get time of system*********/
void gettime(char* outtime)
{
   SYSTEMTIME st;
   GetLocalTime(&st);
   sprintf(outtime,"%d-%02d-%02dT%02d:%02d:%02d:%03d",st.wYear,st.wMonth,st.wDay,  \
   st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
}
//gettickcount


int main(int argc ,char* argv[])
{
    char sTime[128];
    gettime(sTime);
    printf("time:%s",sTime);
}
