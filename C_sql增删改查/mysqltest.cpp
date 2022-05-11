#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
//#include <mysql.h>
//MYSQL mysql;
#include "MysqlApp.h"
#pragma comment(lib,"libmysql.lib")

using namespace std;


int main()
{
	//USER_MYSQL(string myuser, string mypassword, string mytablename, string myhost = "127.0.0.1", string mydatabase = "mysql");
	VEHICLE_INFO   vehinfo;
	vehinfo.vehId = 1;
	vehinfo.velType = 11;
	vehinfo.axisnum = 2;
	vehinfo.axistype=12;
	vehinfo.timestr = "20220509101059";
	vehinfo.plate="À’A123456";
	vehinfo.headImgPath="./mysql_Study/MysqlTest/20220509101059/head.jpg";
	vehinfo.binPlatePath="./mysql_Study/MysqlTest/20220509101059bin.jpg";
	vehinfo.colPlatePath="./mysql_Study/MysqlTest/20220509101059col.jpg";
	vehinfo.sideImgPath="./mysql_Study/MysqlTest/20220509101059side.jpg";
	vehinfo.tailImgPath="./mysql_Study/MysqlTest/20220509101059tail.jpg";
	vehinfo.headVideoPath="./mysql_Study/MysqlTest/20220509101059video.jpg";
	USER_MYSQL mysql("root", "123456", "carInfox7");
	mysql.connectMysql();
	mysql.addRecord(vehinfo);
#if 0
	for(int i=0;i<10;i++)
	{
		vehinfo.vehId++;
		mysql.addRecord(vehinfo);
	}
#endif
	mysql.deleteRecord(3);
	mysql.searchRecord(vehinfo.plate);
	mysql.modifyRecord("Plate", vehinfo.plate, "À’E123456");
	system("pause");
#if 0
	char printBuf[1024];
	MYSQL mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;
	mysql_init(&mysql);
	if (!mysql_real_connect(&mysql, "127.0.0.1", "root", "123456", "mysql", 3306, 0, 0))
	{
		sprintf(printBuf, "%s_%s_mysql_real_connect failure!",__FUNCTIONW__,__LINE__);
		cout << printBuf << endl;
	}
	else
	{
		sprintf(printBuf, "%s_%s_mysql connect success", __FUNCTIONW__, __LINE__);
		cout <<printBuf<< endl;
	}
	system("pause");
	mysql_close(&mysql);
#endif
	return 0;
}