/*
 *	longtime.c
 *
 *	handle the time in msec resolution.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "longtime.h"

#ifdef _WIN32
#include "windows/types.h"

inline LONGLONG GetOffset() {
	SYSTEMTIME st1;
	LONGLONG lt1;
	memset( &st1, 0, sizeof( st1 ) );
	st1.wYear = 1970;
	st1.wMonth = 1;
	st1.wDay = 1;
	SystemTimeToFileTime( &st1, (FILETIME*)&lt1 );
	return lt1;
}

static LONGLONG llOffset = 0;

struct timezone {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	LONGLONG ft;
	GetSystemTimeAsFileTime( (FILETIME*)&ft );
	if( llOffset == 0 )
		llOffset = GetOffset();
	tv ->tv_usec = (long)( ft % 10000000 ) / 10;
	tv ->tv_sec = (long)(( ft - llOffset ) / 10000000);
	return 0;
}
#else	// linux
#include <sys/time.h>
#endif

// ��2001/1/1 00:00:00 Ϊ��ʼ��
//static const time_t _secondOffset = ( 365 * 31 + 8 ) * 24 * 60 * 60;	// time offset from 1970.1.1 to 2001.1.1, 8 leap years( 72, 76, 80, 84, 88, 92, 96, 00 )
// ��2000/1/1 00:00:00 Ϊ��ʼ��
static const time_t _secondOffset = ( 365 * 30 + 7 ) * 24 * 60 * 60;		// time offset from 1970.1.1 to 2000.1.1, 8 leap years( 72, 76, 80, 84, 88, 92, 96 )

_longtime timeGetLongTime()
{
	struct timeval tv;
	if( gettimeofday( &tv, NULL ) )
	{
		return -1;
	}
	return (_longtime)( tv.tv_sec - _secondOffset ) * 1000 + tv.tv_usec / 1000;
}

int timeSetLongTime(_longtime lt)
{
	struct timeval tv;
	tv.tv_sec = timeLongTimeToTime(lt);
	tv.tv_usec = (lt % 1000) * 1000;
	return settimeofday( &tv, NULL );
}

_longtime timeGetOldLongTime()
{
	struct timeval tv;
	if( gettimeofday( &tv, NULL ) )
	{
		return -1;
	}
	return (_longtime)tv.tv_sec * 1000000 + tv.tv_usec;
}

time_t timeLongTimeToTime( _longtime t )
{
	return (time_t)(t / 1000) + _secondOffset;
}

_longtime timeLongTimeFromTime( time_t t )
{
	return (_longtime)( t - _secondOffset ) * 1000;
}

time_t timeGetBODTime( time_t t )
{
//	struct tm	_tm;

	if ( t == 0 ) t = time(NULL);
#if 0
	localtime_r( &t, &_tm );
	_tm.tm_sec = 0;
	_tm.tm_min = 0;
	_tm.tm_hour = 0;
	return mktime( &_tm );
#endif
	t -= timezone;		// convert to localtime
	return t - (t % 86400);
}

time_t timeGetEODTime( time_t t )
{
//	struct tm	_tm;

	if ( t == 0 ) t = time(NULL);
#if 0
	localtime_r( &t, &_tm );
	_tm.tm_sec = 0;
	_tm.tm_min = 0;
	_tm.tm_hour = 0;
	_tm.tm_mday++;
	return mktime( &_tm );
#endif
/* EOD is next day's BOD */
	t -= timezone;
	return t + 86400 - (t % 86400);
}

unsigned long timeGetDays( _longtime t )
{
	return (unsigned long)( t / 1000 / 86400 );
}

unsigned long timeGetUSec( _longtime t )
{
	return (unsigned long)(( t % 1000 ) * 1000);
}

unsigned long timeGetMSec( _longtime t )
{
	return (unsigned long)( t % 1000);
}

unsigned long timeGetSec( _longtime t )
{
	return (unsigned long)( t / 1000);
}


static char	_mybuf[ LONG_TIME_FORMAT_SIZE ];
char *timeFormatLongTime( _longtime t, char *buffer )
{
	char	*p = buffer ? buffer : _mybuf;
	if ( t==0 ) t = timeGetLongTime();
	time_t time = timeLongTimeToTime( t );
	strftime( p, LONG_TIME_FORMAT_SIZE, "%y/%m/%d %H:%M:%S", localtime( &time ) );
	sprintf( p + strlen(p), ".%03lu", timeGetMSec( t ) );
	return p;
}

char *timeFormatLongTimeEx(_longtime t, char *buffer, int format)
{
	char	*p = buffer ? buffer : _mybuf;
	char *ptr = p;
	
	if ( t==0 ) t = timeGetLongTime();
	time_t time = timeLongTimeToTime( t );
	if ( format & DT_FMT_DATE )
	{
		strftime( ptr, LONG_TIME_FORMAT_SIZE, "%y/%m/%d", localtime( &time ) );
		ptr += strlen(ptr);
	}
	if ( format & DT_FMT_TIME )
	{
		if ( ptr != p )
			*(ptr++) = ' ';
		strftime( ptr, LONG_TIME_FORMAT_SIZE, "%H:%M:%S", localtime( &time ) );
		ptr += strlen(ptr);
		if ( format & DT_FMT_MSEC )
			sprintf( ptr, ".%03lu", timeGetMSec( t ) );
	}
	return p;
}

