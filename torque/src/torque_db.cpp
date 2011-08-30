//---------------------------------------------------------------------------

#include "torque_db.h"
#include <stdio.h>
#include <assert.h>
#include <Poco/NumberParser.h>
//---------------------------------------------------------------------------
#ifndef TC_DEBUG
#define TC_DBG 
#else

void __inline TC_DBG(const char *format, ...)
{
    fprintf(stderr,"%s",format);
}
#endif
TorQueInfo::TorQueInfo()
{
    len = "";
    weight = "";

}
bool TorQueInfo::tryGetLen(double &value)
{
    double tmp = 0;
    if(Poco::NumberParser::tryParseFloat(len,tmp))
    {
        value = tmp;
        return true;
    }
    return false;
}
bool TorQueInfo::tryGetWeight(double &value)
{
    double tmp = 0;
    if(Poco::NumberParser::tryParseFloat(weight,tmp))
    {
        value = tmp;
        return true;
    }
    return false;
}
CTorQueDB::CTorQueDB():m_open(false)

{

}
/*
check db is open?
*/
void CTorQueDB::checkDB()
{
    assert(m_open);
}
bool CTorQueDB::open(std::string path)
{
  try{
      m_DB.open(path.c_str());
      m_open = true;
      if(!existData())
        if(!createParamTable()) {
            m_open = false;
            return false;
        }
  }catch(...)
  {
   return false;

  }
  return true;

}
bool CTorQueDB::existData()
{
  checkDB();
  return m_DB.tableExists("tcparam");
}
bool CTorQueDB::listAllTCTypes(TStrList& types)
{
  checkDB();
  try{
    m_lastSql.format("select distinct Type from tcparam");
    CppSQLite3Query qry = m_DB.execQuery(m_lastSql);
    while (!qry.eof()) {
       types.push_back(qry.fieldValue("Type"));
       qry.nextRow();
    }
    qry.finalize();
  }catch (CppSQLite3Exception& e) {
    TC_DBG("%s",e.errorCodeAsString(e.errorCode()));
    return false;
  }
  return true;
}
bool CTorQueDB::listArmLenByType(std::string tyname, TStrList& armlens)
{
checkDB();
  try{
    //CppSQLite3Buffer sql;
    m_lastSql.format("select distinct Length from tcparam where Type=%Q",tyname.c_str());
    CppSQLite3Query qry = m_DB.execQuery(m_lastSql);
    while (!qry.eof()) {
       armlens.push_back(qry.fieldValue("Length"));
       qry.nextRow();
    }
    qry.finalize();
  }catch (CppSQLite3Exception& e) {
    TC_DBG("%s",e.errorCodeAsString(e.errorCode()));
    return false;
  }
  return true;
}
bool CTorQueDB::listRateByLen(std::string tyname, std::string armlen,TStrList& rates)
{
    checkDB();
   try{
    //CppSQLite3Buffer sql;
    m_lastSql.format("select distinct Rate from tcparam where Type=%Q and Length=%Q",tyname.c_str(),armlen.c_str());
    //fprintf(stderr,"%s\n",m_lastSql.data ());
    CppSQLite3Query qry = m_DB.execQuery(m_lastSql);
    while (!qry.eof()) {
       rates.push_back(qry.fieldValue("Rate"));
       qry.nextRow();
    }
    qry.finalize();
  }catch (CppSQLite3Exception& e) {
    TC_DBG("%s",e.errorCodeAsString(e.errorCode()));
    return false;
  }
   return true;
}
bool CTorQueDB::listTorQueInfoByRate(std::string tyname, std::string armlen, std::string rate,TorQueSet& rst)
{
    checkDB();
   try{
    //CppSQLite3Buffer sql;
    m_lastSql.format("select * from tcparam where Type=%Q and Length=%Q and rate=%Q",tyname.c_str(),armlen.c_str(),rate.c_str());
    CppSQLite3Query qry = m_DB.execQuery(m_lastSql);
    while (!qry.eof()) {
       TorQueInfo info;
       info.len    = qry.fieldValue("Distance");
       info.weight = qry.fieldValue("Weight");
       rst.push_back(info);
       qry.nextRow();
    }
    qry.finalize();
  }catch (CppSQLite3Exception& e) {
    TC_DBG("%s",e.errorCodeAsString(e.errorCode()));
    return false;
  }
   return true;
}
bool CTorQueDB::addTorQueInfo(std::string tyname, std::string armlen, std::string rate,TorQueInfo& item)
{
    checkDB ();
    bool ret = false;
    try{
            m_DB.execDML("BEGIN;");
            m_lastSql.format("Insert or rollback into tcparam (No,Type,Length,Rate,Distance,Weight) values(NULL,%Q,%Q,%Q,%Q,%Q);", \
                        tyname.c_str(),armlen.c_str(),rate.c_str(),item.len.c_str(),item.weight.c_str());
            int rows = m_DB.execDML(m_lastSql);
            if (rows) {
              ret = true;
            }else
            {
               ret = false;
            }
            m_DB.execDML("COMMIT;");
    }
    catch (CppSQLite3Exception& e) {
            TC_DBG("%s",m_lastErr.c_str());
            ret = false;
            formatError(e);
            m_DB.execDML("ROLLBACK;");
    }
    return ret;
}
/*
采用事务方式，否则每插入一条数据就会开启和关闭事务一次，导致每插入一条差不多100ms
*/
bool CTorQueDB::addTorQueInfo(std::string tyname, std::string armlen, std::string rate,TorQueSet& rst)
{
    checkDB();
    bool ret = false;
    if(!delTorQueInfo(tyname,armlen,rate)) return false;
   	try{
            m_DB.execDML("BEGIN;");
            for(size_t i = 0 ; i < rst.size(); i++)
            {
                m_lastSql.format("Insert or rollback into tcparam (No,Type,Length,Rate,Distance,Weight) values(NULL,%Q,%Q,%Q,%Q,%Q);", \
                            tyname.c_str(),armlen.c_str(),rate.c_str(),rst.at(i).len.c_str(),rst.at(i).weight.c_str());
                  int rows = m_DB.execDML(m_lastSql);
                  if (rows) {
                      ret = true;
                      continue;
                  }else
                  {
                       ret = false;
                       break;
                  }
            }
            m_DB.execDML("COMMIT;");
	}
	catch (CppSQLite3Exception& e) {
            TC_DBG("%s",m_lastErr.c_str());
            ret = false;
            formatError(e);
            m_DB.execDML("ROLLBACK;");
	}
	return ret;
}

