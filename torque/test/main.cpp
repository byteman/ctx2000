#include <stdio.h>
#include "torque_db.h"
#include <Poco/Thread.h>
#include <Poco/Format.h>
#include <Poco/Timespan.h>
#include "torque_mgr.h"
Poco::Timestamp now;
bool torque_test_write(CTorQueDB &db)
{
    now.update();
    TorQueInfo info;
    TorQueSet  rst;
    /*
    for(int i = 5; i < 1; i--)
    {
        info.len    =  Poco::format("%d",i);
        info.weight =  Poco::format("%0.2f",(double)i+32.3);;
        rst.push_back(info);
    }*/
    info.len    =  "123";
    info.weight =  "9.3";
    rst.push_back(info);
    info.len    =  "103";
    info.weight =  "9.3";
    rst.push_back(info);
    info.len    =  "113";
    info.weight =  "9.3";
    rst.push_back(info);
    info.len    =  "134";
    info.weight =  "9.3";
    rst.push_back(info);
    info.len    =  "136";
    info.weight =  "9.3";
    rst.push_back(info);
    info.len    =  "13";
    info.weight =  "9.3";
    rst.push_back(info);

    now.update();
    db.addTorQueInfo("tc101","100","2",rst);
    db.addTorQueInfo("tc101","100","4",rst);
    db.addTorQueInfo("tc101","100","6",rst);
    db.addTorQueInfo("tc102","120","2",rst);
    db.addTorQueInfo("tc103","140","2",rst);
    printf("torque_test_write elasep = %dms\n",now.elapsed()/1000);


}
bool torque_test()
{
    CTorQueDB db;
    TStrList tclist;

    if(!db.open("torque.db"))
    {
        printf("can not open torque.db");
        return false;
    }
    printf("open elase = %dms\n",now.elapsed()/1000);
    torque_test_write(db);
    if(db.listAllTCTypes(tclist))
    {
        for(size_t i = 0; i < tclist.size(); i++)
        {
            printf("tc type[%d] %s\n",i,tclist.at(i).c_str());
        }
    }
    return false;
}
bool torque_mgr_test()
{
    CTorQueMgr::get ().loadCfg ("tc101","100","6");

    ETorQueState st = CTorQueMgr::get().getState (3,32);
    switch(st){
        case TQ_NORMAL:
            printf("normal\n");
            break;
        case TQ_ALTER:
            printf("alter\n");
            break;
        case TQ_WARN:
            printf("warn\n");
            break;
        case TQ_ALARM:
            printf("alarm\n");
            break;
        case TQ_FATAL:
            printf("fatal\n");
            break;
        default:
            printf("unkown\n");
            break;
    };
    for(int j = 0 ; j < 10; j++)
    {
        CTorQueMgr::get ().changeRate();

        printf("chage rate = %d\n",j);
    }
}
int main(int argc, char *argv[])
{
    torque_test();
    torque_mgr_test();
    //pause();
}
