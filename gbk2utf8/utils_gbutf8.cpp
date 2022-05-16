#include <Windows.h>
#include <wchar.h>
#include <iostream>
#include <new>
#include "utils_gbutf8.h"
using namespace std;


/*********************************************************************
函数名      : multiByteToUnicode
功能        : 将多字节字符串转换为Unicode字符串（例如：gbk字符串转为unicode字符串；utf-8字符串转为unicode字符串）
参数        : 
              multiByteStr：指向待转换的字符串的缓冲区
              multiByteStrLen：指定由参数multiByteStr指向的字符串中字节的个数
			  multiByteCharsetFlag：指定执行转换的多字节字符所使用的字符集，可选值有CP_ACP（gbk/gb2312）、CP_UTF8（utf-8）
			  unicodeStr：指向接收被转换字符串的缓冲区
			  unicodeStrLen：指定由参数unicodeStr指向的缓冲区的字节数
返回值      : 成功时，返回解析好的字节个数，错误时，返回相应错误码（-1：待转换的字符串为空；-2：待转换的字符串的长度小于等于0；-3：字符集设置不合理，必须               是CP_ACP和CP_UTF8之一；-4：接收转换字符串缓存区为空；-5：接收转换字符串缓存区长度小于等于0；-6：接收被转换字符串的缓冲区小于实际需要的字节                数）
日期        : 2018-09-26
作者        : xxx
*********************************************************************/
int multibyteToUnicode(const char *multiByteStr,int multiByteStrLen,UINT multiByteCharsetFlag,char *unicodeStr,int unicodeStrLen)
{
	
	
	if (multiByteStr==NULL)//原始字符串为空
	{
		return -1;
	}

	if (multiByteStrLen<=0)//原始字符串长度小于等于0
	{
		return -2;
	}

	if (multiByteCharsetFlag!=CP_ACP && multiByteCharsetFlag!=CP_UTF8)//原始字符串字符集标识既不是GBK/GB2312又不是UTF-8
	{
		return -3;
	}

	if (unicodeStr==NULL)//用于存放unicode串的缓存区为空
	{
		return -4;
	}

	if (unicodeStrLen<=0)//用于存放unicode串的缓存区的长度小于等于0
	{
		return -5;
	}

	int requiredUnicode=MultiByteToWideChar(multiByteCharsetFlag,0,multiByteStr,multiByteStrLen,NULL,0);//此种情况用来获取转换所需的wchar_t的个数

	if(sizeof(WCHAR)*requiredUnicode>unicodeStrLen)//接收被转换字符串的缓冲区小于实际需要的字节数
	{
		return -6;
	}

   WCHAR *tmp=new WCHAR[requiredUnicode];//动态分配unicode临时缓存区

	wmemset(tmp,0,requiredUnicode);//将临时缓存区数据清零

	//执行真实转换，并将转换后的unicode串写到tmp缓存区
	int parsedUnicode=MultiByteToWideChar(multiByteCharsetFlag,0,multiByteStr,multiByteStrLen,tmp,requiredUnicode);

	if (parsedUnicode!=requiredUnicode)//判断真实解析的unicode字符数和分配的字符数是否一致
	{
		delete []tmp;
		tmp=NULL;
		return -7;
	}

	memset(unicodeStr,0,unicodeStrLen);//将目标unicode缓存区清零
	memcpy(unicodeStr,tmp,sizeof(WCHAR)*requiredUnicode);//将数据由临时缓存区拷贝到目标unicode缓存区

	delete [] tmp;//释放空间
	tmp=NULL;

	return sizeof(WCHAR)*requiredUnicode;//返回解析好的总字节数
}


/*********************************************************************
函数名      : unicodeToMultibyte
功能        : 将Unicode字符串转换为多字节字符串（例如：将unicode字符串转换为gb2312/utf-8编码的字符串）
参数        : 
              unicodeStr：指向待转换的unicode字符串的缓冲区，该字符串必须是小端字节序
              unicodeStrLen：指定由参数unicodeStr指向的字符串中字节的个数
			  multiByteStr：指向接收被转换字符串的缓冲区
			  multiByteStrLen：指定由参数multiByteStr指向的缓冲区的字节数
			  multiByteCharsetFlag：指定目标字符串所使用的字符集，可选值有CP_ACP（gbk/gb2312）、CP_UTF8（utf-8）
返回值      : 成功时，返回解析好的字节个数，错误时，返回相应错误码（-1：待转换的字符串为空；-2：待转换的字符串的长度小于等于0；-3：接收转换字符串缓存区为                 空；-4：接收转换字符串缓存区长度小于等于0；-5：字符集设置不合理，必须是CP_ACP和CP_UTF8之一；-6：接收被转换字符串的缓冲区小于实际需要的字节数）
日期        : 2018-09-27
作者        : xxx
*********************************************************************/
int unicodeToMultibyte(const char *unicodeStr,int unicodeStrLen,char *multiByteStr,int multiByteStrLen,UINT multiByteCharsetFlag)
{
	if (unicodeStr==NULL)//原始unicode字符串为空
	{
		return -1;
	}

	if (unicodeStrLen<=0)//原始unicode字符串的长度小于等于0
	{
		return -2;
	}

	if (multiByteStr==NULL)//用于存放多字节字符串的缓存区为空
	{
		return -3;
	}

	if (multiByteStrLen<=0)//用于存放多字节字符串的缓存区的长度小于等于0
	{
		return -4;
	}
	
	if (multiByteCharsetFlag!=CP_ACP && multiByteCharsetFlag!=CP_UTF8)//目标字符串所使用的字符集既不是CP_ACP（gbk/gb2312）又不是CP_UTF8（utf-8）
	{
		return -5;
	}

	WCHAR *tmp=new WCHAR[unicodeStrLen/2];//动态分配用于存放原始Unicode字符串的临时缓存区

	wmemset(tmp,0,unicodeStrLen/2);//将临时缓存区清零
	
	memcpy(tmp,unicodeStr,unicodeStrLen);//将原始Unicode字符串拷贝到临时缓存区

	int requiredByte=WideCharToMultiByte(multiByteCharsetFlag,0,tmp,unicodeStrLen/2,NULL,0,NULL,NULL);//获取用于存放目标字符串的字节数

	if (requiredByte>multiByteStrLen)//接收被转换字符串的缓冲区小于实际需要的字节数
	{
		delete []tmp;
		tmp=NULL;
		return -6;
	}


	memset(multiByteStr,0,multiByteStrLen);//将目标缓存区清零
	//执行真正转换，将转换后的多字节字符串存放到目标缓存区中，并返回实际解析的字节数
	int parsedByte=WideCharToMultiByte(multiByteCharsetFlag,0,tmp,unicodeStrLen/2,multiByteStr,multiByteStrLen,NULL,NULL);
	if (parsedByte!=requiredByte)//实际解析的字节数不正确
	{
		delete []tmp;
		tmp=NULL;	
		return -7;
	}

	delete []tmp;//释放临时缓存区
	tmp=NULL;
	
	return parsedByte;//返回解析好的总字节数
}


