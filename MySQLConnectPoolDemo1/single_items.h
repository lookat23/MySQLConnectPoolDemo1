#ifndef __SINGLE_ITEM_H_
#define __SINGLE_ITEM_H_
#include "singleton.hpp"
#include "ConnectionThread.hpp"
#include "PracticalApplication.h"

typedef singleton<ConnectionThread> g_conn_thread;

typedef singleton<mysqlpp_connection_pool> g_conn_pool;

typedef singleton<PracticalApplication> g_practical_application;

#endif