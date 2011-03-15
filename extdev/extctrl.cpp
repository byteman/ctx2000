#include "extctrl.h"
#include "gpio.h"
#include <yatephone.h>

using namespace TelEngine;

class ExtCtrlHandler : public MessageHandler
{
public:
    ExtCtrlHandler(const char *name);
    virtual bool received(Message &msg);
private:
    CGpio gpio;
};

ExtCtrlHandler::ExtCtrlHandler(const char *name)
    : MessageHandler(name)
{
    Output("ExtCtrlHandler\n");
    gpio.CreateVirtualFiles();
    gpio.Init();
}
bool ExtCtrlHandler::received(Message &msg)
{
    int ret = IO_INVALID;
    int       index  = msg.getIntValue("port", -1);
    IO_STATUS status = (IO_STATUS)msg.getIntValue("output",IO_INVALID);
    if( (index != -1) || (status != IO_INVALID))
    {
        gpio.Output(index,status);
        ret = gpio.Input(IO_OUTPUT,index);
    }

    msg.setParam("ret",String(ret));
    Output("set port %d = %d\n",index,ret);
    return true;
}

class ExtCtrlPlugin :public Plugin
{
public:
    ExtCtrlPlugin();
    virtual ~ExtCtrlPlugin();
    virtual void initialize();
};

ExtCtrlPlugin::ExtCtrlPlugin()
{
}
ExtCtrlPlugin::~ExtCtrlPlugin()
{
}
void ExtCtrlPlugin::initialize()
{
    Output("Initializing module ExtCtrl");
    //Configuration cfg(Engine::configFile("extctrl"));
    Engine::install(new ExtCtrlHandler("ext.ctrl"));
}
INIT_PLUGIN(ExtCtrlPlugin);
