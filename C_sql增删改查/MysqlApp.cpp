#include <iostream>
#include <WinSock2.h>
#include "MysqlApp.h"
USER_MYSQL::USER_MYSQL(string myuser, string mypassword, string mytablename, string myhost, string mydatabase)
{
	m_user = myuser;
	m_password = mypassword;
	m_tablename = mytablename;
	m_host = myhost;
	m_database = mydatabase;
	sql =NULL;
}

USER_MYSQL::~USER_MYSQL()
{
	if(sql)   mysql_close(sql);
}

//连接数据库
int USER_MYSQL::connectMysql()
{
	int timeout = 5;
	sql=mysql_init(NULL);
	if (!sql)
	{
		cout << "mysql_init fail" << endl;
		return -1;
	}
	mysql_options(sql,MYSQL_OPT_CONNECT_TIMEOUT,&timeout);
	mysql_options(sql, MYSQL_OPT_READ_TIMEOUT, &timeout);
	mysql_options(sql, MYSQL_OPT_WRITE_TIMEOUT, &timeout);
	if (!mysql_real_connect(sql, m_host.c_str(), m_user.c_str(), m_password.c_str(), m_database.c_str(), 3306, NULL, 0))
	{
		cout << "conect fail" << endl;
		return -1;
	}
	else
	{
		//mysql_query(sql, "set names 'GBK'");
		cout << "conect success" << endl;
		createTable();
	}
#if 0
	string table;
	table = "select * from " + m_tablename;
	mysql_query(sql, table.c_str());
	res = mysql_store_result(sql);
	if (res != NULL)
	{
		num_fields = mysql_num_fields(res);
		for (int i = 0; i < num_fields; i++)
		{
			strcpy(column[i], mysql_fetch_field(res)->name);
			cout << column[i];
		}
	}
	else
	{
		createTable();
	}
#endif
	return 0;
}

//创建表
int USER_MYSQL::createTable()
{
	string table;
	table = "select * from " + m_tablename;
	int ax=mysql_query(sql, table.c_str());
	auto res = mysql_store_result(sql);
	if (res != NULL)
	{
		num_fields = mysql_num_fields(res);
		for (int i = 0; i < num_fields; i++)
		{
			strcpy(column[i], mysql_fetch_field(res)->name);
			cout << column[i]<<endl;
		}
		mysql_free_result(res);
		return 0;
	}
	/*
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
	*/
	table = "CREATE TABLE `" + m_tablename + "` ("+
		"`VehId` tinyint(4) NOT NULL AUTO_INCREMENT ,"+
		"`VehType` tinyint(4) NOT NULL ,"+
		"`Axisnum` tinyint(4) NOT NULL ," +
		"`Axistype` tinyint(4) NOT NULL ," +
		"`Time` varchar(20) NOT NULL," +
		"`Plate` varchar(20) NOT NULL,"+
		"`HeadPic` varchar(128) NOT NULL,"+
		"`BinPlatePic` varchar(128) NOT NULL," +
		"`ColPlatePic` varchar(128) NOT NULL," +
		"`SidePic` varchar(128) NOT NULL,"+
		"`TailPic` varchar(128) NOT NULL,"+
		"`VideoFile` varchar(128) NOT NULL,"+
		"PRIMARY KEY (`VehId`)"+
		") ENGINE=InnoDB DEFAULT CHARSET=utf8;";


	/* 
	table = "CREATE TABLE `" + m_tablename + "` ("
		"`id` tinyint(4) NOT NULL AUTO_INCREMENT,"
		"`toUser` varchar(20) NOT NULL,"
		"`fromUser` varchar(20) NOT NULL,"
		"`time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,"
		"`msg` text NOT NULL,"
		"PRIMARY KEY (`id`)"
		") ENGINE=InnoDB DEFAULT CHARSET=utf8;";
		*/
		 if (mysql_query(sql, table.c_str()))
		 {
			 cout << "create table fail" << endl;
		 }
		 else
		 {
			 cout << "create table success" << endl;
		 }
	 return 0;
}

//在表中插入数据
int USER_MYSQL::addRecord(VEHICLE_INFO inputVehInfo)
{
	string  addinfo;
	addinfo = "insert into " + m_tablename + " values ('" +
		to_string(inputVehInfo.vehId) + "', '" +
		to_string(inputVehInfo.velType) + "', '" +
		to_string(inputVehInfo.axisnum) + "', '" +
		to_string(inputVehInfo.axistype) + "', '" +
		inputVehInfo.timestr + "', '" +
		inputVehInfo.plate + "', '" +
		inputVehInfo.headImgPath+"', '"+
		inputVehInfo.binPlatePath+"', '"+
		inputVehInfo.colPlatePath+"', '"+
		inputVehInfo.sideImgPath+"', '"+
		inputVehInfo.tailImgPath+"', '"+
		inputVehInfo.headVideoPath+"')";
	cout<<addinfo<<endl;
	if(mysql_query(sql,addinfo.c_str()))
	{
		cout<<"insert fail"<<endl;
		return -1;
	}
	return 0;
}

//在表中删除数据
int USER_MYSQL::deleteRecord(int id )
{
	string deleteinfo = "delete from " + m_tablename + "  where VehId<" + to_string(id);
	cout << deleteinfo << endl;
	if (mysql_query(sql, deleteinfo.c_str()))
	{
		cout << "delete error" << endl;
		return -1;
	}
	cout << "delete success" << endl;
	return 0;
}

//在表中查找数据
int USER_MYSQL::searchRecord(string plate)
{
	MYSQL_ROW row;
	string searchinfo = "select * from " + m_tablename + " where Plate='" + plate+"'";
	cout << searchinfo << endl;
	if (mysql_query(sql, searchinfo.c_str()))
	{
		cout << "select fail" << endl;
		return -1;
	}
	auto result = mysql_store_result(sql);
	if (mysql_num_rows(result))
	{
		while ((row = mysql_fetch_row(result)))
		{
			int i = 0;
			for (i = 0; i < 12;i++)
			{
				cout << row[i]<<" ";
			}
			cout << endl;
		}
	}
	return 0;
}

//在表中修改数据
int USER_MYSQL::modifyRecord(string title ,string textold,string textnew)
{
	string modifyinfo = "update " + m_tablename + " set " + title + "='" +
		textnew + "' " + "where "+title+"='"+textold+"'";
	cout << modifyinfo << endl;
	if (mysql_query(sql, modifyinfo.c_str()))
	{
		cout << "modify fail" << endl;
		return -1;
	}
	cout << "modify success" << endl;
	return 0;
		                
}