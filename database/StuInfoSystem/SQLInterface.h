#pragma once
#define SQL_MAX 256 //sql语句最大字段
#include"StuInfo.h"
class SQLInterface
{
public:
	MYSQL_RES* res;//result
	MYSQL_ROW row;
	MYSQL mysqlCon;
	char sql[SQL_MAX];

	bool Connect_MySQL();//连接数据库
	vector<StuInfo> Get_All_Info();//获取全部信息
};

