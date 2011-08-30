#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include "iniFile.h"


typedef unsigned int GHANDLE;

#define ETC_MAX_VALUE           128
#define ETC_MAXLINE             1024

/**
* \def ETC_FILENOTFOUND
* \brief No found etc file.
**/
#define ETC_FILENOTFOUND        -1
/**
* \def ETC_SECTIONNOTFOUND
* \brief No found section in etc file.
**/
#define ETC_SECTIONNOTFOUND     -2
/**
* \def ETC_KEYNOTFOUND
* \brief No found key in etc file.
**/
#define ETC_KEYNOTFOUND         -3
/**
* \def ETC_TMPFILEFAILED
* \brief Create tmpfile failed.
**/
#define ETC_TMPFILEFAILED       -4
/**
* \def ETC_FILEIOFAILED
* \brief IO operation failed to etc file.
**/
#define ETC_FILEIOFAILED        -5
/**
* \def ETC_INTCONV
* \brief  Convert the value string to an integer failed.
**/
#define ETC_INTCONV             -6
/**
* \def ETC_INVALIDOBJ
* \brief Invalid object to etc file.
**/
#define ETC_INVALIDOBJ          -7
/**
* \def ETC_READONLYOBJ
* \brief Read only to etc file.
**/
#define ETC_READONLYOBJ         -8
/**
* \def ETC_OK
* \brief Operate success to etc file.
 */
#define ETC_OK                  0
#define TRUE			1
typedef int 			BOOL;

void __mg_rewind (FILE *fp)
{
    rewind (fp);
}

FILE* __mg_tmpfile (void)
{
    return fopen ("/tmp/mg-etc-tmp", "w+");

}

int __mg_close_tmpfile (FILE *tmp_fp)
{
    return fclose (tmp_fp);
}


static char* get_section_name (char *section_line)
{
    char* current;
    char* name;

    if (!section_line)
        return NULL;

    current = section_line;

    while (*current == ' ' ||  *current == '\t') current++;

    if (*current == ';' || *current == '#')
        return NULL;

    if (*current++ == '[')
        while (*current == ' ' ||  *current == '\t') current ++;
    else
        return NULL;

    name = current;
    while (*current != ']' && *current != '\n' &&
            *current != ';' && *current != '#' && *current != '\0')
        current++;
    *current = '\0';
    while (*current == ' ' || *current == '\t')
    {
        *current = '\0';
        current--;
    }

    return name;
}
static int get_key_value (char *key_line, char **mykey, char **myvalue)
{
    char* current;
    char* tail;
    char* value;

    if (!key_line)
        return -1;

    current = key_line;

    while (*current == ' ' ||  *current == '\t') current++;

    if (*current == ';' || *current == '#')
        return -1;

    if (*current == '[')
        return 1;

    if (*current == '\n' || *current == '\0')
        return -1;

    tail = current;
    while (*tail != '=' && *tail != '\n' &&
            *tail != ';' && *tail != '#' && *tail != '\0')
        tail++;

    value = tail + 1;
    if (*tail != '=')
        *value = '\0';

    *tail-- = '\0';
    while (*tail == ' ' || *tail == '\t')
    {
        *tail = '\0';
        tail--;
    }

    tail = value;
    while (*tail != '\n' && *tail != '\0') tail++;
    *tail = '\0';

    if (mykey)
        *mykey = current;
    if (myvalue)
        *myvalue = value;

    return 0;
}

/* This function locate the specified key in the etc file. */
static int etc_LocateKeyValue(FILE* fp, const char* pKey,
                              BOOL bCurSection, char* pValue, int iLen,
                              FILE* bak_fp, char* nextSection)
{
    char szBuff[ETC_MAXLINE + 1 + 1];
    char* current;
    char* value;
    int ret;

    while (TRUE)
    {
        int bufflen;

        if (!fgets(szBuff, ETC_MAXLINE, fp))
            return ETC_FILEIOFAILED;
        bufflen = strlen (szBuff);
        if (szBuff [bufflen - 1] == '\n')
            szBuff [bufflen - 1] = '\0';

        ret = get_key_value (szBuff, &current, &value);
        if (ret < 0)
            continue;
        else if (ret > 0)
        {
            fseek (fp, -bufflen, SEEK_CUR);
            return ETC_KEYNOTFOUND;
        }

        if (strcmp (current, pKey) == 0)
        {
            if (pValue)
                strncpy (pValue, value, iLen);

            return ETC_OK;
        }
        else if (bak_fp && *current != '\0')
        {
            fprintf (bak_fp, "%s=%s\n", current, value);
        }
    }

    return ETC_KEYNOTFOUND;
}
/* This function locate the specified section in the etc file. */
static int etc_LocateSection(FILE* fp, const char* pSection, FILE* bak_fp)
{
    char szBuff[ETC_MAXLINE + 1];
    char *name;

    while (TRUE)
    {
        if (!fgets(szBuff, ETC_MAXLINE, fp))
        {
            if (feof (fp))
                return ETC_SECTIONNOTFOUND;
            else
                return ETC_FILEIOFAILED;
        }
        else if (bak_fp && fputs (szBuff, bak_fp) == EOF)
            return ETC_FILEIOFAILED;

        name = get_section_name (szBuff);
        if (!name)
            continue;

        if (strcmp (name, pSection) == 0)
            return ETC_OK;
    }

    return ETC_SECTIONNOTFOUND;
}

