#ifndef _LONGTIME_H
#define _LONGTIME_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef signed long long	_longtime;

_longtime timeGetLongTime();
int timeSetLongTime(_longtime lt);		// ����ϵͳʱ��
time_t timeLongTimeToTime( _longtime t );
_longtime timeLongTimeFromTime( time_t t );

_longtime timeGetOldLongTime();			// get time in usec

time_t timeGetBODTime( time_t );		// get begin of day (localtime)
time_t timeGetEODTime( time_t );		// get end of day

unsigned long timeGetDays( _longtime t );
unsigned long timeGetUSec( _longtime t );
unsigned long timeGetMSec( _longtime t );
unsigned long timeGetSec( _longtime t );

#define LONG_TIME_FORMAT_SIZE	24
#define DT_FMT_DATE		0x01
#define DT_FMT_TIME		0x02
#define DT_FMT_MSEC		0x04
#define DT_FMT_LONG		0x0f
#define DT_FMT_DTIM		0x03
#define DT_FMT_TMSEC	0x06

char *timeFormatLongTime( _longtime t, char *buffer );	// buffer must have space >= LONG_TIME_FORMAT_SIZE
char *timeFormatLongTimeEx(_longtime t, char *buffer, int format);
#define TIMESTAMP()	timeFormatLongTime(0,NULL)
#define TIMESTAMP0() timeFormatLongTimeEx(0,NULL,DT_FMT_TMSEC)

typedef struct {
	int	year;			// western calendar year
	int month;			// 1 ~ 12
	int day;				// day of month 1~31
	int hour;			// hour in a day 0~23
	int	minute;		// minutes in hour 0~59
	int second;		// seconds in minutes
	int msec;			// milli-seconds 0~999
} CTM, *PCTM;

void timeLongTimeToCTM( _longtime lt, PCTM pctm );
_longtime timeLongTimeFromCTM( PCTM pctm );

// - timeGetCTMFromStr:
// ����0�ɹ���-1��ʽ����
// strTime������/ʱ���ִ������Խ��ܵ�ʱ���ִ���ʽΪ:
// 1) 2015/11/12 12:13:46.234
// 2) 2015-11-12 12:13:46.234
// 3) 20151112121346234
// 4) 20151112121346.234
// 5)~8) - ͬ1)~4), ������Ĳ���ֻ��2λ����
// ������ͬ�ϣ�����û�к��벿��,����û�����ڲ��֣�����û��һ���ڵ�ʱ�䲿�֡�
// û�����ڵĻ���ʹ�õ������ڣ�û��ʱ��Ĳ��֣�ʱ���֡��롢����ȫ��Ϊ0.
// ���ֻ��6λ���֣������޷����������ڻ���ʱ�䣬��ʱ�����ȡ�
int timeGetCTMFromStr(const char*strTime, PCTM ctm);

#ifdef __cplusplus
};
#endif

#endif	// _LONGTIME_H
