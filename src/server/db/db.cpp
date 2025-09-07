#include "db.h"
#include <string>
#include <muduo/base/Logging.h>

// 数据库配置信息
static string server = "127.0.0.1";
static string user = "chengyc";
static string password = "root";
static string dbname = "chat";

MySQL::MySQL()
{
    _conn = mysql_init(nullptr);
}

MySQL::~MySQL()
{
    if (_conn != nullptr)
    {
        mysql_close(_conn);
    }
}

bool MySQL::connect()
{
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr)
    {
        mysql_query(_conn, "set names gbk");
        LOG_INFO << "MySQL Connect Success!";
    }
    else
    {
        LOG_ERROR << "MySQL Connect Failed!";
    }
    return p;
}

bool MySQL::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_ERROR << "MySQL update error: " << mysql_error(_conn);
        return false;
    }
    LOG_INFO << "MySQL update success!";
    return true;
}

MYSQL_RES *MySQL::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_ERROR << "MySQL query error: " << mysql_error(_conn);
        return nullptr;
    }
    return mysql_store_result(_conn);
}

MYSQL *MySQL::getConnection()
{
    return _conn;
}
