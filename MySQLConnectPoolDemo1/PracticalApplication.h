#pragma once
#include "mysqlpp_connection_pool.hpp"
#include "const_data.hpp"
class PracticalApplication
{
public:
    PracticalApplication(void);
    ~PracticalApplication(void);
    // 打印数据	
    DB_RESULT PrintData();
    // 添加数据到数据库
    DB_RESULT AddDataToDB();
    // 把数据库数据序列化，然后保存到文件
    DB_RESULT SaveDataToFile();
    // 把文件中的数据反序列化，然后放到数据库中
    DB_RESULT LoadFileDataToDB();
    // 线程池指针
    mysqlpp_connection_pool* poolptr;
    //GB2312到UTF-8的转换
    char* G2U(const char* gb2312);  // 用完记得把返回值delete[]
};

