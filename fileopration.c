#include <stdio.h>
#include <stdlib.h>
#include  <string.h>

int  getFileLine(char* filename)
{
	int c,length=0;
	FILE *fp1; //打开文件
    fp1 = fopen(filename, "r+");
	//打开文件，"r+"为访问模式：打开一个用于更新的文件，可读取也可写入。该文件必须存在。
    if(NULL == fp1)
    {
       //perror("connect error \n");
       //exit(-1);
       return -1;
    }
    //计算行数
    while((c = fgetc(fp1)) != EOF)
    {
        if(c == '\n')//碰到换行符，则行数+1
        length++;
    }
    fclose(fp1);//关闭文件 
    return length;
}





/**

删除指定文件filepath中第n行内容

**/
int readOneLine(const char* filepath,const int linenum,char* outStr,int len)
{
    char buf[4096];
    FILE* fp = fopen(filepath, "r"); //filepath里是原内容
    int   i=0;
    if(linenum<=0)
    {
        return -2;
    }
    while(!feof(fp))
    {
        i++;
        if(i==linenum)
        {
            fgets(outStr,len,fp);
            break;
        }
        else if(i>linenum)
        {
            break;
        }
          else
        {
            fgets(buf,sizeof(buf),fp);
        }
    }
    return (i==linenum?0:-2);
}
void deleteOneLine(char* filepath, int n)
{
    char buf[4096]; //每读取一行，都将内容放到该数组中
    FILE* fp = fopen(filepath, "r"); //filepath里是原内容
    FILE* fpt = fopen("temp.txt", "w"); //将filepath文件第n行内容删除后存入temp.txt中
    int i = 0;
    while(!feof(fp))   //file over flag
    {
        i++;
        if (i == n) //删掉第n行，其实就是读到第n行时，仅仅移动一下文件指针，否则就
        { //将读取到的一行文件内容放到临时文件中temp.txt中，这就是所谓的删除
        fgets(buf,sizeof(buf), fp); //必须要这一行，因为加上后文件指针将会移动
        }
        else
        {
        fgets(buf, sizeof(buf), fp);
        fprintf(fpt, "%s", buf);
        }

    }
    fclose(fp);
    fclose(fpt);
    #if 1
    fpt = fopen("temp.txt", "r");
    fp = fopen(filepath, "wb"); //清空filepath文件
    fclose(fp);
    fp = fopen(filepath, "a");
    while(!feof(fpt))
    {
        fgets(buf, sizeof(buf), fpt);
        fprintf(fp, "%s", buf);
    }
    fclose(fp);
    fclose(fpt);
    #endif
    system("ERASE temp.txt"); //删除文件temp.txt,使用该命令需要包含文件
}

void main()
{ 
    int lineCount=0;
    lineCount=getFileLine("a.txt");
    printf("line count:%d \n",lineCount);
//Delete("E:/VC6练习/Test/1.txt", 3);
}
