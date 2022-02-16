#ifndef VSHOP_MODUL_H
#define VSHOP_MODUL_H

#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>


#ifdef __cplusplus 
extern "C" {
#endif

typedef enum 
{ 
  Bool_Value, 
  Int_Value, 
  Float_Value, 
  String_Value, 
  Time_Value, 
  Date_Value, 
  Timestamp_Value, 
  Not_Set, 
  Unknown
} Id_type;

typedef struct t_Value 
{
  Id_type typ;
  long cValue;  
  char *sValue; 
  long nValue;
  double dValue;
  time_t tValue;
} t_Value;

typedef enum 
{ 
  FT_STRING, 
  FT_LONG, 
  FT_BOOLEAN, 
  FT_DOUBLE, 
  FT_DATE, 
  FT_TIME, 
  FT_TIMESTAMP, 
  FT_UNKNOWN,
  FT_EMPTY, 
  FT_MEMO 
} FieldType;


t_Value DBInsert(char* SQL);
t_Value DBUpdate(char *SQL);
t_Value DBSQL(char *SQL);
t_Value DBCopyTable(char* OldTable, char* NewTable);
t_Value DBDrop(char* TableName);
t_Value DBDelete(char* TableName, char* Index);
t_Value DBSelect(int SelectIndex, char *SQL);
t_Value DBIsEOF(int SelectIndex);
t_Value DBIsBOF(int SelectIndex, int Size);
t_Value DBGetColumnByIndex(int SelectIndex, int ColumnIndex);
t_Value DBGetColumn(int SelectIndex, char *ColumnName);
t_Value DBGetNext(int SelectIndex);
t_Value DBGetPrev(int SelectIndex);
t_Value DBGetFirst(int SelectIndex);
t_Value DBGetLast(int SelectIndex);
t_Value DBGetCurrentRow(int SelectIndex);
t_Value DBGetRows(int SelectIndex);
t_Value DBGetColumns(int SelectIndex);


#ifdef __cplusplus 
}
#endif

#endif
