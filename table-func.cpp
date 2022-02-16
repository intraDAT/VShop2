#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "vsl.h"
#include "error.h"
#include "db.h"

#include "table-func.h"

#ifdef WIN32
  #include "win32tools.h"
#endif


#ifdef __cplusplus 
  extern "C" {
#endif

#define MAX_SQLLEN 2048
#define MAX_TABLENAME 50

#define MAX_FILENAME 250

#define MAX_FILESOPEN 50

extern char *seg_text;

typedef struct Recordset 
{
  char TableName[MAX_TABLENAME];
  int SelectIndex;
  struct Recordset* Next;
} Recordset;

int LastSelectIndex = 0;

Recordset* First;


typedef struct Fileset 
{
  FILE* FileHandle;
  char FileName[MAX_FILENAME];
  int FileNumber;
  struct Fileset* Next;
} Fileset;

Fileset* FirstFileset;

int LastFileOpen = 0;

/*--------------------------------------------------------------------------------*/
Recordset* GetRecordset(char *TableName)
{

Recordset* Ptr=First;
Recordset* Prev=First;

  while(Ptr) 
    {
      if (!strcasecmp(Ptr->TableName, TableName)) 
        break;

      Prev = Ptr;
      Ptr = Ptr->Next;
    }

  if (!Ptr) 
    {
      if (Prev) 
        {
          Prev->Next = (Recordset*) malloc(sizeof(Recordset));
          Ptr = Prev->Next;
        } 
      else 
        {
          First = (Recordset*) malloc(sizeof(Recordset));
          Ptr = First;
        }

      strcpy(Ptr->TableName, TableName);
      Ptr->SelectIndex = -1;
      Ptr->Next = NULL;
    }    
  return Ptr;
}
/*--------------------------------------------------------------------------------*/
t_Value table_lookup(t_Value *arg_list) 
{
t_Value ret_val;
Recordset *CurrentRecordset;
char SQL[MAX_SQLLEN];

  ret_val.typ = Not_Set;
  if (arg_list[0].typ == String_Value)
    {
      CurrentRecordset = GetRecordset(arg_list[0].sValue);
      if (CurrentRecordset->SelectIndex == -1) 
        {
          CurrentRecordset->SelectIndex = LastSelectIndex;
          LastSelectIndex++;
       
          sprintf(SQL,"select * from %s",arg_list[0].sValue);
          EngineDB->SQLSelect(CurrentRecordset->SelectIndex, SQL);
          //DBSelect(CurrentRecordset->SelectIndex,SQL);
        }

      if (arg_list[1].typ == String_Value) 
        ret_val = DBGetColumn(CurrentRecordset->SelectIndex, arg_list[1].sValue);
      else
      if (arg_list[1].typ == Int_Value) 
        ret_val = DBGetColumnByIndex(CurrentRecordset->SelectIndex, arg_list[1].nValue);
      else
        {
          RuntimeError1("%%%%[TABLE].[FIELDNAME] - FIELDNAME must be NAME");
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].[FIELDNAME] - TABLE must be NAME");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_get(t_Value *arg_list)
{
t_Value ret_val;
Recordset *CurrentRecordset;
char SQL[MAX_SQLLEN];

  ret_val.typ = Not_Set;
  if (arg_list[0].typ == String_Value)
    {
      CurrentRecordset = GetRecordset(arg_list[0].sValue);
      if (CurrentRecordset->SelectIndex == -1) 
        {
          CurrentRecordset->SelectIndex = LastSelectIndex;
          LastSelectIndex++;
       
          sprintf(SQL,"select * from %s",arg_list[0].sValue);
          DBSelect(CurrentRecordset->SelectIndex,SQL);
        }

      if (arg_list[1].typ == String_Value) 
        ret_val = DBGetColumn(CurrentRecordset->SelectIndex, arg_list[1].sValue);
      else
      if (arg_list[1].typ == Int_Value) 
        ret_val = DBGetColumnByIndex(CurrentRecordset->SelectIndex, arg_list[1].nValue);
      else
      if (arg_list[1].typ == Float_Value) 
        ret_val = DBGetColumnByIndex(CurrentRecordset->SelectIndex, (long) arg_list[1].dValue);
      else
        {
          RuntimeError1("%%%%[TABLE].get[PARAMETER] - PARAMETER must be STRING/INTEGER/FLOAT");
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].get[PARAMETER] - TABLE must be NAME");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_columnname(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  ret_val.typ=Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if ((arg_list[1].typ == Int_Value) || (arg_list[1].typ == Float_Value))
        {
          CurrentRecordset = GetRecordset(arg_list[0].sValue);
          if (CurrentRecordset->SelectIndex != -1)
            {
              if (arg_list[1].typ == Int_Value)
                ret_val = DBGetColumnName(CurrentRecordset->SelectIndex, arg_list[1].nValue);
              else
                ret_val = DBGetColumnName(CurrentRecordset->SelectIndex, (long) arg_list[1].dValue);
            }
          else
            {
              RuntimeError2("%%%%[TABLE].columnname[PARAMETER] - No Select for TABLE ",arg_list[0].sValue);
            }
        }
      else
        {
          RuntimeError1("%%%%[TABLE].columnname[PARAMETER] - PARAMETER must be INTEGER/FLOAT");
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].columnname[PARAMETER] - TABLE must be NAME");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_inputfield(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;
long Param1;

  ret_val.typ=Not_Set;
  
  if (arg_list[0].typ == String_Value)
    {
      if ((arg_list[1].typ == Int_Value) || (arg_list[1].typ == Float_Value))
        {
          CurrentRecordset = GetRecordset(arg_list[0].sValue);
          if (CurrentRecordset->SelectIndex != -1)
            {
              if (arg_list[1].typ == Int_Value)
                Param1 = arg_list[1].nValue;
              else
                Param1 = (long) arg_list[1].dValue;

              if (arg_list[2].typ == String_Value)
                ret_val = DBCreateInputField(CurrentRecordset->SelectIndex, Param1,arg_list[2].sValue);
              else
                ret_val = DBCreateInputField(CurrentRecordset->SelectIndex, Param1, "");
            }
          else
            {
              RuntimeError2("%%%%[TABLE].inputfield[PARAMETER] - No Select for TABLE ",arg_list[0].sValue);
            }
        }
      else
        {
          RuntimeError1("%%%%[TABLE].inputfield[PARAMETER] - PARAMETER must be INTEGER/FLOAT");
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].inputfield[PARAMETER] - TABLE must be NAME");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_next(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      CurrentRecordset = GetRecordset(arg_list[0].sValue);
      if (CurrentRecordset->SelectIndex != -1)
        return DBGetNext(CurrentRecordset->SelectIndex);
      else
        {
          RuntimeError2("%%%%[TABLE].next - No Select for TABLE ",arg_list[0].sValue);
          ret_val.typ = Bool_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].prev - TABLE must be NAME");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_prev(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      CurrentRecordset = GetRecordset(arg_list[0].sValue);
      if (CurrentRecordset->SelectIndex != -1)
        return DBGetPrev(CurrentRecordset->SelectIndex);
      else
        {
          RuntimeError2("%%%%[TABLE].prev - No Select for TABLE ",arg_list[0].sValue);
          ret_val.typ = Bool_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].prev - TABLE must be NAME");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_last(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      CurrentRecordset = GetRecordset(arg_list[0].sValue);
      if (CurrentRecordset->SelectIndex != -1)
        return DBGetLast(CurrentRecordset->SelectIndex);
      else
        {
          RuntimeError2("%%%%[TABLE].last - No Select for TABLE ",arg_list[0].sValue);
          ret_val.typ = Bool_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].last - TABLE must be NAME");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_first(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      CurrentRecordset = GetRecordset(arg_list[0].sValue);
      if (CurrentRecordset->SelectIndex != -1)
        return DBGetFirst(CurrentRecordset->SelectIndex);
      else
        {
          RuntimeError2("%%%%[TABLE].first - No Select for TABLE ",arg_list[0].sValue);
          ret_val.typ = Bool_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].first - TABLE must be NAME");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_at(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      if ((arg_list[1].typ == Int_Value) || (arg_list[1].typ == Float_Value))
        {
          CurrentRecordset = GetRecordset(arg_list[0].sValue);
          if (CurrentRecordset->SelectIndex != -1)
            {
              if (arg_list[1].typ == Int_Value)
                return DBGetAt(CurrentRecordset->SelectIndex, arg_list[1].nValue);
              else
                return DBGetAt(CurrentRecordset->SelectIndex, (long) arg_list[1].dValue);
            }
          else
            {
              RuntimeError2("%%%%[TABLE].at[PARAMETER] - No Select for TABLE ",arg_list[0].sValue);
              ret_val.typ = Bool_Value;
              ret_val.nValue = 0;
            }
        }
      else
        {
          RuntimeError1("%%%%[TABLE].at[PARAMETER] - PARAMETER must be INTEGER/FLOAT");
          ret_val.typ = Bool_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].at[PARAMETER] - TABLE must be NAME");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_eof(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      CurrentRecordset = GetRecordset(arg_list[0].sValue);
      if (CurrentRecordset->SelectIndex != -1)
        return DBIsEOF(CurrentRecordset->SelectIndex);
      else
        {
          RuntimeError2("%%%%[TABLE].eof - No Select for TABLE ",arg_list[0].sValue);
          ret_val.typ = Bool_Value;
          ret_val.nValue = 1;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].get[PARAMETER] - TABLE must be NAME");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 1;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_bof(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      if ((arg_list[1].typ == Int_Value) || (arg_list[1].typ == Float_Value))
        {
          CurrentRecordset = GetRecordset(arg_list[0].sValue);
          if (CurrentRecordset->SelectIndex != -1)
            {
              if (arg_list[1].typ == Int_Value)
                return DBIsBOF(CurrentRecordset->SelectIndex,arg_list[1].nValue);
              else
                return DBIsBOF(CurrentRecordset->SelectIndex,(long) arg_list[1].dValue);
            }
          else
            {
              RuntimeError2("%%%%[TABLE].bof[PARAMETER] - No Select for TABLE ",arg_list[0].sValue);
              ret_val.typ = Bool_Value;
              ret_val.nValue = 1;
            }
        }
      else
        {
          RuntimeError1("%%%%[TABLE].inputfield[PARAMETER] - PARAMETER must be INTEGER/FLOAT");
          ret_val.typ = Bool_Value;
          ret_val.nValue = 1;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].get[PARAMETER] - TABLE must be NAME");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 1;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_columns(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      CurrentRecordset = GetRecordset(arg_list[0].sValue);
      if (CurrentRecordset->SelectIndex != -1)
        {
          return DBGetColumns(CurrentRecordset->SelectIndex);
        }
      else
        {
          RuntimeError2("%%%%[TABLE].columns[PARAMETER] - No Select for TABLE ",arg_list[0].sValue);
          ret_val.typ = Int_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].get[PARAMETER] - TABLE must be NAME");
      ret_val.typ = Int_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_rows(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      CurrentRecordset = GetRecordset(arg_list[0].sValue);
      if (CurrentRecordset->SelectIndex != -1)
        {
          return DBGetRows(CurrentRecordset->SelectIndex);
        }
      else
        {
          RuntimeError2("%%%%[TABLE].rows[PARAMETER] - No Select for TABLE ",arg_list[0].sValue);
          ret_val.typ = Int_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].get[PARAMETER] - TABLE must be NAME");
      ret_val.typ = Int_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_row(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      CurrentRecordset = GetRecordset(arg_list[0].sValue);
      if (CurrentRecordset->SelectIndex != -1)
        {
          return DBGetCurrentRow(CurrentRecordset->SelectIndex);
        }
      else
        {
          RuntimeError2("%%%%[TABLE].row[PARAMETER] - No Select for TABLE ",arg_list[0].sValue);
          ret_val.typ = Int_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].get[PARAMETER] - TABLE must be NAME");
      ret_val.typ = Int_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_select(t_Value *arg_list)
{
t_Value ret_val;
Recordset* CurrentRecordset;
int ArgCounter;
char* Ptr;
char* SQLPtr;
char* CurrentPtr;
char SQL[MAX_SQLLEN];
char Buffer[MAX_SQLLEN];
struct tm* Today;

  ret_val.typ=Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          CurrentRecordset = GetRecordset(arg_list[0].sValue);
          if (CurrentRecordset->SelectIndex == -1) 
            {
              CurrentRecordset->SelectIndex = LastSelectIndex;
              LastSelectIndex++;
            }

          SQLPtr = &(SQL[0]);

          if ((Ptr = strchr(arg_list[1].sValue,'?')) == NULL)
            {
              strcpy(SQL, arg_list[1].sValue);    
            }
          else
            {
              ArgCounter = 2;
              CurrentPtr = arg_list[1].sValue;

              do
                {
                  while (CurrentPtr<Ptr)
                    {
                      *SQLPtr++=*CurrentPtr++;
                    }
              
                  switch(arg_list[ArgCounter].typ)
                    {
                      case String_Value:
                        strcpy(Buffer,arg_list[ArgCounter].sValue);
                        break;
                      case Int_Value:
                      case Bool_Value:
                        sprintf(Buffer,"%ld",arg_list[ArgCounter].nValue);
                        break;
                      case Float_Value:
                        sprintf(Buffer,"%f",arg_list[ArgCounter].dValue);
                        break;
                  case Date_Value:
                     Today = localtime(&(arg_list[ArgCounter].tValue));
                     strftime(Buffer,MAX_SQLLEN,"%Y%m%d",Today);
                  case Time_Value:
                     Today = localtime(&(arg_list[ArgCounter].tValue));
                     strftime(Buffer,MAX_SQLLEN,"%H%M",Today);
                  case Timestamp_Value:
                     Today = localtime(&(arg_list[ArgCounter].tValue));
                     strftime(Buffer,MAX_SQLLEN,"%Y%m%d%H%M",Today);
                    break;
                  case Not_Set:
                  case Unknown:
                  default:
                    RuntimeError1("%%%%select[PARAMETER1,(PARAMETER2,...)] - Not enough parameters");
                    strcpy(Buffer,"");
                    break;
                    }
              
                  strcpy(SQLPtr,Buffer);
                  SQLPtr = (char*)SQLPtr+strlen(Buffer);

                  CurrentPtr++;

                  ArgCounter++;

                }
              while((Ptr = strchr(CurrentPtr,'?')) != NULL);

              strcat(SQL,CurrentPtr);

            }
          ret_val = DBSelect(CurrentRecordset->SelectIndex,SQL);
        }
      else
        {
          RuntimeError1("%%%%[TABLE].select[PARAMETER1,(PARAMETER2,...)] - PARAMETER1 must be STRING");
          ret_val.typ = Bool_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].select[PARAMETER1,(PARAMETER2,...)] - TABLE must be NAME");
      ret_val.typ = Int_Value;
      ret_val.nValue = 0;
    }                         

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_searchform(t_Value *arg_list)
{
t_Value ret_val;
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          CurrentRecordset = GetRecordset(arg_list[0].sValue);
          if (CurrentRecordset->SelectIndex == -1) 
            {
              CurrentRecordset->SelectIndex = LastSelectIndex;
              LastSelectIndex++;
            }

          ret_val = DBSearchForm(CurrentRecordset->SelectIndex,arg_list[1].sValue);
        }
      else
        {
          RuntimeError1("%%%%[TABLE].searchform[PARAMETER] - PARAMETER must be STRING");
          ret_val.typ = Bool_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].searchform[PARAMETER] - TABLE must be NAME");
      ret_val.typ = Int_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_seek(t_Value *arg_list)
{
t_Value ret_val; 
Recordset* CurrentRecordset;

  if (arg_list[0].typ == String_Value)
    {
      if ((arg_list[1].typ == Int_Value) || (arg_list[1].typ == Float_Value) || (arg_list[1].typ == String_Value))
        {
          if ((arg_list[2].typ == Int_Value) || (arg_list[2].typ == Float_Value))
            {
              CurrentRecordset = GetRecordset(arg_list[0].sValue);
              if (CurrentRecordset->SelectIndex != -1)
                {
                  if ((arg_list[1].typ == String_Value) && (arg_list[2].typ == Int_Value))
                    return DBSeek(CurrentRecordset->SelectIndex,(long) atof(arg_list[1].sValue),arg_list[2].nValue);
                  else
                  if ((arg_list[1].typ == String_Value) && (arg_list[2].typ == Float_Value))
                    return DBSeek(CurrentRecordset->SelectIndex,(long) atof(arg_list[1].sValue),(long) arg_list[2].dValue);
                  else
                  if ((arg_list[1].typ == String_Value) && (arg_list[2].typ == String_Value))
                    return DBSeek(CurrentRecordset->SelectIndex,(long) atof(arg_list[1].sValue),(long) atof(arg_list[2].sValue));
                  else
                  if ((arg_list[1].typ == Int_Value) && (arg_list[2].typ == Int_Value))
                    return DBSeek(CurrentRecordset->SelectIndex,arg_list[1].nValue,arg_list[2].nValue);
                  else
                  if ((arg_list[1].typ == Int_Value) && (arg_list[2].typ == Float_Value))
                    return DBSeek(CurrentRecordset->SelectIndex,arg_list[1].nValue,(long) arg_list[2].dValue);
                  else
                  if ((arg_list[1].typ == Int_Value) && (arg_list[2].typ == String_Value))
                    return DBSeek(CurrentRecordset->SelectIndex,arg_list[1].nValue,(long) atof(arg_list[2].sValue));
                  else
                  if ((arg_list[1].typ == Float_Value) && (arg_list[2].typ == Int_Value))
                    return DBSeek(CurrentRecordset->SelectIndex,(long) arg_list[1].dValue,arg_list[2].nValue);
                  else
                  if ((arg_list[1].typ == Float_Value) && (arg_list[2].typ == String_Value))
                    return DBSeek(CurrentRecordset->SelectIndex,(long) arg_list[1].dValue,(long) atof(arg_list[2].sValue));
                  else
                    return DBSeek(CurrentRecordset->SelectIndex,(long) arg_list[1].dValue,(long) arg_list[2].dValue);
                }
              else
                {
                  RuntimeError2("%%%%[TABLE].seek[PARAMETER1,PARAMETER2] - No Select for TABLE ",arg_list[0].sValue);
                  ret_val.typ = Bool_Value;
                  ret_val.nValue = 0;
                }
            }
          else
            {
              RuntimeError1("%%%%[TABLE].seek[PARAMETER1,PARAMETER2] - PARAMETER2 must be INTEGER/FLOAT/STRING");
              ret_val.typ = Bool_Value;
              ret_val.nValue = 0;
            }
        }
      else
        {
          RuntimeError1("%%%%[TABLE].seek[PARAMETER1,PARAMETER2] - PARAMETER1 must be INTEGER/FLOAT/STRING");
          ret_val.typ = Bool_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[TABLE].seek[PARAMETER1,PARAMETER2] - TABLE must be NAME");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
/* File Functions */
/*--------------------------------------------------------------------------------*/
Fileset* GetFileset(char *FileName)
{
Fileset* Ptr=FirstFileset;
Fileset* Prev=FirstFileset;

  while(Ptr) 
    {
      if (!strcasecmp(Ptr->FileName, FileName)) 
        break;

      Prev = Ptr;
      Ptr = Ptr->Next;
    }

  if (!Ptr) 
    {
      if (Prev) 
        {
          Prev->Next = (Fileset*) malloc(sizeof(Fileset));
          Ptr = Prev->Next;
        } 
      else 
        {
          FirstFileset = (Fileset*) malloc(sizeof(Fileset));
          Ptr = FirstFileset;
        }

      strcpy(Ptr->FileName, FileName);
      Ptr->FileNumber = -1;
      Ptr->Next = NULL;
    }    
  return Ptr;
}
/*--------------------------------------------------------------------------------*/
t_Value table_fileopen(t_Value *arg_list)
{
t_Value ret_val; 
Fileset* CurrentFileset;
FILE* FileHandle;

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          if (arg_list[2].typ == String_Value)
            {
              CurrentFileset = GetFileset(arg_list[0].sValue);
              if (CurrentFileset->FileNumber != -1)
                {
                  fclose(CurrentFileset->FileHandle);
                }                                             
              else
                {
//                  CurrentFileset->FileNumber = LastFileOpen;
//                  LastFileOpen++;
                }
              FileHandle = fopen(arg_list[1].sValue,arg_list[2].sValue);
              if (FileHandle == NULL)
                {
                  ret_val.nValue = 0;
                }
              else
                {
                  CurrentFileset->FileNumber = LastFileOpen;
                  LastFileOpen++;
 
                  CurrentFileset->FileHandle = FileHandle;
                  ret_val.nValue = 1;
                }
            }
          else
            {
              RuntimeError1("%%%%[FILE].fileopen[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
              ret_val.nValue = 0;
            }
        }
      else
        {
          RuntimeError1("%%%%[FILE].fileopen[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[FILE].fileopen[PARAMETER1,PARAMETER2] - FILE must be NAME");
      ret_val.nValue = 0;
    }
  
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_fileclose(t_Value *arg_list)
{
t_Value ret_val; 
Fileset* CurrentFileset;

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      CurrentFileset = GetFileset(arg_list[0].sValue);
      if (CurrentFileset->FileNumber != -1)
        {
          ret_val.nValue = (fclose(CurrentFileset->FileHandle) == 0);
          CurrentFileset->FileNumber = -1;
        }
      else
        {
          RuntimeError2("%%%%[FILE].fileclose - No open file for FILE ",arg_list[0].sValue);
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%[FILE].fileclose - FILE must be NAME");
      ret_val.nValue = 0;
    }
  
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_filewrite(t_Value *arg_list)
{
t_Value ret_val; 
Fileset* CurrentFileset;

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      CurrentFileset = GetFileset(arg_list[0].sValue);
      if (CurrentFileset->FileNumber != -1)
        {
          switch(arg_list[1].typ)
            {
              case String_Value:
                ret_val.nValue = (fprintf(CurrentFileset->FileHandle,"%s",arg_list[1].sValue) >= 0);
                break;
              case Int_Value:
              case Bool_Value:
                ret_val.nValue = (fprintf(CurrentFileset->FileHandle,"%ld",arg_list[1].nValue) >= 0);
                break;
              case Float_Value:
                ret_val.nValue = (fprintf(CurrentFileset->FileHandle,"%f",arg_list[1].dValue) >= 0);
                break;
              case Not_Set:
                RuntimeError1("%%%%[FILE].filewrite[PARAMETER] - PARAMETER not set ");
                ret_val.nValue = 0;
                break;
              default:
                RuntimeError1("%%%%[FILE].filewrite[PARAMETER] - PARAMETER must be STRING/INTEGER/FLOAT ");
                ret_val.nValue = 0;
                break;
            }
        }
      else
        {
          RuntimeError2("%%%%[FILE].filewrite[PARAMETER] - No open file for FILE ",arg_list[0].sValue);
          ret_val.nValue = 0;
        }

    }
  else
    {
      RuntimeError1("%%%%[FILE].filewrite[PARAMETER] - FILE must be NAME");
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_filewriteline(t_Value *arg_list)
{
t_Value ret_val; 
Fileset* CurrentFileset;

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      CurrentFileset = GetFileset(arg_list[0].sValue);
      if (CurrentFileset->FileNumber != -1)
        {
          switch(arg_list[1].typ)
            {
              case String_Value:
                ret_val.nValue = (fprintf(CurrentFileset->FileHandle,"%s\n",arg_list[1].sValue) >= 0);
                break;
              case Int_Value:
              case Bool_Value:
                ret_val.nValue = (fprintf(CurrentFileset->FileHandle,"%ld\n",arg_list[1].nValue) >= 0);
                break;
              case Float_Value:
                ret_val.nValue = (fprintf(CurrentFileset->FileHandle,"%f\n",arg_list[1].dValue) >= 0);
                break;
              case Not_Set:
                RuntimeError1("%%%%[FILE].filewriteline[PARAMETER] - PARAMETER not set ");
                ret_val.nValue = 0;
                break;
              default:
                RuntimeError1("%%%%[FILE].filewriteline[PARAMETER] - PARAMETER must be STRING/INTEGER/FLOAT ");
                ret_val.nValue = 0;
                break;
            }
        }
      else
        {
          RuntimeError2("%%%%[FILE].filewriteline[PARAMETER] - No open file for FILE ",arg_list[0].sValue);
          ret_val.nValue = 0;
        }

    }
  else
    {
      RuntimeError1("%%%%[FILE].filewriteline[PARAMETER] - FILE must be NAME");
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_filereadline(t_Value *arg_list)
{
t_Value ret_val; 
Fileset* CurrentFileset;
char Buffer[2001];

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      CurrentFileset = GetFileset(arg_list[0].sValue);
      if (CurrentFileset->FileNumber != -1)
        {
          if (fgets(Buffer,2000,CurrentFileset->FileHandle) == NULL)
            strcpy(Buffer,"");

          ret_val.typ = String_Value;
          ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
          strcpy(ret_val.sValue,Buffer);
        }
      else
        {
          RuntimeError2("%%%%[FILE].filereadline[PARAMETER] - No open file for FILE ",arg_list[0].sValue);
        }
    }
  else
    {
      RuntimeError1("%%%%[FILE].filereadline[PARAMETER] - FILE must be NAME");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value table_fileeof(t_Value *arg_list)
{
t_Value ret_val; 
Fileset* CurrentFileset;

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      CurrentFileset = GetFileset(arg_list[0].sValue);
      if (CurrentFileset->FileNumber != -1)
        {
          ret_val.nValue = feof(CurrentFileset->FileHandle);
        }
      else
        {
          RuntimeError2("%%%%[FILE].fileeof[PARAMETER] - No open file for FILE ",arg_list[0].sValue);
          ret_val.nValue = 1;
        }
    }
  else
    {
      RuntimeError1("%%%%[FILE].fileeof[PARAMETER] - FILE must be NAME");
      ret_val.nValue = 1;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
#ifdef __cplusplus 
}
#endif
