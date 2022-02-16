#ifndef ADABAS_H
#define ADABAS_H

#ifndef WIN32
#ifndef sql_sqldb
  #define sql_sqldb 0
  #include <cpc.h>
#endif
#endif

/* Maximale Laenge eines SQL-Befehls*/
#define MAX_SQLLENGTH 20000
#define MAX_SQLVAR 80
#define MAX_SQLSELECT 6


typedef enum { FT_STRING, FT_LONG, FT_BOOLEAN, FT_DOUBLE, FT_DATE, FT_TIME, FT_TIMESTAMP, FT_UNKNOWN,
               FT_EMPTY, FT_MEMO } FieldType;


extern char SQLVarQueryString[8192];  

int GetVar(char* VarName, char* VarValue);

/*********************************************************************************/
// CField 
/*********************************************************************************/
class CDBField
{
public:

  CDBField();
  CDBField(char* FieldName);
  CDBField(char* FieldName, int NewStringLength);
  ~CDBField();

  FieldType Type;

  char Name[30];
  char* String;
  int StringLength;
  long Long;
  double Double;
  char Date[9];
  char Time[9];
  char TimeStamp[21];
  long Indicator;
  
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
  struct _Item         
    {                
      CDBField* Element;
      _Item *Next;
    };
  _Item *Items;
  _Item *p;
};

/*********************************************************************************/
// CAdabas
/*********************************************************************************/
class CBaseAdabasDB
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

};

class CEngineDB : public CBaseAdabasDB
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
  int GetNext(int SelectIndex);
  int GetPrev(int SelectIndex);
  int GetFirst(int SelectIndex);
  int GetLast(int SelectIndex);
  int GetNumRows(int SelectIndex);
  int GetNumCols(int SelectIndex);
  int GetCurrentRow(int SelectIndex);
  void Commit();
  CDBField* GetField(int SelectIndex, char* FieldName);
  CDBField* GetField(int SelectIndex, int Index);
  CDBField* GetFirstField(int SelectIndex);
  CDBField* GetNextField(int SelectIndex);

  CEngineDB();
  ~CEngineDB();

  int Connect(char* ServerDB, char* Username, char* Password, char* Servernode);
  int GetAt(int SelectIndex, int Position);


  int CreateSession(char* ID);
  int UpdateSession(char* ID);
  int UpdateVar(char* ID,char* VarName,char* VarValue);
  int InsertVar(char* ID,char* VarName,char* VarValue);
  char* GetVar(char* ID,char* VarName);

  int GetFirstListField(char* Category,char* ColumnName,char* Name, char* Value);
  int GetNextListField(char* Name, char* Value);

private:
  int Row[MAX_SQLSELECT];
  int EndOfFile[MAX_SQLSELECT];
  int NumRows[MAX_SQLSELECT];
  CDBFieldList DBFieldList[MAX_SQLSELECT];  
  #ifndef WIN32
    sqldatype mysqlda[MAX_SQLSELECT];
  #endif
  void SQLError();
  void DescribeFields(int Index);
  void SetFields();
};
/*********************************************************************************/
            
extern CEngineDB* EngineDB;


#endif
