#include "PracticalApplication.h"
#include "const_data.hpp"
#include <mysql++.h>
#include "single_items.h"
#include "conio.h"
#include "mysql_test_db.pb.h"
#include <fstream>

using namespace std;

sql_create_5(person, 1, 5,
    mysqlpp::sql_int, id,
    mysqlpp::sql_varchar, name,
    mysqlpp::sql_varchar, email,
    mysqlpp::sql_int, phonenumber,
    mysqlpp::sql_varchar, unsure)


PracticalApplication::PracticalApplication(void)
{
    poolptr = &g_conn_pool::instance();
}


PracticalApplication::~PracticalApplication(void)
{
}

// 打印数据	
DB_RESULT PracticalApplication::PrintData()
{
    INIT_MYSQL_CONNECTION();

    query << "select * from person";
    mysqlpp::UseQueryResult use_res = query.use();
    if(use_res == false)
    {
        cerr << "Failed to get stock item: " << query.error() << endl;
        return DR_FAIL;
    }
    cout << "**************************************************" << endl;
    while(mysqlpp::Row row = use_res.fetch_row())
    {
        for(size_t i = 0; i < row.size(); i++)
        {
            cout << setw(10) << row[i] << ' ';
        }
        cout << endl;
    }
    cout << "显示完毕，请按回车!" << endl;
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
    // getchar()一定要回车
    //getchar();
    // _getch() 能实现按任意键退出
    _getch();
    return DR_SUCCEED;
}
// 添加数据到数据库
DB_RESULT PracticalApplication::AddDataToDB()
{
    //person row(10, "ken", "yyy@q.com", "12312123", mysqlpp::null);
    INIT_MYSQL_CONNECTION();
    //query.insert(row);
    string strName;
    string strEmail;
    int    nPhoneNumber;
    string strUnsure;
    cout << "输入名字: ";
    cin >> strName;
    cin.ignore(numeric_limits<streamsize>::max() ,'\n');
    cout << "输入Email: ";
    cin >> strEmail;
    cin.ignore(numeric_limits<streamsize>::max() ,'\n');
    cout << "输入手机号: ";
    while(!(cin >> nPhoneNumber))
    {
        cin.clear();
        cin.sync();
        cout << endl;
        cout << "手机号码只能输入数字，请重新输入" << endl;
    }
    cin.ignore(numeric_limits<streamsize>::max() ,'\n');
    cout << "输入备注: ";
    cin >> strUnsure;
    cin.ignore(numeric_limits<streamsize>::max() ,'\n');
    query << "insert into person"
        <<
        "(name, email, phonenumber, unsure)"
        << "values(" 
        << mysqlpp::quote << strName <<
        ","
        << mysqlpp::quote << strEmail <<
        ","
        << nPhoneNumber <<
        ","
        << mysqlpp::quote << strUnsure <<
        ");";

    mysqlpp::SimpleResult res = query.execute();
    if( res == false )
    {
        cerr << "failed to insert err!" <<
            "errnum: " << query.errnum() << " err: " <<
            query.error() << endl;
        return DR_FAIL;
    }
}
// 把数据库数据序列化，然后保存到文件
DB_RESULT PracticalApplication::SaveDataToFile()
{
    // 把数据库数据拿出来
    INIT_MYSQL_CONNECTION();

    query << "select * from person";
    mysqlpp::UseQueryResult use_res = query.use();
    if(use_res == false)
    {
        cerr << "Failed to get stock item: " << query.error() << endl;
        return DR_FAIL;
    }
    Mysql_Test_DB test_db;
    while(mysqlpp::Row row = use_res.fetch_row())
    {
        int nID;
        char* pzName = NULL;
        char* pzEmail = NULL;
        int nPhoneNumber = 0; 
        char* pzUnsure = NULL;
        nID             = row["id"];
        pzName          = G2U(row["name"].c_str());
        pzEmail         = G2U(row["email"].c_str());
        nPhoneNumber    = row["phonenumber"];
        pzUnsure        = G2U(row["unsure"].c_str());
        Person* pPerson = test_db.add_person();
        pPerson->set_id(nID);
        pPerson->set_name(pzName);
        pPerson->set_email(pzEmail);
        pPerson->set_phone(nPhoneNumber);
        pPerson->set_unsure(pzUnsure);
        if(pzName) delete[] pzName;
        if(pzEmail) delete[] pzEmail;
        if(pzUnsure) delete[] pzUnsure;
    }
    // 把序列化后的数据放进文件中
    ofstream of(DATA_FILE_NAME, ios::binary);
    if(of.bad())
    {
        cerr << "Open file " << DATA_FILE_NAME 
            << " error!" << endl;
        return DR_FAIL;
    }
    if(!(test_db.SerializePartialToOstream(&of)))
    {
        cerr << "Failed to serial from ostream!" << endl;
        return DR_FAIL;
    }
    of.close();
    return DR_SUCCEED;
}
// 把文件中的数据反序列化，然后放到数据库中
DB_RESULT PracticalApplication::LoadFileDataToDB()
{
    ifstream ifs(DATA_FILE_NAME, ios::binary);
    if(ifs.bad())
    {
        cerr << "Open file " << DATA_FILE_NAME
            << "error!" << endl;
        return DR_FAIL;
    }
    Mysql_Test_DB test_db;
    test_db.Clear();
    if(!(test_db.ParsePartialFromIstream(&ifs)))
    {
        cerr << "Failed to parse from istream!" << endl;
        return DR_FAIL;
    }
    // 把序列化数据放进数据库
    INIT_MYSQL_CONNECTION();
    size_t personSize = test_db.person_size();
    for(size_t i = 0; i < personSize; i++ )
    {
        Person person = test_db.person(i);
        query << "insert into person"
        <<
        "(name, email, phonenumber, unsure)"
        << "values(" 
        << mysqlpp::quote << person.name() <<
        ","
        << mysqlpp::quote << person.email() <<
        ","
        << person.phone() <<
        ","
        << mysqlpp::quote << person.unsure() <<
        ");";
        mysqlpp::SimpleResult res = query.execute();
        if( res == false )
        {
            cerr << "failed to insert err!" <<
                "errnum: " << query.errnum() << " err: " <<
                query.error() << endl;
            return DR_FAIL;
        }
    }
    ifs.close();
    return DR_SUCCEED;
}

char* PracticalApplication::G2U(const char* gb2312)
{
   int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
   wchar_t* wstr = new WCHAR[len + 1];
   memset(wstr, 0, len+1);
   MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
   len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
   char* str = new char[len + 1];
   memset(str, 0, len+1);
   WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
   if(wstr) delete[] wstr;
   return str;
   
}