void timeLongTimeToCTM( _longtime lt, PCTM ctm )
{
	time_t t;
	struct tm	_tm;

	if ( lt==0 )
	{
		t = timeGetLongTime();	//t = time(NULL); // 2015.1.5
	}
	else
		t = timeLongTimeToTime(lt);
	localtime_r( &t, &_tm );
	ctm->year = _tm.tm_year + 1900;
	ctm->month = _tm.tm_mon + 1;
	ctm->day = _tm.tm_mday;
	ctm->hour = _tm.tm_hour;
	ctm->minute = _tm.tm_min;
	ctm->second = _tm.tm_sec;
	ctm->msec = lt % 1000;	
}

_longtime timeLongTimeFromCTM( PCTM ctm )
{
	time_t t;
	struct tm	_tm;
	_longtime lt;
	
	_tm.tm_year = ctm->year - 1900;
	_tm.tm_mon = ctm->month - 1;
	_tm.tm_mday = ctm->day;
	_tm.tm_hour = ctm->hour;
	_tm.tm_min = ctm->minute;
	_tm.tm_sec = ctm->second;
	t = mktime(&_tm);
	lt = timeLongTimeFromTime(t) + ctm->msec;
	return lt;
}

static int is_all_digit(const char *str, int len)
{
	int i;
	for(i=0; i<len; i++)
		if ( !isdigit(str[i]) )
			return (str[i]==':' || str[i]==' ' || str[i]=='/' || str[i]=='.' || str[i]=='-') ? 0 : -1;
	return 1;
}

int timeGetCTMFromStr(const char*strTime, PCTM ctm)
{
	int rc, len;
	struct tm	_tm;
	time_t tnow;
	
	memset(ctm, 0, sizeof(CTM) );
	tnow=time(NULL);
	// �ȸ�ֵ�����������
	localtime_r( &tnow, &_tm );
	ctm->year = _tm.tm_year + 1900;
	ctm->month = _tm.tm_mon + 1;
	ctm->day = _tm.tm_mday;
	
	if ( (len=strlen(strTime))<6 ) return -1;
	if ( (rc=is_all_digit(strTime, len))==-1 )	
	{
		//printf("���������ִ�����: %s (len=%d)\n", 	strTime, len );	
		return -1;
	}
	if ( rc )	// all digit
	{
		if ( len==6 )	
		{
			int n1, n2, n3;
			if ( sscanf(strTime, "%02d%02d%02d", &n1, &n2, &n3 ) != 3 )
				return -1;
			if ( n2 <= 12 && n3 <= 31 && n1>0 && n2>0 && n3>0 )
			{
				// 6λ��������
				ctm->year = n1 + 2000;
				ctm->month = n2;
				ctm->day = n3;
			}
			else
			{
				ctm->hour = n1;
				ctm->month = n2;
				ctm->second = n3;
			}
		}
		else if ( len==8 )	// һ��������
		{
			sscanf(strTime, "%04d%02d%02d", &ctm->year, &ctm->month, &ctm->day);
		}
		else if ( len==9 )	// ʱ��+����
		{
			sscanf(strTime, "%02d%02d%02d%03d", &ctm->hour, &ctm->month, &ctm->second, &ctm->msec);
		}
		else if ( len==12 )	// YYMMDDhhmmss
		{
			sscanf(strTime, "%02d%02d%02d%02d%02d%02d", 
				&ctm->year, &ctm->month, &ctm->day, &ctm->hour, &ctm->minute, &ctm->second);
		}
		else if ( len==14 )	// YYYYMMDDhhmmss
		{
			sscanf(strTime, "%04d%02d%02d%02d%02d%02d", 
				&ctm->year, &ctm->month, &ctm->day, &ctm->hour, &ctm->minute, &ctm->second);
		}
		else if ( len==15 )	// YYMMDDhhmmssmsc
		{
			sscanf(strTime, "%02d%02d%02d%02d%02d%02d%03d", 
				&ctm->year, &ctm->month, &ctm->day, &ctm->hour, &ctm->minute, &ctm->second, &ctm->msec);
		}
		else if ( len==17 )	// YYYYMMDDhhmmssmsc
		{
			sscanf(strTime, "%04d%02d%02d%02d%02d%02d%03d", 
				&ctm->year, &ctm->month, &ctm->day, &ctm->hour, &ctm->minute, &ctm->second, &ctm->msec);
		}
	}
	else 	// ����ʽ
	{
		char *ptr;
		if ( (ptr=strchr(strTime,'/')) != NULL || (ptr=strchr(strTime,'-')) != NULL )
		{
			// �д���ʽ������
			ctm->year = atoi(strTime);
			if ( ctm->year < 100 ) ctm->year += 2000;
			ptr++;
			ctm->month = strtol(ptr,&ptr,10);
			ctm->day = strtol(ptr+1, &ptr, 10);
			if ( *ptr==' ' ) ptr++;
		}
		else if ( ptr==NULL )	// û�����ڣ�ֻ��ʱ��
			ptr = (char *)strTime;
		ctm->hour = strtol(ptr,&ptr,10);
		ctm->minute = strtol(ptr+1,&ptr,10);
		ctm->second = strtol(ptr+1,&ptr,10);
		if ( *ptr=='.' )
		{
			ctm->msec = strtol(ptr+1,&ptr,10);
		}
	}
	// �����
	//printf("����ʱ����������%04d/%02d/%02d %02d:%02d:%02d\n",
	//		ctm->year, ctm->month, ctm->day, ctm->hour, ctm->minute, ctm->second );
	return (ctm->month<=12 && ctm->day<=31 && 
				 ctm->hour<24 && ctm->minute<60 && ctm->second<60 && ctm->msec < 1000) ?
				 0 : -1;
}
