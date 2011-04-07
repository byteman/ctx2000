#ifndef CENTERRECVER_H
#define CENTERRECVER_H
#include <string>
class CenterHandler;
class CenterRecver
{
public:
    CenterRecver();
    static CenterRecver& Get();
    int    Send(char* data, int length);
    int    Send(std::string);
    bool   Start(std::string remoteIp, int port);
private:
    CenterHandler* _handler;
};

#endif // CENTERRECVER_H
