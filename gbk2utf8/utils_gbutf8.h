#ifndef UTILS_GBUTF8_INCLUDED
#define UTILS_GBUTF8_INCLUDED

extern int utf8ToGbk(const char * utf8str,int utf8strlen,char * gbkstr,int gbkstrlen);
extern int gbkToUtf8(const char * gbkstr,int gbkstrlen,char * utf8str,int utf8strlen);
extern int unicodeToMultibyte(const char *unicodeStr,int unicodeStrLen,char *multiByteStr,int multiByteStrLen,UINT multiByteCharsetFlag);
extern int multibyteToUnicode(const char *multiByteStr,int multiByteStrLen,UINT multiByteCharsetFlag,char *unicodeStr,int unicodeStrLen);
#endif
