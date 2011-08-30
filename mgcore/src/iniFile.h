#ifndef __INIFILE_H
#define __INIFILE_H

#include <iostream>
using namespace std;

class TIniFile
{
public:
    TIniFile();
    TIniFile(string FileName);
    virtual ~TIniFile();
    virtual double ReadFloat(const string Section, const string Ident, double Default);
    virtual int    ReadInteger(const string Section, const string Ident, int Default);
    virtual string ReadString(const string Section, const string Ident, const string Default);

    virtual void   WriteFloat(const string Section, const string Ident, double Value);
    virtual void   WriteInteger(const string Section, const string Ident, int Value);
    virtual void   WriteString(const string Section, const string Ident, const string Value);
    virtual void   WriteBool(const string Section, const string Ident, bool Value);
    virtual bool   ReadBool(const string Section, const string Ident, bool Default);
    virtual bool   ValueExists(const string Section, const string Ident);
    virtual bool   SectionExists(const string Section);

    virtual string ReadString(const string Ident, const string Default);

    virtual void   WriteString(const string Ident, const string Value);

    string m_FileName;
private:
    bool CheckValid();
};

#endif
