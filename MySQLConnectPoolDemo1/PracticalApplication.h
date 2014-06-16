#pragma once
#include "mysqlpp_connection_pool.hpp"
#include "const_data.hpp"
class PracticalApplication
{
public:
    PracticalApplication(void);
    ~PracticalApplication(void);
    // ��ӡ����	
    DB_RESULT PrintData();
    // ������ݵ����ݿ�
    DB_RESULT AddDataToDB();
    // �����ݿ��������л���Ȼ�󱣴浽�ļ�
    DB_RESULT SaveDataToFile();
    // ���ļ��е����ݷ����л���Ȼ��ŵ����ݿ���
    DB_RESULT LoadFileDataToDB();
    // �̳߳�ָ��
    mysqlpp_connection_pool* poolptr;
    //GB2312��UTF-8��ת��
    char* G2U(const char* gb2312);  // ����ǵðѷ���ֵdelete[]
};

