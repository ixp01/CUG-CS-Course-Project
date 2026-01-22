#include "pch.h"
#include "SQLInterface.h"

bool SQLInterface::Connect_MySQL()
{
    mysql_init(&mysqlCon);
    //mysqlCon 套接字结构体 localhost 本机名 root 用户名 "" 密码为空 mytest 数据库 3306 端口
    if (!mysql_real_connect(&mysqlCon, "localhost", "root", "202010", "stuinfo", 3306, NULL, 0))
    {
        AfxMessageBox(_T("访问数据库失败!"));
        return false;
    }
    else
    {
        AfxMessageBox(_T("成功连接数据库!"));
        // 设置字符集，使得程序支持中文。
        mysql_query(&mysqlCon, "SET NAMES `GB2312`"); // 解决从数据库中读取数据后汉字乱码显示的问题
    }

    return true;
}

vector<StuInfo> SQLInterface::Get_All_Info()
{
    vector<StuInfo> Info;

    snprintf(sql, SQL_MAX, "SELECT * from student");
    //查询数据
    int ret = mysql_real_query(&mysqlCon, sql, (unsigned long)strlen(sql));
    if (ret) {
        AfxMessageBox(_T("查询失败!"));
        return Info;
    }
    // 获取结果集
    res = mysql_store_result(&mysqlCon);
    // 获取查询到的一行数据
    // 给row赋值，判断row是否为空，不为空就打印数据。
    while (row = mysql_fetch_row(res)) {
        StuInfo stuinfo = StuInfo(atoi(row[0]),row[1],row[2], atoi(row[3]), atoi(row[4]));
        Info.push_back(stuinfo);
    }
    mysql_free_result(res); 
    return Info;

}
