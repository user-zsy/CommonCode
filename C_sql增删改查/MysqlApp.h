#ifndef __MYSQLAPP_H__
#define __MYSQLAPP_H__
#include "mysql.h"
#include <string>
using namespace std;
class VEHICLE_INFO{
public:
	int vehId;
	int velType;
	int axisnum;
	int axistype;
	string timestr;
	string plate;
	string headImgPath;
	string binPlatePath;
	string colPlatePath;
	string sideImgPath;
	string tailImgPath;
	string headVideoPath;
};
class USER_MYSQL {
public:
     USER_MYSQL(string myuser, string mypassword,string mytablename, string myhost = "127.0.0.1", string mydatabase = "mysql");
	 ~USER_MYSQL();
	 int connectMysql();
	 int createTable();
	 int addRecord(VEHICLE_INFO inVelInfo);
	 int deleteRecord(int id);
	 int searchRecord(string plate);
	 int modifyRecord(string title, string textold, string textnew);
private:
	string m_user, m_password, m_host, m_database, m_tablename;
	MYSQL *sql;
	int num_fields;
	char column[20][30];
};

#endif