#ifndef __COMMONCODE_H__
#define __COMMONCODE_H__

#ifdef __cplusplus
extern "C"
{
#endif
/******get size of file********/
extern int GetImgSize(const char* filename);

 /******save file*********/
extern int SaveImg(const char* filename,const char* src,const int size);


#ifdef __cplusplus
}
#endif



#endif