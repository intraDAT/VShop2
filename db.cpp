#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#ifndef WIN32
  #include <unistd.h>
#else
  #include "win32tools.h"
#endif
#include "vsl.h"
#include "vshop.h"
#include "hfct.h"
#include "error.h"
#include "cgic.h"
#include "vslanguage.h"
#include "db.h"
#include "parse.h"

double ShippingcostQuantity;
double ShippingcostPrice;

/*---------------------------------------------------------------------------------*/
char* DBGetConfig(char* Setting)
{
char SQL[121];
CDBField* DBField;

  sprintf(SQL,"select VALUE from SYSTEMPREFERENCES where SETTING='%s'",Setting);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 1)
    return NULL;

  if ((DBField = EngineDB->GetField(MAX_SQLSELECT-1,"VALUE")) == NULL)
    return NULL;

  return DBField->String;
}
/*---------------------------------------------------------------------------------*/
int DBSetConfig(char* Keyword,char* Setting,char* Value)
{
char SQL[521];
char Buffer[20];

  sprintf(SQL,"update SYSTEMPREFERENCES SET VALUE='%s' WHERE upper(SETTING)=upper('%s')",Value,Setting);
  if (EngineDB->SQL(SQL) != 1)
    {
      if (EngineDB->SQLInsert("EVENTS",Buffer))
        {
          sprintf(SQL,"update SYSTEMPREFERENCES SET VALUE='%s',KEYWORD='%s',SETTING='%s' WHERE VSINDEX='%s'",Value,Keyword,Setting,Buffer);
          return EngineDB->SQL(SQL);
        }
      else
        return 0;
    }

  return 1;
}

//
// Returns the string value of a system setting or 0 if there is no such
// setting.
//
char *getSetting( int qh, char *keyword, char *setting ) {

  char sql[ SQL_LEN ];
  sprintf( sql,
           "SELECT value "
           "  FROM systempreferences "
           " WHERE keyword='%s' "
           "   AND setting='%s'",
           keyword,
           setting );

  if ( EngineDB->SQLSelect( qh, sql ) != 1 )
    return 0;

  CDBField *f;
  if ( ( f = EngineDB->GetField( qh, "VALUE" ) ) == NULL )
    return 0;

  return f->String;
}