bool CTorQueDB::findDistance(std::string tyname, std::string armlen, std::string rate, std::string dist)
{
    checkDB();
 bool find = false;
 try{
    //CppSQLite3Buffer sql;
    m_lastSql.format("select * from tcparam where Type=%Q and Length=%Q and rate=%Q and Distance=%Q",tyname.c_str(),armlen.c_str(),rate.c_str(),dist.c_str());
    CppSQLite3Query qry = m_DB.execQuery(m_lastSql);
    if (!qry.eof()) {
       find = true;
    }
    qry.finalize();
  }catch (CppSQLite3Exception& e) {
    formatError(e);
    TC_DBG("%s",m_lastErr.c_str());
    return false;
  }
   return find;

}
bool CTorQueDB::delTorQueInfo(std::string tyname, std::string armlen, std::string rate, TorQueInfo& item)
{
    checkDB();
    try{
            m_lastSql.format("delete from tcparam where Type=%Q and Length=%Q and rate=%Q and Distance=%Q and Weight=%Q", \
                            tyname.c_str(),armlen.c_str(),rate.c_str(),item.len.c_str (),item.weight.c_str ());
            int rows = m_DB.execDML(m_lastSql);
            return true;
    }
    catch (CppSQLite3Exception& e) {
        formatError(e);
    }
    return false;
}
bool CTorQueDB::delTorQueInfo(std::string tyname, std::string armlen, std::string rate)
{
    checkDB();
    try{
            m_lastSql.format("delete from tcparam where Type=%Q and Length=%Q and rate=%Q", \
                            tyname.c_str(),armlen.c_str(),rate.c_str());
            int rows = m_DB.execDML(m_lastSql);
            return true;
	}
	catch (CppSQLite3Exception& e) {
		formatError(e);
	}
	return false;
}
bool CTorQueDB::delTorQueInfo(std::string tyname, std::string armlen)
{
    checkDB();
     try{
		m_lastSql.format("delete from tcparam where Type=%Q and Length=%Q", \
				tyname.c_str(),armlen.c_str());
		int rows = m_DB.execDML(m_lastSql);
		if (rows) {
			return true;
		}else
		{
            return false;
        }
	}
	catch (CppSQLite3Exception& e) {
		formatError(e);
	}
	return false;
}
bool CTorQueDB::delTorQueInfo(std::string tyname)
{
    checkDB();
     try{
		m_lastSql.format("delete from tcparam where Type=%Q", \
				tyname.c_str());
		int rows = m_DB.execDML(m_lastSql);
		if (rows) {
			return true;
		}else
		{
            return false;
        }
	}
	catch (CppSQLite3Exception& e) {
		formatError(e);
	}
	return false;
}
bool CTorQueDB::createParamTable()
{

     try{
		m_lastSql.format("CREATE TABLE  if not exists tcparam (No INTEGER NOT NULL PRIMARY KEY,Rate TEXT NOT NULL, Length TEXT NOT NULL,Distance TEXT NOT NULL,Weight TEXT NOT NULL, Type TEXT NOT NULL);");
		int rows = m_DB.execDML(m_lastSql);
                return true;
        /*
		if (rows) {
			return true;
		}else
		{
            return false;
        } */
	}
	catch (CppSQLite3Exception& e) {
		formatError(e);
	}
	return false;

}
bool CTorQueDB::modifyTorQueInfo(std::string  tyname,  std::string  armlen, std::string  rate,TorQueInfo& olditem,TorQueInfo& newitem)
{

    try{
        m_lastSql.format("update  tcparam set Distance=%Q, Weight=%Q where Type=%Q and Length=%Q and Rate=%Q and Distance = %Q Weight=%Q;", \
                        newitem.len.c_str (),newitem.weight.c_str(),tyname.c_str (),armlen.c_str (),rate.c_str (),olditem.len.c_str(),olditem.weight.c_str());
        int rows = m_DB.execDML(m_lastSql);
        if (rows) {
             return true;
        }
    }
    catch (CppSQLite3Exception& e) {
        formatError(e);
    }
    return false;
}
void CTorQueDB::formatError(CppSQLite3Exception& e)
{

     m_lastErr = std::string(e.errorCodeAsString(e.errorCode())) + " : "  + std::string(m_lastSql.data());
}
