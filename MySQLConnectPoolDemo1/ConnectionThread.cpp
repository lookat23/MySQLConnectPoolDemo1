#include "ConnectionThread.hpp"
#include "mysqlpp_connection.hpp"
#include "const_data.hpp"
#include <fstream>
#include "single_items.h"

using namespace std;

extern HANDLE THREAD_HANDLE;

ConnectionThread::ConnectionThread(void)
{
    poolptr = &g_conn_pool::instance();
    if(poolptr)
    {
        poolptr->set_connect_info(   
            "localhost",
            "root",
            "",
            "mysql_test_db"
            );


        try
        {
            mysqlpp_connection mc(*poolptr, true);
            if(!mc->thread_aware())
            {
                std::cerr << "MySQL++ wasn't built with thread awareness!" <<
                    std::endl;
                exit(-1);
            }
        }
        catch (mysqlpp::Exception& e)
        {
            std::cerr << "Failed to initial pool connection!" <<
                e.what() << std::endl;
            exit(-1);
        }
    }
}


ConnectionThread::~ConnectionThread(void)
{
    poolptr = NULL;
}

unsigned __stdcall ConnectionThread::worker_thread (void * lpThreadParameter )
{
    WaitForSingleObject(THREAD_HANDLE, INFINITE);
    THREADPARAM* pTp = static_cast<THREADPARAM*>(lpThreadParameter);
    ConnectionThread* pConnThread = static_cast<ConnectionThread*>(pTp->pClassPtr);
    mysqlpp_connection_pool* poolptr = pConnThread->poolptr;

    mysqlpp_connection mc(*poolptr, true);
    if(!mc)
    {
        std::cerr << "Failed to get a connection from the pool!" 
            << std::endl;
    }

    //     // 序列初始化
    //     AddressBook addressBook;
    //     Person * person = addressBook.add_person();
    //     person->set_name("张三");
    //     person->set_id(1234);
    //     person->set_email("zhangsan@163.com");
    //     person->set_unsure("sdfs43lkjlkjfsd");
    //     Person_PhoneNumber* phonenumber = person->add_phone();
    //     phonenumber->set_number("13000000000");
    //     phonenumber->set_type(Person_PhoneType_MOBILE);
    //     addressBook.SerializeAsString()

    //     // 序列化到fstream流
    //     std::fstream fs("addressbook.data", std::ios::out | std::ios::trunc | std::ios::binary);
    //     if(!addressBook.SerializePartialToOstream(&fs))
    //     {
    //         std::cerr << "Failed to serial address book data!" << std::endl;
    //         return 1;
    //     }
    //     std::cout << "Serial address book data successfully!" << std::endl;
    //     fs.close();
    //     fs.clear();

    //     // 序列化到string
    //     FILE* fo = fopen("addressbook1.data", "wb, ccs=UNICODE");
    //     if(fo == NULL)
    //     {
    //          std::cerr << "Failed to create addressbook1.data!" << std::endl;
    //     }   
    //     std::string serialStream;
    //     if(!addressBook.SerializePartialToString(&serialStream))
    //     {
    //         std::cerr << "Failed to serial addressbook1 data!" << std::endl;
    //     }
    //     fwrite(serialStream.c_str(), sizeof(char), addressBook.ByteSize(), fo);
    //     std::cout << "serial address successfully!" << std::endl;
    //     if(fo)
    //     {
    //         fclose(fo);
    //         fo = NULL;
    //     }

    //     std::string parseString;
    //     std::fstream fs("addressbook1.data", std::ios::in | std::ios::binary);
    //     if(!fs)
    //     {
    //         std::cerr << "Failed to open addressbook1.data file" << std::endl;
    //         return 1;
    //     }
    //     char c[5] = {0};
    //     while(!fs.eof())
    //     {
    //         fs.read(c, 4);
    //         size_t len = fs.gcount();
    //         memset(c+len, '/0', 4 - len);
    //         parseString += c;
    //     }
    //     AddressBook addressBook;
    //     addressBook.Clear();
    // 
    //     if(!addressBook.ParseFromString(parseString))
    //     {
    //         std::cerr << "Failed to Parse From parseString!" << std::endl;
    //         return 1;
    //     }
    // 
    //     int personSize = addressBook.person_size();
    //     for(int i = 0; i < personSize; i++)
    //     {
    //         Person p = addressBook.person(0);
    //         std::cout << "Person " << i+1 << ":" << std::endl;
    //         std::cout << "\t" << p.id() << "\t" << p.name();
    //     }

    //     FILE* fo = fopen("addressbook1.data", "rb, ccs=UNICODE");
    //     if( fo == NULL)
    //     {
    //         std::cerr << "Open addressbook1.data failed!" << std::endl;
    //         return 1;
    //     }
    //     int lfilesize = 0;
    //     fseek(fo, 0, SEEK_END);
    //     lfilesize = ftell(fo);
    //     fseek(fo, 0, SEEK_SET);
    // 
    //     char* buffer = new char[lfilesize+1];
    //     if(buffer == NULL)
    //     {
    //         std::cerr << "malloc memory error!" << std::endl;
    //         return 1;
    //     }
    // 
    //     memset(buffer, '\0', sizeof(buffer));
    //     fread(buffer, sizeof(char), lfilesize, fo);
    //     if(fo)
    //     {
    //         fclose(fo);
    //         fo = NULL;
    //     }
    // 
    //     AddressBook addressBook;
    //     addressBook.Clear();
    //     if(!addressBook.ParseFromArray(buffer, lfilesize))
    //     {
    //         std::cerr << "Failed to parse from addressbook.data!" << std::endl;
    //         delete buffer;
    //         buffer = NULL;
    //         return 1;
    //     }
    // 
    //     int personSize = addressBook.person_size();
    //     for(int i = 0; i < personSize; i++)
    //     {
    //         Person p = addressBook.person(0);
    //         std::cout << "Person " << i+1 << ":" << std::endl;
    //         std::cout << "\t" << p.id() << "\t" << p.name();
    //     }
    while(TRUE)
    {
        int nUserSelect = pConnThread->PrintList();
        DB_RESULT db_res = DR_EMPTY;
        switch (nUserSelect)
        {
        case 0:
            // 打印数据库数据
            db_res = g_practical_application::instance().PrintData();
            break;
        case 1:
            // 添加数据到数据库
            db_res = g_practical_application::instance().AddDataToDB();
            break;
        case 2:
            // 把数据库数据序列化，然后保存到文件
            db_res = g_practical_application::instance().SaveDataToFile();
            break;
        case 3:
            // 把文件中的数据反序列化，然后放到数据库中
            db_res = g_practical_application::instance().LoadFileDataToDB();
            break;
        default:
            // 退出
            *(pTp->running) = false;
            return 1;
            break;
        }
        switch(db_res)
        {
        case DR_FAIL:
            cout << "操作失败!" << endl;
            break;
        case DR_SUCCEED:
            cout << "操作成功!" << endl;
            break;
        }
    }
    

//     mysqlpp::Query query(mc->query("select * from stock"));
//     mysqlpp::StoreQueryResult res = query.store();
//     for(size_t i = 0; i < res.num_rows(); i++)
//     {
//         std::cout << res[i][0] << '\t' <<
//             res[i][1] << '\t' << std::endl;
//     }
//     sleep(rand() % 4 +1);

    *(pTp->running) = false;
    /*    *(BOOL*)lpThreadParameter = false;*/
    return 0;
}

int ConnectionThread::PrintList()
{
    string strFile = "功能列表.txt";
    ifstream ifs(strFile.c_str(), NULL);
    if(!ifs)
    {
        cerr << "File open " << strFile << " error!" << endl;
        return -1;
    }
    string strFileData;
    while(!ifs.eof())
    {
        ifs >> strFileData;
        cout << strFileData << endl;
    }
    fflush(stdin);
    return (getc(stdin) - '0');
}