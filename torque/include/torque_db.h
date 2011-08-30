//---------------------------------------------------------------------------

#ifndef TcDBH
#define TcDBH
#include <string>
#include <vector>
#include "CppSQLite3.h"
typedef std::vector<std::string> TStrList;
//---------------------------------------------------------------------------
struct TorQueInfo{
public:
      TorQueInfo();
      TorQueInfo(std::string l, std::string w):
          len(l),weight(w)
      {
      }
      std::string len;
      std::string weight;
      double      dbl_len;
      double      dbl_weight;
      bool tryGetLen(double &value);
      bool tryGetWeight(double &value);
};
typedef std::vector< TorQueInfo > TorQueSet;
class CTorQueDB{
public:
     CTorQueDB();
     bool open(std::string path);
     bool existData();
     bool listAllTCTypes(TStrList& types);
     bool listArmLenByType(std::string tyname, TStrList& armlens);
     bool listRateByLen(std::string tyname, std::string armlen,TStrList& rates);
     bool listTorQueInfoByRate(std::string tyname, std::string armlen, std::string rate,TorQueSet& rst);
     bool addTorQueInfo(std::string tyname, std::string armlen, std::string rate,TorQueSet& rst);
     bool addTorQueInfo(std::string tyname, std::string armlen, std::string rate,TorQueInfo& item);

     bool findDistance(std::string tyname, std::string armlen, std::string rate, std::string dist);
     bool delTorQueInfo(std::string tyname, std::string armlen, std::string rate, TorQueInfo& item);
     bool delTorQueInfo(std::string tyname, std::string armlen, std::string rate);
     bool delTorQueInfo(std::string tyname, std::string armlen);
     bool delTorQueInfo(std::string tyname);

     bool modifyTorQueInfo(std::string  tyname,  std::string  armlen, std::string  rate,TorQueInfo& olditem,TorQueInfo& newitem);
     bool createParamTable();
     std::string  m_lastErr;
     //std::string  m_lastSql;
private:
     void checkDB();
     void formatError(CppSQLite3Exception& e);
     CppSQLite3DB m_DB;
     CppSQLite3Buffer m_lastSql;
     bool m_open;



};

#endif
