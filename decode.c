#include <stdio.h>
#include <string.h>

int main()
{
    char* pipstr=NULL;
    int ret;
    char ipstr[]="192.168.1.22;192.168.1.23";
    if(ipstr!=NULL)
    {
        pipstr=strtok(ipstr,";");
        ret=0;
        while((pipstr!=NULL)&&(ret<2))
        {
            printf("ip:%s \n",pipstr);
            pipstr=strtok(NULL,";");
            ret++;
        }
    }
}