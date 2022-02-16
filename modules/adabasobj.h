#ifndef ADABASOBJ_H
#define ADABASOBJ_H

#include "vshop_modul.h"


/*********************************************************************************/
// CDBField
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
// CBaseAdabasDB
/*********************************************************************************/
class CBaseAdabasDB
{
public:
  virtual int SQLInsert(char* SQLString, char* ID);
  virtual int SQLUpdate(char* SQLString);
  virtual int SQLCreate(char* TableName,char* OldTable);
  virtual int SQLDrop(char* TableName);
  virtual int SQLDelete(char* TableName, char* Index);
  virtual int SQLSelect(int SelectIndex, char* SQLString);
  virtual int IsEOF(int SelectIndex);
  virtual int GetNext(int SelectIndex);
  virtual int GetPrev(int SelectIndex);
  virtual int GetFirst(int SelectIndex);
  virtual int GetLast(int SelectIndex);
  virtual int GetNumRows(int SelectIndex);
  virtual int GetNumCols(int SelectIndex);
  virtual int GetCurrentRow(int SelectIndex);
  virtual CDBField* GetField(int SelectIndex, char* FieldName);
  virtual CDBField* GetField(int SelectIndex, int Index);
  virtual CDBField* GetFirstField(int SelectIndex);
  virtual CDBField* GetNextField(int SelectIndex);
};
/*--------------------------------------------------------------------------------*/
extern CBaseAdabasDB* AdabasDB;

#endif
