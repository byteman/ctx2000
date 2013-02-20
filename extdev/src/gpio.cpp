#include "gpio.h"
#include <string>
#include <bitset>
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/Exception.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "dataacquire.h"

#define MAX_OUTPUT_PORT 12
#ifdef DEBUG
#define DBG(fmt...) fprintf(stderr,fmt);
#else
#define DBG(fmt...) do { } while (0)
#endif


CGpio::CGpio()
{
    m_init_ok = false;
}

/*
����������IO�豸��Ĭ�ϲ��������ò���
ֻ��������ʱ����һ�Σ����Բ��ü���
*/
bool    CGpio::Init(int num_input, int num_output,bool reset)
{


     m_init_ok = true;
  

	 return true;
}

IO_STATUS CGpio::Input(IO_TYPE type, int index)
{
	
}

IO_STATUS CGpio::Output(int index, IO_STATUS status)
{
    int ret = CDataAcquire::Get().output(index,status);
    
    if(ret != -1) return status;
    
    return IO_INVALID;
}
bool      CGpio::CreateVirtualFiles()
{
	return true;
}
