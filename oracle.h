#include <string>


#ifndef VSADMIN
  #include "cgic.h"
  #include "error.h"
#endif


/* Include OCI-specific headers. */
extern "C"
{
#include <oratypes.h>
#include <ociapr.h>
#include <ocidem.h>
#include <orl.h>
}

#ifndef ORACLE_H
#define ORACLE_H

#define MAX_SQLSELECT 8
#define MAX_NAMELENGTH 40

#define MAX_SELECT_LIST_SIZE    255 

#define PARSE_NO_DEFER           0
#define PARSE_V7_LNG             2

//typedef void dvoid; 

typedef enum { FT_STRING, FT_LONG, FT_BOOLEAN, FT_DOUBLE, FT_DATE, FT_TIME, FT_TIMESTAMP, FT_UNKNOWN,
               FT_EMPTY, FT_MEMO } FieldType;

int GetVar(char* VarName, char* VarValue);


/*
struct describe
{
    sb4             dbsize;
    sb2             dbtype;
    sb1             buf[MAX_ITEM_BUFFER_SIZE];
    sb4             buflen;
    sb4             dsize;
    sb2             precision;
    sb2             scale;
    sb2             nullok;
};

struct define 
{
    ub1             buf[MAX_ITEM_BUFFER_SIZE];
    float           flt_buf;
    sword           int_buf;
    sb2             indp;
    ub2             col_retlen, col_retcode;
};
*/
/*********************************************************************************/
// CDBField 
/*********************************************************************************/
class CDBField
{
 public:

  CDBField();
  CDBField(char* FieldName);
  CDBField(char* FieldName, int NewStringLength);
  CDBField(char* FieldName, int NewStringLength, int Flag);
  ~CDBField();

  FieldType Type;

  char* String;
  
  char Name[MAX_NAMELENGTH];
  char Date[9];
  char Time[9];
  char TimeStamp[18];
  long Long;
  double Double;

  time_t TimeInSeconds;

  int StringLength;
  short int Indicator;

  struct {
    sb1 _century;
    sb1 _year;
    ub1 month;
    ub1 day;
    ub1 hour;
    ub1 minute;
    ub1 second;
    int year;
  } DateEx;
  int Col;

  sb4             dbsize;
  sb2             dbtype;
  sb4             dsize;
  sb2             precision;
  sb2             scale;
  sb2             nullok;
  sb4             buflen;

//    ub1             buf[MAX_ITEM_BUFFER_SIZE];
    float           flt_buf;
    sword           int_buf;
    sb2             indp;
    ub2             col_retlen, col_retcode;

};
/*********************************************************************************/
// CDBFieldList
/*********************************************************************************/
class CDBFieldList
{
public:         
  CDBFieldList();
  ~CDBFieldList();

  void  Add(CDBField* Item);
  void  Clear();
  CDBField* Seek(char* Name);
  CDBField* Seek(int Index);
  CDBField* GetFirstItem();
  CDBField* GetNextItem();
  CDBField* GetItem();
  void SetNull();

private:
  class _Item         
    {                
      public: 
      ~_Item() { delete Element; }
      CDBField* Element;
      _Item *Next;
    };
  _Item *Items;
  _Item *p;
};

/*********************************************************************************/
// COracle
/*********************************************************************************/
class CBaseEngineDB
{
 public:
  int SQLInsert(char* SQLString, char* ID);
  int SQLUpdate(char* SQLString);
  int SQLCreate(char* TableName,char* OldTable);
  int SQLDrop(char* TableName);
  int SQLDelete(char* TableName, char* Index);
  int SQLSelect(int SelectIndex, char* SQLString);
  int IsEOF(int SelectIndex);
  int GetNext(int SelectIndex);
  int GetPrev(int SelectIndex);
  int GetFirst(int SelectIndex);
  int GetLast(int SelectIndex);
  int GetNumRows(int SelectIndex);
  int GetNumCols(int SelectIndex);
  int GetCurrentRow(int SelectIndex);
  CDBField* GetField(int SelectIndex, char* FieldName);
  CDBField* GetField(int SelectIndex, int Index);
  CDBField* GetFirstField(int SelectIndex);
  CDBField* GetNextField(int SelectIndex);

  char SQLVarQueryString[8192];  
};
/*********************************************************************************/
class CEngineDB : public CBaseEngineDB
{
 public:

  int SQLInsert(char* SQLString, char* ID);
  int SQLUpdate(char* SQLString);
  int SQL(char* SQLString);
  int SQLCreate(char* TableName,char* OldTable);
  int SQLDrop(char* TableName);
  int SQLDelete(char* TableName, char* Index);
  int SQLSelect(int SelectIndex, char* SQLString);
  int IsEOF(int SelectIndex);
  int GetAt(int SelectIndex, int Position);
  int GetNext(int SelectIndex);
  int GetPrev(int SelectIndex);
  int GetFirst(int SelectIndex);
  int GetLast(int SelectIndex);
  int GetNumRows(int SelectIndex);
  int GetNumCols(int SelectIndex);
  int GetCurrentRow(int SelectIndex);
  CDBField* GetField(int SelectIndex, char* FieldName);
  CDBField* GetField(int SelectIndex, int Index);
  CDBField* GetFirstField(int SelectIndex);
  CDBField* GetNextField(int SelectIndex);
  int Commit ();

  CEngineDB();
  ~CEngineDB();

  int Connect(char* ServerDB, char* Username, char* Password, char* Servernode);
  void Close();
  int GetLastError();



  int CreateSession(char* ID);
  int UpdateSession(char* ID);
  int UpdateVar(char* ID,char* VarName,char* VarValue);
  int InsertVar(char* ID,char* VarName,char* VarValue);
  char* GetVar(char* ID,char* VarName);

  int GetFirstListField(char* Category,char* ColumnName,char* Name, char* Value);
  int GetNextListField(char* Name, char* Value);

 private:

  int LastError;

  CDBField* CEngineDB::SetField(CDBField* DBField, int Index);

  sword numwidth;

  int Row[MAX_SQLSELECT+1];
  int EndOfFile[MAX_SQLSELECT+1];
  int fetchSize;
  int NumCols[MAX_SQLSELECT+1];
  int NumRows[MAX_SQLSELECT+1];
  CDBFieldList DBFieldList[MAX_SQLSELECT+1];  

  Lda_Def lda;
  Cda_Def cda[MAX_SQLSELECT+1];
  ub1     hda[512];
  //ub4     hda[HDA_SIZE/(sizeof(ub4))];
  /*
  struct describe desc[MAX_SQLSELECT+1][MAX_SELECT_LIST_SIZE];
  struct define   def[MAX_SQLSELECT+1][MAX_SELECT_LIST_SIZE];
  */
  void SQLError(Cda_Def* cda);
  int DescribeFields(int Index, Cda_Def* cda);
  void SetFields();
};
/*********************************************************************************/
extern CEngineDB* EngineDB;


#endif