static int  GetValueFromEtcFile(const char* pEtcFile, const char* pSection,
                                const char* pKey, char* pValue, int iLen)
{
    FILE* fp;
    char tempSection [ETC_MAXLINE + 2];

    if (!(fp = fopen(pEtcFile, "r")))
        return ETC_FILENOTFOUND;

    if (pSection)
        if (etc_LocateSection (fp, pSection, NULL) != ETC_OK)
        {
            fclose (fp);
            return ETC_SECTIONNOTFOUND;
        }

    if (etc_LocateKeyValue (fp, pKey, pSection != NULL,
                            pValue, iLen, NULL, tempSection) != ETC_OK)
    {
        fclose (fp);
        return ETC_KEYNOTFOUND;
    }

    fclose (fp);
    return ETC_OK;
}
static int etc_CopyAndLocate (FILE* etc_fp, FILE* tmp_fp,
                              const char* pSection, const char* pKey, char* tempSection)
{
    if (pSection && etc_LocateSection (etc_fp, pSection, tmp_fp) != ETC_OK)
        return ETC_SECTIONNOTFOUND;

    if (etc_LocateKeyValue (etc_fp, pKey, pSection != NULL,
                            NULL, 0, tmp_fp, tempSection) != ETC_OK)
        return ETC_KEYNOTFOUND;

    return ETC_OK;
}
static int etc_FileCopy (FILE* sf, FILE* df)
{
    char line [ETC_MAXLINE + 1];

    while (fgets (line, ETC_MAXLINE + 1, sf) != NULL)
        if (fputs (line, df) == EOF)
        {
            return ETC_FILEIOFAILED;
        }

    return ETC_OK;
}
static int  SetValueToEtcFile (const char* pEtcFile, const char* pSection,
                               const char* pKey, char* pValue)
{
    FILE* etc_fp;
    FILE* tmp_fp;
    int rc;
    char tempSection [ETC_MAXLINE + 2] = {0};

    if ((tmp_fp = __mg_tmpfile ()) == NULL)
        return ETC_TMPFILEFAILED;
    if (!(etc_fp = fopen (pEtcFile, "r+")))
    {
        __mg_close_tmpfile (tmp_fp);

        if (!(etc_fp = fopen (pEtcFile, "w")))
        {
            return ETC_FILEIOFAILED;
        }
        fprintf (etc_fp, "[%s]\n", pSection);
        fprintf (etc_fp, "%s=%s\n", pKey, pValue);
        fclose (etc_fp);
        return ETC_OK;
    }

    switch (etc_CopyAndLocate (etc_fp, tmp_fp, pSection, pKey, tempSection))
    {
    case ETC_SECTIONNOTFOUND:
        fprintf (tmp_fp, "\n[%s]\n", pSection);
        fprintf (tmp_fp, "%s=%s\n", pKey, pValue);
        break;

    case ETC_KEYNOTFOUND:
        fprintf (tmp_fp, "%s=%s\n", pKey, pValue);
        fprintf (tmp_fp, "%s\n", tempSection);
        break;

    default:

        fprintf (tmp_fp, "%s=%s\n", pKey, pValue);

        break;
    }

    if ((rc = etc_FileCopy (etc_fp, tmp_fp)) != ETC_OK)
        goto error;

    // replace etc content with tmp file content
    // truncate etc content first
    fclose (etc_fp);
    if (!(etc_fp = fopen (pEtcFile, "w")))
    {
        __mg_close_tmpfile (tmp_fp);
        return ETC_FILEIOFAILED;
    }

    __mg_rewind (tmp_fp);
    rc = etc_FileCopy (tmp_fp, etc_fp);

error:
    fclose (etc_fp);
    __mg_close_tmpfile (tmp_fp);
    return rc;
}

