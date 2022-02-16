#ifndef DB_H
#define DB_H

#ifndef DB_ORACLE
  #include "adabas.h"
#else
  #include "oracle.h"
#endif

//
// max length of typical sql statements
//
#define SQL_LEN 200


int WriteEvent(int Level,char* Name,char* Event);
void DBReplace(char* Buffer);
char* DBGetConfig(char* Setting);
int DBSetConfig(char* Keyword,char* Setting,char* Value);

/**
  * Returns the string value of a system setting.  The specified query handle
  * is used.
  */
char *getSetting( int qh, char *keyword, char *setting );

t_Value DBSelect(int SelectIndex, char *SQL);
t_Value DBGetColumnByIndex(int SelectIndex, int ColumnIndex);
t_Value DBGetColumn(int SelectIndex, char *ColumnName);
t_Value DBGetNext(int SelectIndex);
t_Value DBGetPrev(int SelectIndex);
t_Value DBGetFirst(int SelectIndex);
t_Value DBGetLast(int SelectIndex);
t_Value DBGetAt(int SelectIndex, int Position);
t_Value DBGetColumnName(int SelectIndex, int ColumnIndex);
t_Value DBCreateInputField(int SelectIndex, int ColumnIndex,char* TextareaStyle);
t_Value DBSaveForm(char* TableName);
t_Value DBSearchForm(int SelectIndex, char* TableName);
t_Value DBGetCurrentRow(int SelectIndex);
t_Value DBGetRows(int SelectIndex);
t_Value DBGetColumns(int SelectIndex);
t_Value DBIsEOF(int SelectIndex);
t_Value DBIsBOF(int SelectIndex, int Size);
t_Value DBSeek(int SelectIndex, int Page, int Size);
t_Value DBUpdate(char *SQL);
t_Value DBSQL(char *SQL);
t_Value DBInsert(char* SQL);
t_Value DBCopyTable(char* OldTable, char* NewTable);
t_Value DBDelete(char* TableName, char* Index);
t_Value DBDrop(char* TableName);

t_Value DBFetch(char* TableName, char* Fieldname, char* Index);


int DBListSelect(char* TableName, int Size);
int DBListNext();
t_Value DBListGetColumn(char* ColumnName);

int DBCatalogSelect(t_Value Param1, t_Value Param2);
int DBCatalogNext();
t_Value DBCatalogGetColumn(char* ColumnName);

int DBBasketSelect(t_Value Param);
int DBBasketNext();
t_Value DBBasketGetColumn(char* ColumnName);

int DBPaymentSelect();
int DBPaymentNext();
t_Value DBPaymentGetColumn(char* ColumnName);

int DBPromotionSelect(t_Value Param);
int DBPromotionNext();
t_Value DBPromotionGetColumn(char* ColumnName);

int DBRelevantSelect();
int DBRelevantNext();
t_Value DBRelevantGetColumn(char* ColumnName);

int DBShippingcostSelect();
int DBShippingcostNext();
t_Value DBShippingcostGetColumn(char* ColumnName);

t_Value DBImportFile(char* FileName, char* TableName, char FieldSeparator);
t_Value DBExportTable(char* FileName, char* TableName, int Type , char* FieldSeparator);
t_Value DBExportDB(char* FileName, int LoadFlag);

void DBCateporyprofileAdd(char* CategoryID);
void DBProductpofileAdd(char* NameOfCategory, char* ProductID, int Type, int Value);

t_Value DBToVSL(CDBField* DBField);

#endif

