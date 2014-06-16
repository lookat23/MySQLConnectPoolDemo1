#include "mysqlpp_connection.hpp"
#include "mysqlpp_connection_pool.hpp"
#include "addressbook.pb.h"
#include "const_data.hpp"
#include <fstream>
#include <iostream>
#include "ConnectionThread.hpp"
#include "single_items.h"
#include <process.h>

// �߳��¼�
HANDLE THREAD_HANDLE;

int main()
{
    // ��ʼ���߳��¼�
    THREAD_HANDLE = CreateEvent(NULL, FALSE, FALSE, NULL);
    srand((unsigned int)time(0));

    BOOL* arrRunning = new BOOL[THREAD_NUM];

    const size_t num_threads = sizeof(arrRunning) / sizeof(arrRunning[0]);

    for(size_t i = 0; i < num_threads; i++)
    {
        arrRunning[i] = true;
    }

    for(size_t i = 0; i< num_threads; ++i)
    {
        THREADPARAM tp;
        tp.running = arrRunning + i;
        tp.pClassPtr = (void*)&(g_conn_thread::instance());
        if(FALSE == _beginthreadex(NULL, 0, ConnectionThread::worker_thread, &tp, 0, NULL))
        {
            std::cerr << "Failed to craete thread !" << i <<std::endl;
            return 1;
        }
        std::cout << "�߳�" << i + 1 << "��ʼ����" << std::endl;
        SetEvent(THREAD_HANDLE);
    }
    size_t i;
    do 
    {
        Sleep(1000);
        i = 0;
        while(i < num_threads && arrRunning[i] == false ) ++i;
    } while (i < num_threads);
    std::cout << "�����߳��Ѿ�ֹͣ" << std::endl;

    return 0;
}