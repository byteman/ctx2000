#ifndef GPIO_H
#define GPIO_H

enum IO_TYPE{
    IO_INPUT,
    IO_OUTPUT
};
enum IO_STATUS{
    IO_LOW_LEVEL    = 0,
    IO_HIGH_LEVEL   = 1,
    IO_LOW_2_HIGH   = 2 ,
    IO_HIGH_2_LOW   = 3,
    IO_INVALID           = 4
};

class CGpio
{
public:
    CGpio();
    bool      Init(int num_input=8, int num_output=8,bool reset=true);
    IO_STATUS Output(int index, IO_STATUS status);
    IO_STATUS Input( IO_TYPE type, int index);
    bool      CreateVirtualFiles();
private:
    bool      m_init_ok;
};

#endif // GPIO_H
