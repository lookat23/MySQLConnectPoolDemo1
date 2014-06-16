#pragma once
#include <process.h>
#include "mysqlpp_connection_pool.hpp"

class ConnectionThread
{
public:
    ConnectionThread(void);
    ~ConnectionThread(void);
    int PrintList();
    static unsigned __stdcall worker_thread (void * lpThreadParameter );
/*    DWORD __stdcall worker_thread(LPVOID lpThreadParameter);*/
    // œﬂ≥Ã≥ÿ÷∏’Î
    mysqlpp_connection_pool* poolptr;
};

