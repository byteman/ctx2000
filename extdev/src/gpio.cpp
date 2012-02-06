#include "gpio.h"
#include <string>
#include <bitset>
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/Exception.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#ifdef  _PC_DBG
    #define REG_DIR0      "/dev/shm/direction0"
    #define REG_DIR1      "/dev/shm/direction1"
    #define REG_INPUT0    "/dev/shm/input0"
    #define REG_INPUT1    "/dev/shm/input1"
    #define REG_OUTPUT0   "/dev/shm/output0"
    #define REG_OUTPUT1   "/dev/shm/output1"
#else
    #define REG_DIR0     "/sys/class/i2c-dev/i2c-1/device/1-0020/direction0"
    #define REG_DIR1     "/sys/class/i2c-dev/i2c-1/device/1-0020/direction1"
    #define REG_INPUT0   "/sys/class/i2c-dev/i2c-1/device/1-0020/input0"
    #define REG_INPUT1   "/sys/class/i2c-dev/i2c-1/device/1-0020/input1"
    #define REG_OUTPUT0  "/sys/class/i2c-dev/i2c-1/device/1-0020/output0"
    #define REG_OUTPUT1  "/sys/class/i2c-dev/i2c-1/device/1-0020/output1"
#endif
#define MAX_OUTPUT_PORT 12
#ifdef DEBUG
#define DBG(fmt...) fprintf(stderr,fmt);
#else
#define DBG(fmt...) do { } while (0)
#endif
static std::string gRegFiles[] = {
    REG_DIR0,
    REG_DIR1,
    REG_INPUT0,
    REG_INPUT1,
    REG_OUTPUT0,
    REG_OUTPUT1
};

CGpio::CGpio()
{
    m_init_ok = false;
}

/*
设置启动后IO设备的默认参数，配置参数
只会在启动时调用一次，所以不用加锁
*/
bool    CGpio::Init(int num_input, int num_output,bool reset)
{

       DBG("dir reg = %s\n",REG_DIR0);
       Poco::File f0(REG_DIR0);
       Poco::File f1(REG_DIR1);
       if ( (!f0.exists () ) || (!f1.exists ()))
       {
            DBG("%s don't exist \n",REG_DIR0);
            m_init_ok = false;
            return false;
       }
        try
        {
            Poco::FileStream fos0(REG_DIR0);
            Poco::FileStream fos1(REG_DIR1);


            Poco::FileStream fos2(REG_OUTPUT0);
            Poco::FileStream fos3(REG_OUTPUT1);

            fos0.seekg(0, std::ios::beg);
            fos1.seekg(0, std::ios::beg);
//set direction
            if(reset)
            {
                fos2 << 0x0 << std::endl;   //output0
                fos3 << 0x0 << std::endl;   //output1
            }
            fos0 << 0;   //output0
            fos1 << 0;   //output1
            fos0.close();
            fos1.close();
            fos2.close();
            fos3.close();
            m_init_ok = true;
            return true;
        }
        catch(Poco::Exception& e)
        {
            std::cerr << e.displayText () << std::endl;
        }catch(std::exception &e)
        {
            std::cerr << e.what () << std::endl;
        }catch(...)
        {
            std::cerr << "InitDefaultParam fail" << std::endl;
            system("echo gpio `date` >> err.out");
        }
        return false;
}

IO_STATUS CGpio::Input(IO_TYPE type, int index)
{
    int value = 0;
    if(!m_init_ok) return IO_INVALID;
    std::string regfile = REG_INPUT1;
    if(type == IO_OUTPUT){
        if(index >=8 )
        {
            regfile = REG_OUTPUT0;
        }else{
            regfile = REG_OUTPUT1;
        }
    }else if(type == IO_INPUT){

    }
    index = 7 - (index%8);
    Poco::FileStream reg(regfile);
    reg.seekg(0, std::ios::beg);

    reg >> value;
    std::bitset<32>    status_bits;
    status_bits = value;
    return status_bits[index]?IO_HIGH_LEVEL:IO_LOW_LEVEL;
}
IO_STATUS CGpio::Output(int index, IO_STATUS status)
{
    std::string regfile = REG_OUTPUT1;
    unsigned int output = 0;
    if(!m_init_ok) return IO_INVALID;
    if(index >=8 )
    {
        regfile = REG_OUTPUT0;
    }
    index = 7 - (index%8);
    //fprintf(stderr,"output %d to %d in %s\n",index,status,regfile.c_str());
    Poco::FileStream reg(regfile);
    reg.seekg(0, std::ios::beg);

    reg >> output;

    //bitset<32>   status;
    if(  index >= MAX_OUTPUT_PORT )
    {
        //DBG("%s port too large\n",__FUNCTION__);
        return IO_INVALID;
    }
    if(status == IO_LOW_LEVEL)
    {
         output |= (unsigned int)(1 << index);
    }else if(status == IO_HIGH_LEVEL){
         output &= (unsigned int)(~( 1 << index ) );
    }
    reg.seekg(0, std::ios::beg);
    reg << output << std::endl;
    reg.close();
    return status;
}
bool    CGpio::CreateVirtualFiles()
{
/*
  must be 4bytes
*/
    //static int default_values[]={255,0,63,0,255,0};
    fprintf(stderr,"CreateVirtualFiles\n");
    static std::string default_values[]={"255","0","63","0","255","0"};
    for( int i = 0; i < 6; i++)
    {
            Poco::File file(gRegFiles[i]);
            //创建成功返回true，已经存在返回false
            if( file.createFile ())
            {
                Poco::FileStream fos(gRegFiles[i]);
                fos.seekg(0, std::ios::beg);
                fos << default_values[i] << std::endl;
                fos.close();
            }

    }
    return true;
}
