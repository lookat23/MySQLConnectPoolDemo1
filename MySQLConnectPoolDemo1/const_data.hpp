#ifndef __CONST_DATA_HPP_
#define __CONST_DATA_HPP_
#include "mysqlpp_connection_pool.hpp"
#include "mysql++.h"
#include "ssqls.h"

// �򿪵��߳���
#define THREAD_NUM 1

// �����̵߳Ĳ���
typedef struct tagTHREADPARAM
{
    BOOL* running;           //�Ƿ������е�ָ��
    void* pClassPtr;         //���ָ��
}THREADPARAM, *LPTHREADPARAM;

// �������ݵ��ļ���
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