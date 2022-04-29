#include <stdio.h>



/**get the size of img**/
int GetImgSize(const char* filename)
{
  FILE *fp;
  int  len;
  fp=fopen(filename,"r");
  if(!fp)
  {
      return -1;
  }
  fseek(fp,0,SEEK_END);
  len=ftell(fp);
  fseek(fp,0,SEEK_SET);
  fclose(fp);
  return len;
}


/********save img*************/
int SaveImg(const char* filename,const char* src,const int size)
{
    FILE *fp=fopen(filename,"wb");
    int ret;
    if(!fp)
    {
        return -1;
    }
    ret=fwrite(src,1,size,fp);
    fclose(fp);
    return ret;
}