TIniFile::TIniFile(string FileName)
{
    m_FileName = FileName;
}

TIniFile::~TIniFile()
{

}
bool TIniFile::CheckValid()
{
    if(m_FileName == "")
        return false;
    if(access(m_FileName.c_str(), F_OK) != 0)
        return false;
    return true;
}

//http://net.pku.edu.cn/~yhf/linux_c/
double TIniFile::ReadFloat(const string Section, const string Ident, double Default)
{
    FILE* fp;
    char* endPtr = NULL;
    double tmp = 0.0;
    if(!CheckValid())
        return Default;
    char value[ETC_MAX_VALUE]= {0,};
    if(ETC_OK != GetValueFromEtcFile(m_FileName.c_str(), Section.c_str(),Ident.c_str(),value,ETC_MAX_VALUE-1))
        return  Default;
    tmp = strtod(value,&endPtr);
    if(endPtr == NULL)
        return Default;
    return tmp;

}
int    TIniFile::ReadInteger(const string Section, const string Ident, int Default)
{

    char* endPtr = NULL;
    int tmp = 0;
    char value[ETC_MAX_VALUE]= {0,};

    if(!CheckValid())
        return Default;

    if(ETC_OK != GetValueFromEtcFile(m_FileName.c_str(), Section.c_str(),Ident.c_str(),value,ETC_MAX_VALUE-1))
        return  Default;
    tmp = strtol(value,&endPtr,10);
    if(endPtr == NULL)
        return Default;
    return tmp;
}

string TIniFile::ReadString(const string Section, const string Ident, const string Default)
{
    char value[ETC_MAX_VALUE]= {0,};
    if(!CheckValid())
        return Default;

    if(ETC_OK != GetValueFromEtcFile(m_FileName.c_str(), Section.c_str(),Ident.c_str(),value,ETC_MAX_VALUE-1))
        return  Default;
    return value;
}

bool   TIniFile::SectionExists(const string Section)
{

}

void   TIniFile::WriteFloat(const string Section, const string Ident, double Value)
{
    char buf[32] = {0,};
    if(m_FileName == "")
        return;
    snprintf(buf,32,"%0.8f",Value);
    SetValueToEtcFile(m_FileName.c_str(),Section.c_str(),Ident.c_str(),buf);
}

void   TIniFile::WriteInteger(const string Section, const string Ident, int Value)
{
    char buf[32] = {0,};
    if(m_FileName == "")
        return;
    snprintf(buf,32,"%d",Value);
    SetValueToEtcFile(m_FileName.c_str(),Section.c_str(),Ident.c_str(),buf);
}

void   TIniFile::WriteString(const string Section, const string Ident, const string Value)
{
    if(m_FileName == "")
        return;
    SetValueToEtcFile(m_FileName.c_str(),Section.c_str(),Ident.c_str(),(char*)Value.c_str());
}

bool   TIniFile::ValueExists(const string Section, const string Ident)
{

}

void   TIniFile::WriteBool(const string Section, const string Ident, bool Value)
{
    char buf[16] = {0,};
    if(m_FileName == "")
        return;
    snprintf(buf,16,"%d",Value?1:0);
    SetValueToEtcFile(m_FileName.c_str(),Section.c_str(),Ident.c_str(),buf);
}
bool   TIniFile::ReadBool(const string Section, const string Ident, bool Default)
{
    char value[ETC_MAX_VALUE]= {0,};
    int tmp = 0;
    char* endPtr = NULL;
    if(!CheckValid())
        return Default;

    if(ETC_OK != GetValueFromEtcFile(m_FileName.c_str(), Section.c_str(),Ident.c_str(),value,ETC_MAX_VALUE-1))
        return  Default;
    tmp = strtol(value,&endPtr,10);
    if(endPtr == NULL)
        return Default;
    return tmp?true:false ;

}

