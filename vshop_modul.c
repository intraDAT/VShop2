#include "adabasobj.h"
#include "vshop_modul.h"

#ifdef __cplusplus 
extern "C" {
#endif

/*---------------------------------------------------------------------------------*/
void Mid(char *d, char *s, int n)
{
  strncpy(d,s,n); 
  d[n]=0;
}
/*---------------------------------------------------------------------------------*/
t_Value DBToVSL(CDBField* DBField)
{
t_Value ret;
tm Time;
char Buffer[10];

  Time.tm_hour = 0;
  Time.tm_isdst = 0;
  Time.tm_mday = 0;
  Time.tm_min = 0;
  Time.tm_mon = 0;
  Time.tm_sec = 0;
  Time.tm_wday = 0;
  Time.tm_yday = 0;
  Time.tm_year = 0;

  if (DBField != NULL)
    {
      switch(DBField->Type)
	      {
	        case FT_STRING:
          case FT_MEMO:
            ret.sValue = (char*) malloc(strlen(DBField->String)+1);
            strcpy(ret.sValue,DBField->String);
            ret.typ = String_Value;
            break;
          case FT_LONG:
            ret.nValue = DBField->Long;
            ret.typ = Int_Value;
            break;
          case FT_BOOLEAN:
		        if (DBField->Long == 0)
              ret.nValue = DBField->Long;
            else
              ret.nValue = 1;
            ret.typ = Int_Value;
            break;
          case FT_DOUBLE:
            ret.dValue = DBField->Double;
            ret.typ = Float_Value;
            break;
          case FT_DATE:
            Mid(Buffer, DBField->Date,4);
            Time.tm_year = atoi(Buffer)-1900;
            Mid(Buffer, &DBField->Date[4],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &DBField->Date[6],2);
            Time.tm_mday = atoi(Buffer);
            ret.tValue = mktime(&Time);
            ret.typ = Date_Value;
            break;
          case FT_TIME:
            Time.tm_mon =   1;
            Time.tm_yday =  2;
            Time.tm_year =  70;
            Mid(Buffer, DBField->Time,4);
            Time.tm_hour = atoi(Buffer);
            Mid(Buffer, &DBField->Time[4],2);
            Time.tm_min = atoi(Buffer);
            Mid(Buffer, &DBField->Time[6],2);
            Time.tm_sec = atoi(Buffer);
            ret.tValue = mktime(&Time);
            ret.typ = Time_Value;
            break;
          case FT_TIMESTAMP:
            Mid(Buffer, DBField->Date,4);
            Time.tm_year = atoi(Buffer)-1900;
            Mid(Buffer, &DBField->Date[4],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &DBField->Date[6],2);
            Time.tm_mday = atoi(Buffer);
            Mid(Buffer, &DBField->Time[8],2);
            Time.tm_hour = atoi(Buffer);
            Mid(Buffer, &DBField->Time[10],2);
            Time.tm_min = atoi(Buffer);
            Mid(Buffer, &DBField->Time[12],2);
            Time.tm_sec = atoi(Buffer);
            ret.tValue = mktime(&Time);
            ret.typ = Timestamp_Value;
            break;
          default:
            ret.typ = Not_Set;
        }
    }
  else
    {
      ret.typ = Not_Set;
    }

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBInsert(char* SQL)
{
t_Value ret;
char Buffer[20];

  ret.typ = Bool_Value;

  if (AdabasDB->SQLInsert(SQL,Buffer))
    {
      /* Buffer ist der neue INDEX */
      /* InsertEntry("VSINDEX", Buffer); */
      ret.nValue = 1;
    }
  else
    ret.nValue = 0;
  
  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBUpdate(char *SQL)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = AdabasDB->SQLUpdate(SQL);
  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBSQL(char *SQL)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = AdabasDB->SQLUpdate(SQL);
  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBCopyTable(char* OldTable, char* NewTable)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = AdabasDB->SQLCreate(OldTable,NewTable);

  return ret;
}
/*-------------------------------------------------------------------------------*/
t_Value DBDrop(char* TableName)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = AdabasDB->SQLDrop(TableName);
  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBDelete(char* TableName, char* Index)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = AdabasDB->SQLDelete(TableName,Index);
  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBSelect(int SelectIndex, char *SQL)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = AdabasDB->SQLSelect(SelectIndex, SQL);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBIsEOF(int SelectIndex)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = AdabasDB->IsEOF(SelectIndex);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBIsBOF(int SelectIndex, int Size)
{
t_Value ret;

  ret.nValue = (AdabasDB->GetCurrentRow(SelectIndex) <= (Size+1));
  ret.typ = Bool_Value;

  return ret;
}
/*-------------------------------------------------------------------------*/
t_Value DBGetColumnByIndex(int SelectIndex, int ColumnIndex)
{
  return DBToVSL(AdabasDB->GetField(SelectIndex,ColumnIndex));
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetColumn(int SelectIndex, char *ColumnName)
{
  return DBToVSL(AdabasDB->GetField(SelectIndex,ColumnName));
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetNext(int SelectIndex)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = AdabasDB->GetNext(SelectIndex);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetPrev(int SelectIndex)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = AdabasDB->GetPrev(SelectIndex);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetFirst(int SelectIndex)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = AdabasDB->GetFirst(SelectIndex);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetLast(int SelectIndex)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = AdabasDB->GetLast(SelectIndex);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetCurrentRow(int SelectIndex)
{
t_Value ret;

  ret.nValue = AdabasDB->GetCurrentRow(SelectIndex);
  ret.typ = Int_Value;

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetRows(int SelectIndex)
{
t_Value ret;

  ret.nValue = AdabasDB->GetNumRows(SelectIndex);
  ret.typ = Int_Value;

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetColumns(int SelectIndex)
{
t_Value ret;

  ret.nValue = AdabasDB->GetNumCols(SelectIndex);
  ret.typ = Int_Value;

  return ret;
}
/*--------------------------------------------------------------------------------*/
#ifdef __cplusplus 
}
#endif