/*---------------------------------------------------------------------------------*/
int WriteEvent(int Level,char* Name,char* Event)
{
char SQL[200];
char Buffer[20];

  if (EngineDB->SQLInsert("EVENTS",Buffer))
    {                             
      sprintf(SQL,"update EVENTS set VSLEVEL=%i, NAME='%s', EVENT='%s' where VSINDEX='%s'",Level,Name,Event,Buffer);
      return EngineDB->SQL(SQL);
    }

  return 0;
}
/*---------------------------------------------------------------------------------*/
void DBReplace(char* Buffer)
{
char Buffer2[2000];
unsigned int Count1;
int Count2;

  strcpy(Buffer2,Buffer);
  Count2 = 0;
  for (Count1 = 0;Count1 < strlen(Buffer2); Count1++)
    {
      if (Buffer2[Count1] == 39)
        {
          Buffer[Count2] = 39;
          Count2++;
          Buffer[Count2] = 39;
        }
      else
        Buffer[Count2] = Buffer2[Count1];
      Count2++;
    }
  Buffer[Count2] = 0;
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
      ret.Indicator = DBField->Indicator;
      switch(DBField->Type)
	      {
	        case FT_STRING:
          case FT_MEMO:
            if (DBField->Indicator == -1)
              {
                ret.sValue = (char*) malloc(2);
                strcpy(ret.sValue,"?");
                ret.typ = String_Value;
              }
            else
              {
                ret.sValue = (char*) malloc(strlen(DBField->String)+1);
                strcpy(ret.sValue,DBField->String);
                ret.typ = String_Value;
              }
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
            Mid(Buffer, DBField->TimeStamp,4);
            Time.tm_year = atoi(Buffer)-1900;
            Mid(Buffer, &DBField->TimeStamp[4],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &DBField->TimeStamp[6],2);
            Time.tm_mday = atoi(Buffer);
            Mid(Buffer, &DBField->TimeStamp[8],2);
            Time.tm_hour = atoi(Buffer);
            Mid(Buffer, &DBField->TimeStamp[10],2);
            Time.tm_min = atoi(Buffer);
            Mid(Buffer, &DBField->TimeStamp[12],2);
            Time.tm_sec = atoi(Buffer);
            ret.tValue = mktime(&Time);
            ret.typ = Timestamp_Value;
            break;
          default:
            ret.Indicator = -1;
            ret.nValue = 0;
            ret.typ = Bool_Value;
        }
    }
  else
    {
      ret.Indicator = -1;
      ret.nValue = 0;
      ret.typ = Bool_Value;
    }

  return ret;
}
/*-------------------------------------------------------------------------*/
t_Value DBGetColumnByIndex(int SelectIndex, int ColumnIndex)
{
  return DBToVSL(EngineDB->GetField(SelectIndex,ColumnIndex));
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetColumn(int SelectIndex, char *ColumnName)
{
  return DBToVSL(EngineDB->GetField(SelectIndex,ColumnName));
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetNext(int SelectIndex)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->GetNext(SelectIndex);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetPrev(int SelectIndex)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->GetPrev(SelectIndex);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetFirst(int SelectIndex)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->GetFirst(SelectIndex);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetLast(int SelectIndex)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->GetLast(SelectIndex);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetAt(int SelectIndex, int Position)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->GetAt(SelectIndex,Position);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetColumnName(int SelectIndex, int ColumnIndex)
{
t_Value ret;
CDBField* DBField;
  
  if ((DBField = EngineDB->GetField(SelectIndex, ColumnIndex)) != NULL)
    {
      ret.sValue = (char*) malloc(strlen(DBField->Name)+1);
      strcpy(ret.sValue,DBField->Name);
      ret.typ = String_Value;
    }
  else
    ret.typ = Not_Set;

  return ret;                                                     }

/*--------------------------------------------------------------------------------*/
int IsListField(char* Category, CDBField* DBField)
{
char Name[41];
char Value[61];
char Buffer2[101];

  if (EngineDB->GetFirstListField(Category,DBField->Name, Name, Value))
    {
      fprintf(cgiOut,"<SELECT NAME=\"%s\" SIZE=1>\n",DBField->Name);
      do
        {

          fprintf(cgiOut,"<OPTION VALUE=\"");
          fprintf(cgiOut,Value);

          switch(DBField->Type)
            {
	            case FT_STRING:
                sprintf(Buffer2,"%s",DBField->String);
                break;
              case FT_BOOLEAN:
              case FT_LONG:
                sprintf(Buffer2,"%ld",DBField->Long);
                break;
              case FT_DOUBLE:
                sprintf(Buffer2,"%f",DBField->Double);
                break;
              default:
                strcpy(Buffer2,"");
            }

          if (strcmp(Buffer2,Value) == 0)
            fprintf(cgiOut,"\" SELECTED>");
          else
            fprintf(cgiOut,"\">");
          fprintf(cgiOut,Name);
          fprintf(cgiOut,"</OPTION>\n");
        }
      while (EngineDB->GetNextListField(Name, Value));
      fprintf(cgiOut,"</SELECT>\n");

      return 1;
    }

  return 0;
}
/*--------------------------------------------------------------------------------*/
t_Value DBCreateInputField(int SelectIndex, int ColumnIndex, char* TextareaStyle)
{
t_Value ret;
CDBField* DBField;
char Category[21];
struct tm* Today;
char TMBuffer[80];

  
  DBField = EngineDB->GetField(SelectIndex, ColumnIndex);

  if (DBField != NULL)
    {

      if (strcasecmp(DBField->Name,"VSINDEX") == 0)
        {
          fprintf(cgiOut,"<INPUT TYPE=HIDDEN NAME=\"%s\" VALUE=%s>\n",DBField->Name,DBField->String);
          fprintf(cgiOut,"%s",DBField->String);
        }
      else
      if (strcasecmp(DBField->Name,"VSSI") == 0)
        {
          fprintf(cgiOut,"<INPUT TYPE=HIDDEN NAME=\"%s\" VALUE=%i>\n",DBField->Name,DBField->Long);
          fprintf(cgiOut,"%i",DBField->Long);
        }
      else
      if ((cgiFormString("CATEGORY",Category,21) == cgiFormNotFound) ||
          ((cgiFormString("CATEGORY",Category,21) != cgiFormNotFound) && (IsListField(Category,DBField) == 0)))
        {
          switch(DBField->Type)
	          {
              case FT_MEMO:
                fprintf(cgiOut,"<TEXTAREA NAME=\"%s\" ROWS=3 COLS=30 WRAP=VIRTUAL>%s</TEXTAREA>\n",DBField->Name,DBField->String);
                break;
	            case FT_STRING:
                  if (DBField->Indicator == -1)
                    {
                      if (strlen(TextareaStyle) > 0)
                        fprintf(cgiOut,"<TEXTAREA NAME=\"%s\" %s>%s</TEXTAREA>\n",DBField->Name,TextareaStyle,"?");
                      else
                        fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"%s\" MAXLENGTH=%i>\n",DBField->Name,"?",DBField->StringLength-1);
                    }
                  else
                    {
                      if (strlen(TextareaStyle) > 0)
                        fprintf(cgiOut,"<TEXTAREA NAME=\"%s\" %s>%s</TEXTAREA>\n",DBField->Name,TextareaStyle,DBField->String);
                      else
                        {
                          if (strchr(DBField->String,'"') != NULL)
                            fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE='%s' MAXLENGTH=%i>\n",DBField->Name,DBField->String,DBField->StringLength-1);
                          else
                            fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"%s\" MAXLENGTH=%i>\n",DBField->Name,DBField->String,DBField->StringLength-1);
                        }
                    }
                break;
	            case FT_TIME:
                Today = localtime(&(DBToVSL(DBField).tValue));
                if (strftime(TMBuffer,50,TIMEFORMAT,Today) > 0)
                  fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"%s\" MAXLENGTH=%i>\n",DBField->Name,TMBuffer,strlen(TMBuffer));
                else
                  fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"\" MAXLENGTH=5>\n",DBField->Name);
                break;
	            case FT_DATE:
                Today = localtime(&(DBToVSL(DBField).tValue));
                if (strftime(TMBuffer,50,DATEFORMAT,Today) > 0)
                  fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"%s\" MAXLENGTH=%i>\n",DBField->Name,TMBuffer,strlen(TMBuffer));
                else
                  fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"\" MAXLENGTH=10>\n",DBField->Name);
                break;
	            case FT_TIMESTAMP:
                Today = localtime(&(DBToVSL(DBField).tValue));
                if (strftime(TMBuffer,50,TIMESTAMPFORMAT,Today) > 0)
                  fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"%s\" MAXLENGTH=%i>\n",DBField->Name,TMBuffer,strlen(TMBuffer));
                else
                  fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"\" MAXLENGTH=16>\n",DBField->Name);
                break;
              case FT_BOOLEAN:
                fprintf(cgiOut,"<SELECT NAME=\"%s\">\n",DBField->Name);
		            if (DBField->Long == 0)
                  {
                    fprintf(cgiOut,"<OPTION >TRUE</OPTION>\n");
                    fprintf(cgiOut,"<OPTION SELECTED>FALSE</OPTION>\n");
                  }
                else
                  {
                    fprintf(cgiOut,"<OPTION SELECTED>TRUE</OPTION>\n");
                    fprintf(cgiOut,"<OPTION >FALSE</OPTION>\n");
                  }
                fprintf(cgiOut,"</SELECT>\n");
                break;
              case FT_LONG:
                if (DBField->Indicator == -1)
                  fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"?\" MAXLENGTH=18>\n",DBField->Name);
                else
                  fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=%ld MAXLENGTH=18>\n",DBField->Name,DBField->Long);
                break;
              case FT_DOUBLE:
                if (DBField->Indicator == -1)
                  fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"?\" MAXLENGTH=18>\n",DBField->Name);
                else
                  fprintf(cgiOut,"<INPUT TYPE=TEXT NAME=\"%s\" VALUE=%f MAXLENGTH=18>\n",DBField->Name,DBField->Double);
                break;
              default:
                fprintf(cgiOut,"<B>UNKNOWN DATATYPE</B>");
            }
          }
      ret.typ = Bool_Value;
      ret.nValue = 1;
    }
  else
    {
      ret.typ = Bool_Value;
      ret.nValue = 0;
    }

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBSaveForm(char* TableName)
{
t_Value ret;
char Buffer[10000];
char Buffer2[2000];
CDBField* DBField;
int Counter;
struct tm Today;

  sprintf(Buffer,"select * from %s where VSINDEX=''",TableName);
  EngineDB->SQLSelect(MAX_SQLSELECT-1,Buffer);

  sprintf(Buffer,"UPDATE %s SET ",TableName);
  for (Counter=1;Counter <= EngineDB->GetNumCols(MAX_SQLSELECT-1);Counter++)
    {
      DBField = EngineDB->GetField(MAX_SQLSELECT-1,Counter);

      if ((cgiFormString(DBField->Name,Buffer2,1999) != cgiFormNotFound))
        {
          strcat(Buffer,DBField->Name);
          strcat(Buffer2,"");

          switch(DBField->Type)
            {
              case FT_STRING:
                DBReplace(Buffer2);
                strcat(Buffer,"='");
                strcat(Buffer,Buffer2);
                strcat(Buffer,"',");
                break;
              case FT_BOOLEAN:
                strcat(Buffer,"=");
                if (strcmp(Buffer2,"FALSE") == 0)
                  strcat(Buffer,"FALSE");
                else
                  strcat(Buffer,"TRUE");
                strcat(Buffer,",");
                break;
              case FT_LONG:
                strcat(Buffer,"=");
                strcat(Buffer,Buffer2);
                strcat(Buffer,",");
                break;
              case FT_DOUBLE:
                Replace(Buffer2,DECIMALSYMBOL_STRING,".");
                strcat(Buffer,"=");
                strcat(Buffer,Buffer2);
                strcat(Buffer,",");
                break;
              case FT_DATE:
                Today = StringToDate(Buffer2);
                strftime(Buffer2,2000,"%Y%m%d",&Today);
                strcat(Buffer,"='");
                strcat(Buffer,Buffer2);
                strcat(Buffer,"',");
                break;
              case FT_TIME:
                Today = StringToTime(Buffer2);
                strftime(Buffer2,2000,"%H%M",&Today);
                strcat(Buffer,"='");
                strcat(Buffer,Buffer2);
                strcat(Buffer,"',");
                break;
              case FT_TIMESTAMP:
                Today = StringToTimestamp(Buffer2);
                strftime(Buffer2,2000,"%Y%m%d%H%M000000",&Today);
                strcat(Buffer,"='");
                strcat(Buffer,Buffer2);
                strcat(Buffer,"',");
                break;
              default:
                RuntimeError2("DBSaveForm unsupported typ of column ",DBField->Name);
                break;

            }
        }
    }

  Buffer[strlen(Buffer)-1] = '\0';
  strcat(Buffer," WHERE VSINDEX='");
  cgiFormString("VSINDEX",Buffer2,500);
  strcat(Buffer,Buffer2);
  strcat(Buffer,"'");

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->SQLUpdate(Buffer);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBSearchForm(int SelectIndex, char* TableName)
{
t_Value ret;
char Buffer[5000];
char Buffer2[500];
CDBField* DBField;
int Counter;
int Length;

  sprintf(Buffer,"select * from %s where VSINDEX=''",TableName);
  EngineDB->SQLSelect(SelectIndex,Buffer);

  sprintf(Buffer,"SELECT * FROM %s WHERE (",TableName);
  for (Counter=1;Counter <= EngineDB->GetNumCols(SelectIndex);Counter++)
    {
      DBField = EngineDB->GetField(SelectIndex,Counter);

      if ((cgiFormString(DBField->Name,Buffer2,500) != cgiFormNotFound))
        {

          strcat(Buffer2,"");

          switch(DBField->Type)
            {
              case FT_STRING:
              case FT_DATE:
              case FT_TIME:
              case FT_TIMESTAMP:

                strcat(Buffer,"UPPER(");
                strcat(Buffer,DBField->Name);
                strcat(Buffer,")");
          
                for (Length = 0;Length < (int) strlen(Buffer2);Length++)
                  {
                    if (((int) Buffer2[Length] > 96) && ((int) Buffer2[Length] < 123))
                      Buffer2[Length] -= 32;
                    else
                      {
                        switch(Buffer2[Length])
                          {
                            case '÷':
                              Buffer2[Length] = '™';
                              break;
                            case '„':
                              Buffer2[Length] = 'Ž';
                              break;
                            case '':
                              Buffer2[Length] = 'š';
                              break;
                          }
                      }
                  }
                strcat(Buffer," LIKE '%");
                strcat(Buffer,Buffer2);
                strcat(Buffer,"%' OR ");
                break;
              case FT_BOOLEAN:
                strcat(Buffer,DBField->Name);

                strcat(Buffer,"=");
                if (strcmp(Buffer2,"FALSE") == 0)
                  strcat(Buffer,"FALSE");
                else
                  strcat(Buffer,"TRUE");
                strcat(Buffer," OR ");
                break;
              case FT_LONG:
                strcat(Buffer,DBField->Name);
                if (strlen(Buffer2) > 0)
                  {
                    strcat(Buffer,"=");
                    strcat(Buffer,Buffer2);
                    strcat(Buffer," OR ");
                  }
              case FT_DOUBLE:
                strcat(Buffer,DBField->Name);
                if (strlen(Buffer2) > 0)
                  {
                    Replace(Buffer2,",",".");
                    strcat(Buffer,"=");
                    strcat(Buffer,Buffer2);
                    strcat(Buffer," OR ");
                  }
                break;
              default:
                RuntimeError2("DBSearchForm unsupported typ of column ",DBField->Name);
                break;

            }
        }
    }

/* PATCH 1.6 */
  Buffer[strlen(Buffer)-4] = '\0';
  strcat(Buffer," ) AND ACTIVE=1 order by priority asc");  

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->SQLSelect(SelectIndex,Buffer);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetCurrentRow(int SelectIndex)
{
t_Value ret;

  ret.nValue = EngineDB->GetCurrentRow(SelectIndex);
  ret.typ = Int_Value;

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetRows(int SelectIndex)
{
t_Value ret;

  ret.nValue = EngineDB->GetNumRows(SelectIndex);
  ret.typ = Int_Value;

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBGetColumns(int SelectIndex)
{
t_Value ret;

  ret.nValue = EngineDB->GetNumCols(SelectIndex);
  ret.typ = Int_Value;

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBIsEOF(int SelectIndex)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->IsEOF(SelectIndex);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBIsBOF(int SelectIndex, int Size)
{
t_Value ret;

  ret.nValue = (EngineDB->GetCurrentRow(SelectIndex) <= (Size+1));
  ret.typ = Bool_Value;

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBSeek(int SelectIndex, int Page, int Size)
{
int Counter;
int Counter2;
t_Value ret;

  ret.typ = Bool_Value;

  for(Counter = 1; Counter < Page; Counter++)
    for(Counter2 = 0; Counter2 < Size; Counter2++)
      {
        if (EngineDB->IsEOF(SelectIndex))
          {
            ret.nValue = 0;
            return ret;
          }
        EngineDB->GetNext(SelectIndex);
      }
  ret.nValue = 1;

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBSelect(int SelectIndex, char *SQL)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->SQLSelect(SelectIndex, SQL);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBUpdate(char *SQL)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->SQLUpdate(SQL);
  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBSQL(char *SQL)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->SQLUpdate(SQL);
  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBInsert(char* SQL)
{
t_Value ret;
char Buffer[20];

  ret.typ = Bool_Value;

  if (EngineDB->SQLInsert(SQL,Buffer))
    {
      InsertEntry("VSINDEX", Buffer);
      ret.nValue = 1;
    }
  else
    ret.nValue = 0;
  
  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBCopyTable(char* OldTable, char* NewTable)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->SQLCreate(OldTable,NewTable);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBDelete(char* TableName, char* Index)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->SQLDelete(TableName,Index);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBDrop(char* TableName)
{
t_Value ret;

  ret.typ = Bool_Value;
  ret.nValue = EngineDB->SQLDrop(TableName);

  return ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBFetch(char* TableName, char* Fieldname, char* Index)
{
char SQLString[201];
t_Value ret;

  ret.typ = Not_Set;
  sprintf(SQLString,"select %s from %s where VSINDEX='%s'",Fieldname, TableName, Index);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQLString) != 0)
    {
      return DBToVSL(EngineDB->GetField(MAX_SQLSELECT-1,Fieldname));
    }

  return ret;
}
/*--------------------------------------------------------------------------------*/
void DBCateporyprofileAdd(char* CategoryID)
{
char ID[20];
char SQL[400];
char NewID[21];
char CustomerID[21];
CDBField* INDEX;
char* Value;


  cgiFormString("ID",ID,20);

  strcpy(CustomerID,"");


  if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
    {
      sprintf(SQL,"select VSINDEX from CUSTOMERS where LOGIN='%s'",Value);
      if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)
        {
          INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
          strcpy(CustomerID,INDEX->String);
        }
    }

  sprintf(SQL,"select VSINDEX from CATEGORYPROFILE where CATEGORYID='%s' AND SESSIONID='%s'",CategoryID,ID);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
      strcpy(NewID,INDEX->String);

      sprintf(SQL,"update CATEGORYPROFILE set CATEGORYID='%s',SESSIONID='%s',CUSTOMERID='%s',VALUE=VALUE+1 WHERE VSINDEX='%s'",
              CategoryID,ID,CustomerID,NewID);

      EngineDB->SQLUpdate(SQL);

      return;
    }

  if (EngineDB->SQLInsert("CATEGORYPROFILE",NewID) != 0)
    {
      sprintf(SQL,"update CATEGORYPROFILE set CATEGORYID='%s',SESSIONID='%s',CUSTOMERID='%s',VALUE=1 WHERE VSINDEX='%s'",
              CategoryID,ID,CustomerID,NewID);

      EngineDB->SQLUpdate(SQL);
    }
}
/*--------------------------------------------------------------------------------*/
/* LIST Functions */
/*--------------------------------------------------------------------------------*/
int DBListSelect(char* TableName, int Size)
{
char SQL[200];
char VSSI[20];
char VSSL[20];
int Ret;
int VSSIValue;
CDBField* INDEX;
CDBField* CATEGORYID;
char Category[41];
char CategoryID[21];


  if (cgiFormString("VSSI",VSSI,20) != cgiFormNotFound)
    {
      if (strcmp(VSSI,"") == 0)
        strcpy(VSSI,"1");
    }
  else
    strcpy(VSSI,"1");

  if (strcmp(TableName,"") == 0)
    {
      cgiFormString("CATEGORY",Category,20);
    }
  else
    {
      strcpy(Category,TableName);
      InsertEntry("CATEGORY",Category);
    }

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)

    if (VSHOP_PROFILING > 0)
      {
        sprintf(SQL,"SELECT VSINDEX FROM PRODUCTCATEGORIES WHERE NAMEOFCATEGORY='%s'",Category);
        if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
          {
            CATEGORYID = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
            strcpy(CategoryID,CATEGORYID->String);
            DBCateporyprofileAdd(CategoryID);
          }
      }
  #endif

  if (Size > 1)
    {
      VSSIValue = atol(VSSI);
      VSSIValue = VSSIValue - ((VSSIValue % Size)-1);
      sprintf(VSSI,"%i",VSSIValue);
    }

  //sprintf(SQL,"SELECT * from %s where ACTIVE=1 AND VSSI >= %s",Category,VSSI);
  sprintf(SQL,"SELECT * from %s where VSSI >= %s order by VSSI asc",Category,VSSI);

  Ret = EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL);

  INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
  if (INDEX != NULL)
    {
      InsertEntry("VSPINDEX",INDEX->String);
      InsertEntry("VSINDEX",INDEX->String);
    }

  sprintf(VSSL,"%i",Size);
  InsertEntry("VSSL",VSSL);

  InsertEntry("VSSI",VSSI);
  InsertEntry("VSPI",VSSI);

  return Ret;
}
/*--------------------------------------------------------------------------------*/
int DBListNext()
{
int Ret;
CDBField* INDEX;
CDBField* VSSI;
char VSPI[20];
char VSSL[20];
int Counter;

  Ret = EngineDB->GetNext(MAX_SQLSELECT-1);
  cgiFormString("VSSL",VSSL,20);

#if defined(VSHOP_BASIC) || defined(VSHOP_BASIC)
    if (EngineDB->GetCurrentRow(MAX_SQLSELECT-1) > 200)
      return 0;
  #endif

  INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
  if ((INDEX != NULL) && (strcmp(VSSL,"1") != 0))
    {
      InsertEntry("VSPINDEX",INDEX->String);
      InsertEntry("VSINDEX",INDEX->String);
    }


  cgiFormString("VSPI",VSPI,20);
  Counter = atoi(VSPI)+1;
  sprintf(VSPI,"%i",Counter);
  InsertEntry("VSPI",VSPI);

  return Ret;
}

/*--------------------------------------------------------------------------------*/
t_Value DBListGetColumn(char* ColumnName)
{
  return DBToVSL(EngineDB->GetField(MAX_SQLSELECT-1,ColumnName));
}
/*--------------------------------------------------------------------------------*/
/* CATALOG Functions */
/*--------------------------------------------------------------------------------*/
int DBCatalogSelect(t_Value Param1, t_Value Param2)
{
char SQL[200];
char VSCI[20];
char VSCL[20];
char VSCP[20];
int Ret;
int Counter = 0;
CDBField* INDEX;


  if (Param1.typ == String_Value)
    {
      strcpy(VSCP,Param1.sValue);
   
      if (Param2.typ == Int_Value)
        sprintf(VSCL,"%ld",Param2.nValue);
      else
        {
          strcpy(VSCL,"0");
        }
    }
  else
    {
      if (cgiFormString("VSCP",VSCP,20) == cgiFormNotFound)
        strcpy(VSCP,"0");

      if (strlen(VSCP) == 0)
        strcpy(VSCP,"0");


      sprintf(VSCL,"%ld",Param1.nValue);
    }
          

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
    if (VSHOP_PROFILING > 0)
      {
        if (strcmp(VSCP,"0") != 0)
          DBCateporyprofileAdd(VSCP);
      }

  #endif
  sprintf(SQL,"SELECT * from PRODUCTCATEGORIES where VSINDEX > 10 AND PARENT='%s' AND ACTIVE=1 order by priority asc",VSCP);

  Ret = EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL);

  cgiFormString("VSCI",VSCI,20);

  if ((strlen(VSCI) > 0) && (Ret == 1))
    {
      while((Counter <atoi(VSCI)) && ((Ret = EngineDB->GetNext(MAX_SQLSELECT-1))))
        {
          Counter++;
        }
      
    }

  INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
  if (INDEX != NULL)
    InsertEntry("VSCINDEX",INDEX->String);


  InsertEntry("VSCL",VSCL);

    return Ret;
}
/*--------------------------------------------------------------------------------*/
int DBCatalogNext()
{
int Ret;
CDBField* INDEX;

  Ret = EngineDB->GetNext(MAX_SQLSELECT-1);

  INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
  if (INDEX != NULL)
    {
      InsertEntry("VSCINDEX",INDEX->String);
    }

  return Ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBCatalogGetColumn(char* ColumnName)
{

  return DBToVSL(EngineDB->GetField(MAX_SQLSELECT-1,ColumnName));
}
/*--------------------------------------------------------------------------------*/
/* LIST Functions */
/*--------------------------------------------------------------------------------*/
int DBBasketSelect(t_Value Param)
{
char SQL[200];
char VSBI[20];
char VSBL[20];
char vssi[20];
char ID[20];
int Ret;
int Counter = 0;
char ProductID[21];
CDBField* INDEX;
CDBField* CATEGORYID;
CDBField* PRODUCTID;
CDBField* NAMEOFCATEGORY;
CDBField* VSSI;

  cgiFormString("VSBI",VSBI,20);
  cgiFormString("ID",ID,20);

  sprintf(SQL,"SELECT * from BASKETITEMS where BASKETID='%s'",ID);

  Ret = EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL);

  if ((strlen(VSBI) > 0) && (Ret == 1))
    {
      while((Counter <atoi(VSBI)) && ((Ret = EngineDB->GetNext(MAX_SQLSELECT-1))))
        {
          Counter++;
        }
    }

  INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
  if (INDEX != NULL)
    {
      InsertEntry("VSBINDEX",INDEX->String);

      PRODUCTID = EngineDB->GetField(MAX_SQLSELECT-1,"PRODUCTID");
      strcpy(ProductID,PRODUCTID->String);

      CATEGORYID = EngineDB->GetField(MAX_SQLSELECT-1,"CATEGORYID");

      sprintf(SQL,"SELECT NAMEOFCATEGORY from PRODUCTCATEGORIES where VSINDEX='%s'",CATEGORYID->String);
      if ((Ret = EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL)) != 0)
        {
          NAMEOFCATEGORY = EngineDB->GetField(MAX_SQLSELECT-2,"NAMEOFCATEGORY");

          InsertEntry("CATEGORY",NAMEOFCATEGORY->String);

          sprintf(SQL,"SELECT * from %s where VSINDEX='%s'",NAMEOFCATEGORY->String,ProductID);
          Ret = EngineDB->SQLSelect(MAX_SQLSELECT-3,SQL);
          InsertEntry("VSPINDEX",ProductID);

          VSSI = EngineDB->GetField(MAX_SQLSELECT-3,"VSSI");
          if (VSSI != NULL)
            {
              sprintf(vssi,"%i",VSSI->Long);
              InsertEntry("VSPI",vssi);
            }

        }

    }

  if (Param.typ == Int_Value)
    sprintf(VSBL,"%ld",Param.nValue);

  InsertEntry("VSBL",VSBL);

  return Ret;
}
/*--------------------------------------------------------------------------------*/
int DBBasketNext()
{
char SQL[200];
int Ret = 0;
char ProductID[21];
char vssi[20];
CDBField* INDEX;
CDBField* CATEGORYID;
CDBField* NAMEOFCATEGORY;
CDBField* PRODUCTID;
CDBField* VSSI;

  if (EngineDB->GetNext(MAX_SQLSELECT-1) == 0)
    return 0;
  

  INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
  if (INDEX != NULL)
    {
      InsertEntry("VSBINDEX",INDEX->String);

      PRODUCTID = EngineDB->GetField(MAX_SQLSELECT-1,"PRODUCTID");
      strcpy(ProductID,PRODUCTID->String);

      CATEGORYID = EngineDB->GetField(MAX_SQLSELECT-1,"CATEGORYID");

      sprintf(SQL,"SELECT NAMEOFCATEGORY from PRODUCTCATEGORIES where VSINDEX='%s'",CATEGORYID->String);
      if ((Ret = EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL)) != 0)
        {
          NAMEOFCATEGORY = EngineDB->GetField(MAX_SQLSELECT-2,"NAMEOFCATEGORY");

          InsertEntry("CATEGORY",NAMEOFCATEGORY->String);

          sprintf(SQL,"SELECT * from %s where VSINDEX='%s'",NAMEOFCATEGORY->String,ProductID);
          Ret = EngineDB->SQLSelect(MAX_SQLSELECT-3,SQL);
          InsertEntry("VSPINDEX",ProductID);
          if (Ret)
            {
              VSSI = EngineDB->GetField(MAX_SQLSELECT-3,"VSSI");
              if (VSSI != NULL)
                {
                  sprintf(vssi,"%i",VSSI->Long);
                  InsertEntry("VSPI",vssi);
                }
            }
        }
    }
 
  return Ret;
}
/*--------------------------------------------------------------------------------*/
t_Value DBBasketGetColumn(char* ColumnName)
{
CDBField* FIELD;
char* Value;
char ID[20];

cgiFormString("ID",ID,20);

  if (strcasecmp(ColumnName,"PRICE") == 0)
    {
      if ((Value = EngineDB->GetVar(ID,"BASKETCURRENCY")) != NULL)
        strcpy(ColumnName,Value);
    }

  FIELD = EngineDB->GetField(MAX_SQLSELECT-1,ColumnName);
  if (FIELD == NULL)
    FIELD = EngineDB->GetField(MAX_SQLSELECT-3,ColumnName);

  return DBToVSL(FIELD);
}
/*--------------------------------------------------------------------------------*/
/* PAYMENT Functions */
/*--------------------------------------------------------------------------------*/
int DBPaymentSelect()
{
char SQL[800];
char ID[21];
char* Value;
char Customer[51];
CDBField* PAYMENTGROUP;

  if (cgiFormString("LOGIN",Customer,200) == cgiFormNotFound)
    {
      if (cgiFormString("ID",ID,20) != cgiFormNotFound)
        {
          if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
            {
              strcpy(Customer,Value);
            }
          else
            strcpy(Customer,"ANONYM");
        }
      else
        strcpy(Customer,"ANONYM");
    }

  sprintf(SQL,"SELECT CUSTOMERGROUPS.PAYMENTGROUP,CUSTOMERGROUPS.VSINDEX,CUSTOMERS.VSGROUP,CUSTOMERS.LOGIN from CUSTOMERGROUPS,CUSTOMERS where CUSTOMERS.VSGROUP=CUSTOMERGROUPS.VSINDEX AND CUSTOMERS.LOGIN='%s'",Customer);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
    {
      PAYMENTGROUP = EngineDB->GetField(MAX_SQLSELECT-1,"PAYMENTGROUP");

      sprintf(SQL,"SELECT PAYMENTS.DESCRIPTION,PAYMENTS.VSINDEX,HTML_TEMPLATES.PATH AS TEMPLATE,PAYMENTS.TEMPLATE AS TEMPLATEID from PAYMENTS,HTML_TEMPLATES WHERE HTML_TEMPLATES.VSINDEX=PAYMENTS.TEMPLATE AND PAYMENTGROUP='%s'",PAYMENTGROUP->String);
      return EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL);
    }

  return 0;
}
/*--------------------------------------------------------------------------------*/
int DBPaymentNext()
{
  return EngineDB->GetNext(MAX_SQLSELECT-1);
}
/*--------------------------------------------------------------------------------*/
t_Value DBPaymentGetColumn(char* ColumnName)
{
  return DBToVSL(EngineDB->GetField(MAX_SQLSELECT-1,ColumnName));
}
/*--------------------------------------------------------------------------------*/
/* PROMOTION Functions */
/*--------------------------------------------------------------------------------*/
int DBPromotionSelect(t_Value Param)
{
char SQL[800];
CDBField* INDEX;
CDBField* VSSI;
CDBField* NAMEOFCATEGORY;
char Buffer[31];

  if ((Param.typ == String_Value) && (strcmp(Param.sValue,"") != 0))
    #if defined(VSHOP_BASIC) || defined(VSHOP_BASIC)
      sprintf(SQL,"SELECT * FROM PROMOTIONS,PROMOTIONGROUPS where PROMOTIONGROUPS.VSINDEX=PROMOTIONS.PROMOGROUP AND PROMOTIONGROUPS.NAME='HOMEPAGE' AND PROMOTIONS.ACTIVE=1");
    #else
      sprintf(SQL,"SELECT * FROM PROMOTIONS,PROMOTIONGROUPS where PROMOTIONGROUPS.VSINDEX=PROMOTIONS.PROMOGROUP AND PROMOTIONGROUPS.NAME='%s' AND PROMOTIONS.ACTIVE=1",Param.sValue);
    #endif
  else
      sprintf(SQL,"SELECT * FROM PROMOTIONS WHERE ACTIVE=1");

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) == 1)
    {
      INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"PRODUCTINDEX");
      InsertEntry("VSPINDEX",INDEX->String);

      VSSI = EngineDB->GetField(MAX_SQLSELECT-1,"VSSI");
      sprintf(Buffer,"%li",VSSI->Long);
      InsertEntry("VSPI",Buffer);
      InsertEntry("VSSI",Buffer);

      NAMEOFCATEGORY = EngineDB->GetField(MAX_SQLSELECT-1,"NAMEOFCATEGORY");
      sprintf(Buffer,"%s",NAMEOFCATEGORY->String);
      InsertEntry("CATEGORY",Buffer);
      return 1;
    }

  return 0;
}
/*--------------------------------------------------------------------------------*/
int DBPromotionNext()
{
CDBField* INDEX;
CDBField* VSSI;
CDBField* NAMEOFCATEGORY;
char Buffer[31];
 
  if (EngineDB->GetNext(MAX_SQLSELECT-1))
    {
      INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"PRODUCTINDEX");
      InsertEntry("VSPINDEX",INDEX->String);

      VSSI = EngineDB->GetField(MAX_SQLSELECT-1,"VSSI");
      sprintf(Buffer,"%li",VSSI->Long);
      InsertEntry("VSPI",Buffer);
      InsertEntry("VSSI",Buffer);

      NAMEOFCATEGORY = EngineDB->GetField(MAX_SQLSELECT-1,"NAMEOFCATEGORY");
      sprintf(Buffer,"%s",NAMEOFCATEGORY->String);
      InsertEntry("CATEGORY",Buffer);

      return 1;
    }

  return 0;
}
/*--------------------------------------------------------------------------------*/
t_Value DBPromotionGetColumn(char* ColumnName)
{
  return DBToVSL(EngineDB->GetField(MAX_SQLSELECT-1,ColumnName));
}
/*--------------------------------------------------------------------------------*/
/* RELEVANT Functions */
/*--------------------------------------------------------------------------------*/
int DBRelevantSelect()
{
char SQL[800];
CDBField* CHILDCATNAME;
CDBField* CHILDPROD;
char Category[21];
char Index[21];

  cgiFormString("CATEGORY",Category,20);
  if (cgiFormString("VSINDEX",Index,20) == cgiFormNotFound)
    cgiFormString("VSPINDEX",Index,20);


  sprintf(SQL,"SELECT CHILDCATNAME,CHILDPROD from RELEVANTLINKS where MOTHERPROD='%s' AND MOTHERCATNAME='%s'",Index,Category);
  
  if (EngineDB->SQLSelect(MAX_SQLSELECT-3,SQL) != 0)
    {
      CHILDCATNAME = EngineDB->GetField(MAX_SQLSELECT-3,"CHILDCATNAME");
      CHILDPROD = EngineDB->GetField(MAX_SQLSELECT-3,"CHILDPROD");

      sprintf(SQL,"SELECT * FROM %s WHERE VSINDEX='%s'",CHILDCATNAME->String,CHILDPROD->String);

      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) == 1)
        return 1;
    }


  return 0;
}
/*--------------------------------------------------------------------------------*/
int DBRelevantNext()
{
char SQL[800];
CDBField* CHILDCATNAME;
CDBField* CHILDPROD;

 
  while(EngineDB->GetNext(MAX_SQLSELECT-3))
    {
      CHILDCATNAME = EngineDB->GetField(MAX_SQLSELECT-3,"CHILDCATNAME");
      CHILDPROD = EngineDB->GetField(MAX_SQLSELECT-3,"CHILDPROD");

      sprintf(SQL,"SELECT * FROM %s WHERE VSINDEX='%s'",CHILDCATNAME->String,CHILDPROD->String);

      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) == 1)
        return 1;
    }
  return 0;
}
/*--------------------------------------------------------------------------------*/
t_Value DBRelevantGetColumn(char* ColumnName)
{
  return DBToVSL(EngineDB->GetField(MAX_SQLSELECT-1,ColumnName));
}
/*--------------------------------------------------------------------------------*/
/* SHIPPINGCOST Functions */
/*--------------------------------------------------------------------------------*/
int DBShippingcostSelect()
{
char SQL[800];
CDBField* INDICATOR;
CDBField* MAXAMOUNT;
CDBField* MINAMOUNT;
CDBField* QUANTITY;
char ID[21];
char PaymentID[21];

char BasketCurrency[20];
char Buffer[41];
CDBField* PRODUCTID;
CDBField* CUSTOMDISC;
CDBField* CUSTOMVAT;
CDBField* NAMEOFCATEGORY;
CDBField* PRICE;
double Price;

  ShippingcostQuantity = 0;
  ShippingcostPrice = 0;

  cgiFormString("ID",ID,20);

  /* Anzahl der Produkte im Warenkorb */
  sprintf(SQL,"SELECT SUM(QUANTITY) as SUM_QUANTITY from BASKETITEMS where BASKETID='%s'",ID);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
    {
      QUANTITY = EngineDB->GetField(MAX_SQLSELECT-1,"SUM_QUANTITY");
      ShippingcostQuantity = QUANTITY->Double -1;
    }
  else
    return 0;

  /* Summe der Preise der Produkte im Warenkorb */
  if (GetVar("BASKETCURRENCY", BasketCurrency) == 0)
    strcpy(BasketCurrency,"PRICE");

  sprintf(SQL,"SELECT PRODUCTCATEGORIES.NAMEOFCATEGORY, BASKETITEMS.* from PRODUCTCATEGORIES,BASKETITEMS where PRODUCTCATEGORIES.VSINDEX=BASKETITEMS.CATEGORYID AND BASKETID='%s'",ID);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
    {
      do
        {
          PRODUCTID =  EngineDB->GetField(MAX_SQLSELECT-1,"PRODUCTID");
          CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMDISC");
          CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMVAT");
          QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");
          NAMEOFCATEGORY = EngineDB->GetField(MAX_SQLSELECT-1,"NAMEOFCATEGORY");

          sprintf(SQL,"SELECT * FROM %s where VSINDEX='%s'",NAMEOFCATEGORY->String,PRODUCTID->String);
          if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
            {
               PRICE =  EngineDB->GetField(MAX_SQLSELECT-2,BasketCurrency);

               Price = PRICE->Double;
               Price = Price - ((Price / 100) * CUSTOMDISC->Double);
               Price = Price + ((Price / 100) * CUSTOMVAT->Double);
               sprintf(Buffer,"%.2f",Price);
               Price= atof(Buffer);

               ShippingcostPrice += Price * QUANTITY->Long;
             }
         }
       while (EngineDB->GetNext(MAX_SQLSELECT-1));

    }

  if (cgiFormString("PAYMENTID",PaymentID,20) != cgiFormNotFound)
    sprintf(SQL,"SELECT SHIPPINGCOSTS.* from SHIPPINGCOSTS,PAY_AND_SHIP where PAY_AND_SHIP.OFFER=1 AND PAY_AND_SHIP.SHIPPING=SHIPPINGCOSTS.VSINDEX AND PAY_AND_SHIP.PAYMENT='%s'",PaymentID);

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
    {
      do
        {
          MAXAMOUNT = EngineDB->GetField(MAX_SQLSELECT-1,"MAXAMOUNT");
          MINAMOUNT = EngineDB->GetField(MAX_SQLSELECT-1,"MINAMOUNT");
          INDICATOR = EngineDB->GetField(MAX_SQLSELECT-1,"INDICATOR");

          if (strcasecmp(INDICATOR->String,"QUANTITY") == 0)
            {
              if ((MAXAMOUNT->Double > ShippingcostQuantity) && (MINAMOUNT->Double <= ShippingcostQuantity))
                return 1;
              else
              if ((MAXAMOUNT->Double == 0) && (MINAMOUNT->Double <= ShippingcostQuantity))
                return 1;
            }
          else
            {
              if ((MAXAMOUNT->Double == 0) && (MINAMOUNT->Double <= ShippingcostPrice))
                return 1;

              if ((MAXAMOUNT->Double > ShippingcostPrice) && (MINAMOUNT->Double <= ShippingcostPrice))
                {
                  return 1;
                }
            }
        }
      while (EngineDB->GetNext(MAX_SQLSELECT-1));
    }

  return 0;
}
/*--------------------------------------------------------------------------------*/
int DBShippingcostNext()
{
CDBField* INDICATOR;
CDBField* MAXAMOUNT;
CDBField* MINAMOUNT;

  if (EngineDB->GetNext(MAX_SQLSELECT-1) != 0)
    {
      do 
        {
          MAXAMOUNT = EngineDB->GetField(MAX_SQLSELECT-1,"MAXAMOUNT");
          MINAMOUNT = EngineDB->GetField(MAX_SQLSELECT-1,"MINAMOUNT");
          INDICATOR = EngineDB->GetField(MAX_SQLSELECT-1,"INDICATOR");
          if (strcasecmp(INDICATOR->String,"QUANTITY") == 0)
            {
              if ((MAXAMOUNT->Double > ShippingcostQuantity) && (MINAMOUNT->Double <= ShippingcostQuantity))
                return 1;
              
              if ((MAXAMOUNT->Double == 0) && (MINAMOUNT->Double <= ShippingcostQuantity))
                return 1;
            }
          else
            {
              if ((MAXAMOUNT->Double > ShippingcostPrice) && (MINAMOUNT->Double <= ShippingcostPrice))
                return 1;

              if ((MAXAMOUNT->Double == 0) && (MINAMOUNT->Double <= ShippingcostPrice))
                return 1;
            }
        }
      while (EngineDB->GetNext(MAX_SQLSELECT-1));
    }

  return 0;
}
/*--------------------------------------------------------------------------------*/
t_Value DBShippingcostGetColumn(char* ColumnName)
{
  return DBToVSL(EngineDB->GetField(MAX_SQLSELECT-1,ColumnName));
}
/*--------------------------------------------------------------------------------*/
/*FILEIMPORT Functions */
/*--------------------------------------------------------------------------------*/
t_Value DBImportFile(char* FileName, char* TableName, char FieldSeparator)
{
/*
Return Values :

0 - Fehler beim Lesen der Datei
1 - alles OK
2 - Feld in Tabelle, aber nicht in Datei
3 - Feld in Datei, aber nicht in Tabelle
4 - Kombination aus 2 und 3
5 - Fehler beim Einfuegen in Tabelle
*/

#define BUFFERLENGTH 4800
#define MAXFIELDLENGTH 4000
#define MAXFIELDS 200
#define MAXFIELDNAMELENGTH 50

t_Value ret_val;
CDBField* DBField;
FILE *dout;
char FieldName[MAXFIELDS][MAXFIELDNAMELENGTH];
char Field[MAXFIELDS][MAXFIELDLENGTH];
char Buffer[BUFFERLENGTH+1];
int FieldCounter = 0;
char Values[MAXFIELDLENGTH];
char Names[MAXFIELDLENGTH];
char ID[21];

  ret_val.typ = Bool_Value;
  ret_val.nValue = 1;

  cgiFormString("ID",ID,20);
  sprintf(Buffer,"create table TMP%s like %s",ID,TableName);

  if (EngineDB->SQLUpdate(Buffer) != 1)
    {
      ret_val.nValue = 0;
      return ret_val;
    }
  
  sprintf(Buffer,"select * from TMP%s where VSINDEX=''",ID);
  EngineDB->SQLSelect(MAX_SQLSELECT-1,Buffer);

  SetFieldSeparator(FieldSeparator);
  
  if ((dout = fopen(FileName, "ra")) == NULL)
    {
      ret_val.nValue = 0;
      return ret_val;
    }

  while(GetField(dout,Buffer,BUFFERLENGTH))
    {
      if (Buffer[0] == '\'')
        {
          strcpy(FieldName[FieldCounter],&Buffer[1]);
          FieldName[FieldCounter][strlen(FieldName[FieldCounter])-1]='\0';
        }
      else
        strcpy(FieldName[FieldCounter],Buffer);

      FieldCounter++;
    }

  if ((Buffer[0] == '\'') || (Buffer[0] == '"'))
    {
      strcpy(FieldName[FieldCounter],&Buffer[1]);
      FieldName[FieldCounter][strlen(FieldName[FieldCounter])-1]='\0';
    }
  else
    strcpy(FieldName[FieldCounter],Buffer);

  if (EngineDB->GetNumCols(MAX_SQLSELECT-1) > FieldCounter)
    {
      ret_val.nValue = 2;
    }

  while(NextRecord(dout))
     {
       FieldCounter = 0;
       while(GetField(dout,Buffer,BUFFERLENGTH))
         {
           strcpy(Field[FieldCounter],Buffer);
           FieldCounter++;
         }

       strcpy(Field[FieldCounter],Buffer);

       strcpy(Values,"");
       strcpy(Names,"");


       for (int i = 0; i<=FieldCounter;i++)
         {
           if ((DBField = EngineDB->GetField(MAX_SQLSELECT-1,FieldName[i])) != NULL)
             {
               strcat(Names,FieldName[i]);
               strcat(Names,",");

               switch(DBField->Type)
                 {
                   case FT_STRING:
                   case FT_DATE:
                   case FT_TIME:
                   case FT_TIMESTAMP:
                     //DBReplace(Field[i]);

                     if (Field[i][0] != '\'')
                       {
                         strcat(Values,"'");
                       }

                     if (strlen(Field[i]) > 0)
                       {
                         if (((Field[i][0] == '\'') && (strlen(Field[i])-2 > DBField->StringLength)) ||
                             ((Field[i][0] != '\'') && (strlen(Field[i]) > DBField->StringLength)))
                           {
                             Field[i][DBField->StringLength] = '\0';
                             strcat(Values,Field[i]);
                             strcat(Values,"'");
                           }
                         else
                           strcat(Values,Field[i]);

                       if (Field[i][strlen(Field[i])-1] != '\'')
                         {
                           strcat(Values,"'");
                         }

                       }
                     else
                       strcat(Values,"''");
                     
                     
                     strcat(Values,",");
                     break;
                   case FT_BOOLEAN:
                     if ((strcasecmp(Field[i],"FALSE") == 0) || (strcmp(Field[i],"0") == 0))
                       strcat(Values,"FALSE");
                     else
                       strcat(Values,"TRUE");
                     strcat(Values,",");
                     break;
                   case FT_LONG:
                     if (strlen(Field[i]) > 0)
                       strcat(Values,Field[i]);
                     else
                       strcat(Values,"0");
                     strcat(Values,",");
                     break;
                   case FT_DOUBLE:
                     Replace(Field[i],DECIMALSYMBOL_STRING,".");
                     if (strlen(Field[i]) > 0)
                       strcat(Values,Field[i]);
                     else
                       strcat(Values,"0");
                     strcat(Values,",");
                     break;
                   default:
                     RuntimeError2("ImportFile unsupported typ of column ",DBField->Name);
                     fclose(dout);  
                     ret_val.nValue = 0;
                     return ret_val;
                     break;
                 }
             }
           else
             {
               if (ret_val.nValue == 2)
                 ret_val.nValue = 4;
               else
                 ret_val.nValue = 3;
             }
         }
       Values[strlen(Values)-1] = '\0';
       Names[strlen(Names)-1] = '\0';


       sprintf(Buffer,"insert into TMP%s (%s) values (%s)",ID,Names,Values);


       if (EngineDB->SQL(Buffer) != 1)
         {
           ret_val.nValue = 5;
         }

     }
   fclose(dout);  

   return ret_val;
}
/*--------------------------------------------------------------------------------*/
/*FILEIMPORT Functions */
/*--------------------------------------------------------------------------------*/
t_Value DBExportTable(char* FileName, char* TableName, int Type , char* FieldSeparator)
{
t_Value ret_val;
CDBField* DBField;
FILE *dout;
char Line[4500];
char Fields[4500];
char Buffer[200];

  ret_val.typ = Bool_Value;
  ret_val.nValue = 1;

  sprintf(Buffer,"select * from %s",TableName);
  EngineDB->SQLSelect(MAX_SQLSELECT-1,Buffer);

  if ((dout = fopen(FileName, "w")) == NULL)                            
    {
      ret_val.nValue = 0;
      return ret_val;
    }

  if (Type == 0)
    {
      strcpy(Line,"");
      DBField =  EngineDB->GetFirstField(MAX_SQLSELECT-1);
      while(DBField != NULL)
        {
          strcat(Line,"'");
          strcat(Line,DBField->Name);
          strcat(Line,"'");
          if ((DBField = EngineDB->GetNextField(MAX_SQLSELECT-1)) != NULL)
            strcat(Line,FieldSeparator);
        }
      strcat(Line,"\n");
      fprintf(dout,Line);
    }
  else
    {
      sprintf(Fields,"INSERT INTO %s (",TableName);
      DBField =  EngineDB->GetFirstField(MAX_SQLSELECT-1);
      while(DBField != NULL)
        {
          strcat(Fields,"\"");
          strcat(Fields,DBField->Name);
          strcat(Fields,"\"");
          if ((DBField = EngineDB->GetNextField(MAX_SQLSELECT-1)) != NULL)
            strcat(Fields,",");
        }
      strcat(Fields,") VALUES (");
    }

  while(! EngineDB->IsEOF(MAX_SQLSELECT-1))
    {
        strcpy(Line,"");
        DBField =  EngineDB->GetFirstField(MAX_SQLSELECT-1);
        while(DBField != NULL)
          {
            switch(DBField->Type)
              {
                case FT_STRING:
                  strcat(Line,"'");
                  strcat(Line,DBField->String);
                  strcat(Line,"'");
                  break;
                case FT_DATE:
                  strcat(Line,"'");
                  strcat(Line,DBField->Date);
                  strcat(Line,"'");
                  break;
                case FT_TIME:
                  strcat(Line,"'");
                  strcat(Line,DBField->Time);
                  strcat(Line,"'");
                  break;
                case FT_TIMESTAMP:
                  strcat(Line,"'");
                  strcat(Line,DBField->TimeStamp);
                  strcat(Line,"'");
                  break;
                case FT_BOOLEAN:
                case FT_LONG:
                  sprintf(Buffer,"%ld",DBField->Long);
                  strcat(Line,Buffer);
                  break;
                case FT_DOUBLE:
                  sprintf(Buffer,"%f",DBField->Double);
                  strcat(Line,Buffer);
                  break;
              }
            if ((DBField = EngineDB->GetNextField(MAX_SQLSELECT-1)) != NULL)
              {
                if (Type == 0)
                  strcat(Line,FieldSeparator);
                else
                  strcat(Line,",");
              }
          }
        if (Type == 0)
          {
            strcat(Line,"\n");
            fprintf(dout,Line);
          }
        else
          {
            strcat(Line,");\n");
            fprintf(dout,Fields);
            fprintf(dout,Line);
          }
        EngineDB->GetNext(MAX_SQLSELECT-1);
    }


   fclose(dout);  

   return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value DBExportDB(char* FileName, int LoadFlag)
{
t_Value ret_val;
CDBField* DBField;
FILE *dout;
char Line[4500];
char Fields[4500];
char SQL[200];
char Buffer[200];
char TableName[21];
char Datatype[21];
char IndexName[51];
char Owner[21];


  ret_val.typ = Bool_Value;
  ret_val.nValue = 1;

  if ((dout = fopen(FileName, "w")) == NULL)
    {
      ret_val.nValue = 0;
      return ret_val;
    }

  strcpy(Owner,strrchr(CGIBIN,'/')+1);
  #ifdef DB_ORACLE
    sprintf(SQL,"select * from user_tables");
  #else
    sprintf(SQL,"select * from tables where TYPE='TABLE' AND OWNER=upper('%s')",Owner);
  #endif

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
    {
    while(! EngineDB->IsEOF(MAX_SQLSELECT-1))
      {
        DBField = EngineDB->GetField(MAX_SQLSELECT-1,"TABLE_NAME");
        strcpy(TableName,DBField->String);
		fprintf(dout,"\n");
        fprintf(dout,"CREATE TABLE %s (",TableName);

        #ifdef DB_ORACLE
          sprintf(SQL,"select * from user_tab_columns where TABLE_NAME='%s'",TableName);
        #else
          sprintf(SQL,"select * from COLUMNS where TABLENAME='%s'",TableName);
        #endif
        if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
          {
          do
            {
              #ifdef DB_ORACLE
                DBField = EngineDB->GetField(MAX_SQLSELECT-2,"COLUMN_NAME");
              #else
                DBField = EngineDB->GetField(MAX_SQLSELECT-2,"COLUMNNAME");
              #endif
              fprintf(dout,"\"%s\" ",DBField->String);
             
              #ifdef DB_ORACLE
                DBField = EngineDB->GetField(MAX_SQLSELECT-2,"DATA_TYPE");
              #else
                DBField = EngineDB->GetField(MAX_SQLSELECT-2,"DATATYPE");
              #endif
              strcpy(Datatype,DBField->String);
              fprintf(dout,"%s",Datatype);

              if (((strcmp(DBField->String,"BOOLEAN") != 0)) && 
                  ((strcmp(DBField->String,"TIMESTAMP") != 0)) && 
                  ((strcmp(DBField->String,"DATE") != 0)) &&
                  ((strcmp(DBField->String,"TIME") != 0)) &&
                  ((strcmp(DBField->String,"INTEGER") != 0)))
                {
                  #ifdef DB_ORACLE
                    DBField = EngineDB->GetField(MAX_SQLSELECT-2,"DATA_LENGTH");
                  #else
                    DBField = EngineDB->GetField(MAX_SQLSELECT-2,"LEN");
                  #endif
                  fprintf(dout,"(%ld)",DBField->Long);
                }


              DBField = EngineDB->GetField(MAX_SQLSELECT-2,"NULLABLE");
              if (strcmp(DBField->String,"N") == 0)
                fprintf(dout," PRIMARY KEY");
/*              
              DBField = EngineDB->GetField(MAX_SQLSELECT-2,"DEFAULT");
              if (DBField->Indicator != -1)
                {
                  if (strcmp(Datatype,"TIMESTAMP") == 0)
                    fprintf(dout," DEFAULT TIMESTAMP");
                  else
                  if (strcmp(Datatype,"DATE") == 0)
                    fprintf(dout," DEFAULT DATE");
                  else
                  if (strcmp(Datatype,"TIME") == 0)
                    fprintf(dout," DEFAULT TIME");
                  else
                  if (strcmp(Datatype,"VARCHAR") == 0)
                    fprintf(dout," DEFAULT '%s'",DBField->String);
                  else
                  if (strcmp(Datatype,"CHAR") == 0)
                    fprintf(dout," DEFAULT '%s'",DBField->String);
                  else
                  if (strcmp(Datatype,"FLOAT") == 0)
                    fprintf(dout," DEFAULT %.4f",atof(DBField->String));
                  else
                  if (strcmp(Datatype,"INTEGER") == 0)
                    fprintf(dout," DEFAULT %i",atoi(DBField->String));
                  else
                    fprintf(dout," DEFAULT %s",DBField->String);
                }
*/

              EngineDB->GetNext(MAX_SQLSELECT-2);
              if (!EngineDB->IsEOF(MAX_SQLSELECT-2))
                fprintf(dout,",");
            }
          while (!EngineDB->IsEOF(MAX_SQLSELECT-2));
          }

        fprintf(dout,")");
        if (LoadFlag)
          fprintf(dout,";\n");
		else  
		  fprintf(dout,"\n");

/*
        sprintf(SQL,"select * from user_ind_columns where TABLE_NAME='%s'",TableName);
        if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
          {
            do
              {
                DBField = EngineDB->GetField(MAX_SQLSELECT-2,"INDEX_NAME");
                strcpy(IndexName,DBField->String);

                sprintf(Fields,"CREATE INDEX %s ON %s (",IndexName,TableName);
                        DBField = EngineDB->GetField(MAX_SQLSELECT-2,"COLUMN_NAME");
                        strcat(Fields,DBField->String);
                        strcat(Fields," ");

                        DBField = EngineDB->GetField(MAX_SQLSELECT-2,"DESCEND");
                        if (strcmp(DBField->String,"DES") ==0)
                          strcat(Fields,"DESC");
                        else
                          strcat(Fields,DBField->String);

                        EngineDB->GetNext(MAX_SQLSELECT-2);
                        if (!EngineDB->IsEOF(MAX_SQLSELECT-2))
                          strcat(Fields,", ");
                        else
                          strcat(Fields,")");

                   fprintf(dout,Fields);
           
                   if (LoadFlag)
                     fprintf(dout,";\n\n");
				   else	 
				     fprintf(dout,"\n\n");
              }
            while(EngineDB->GetNext(MAX_SQLSELECT-2));
          }
*/
        sprintf(SQL,"select * from %s",TableName);
        if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
          {
            sprintf(Fields,"INSERT INTO %s (",TableName);
            DBField =  EngineDB->GetFirstField(MAX_SQLSELECT-2);
            while(DBField != NULL)
              {
                strcat(Fields,"\"");
                strcat(Fields,DBField->Name);
                strcat(Fields,"\"");
                if ((DBField = EngineDB->GetNextField(MAX_SQLSELECT-2)) != NULL)
                  strcat(Fields,",");
              }
            strcat(Fields,") VALUES (");

            while(! EngineDB->IsEOF(MAX_SQLSELECT-2))
              {
                strcpy(Line,"");
                DBField =  EngineDB->GetFirstField(MAX_SQLSELECT-2);
                while(DBField != NULL)
                  {
                    switch(DBField->Type)
                      {
                        case FT_MEMO:
                        case FT_STRING:
                          strcat(Line,"'");
                          strcat(Line,DBField->String);
                          strcat(Line,"'");
                          break;
                        case FT_DATE:
                          strcat(Line,"'");
                          strcat(Line,DBField->Date);
                          strcat(Line,"'");
                          break;
                        case FT_TIME:
                          strcat(Line,"'");
                          strcat(Line,DBField->Time);
                          strcat(Line,"'");
                          break;
                        case FT_TIMESTAMP:
                          strcat(Line,"'");
                          strcat(Line,DBField->TimeStamp);
                          strcat(Line,"'");
                          break;
                        case FT_BOOLEAN:
                        case FT_LONG:
                          sprintf(Buffer,"%ld",DBField->Long);
                          strcat(Line,Buffer);
                          break;
                        case FT_DOUBLE:
                          sprintf(Buffer,"%f",DBField->Double);
                          strcat(Line,Buffer);
                          break;
                        case FT_UNKNOWN:
                        case FT_EMPTY:
                          break;
                      }
                    if ((DBField = EngineDB->GetNextField(MAX_SQLSELECT-2)) != NULL)
                      strcat(Line,",");
                  }
                strcat(Line,")");
                fprintf(dout,Fields);
                fprintf(dout,Line);
                if (LoadFlag)
                  fprintf(dout,";\n");
				else  
				  fprintf(dout,"\n");

                EngineDB->GetNext(MAX_SQLSELECT-2);
              }
          }
        EngineDB->GetNext(MAX_SQLSELECT-1);
      }
    }
   fclose(dout);  

   return ret_val;
}