//Add by Achilles 2010-06-07
static int get_conf_value (char *key_line, char **mykey, char **myvalue)
{
    char* current;
    char* tail;
    char* value;

    if (!key_line)
        return -1;

    current = key_line;

    while (*current == ' ' ||  *current == '\t') current++;

    if (*current == ';' || *current == '#')
        return -1;

    if (*current == '\n' || *current == '\0')
        return -1;

    tail = index(current, '=');
    if(!tail)
        return -1;
    value = tail+1;
    *tail='\0';
    tail = index(value, '"');
    if(!tail)
        return -1;
    value = tail+1;
    tail=rindex(value, '"');
    if(tail)
        *tail='\0';
    else
        return -1;

    if (mykey)
        *mykey = current;
    if (myvalue)
        *myvalue = value;

    return 0;
}

static int conf_LocateKeyValue(FILE* fp, const char* pKey, char* pValue, int iLen, FILE* bak_fp)
{
    char szBuff[ETC_MAXLINE + 1 + 1];
    char szBuffbak[ETC_MAXLINE + 1 + 1];
    char* current;
    char* value;
    int ret;

    while (TRUE)
    {
        int bufflen;

        if (!fgets(szBuff, ETC_MAXLINE, fp))
            return ETC_FILEIOFAILED;
        bufflen = strlen (szBuff);
        if (szBuff [bufflen - 1] == '\n')
            szBuff [bufflen - 1] = '\0';

        if(bak_fp)
            memcpy(szBuffbak, szBuff, bufflen);
        ret = get_conf_value (szBuff, &current, &value); //是否符合 %s = "%s" 的格式

        if((0==ret)&& strstr(current, pKey))
        {
            if(pValue)
                strncpy (pValue, value, iLen);
            if(bak_fp)
                fprintf(bak_fp, "#%s\n%s=", szBuffbak, current);
            return ETC_OK;
        }
        else if(bak_fp)
        {
            fprintf(bak_fp, "%s\n", szBuffbak);
        }
    }
    return ETC_KEYNOTFOUND;
}

static int  GetValueFromConfFile(const char* pEtcFile, const char* pKey, char* pValue, int iLen)
{
    FILE* fp;

    if (!(fp = fopen(pEtcFile, "r")))
        return ETC_FILENOTFOUND;

    if (conf_LocateKeyValue (fp, pKey, pValue, iLen, NULL) != ETC_OK)
    {
        fclose (fp);
        return ETC_KEYNOTFOUND;
    }

    fclose (fp);
    return ETC_OK;
}

static int conf_CopyAndLocate (FILE* etc_fp, FILE* tmp_fp, const char* pKey)
{
    if (conf_LocateKeyValue (etc_fp, pKey, NULL, 0, tmp_fp) != ETC_OK)
        return ETC_KEYNOTFOUND;

    return ETC_OK;
}

static int  SetValueToConfFile (const char* pEtcFile, const char* pKey, char* pValue)
{
    FILE* etc_fp;
    FILE* tmp_fp;
    int rc;

    if ((tmp_fp = __mg_tmpfile ()) == NULL)
        return ETC_TMPFILEFAILED;
    if (!(etc_fp = fopen (pEtcFile, "r+")))
    {
        __mg_close_tmpfile (tmp_fp);
        return ETC_FILEIOFAILED;	//如果配置文件不存在，直接退出。
    }

    if((rc = conf_CopyAndLocate (etc_fp, tmp_fp, pKey))!= ETC_OK)
        goto error;

    fprintf (tmp_fp, "\"%s\"\n", pValue);

    if ((rc = etc_FileCopy (etc_fp, tmp_fp)) != ETC_OK)
        goto error;

    // replace etc content with tmp file content
    // truncate etc content first
    fclose (etc_fp);
    if (!(etc_fp = fopen (pEtcFile, "w")))
    {
        __mg_close_tmpfile (tmp_fp);
        return ETC_FILEIOFAILED;
    }

    __mg_rewind (tmp_fp);
    rc = etc_FileCopy (tmp_fp, etc_fp);

error:
    fclose (etc_fp);
    __mg_close_tmpfile (tmp_fp);
    return rc;
}

string TIniFile::ReadString(const string Ident, const string Default)
{
    char value[ETC_MAX_VALUE]= {0,};
    if(!CheckValid())
        return Default;

    if(ETC_OK != GetValueFromConfFile(m_FileName.c_str(), Ident.c_str(),value,ETC_MAX_VALUE-1))
        return  Default;
    return value;
}

void   TIniFile::WriteString(const string Ident, const string Value)
{
    if(m_FileName == "")
        return;
    SetValueToConfFile(m_FileName.c_str(),Ident.c_str(),(char*)Value.c_str());
}


