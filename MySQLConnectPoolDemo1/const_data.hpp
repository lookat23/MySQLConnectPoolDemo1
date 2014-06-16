#ifndef __CONST_DATA_HPP_
#define __CONST_DATA_HPP_
#include "mysqlpp_connection_pool.hpp"
#include "mysql++.h"
#include "ssqls.h"

// 打开的线程数
#define THREAD_NUM 1

// 传给线程的参数
typedef struct tagTHREADPARAM
{
    BOOL* running;           //是否正运行的指针
    void* pClassPtr;         //类的指针
}THREADPARAM, *LPTHREADPARAM;

// 保存数据的文件名
const char* const DATA_FILE_NAME = "db_data.dat";

enum DB_RESULT
{
    DR_SUCCEED,
    DR_FAIL,
    DR_EMPTY,
    DR_WORKING,    
};

#define INIT_MYSQL_CONNECTION()\
    mysqlpp_connection conn_ptr(g_conn_pool::instance());\
    if (false == conn_ptr->connected())\
{\
    return DR_FAIL;\
}\
    mysqlpp::Query query = conn_ptr->query()\
    

#endif