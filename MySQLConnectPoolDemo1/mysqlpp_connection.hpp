#ifndef __MYSQLPP_UTILITY_H__
#define __MYSQLPP_UTILITY_H__

#include "connection.h"
#include "scopedconnection.h"

class mysqlpp_connection : public mysqlpp::ScopedConnection
{
public:
    explicit mysqlpp_connection(mysqlpp::ConnectionPool& pool, bool safe = true)
        : mysqlpp::ScopedConnection(pool, safe)
    {
        mysqlpp::Connection::thread_start();
    }

    virtual ~mysqlpp_connection()
    {
        mysqlpp::Connection::thread_end();
    }
};

#endif