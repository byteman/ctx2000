#include "CppSQLite3.h"
#include <stdio.h>
#include <iostream>
int main()
{
    bool bExist = false;
    CppSQLite3DB db;
    db.open("ctx2000.sqlite3");
    CppSQLite3Buffer strSql;
    const char* tmp = strSql.format("select * from tblworksite");
    try{
        CppSQLite3Query qry =  db.execQuery(tmp);
        if(!qry.eof())
        {
            std::cerr << qry.getFloatField("alert_dist") << std::endl;
        }    
        qry.finalize();
    }
    catch (CppSQLite3Exception& e) {
            std::cerr << "loadworksite :" << e.errorCode() << ":" << e.errorMessage() << std::endl;
            bExist = false;
    }
    return bExist;
}
