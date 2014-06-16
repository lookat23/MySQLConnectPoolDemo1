#ifndef  __MYSQL_CONNECTION_POOL_H__
#define  __MYSQL_CONNECTION_POOL_H__

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include "connection.h"
#include "cpool.h"
#include <iostream>

#include "mysqlpp_connection.hpp"
#include "singleton.hpp"

// ���ݿ����ӳ�
class mysqlpp_connection_pool : public mysqlpp::ConnectionPool
{
public:
    mysqlpp_connection_pool() 
        : conns_in_use_(0)
        , server_("")
        , user_("")
        , password_("")
        , db_("")
    {}

    ~mysqlpp_connection_pool()
    {
        clear();
    }

public:
    void set_connect_info(const std::string& address, const std::string& user,
                        const std::string& passwd, const std::string& db_name);
    // �������ݿ����ӷ���ʱ
    // ��������趨ֵ8���ȴ��ͷŲ��ܼ��������µ�����
    mysqlpp::Connection* grab();
    void release(const mysqlpp::Connection* pc);

protected:
    // ���غ���
    mysqlpp::Connection* create();
    void destroy(mysqlpp::Connection* cp);
    // �������ӳ�ʱʱ�� 3��
    unsigned int max_idle_time();

private:
    // ��ǰ����ʹ�õ�������
    boost::mutex    io_mutex_;
    unsigned int    conns_in_use_;

    // ������ϸ��Ϣ
    std::string     db_;
    std::string     server_;
    std::string     user_;
    std::string     password_;
};

inline void mysqlpp_connection_pool::set_connect_info(const std::string& address, 
                                                      const std::string& user,
                                                      const std::string& passwd,
                                                      const std::string& db_name)
{
    db_         = db_name;
    server_     = address;
    user_       = user;
    password_   = passwd;
}

inline mysqlpp::Connection* mysqlpp_connection_pool::grab()
{
    while (conns_in_use_ > 8)
    {
        std::cout<<"Release mysqlpp::Connection";
        std::cout.flush();

        boost::detail::Sleep(50);
    }

    boost::mutex::scoped_lock lock(io_mutex_);
    ++conns_in_use_;
    return mysqlpp::ConnectionPool::grab();
}

inline void mysqlpp_connection_pool::release(const mysqlpp::Connection* pc)
{
    boost::mutex::scoped_lock lock(io_mutex_);
    --conns_in_use_;
    mysqlpp::ConnectionPool::release(pc);
}

inline mysqlpp::Connection* mysqlpp_connection_pool::create()
{
    mysqlpp::Connection* pconn = new mysqlpp::Connection(false);
    pconn->set_option(new mysqlpp::SetCharsetNameOption("gbk"));
    pconn->connect(db_.empty()       ?  0 : db_.c_str(),
                   server_.empty()   ?  0 : server_.c_str(),
                   user_.empty()     ?  0 : user_.c_str(),
                   password_.empty() ? "" : password_.c_str());
    return pconn;
}

inline void mysqlpp_connection_pool::destroy(mysqlpp::Connection* cp)
{
    delete cp;
}

// �������ӳ�ʱʱ�� 3��
inline unsigned int mysqlpp_connection_pool::max_idle_time()
{
    return 3;
}

#endif   // __MYSQL_CONNECTION_POOL_H__