/*********************************************************************
函数名      : gbkToUtf8
功能        : 将gbk编码的字符串转换为utf8编码的字符串
参数        : 
              gbkstr：指向待转换的gbk字符串的缓冲区
              gbkstrlen：指定由参数gbkstr指向的字符串中字节的个数
			  utf8str：指向接收被转换字符串的缓冲区
			  utf8strlen：指定由参数utf8str指向的缓冲区的字节数
返回值      : 成功时，返回解析好的字节个数，错误时，返回相应错误码（-1：字符串为空或长度小于等于0；-2：gbk转unicode失败；-3：unicode转utf-8失败）
日期        : 2018-09-27
作者        : xxx
*********************************************************************/
int gbkToUtf8(const char * gbkstr,int gbkstrlen,char * utf8str,int utf8strlen)
{
	if (gbkstr==NULL || gbkstrlen<=0 || utf8str==NULL || utf8strlen<=0)//字符串为空或长度小于等于0
	{
		return -1;
	}

	char *unicodestr=new char[2*gbkstrlen];//分配缓存区，长度为gbk字符串长度的2倍

	int unicodebytes=multibyteToUnicode(gbkstr,gbkstrlen,CP_ACP,unicodestr,2*gbkstrlen);//gbk转unicode

	if (unicodebytes<0)//gbk转unicode失败
	{
		delete [] unicodestr;
		unicodestr=NULL;
		return -2;
	}

	int utf8bytes=unicodeToMultibyte(unicodestr,unicodebytes,utf8str,utf8strlen,CP_UTF8);//unicode转utf-8

	if (utf8bytes<0)//unicode转utf-8失败
	{
		delete [] unicodestr;
		unicodestr=NULL;
		return -3;
	}

	delete [] unicodestr;//释放内存
	unicodestr=NULL;

	return utf8bytes;//返回解析好的总utf8字节数
}



/*********************************************************************
函数名      : utf8ToGbk
功能        : 将utf8编码的字符串转换为gbk编码的字符串
参数        : 
              utf8str：指向待转换的utf8字符串的缓冲区
              utf8strlen：指定由参数utf8str指向的字符串中字节的个数
			  gbkstr：指向接收被转换字符串的缓冲区
			  gbkstrlen：指定由参数gbkstr指向的缓冲区的字节数
返回值      : 成功时，返回解析好的字节个数，错误时，返回相应错误码（-1：字符串为空或长度小于等于0；-2：utf8转unicode失败；-3：unicode转gbk失败）
日期        : 2018-09-27
作者        : xxx
*********************************************************************/
int utf8ToGbk(const char * utf8str,int utf8strlen,char * gbkstr,int gbkstrlen)
{
	if (utf8str==NULL || utf8strlen<=0 || gbkstr==NULL || gbkstrlen<=0)//字符串为空或长度小于等于0
	{
		return -1;
	}

	char *unicodestr=new char[2*utf8strlen];//分配缓存区，长度为utf8字符串长度的2倍

	int unicodebytes=multibyteToUnicode(utf8str,utf8strlen,CP_UTF8,unicodestr,2*utf8strlen);//utf8转unicode

	if (unicodebytes<0)//utf8转unicode失败
	{
		delete [] unicodestr;
		unicodestr=NULL;
		return -2;
	}

	int gbkbytes=unicodeToMultibyte(unicodestr,unicodebytes,gbkstr,gbkstrlen,CP_ACP);//unicode转GBK

	if (gbkbytes<0)//unicode转gbk失败
	{
		delete [] unicodestr;
		unicodestr=NULL;
		return -3;
	}

	delete [] unicodestr;
	unicodestr=NULL;

	return gbkbytes;//返回解析好的gbk字节数
}

