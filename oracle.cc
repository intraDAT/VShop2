#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <unistd.h>


#ifndef VSADMIN
  #include "cgic.h"
  #include "hfct.h"
  #include "error.h"
#endif
#include "oracle.h"

#ifdef WIN32
  #include <process.h>
  #include "win32tools.h"
#endif

//
//  alter table TABLENAME drop SPALTE -> alter table TABLENAME delete column SPALTE 
//  
//  (SYS.)ALL_TABLES
//  ALL_TAB_COLUMNS
//  ALL_INDEXES
//  ALL_IND_COLUMNS
//
//
#ifdef VSADMIN
char* Replace(char* QuellString,char* SuchString,char* ErsetzString)
{
char Buffer[8192];
char *ptr,*ptr2=QuellString;
char *dest=Buffer;

   if (strstr(QuellString,SuchString))
   {
     *dest=0;
     ptr=strstr(ptr2,SuchString);
     while ( ptr )
       {
         while (ptr2<ptr) *dest++=*ptr2++;

         strcpy(dest,ErsetzString);
         dest= (char*)dest+strlen(ErsetzString);

         ptr2=(char*)ptr+strlen(SuchString);

         ptr=strstr(ptr2,SuchString);
        }
      if (ptr!=QuellString)
        {
          strcpy(dest,ptr2);
          strcpy(QuellString,Buffer);
        }
    }
   return(QuellString);
}
#endif
/*--------------------------------------------------------------------------------*/
#ifndef VSADMIN
int GetVar(char* VarName, char* VarValue)
{
int Ret = 0;
char* Value;
char ID[21];

  Value = cgiString(VarName);

  if (Value == NULL)
    {
      if (cgiFormString("ID",ID,20) != cgiFormNotFound)
        {
          if ((Value = EngineDB->GetVar(ID,VarName)) != NULL)
            {
              strcpy(VarValue,Value);
              Ret = 1;
            }
        }
    }
  else
    {
      strcpy(VarValue,Value);
      Ret = 1;
    }

  return Ret;
}
#endif
/*********************************************************************************/
// CField
/*********************************************************************************/

CDBField::CDBField()
{
  strcpy(Name,"");
  StringLength = 0;

  Long = 0;
  Double = 0;
  String = NULL;
}
/*--------------------------------------------------------------------------------*/
CDBField::CDBField(char* FieldName)
{
  strcpy(Name, FieldName);
  Name[strcspn(Name," ")] = '\0';
  StringLength = 0;

  Long = 0;
  Double = 0;

  String = NULL;
}
/*--------------------------------------------------------------------------------*/
CDBField::CDBField(char* FieldName, int NewStringLength)
{
  strcpy(Name, FieldName);
  Name[strcspn(Name," ")] = '\0';
  StringLength = 0;

  Long = 0;
  Double = 0;

  String = (char*) malloc(NewStringLength+1);
  StringLength = NewStringLength;

  memset (String, '\0', StringLength);

}
/*--------------------------------------------------------------------------------*/
CDBField::~CDBField()
{
  if ((Type == FT_STRING) || (Type == FT_MEMO) || (Type == FT_DATE))
    free(String);
}
/*********************************************************************************/
// CDBFieldList
/*********************************************************************************/
CDBFieldList::CDBFieldList()
{
  Items = new _Item;
  Items->Element = NULL;
  Items->Next = NULL;
  p = Items;
}
/*--------------------------------------------------------------------------------*/
CDBFieldList::~CDBFieldList()
{
  // Clear();

  // delete Items; 
}
/*--------------------------------------------------------------------------------*/
void CDBFieldList::Add(CDBField* Item) 
{
  p = Items;

  while (p->Next != NULL) 
    p = p->Next; 

  p->Next = new _Item; 
  p = p->Next; 
  p->Element = Item;
  p->Next = NULL; 
  // cout << "new  " << p << " " << Item->Name << endl;
}
/*--------------------------------------------------------------------------------*/
void CDBFieldList::Clear()
{
  _Item *tmp;
  p = Items->Next;
  
  while (p!=NULL)
	{
	      	tmp = p->Next; 
		//cout << "free " << p << " " << p->Element->Name << endl;
	        delete p;  
		p = tmp;
	}
  Items->Next = NULL;
}
/*--------------------------------------------------------------------------------*/  
CDBField* CDBFieldList::Seek(char* Name)
{  
  p = Items;
  while(p->Next != NULL)
	{
	  if (strcasecmp(Name, p->Next->Element->Name) == 0)
        return p->Next->Element;

	  p = p->Next;
	}

  return NULL;
}
/*--------------------------------------------------------------------------------*/  
CDBField* CDBFieldList::Seek(int Index)
{
  int Counter = 1;
  p = Items;

  while(p->Next != NULL)
	{
	  if (Index == Counter)
        return p->Next->Element;

	  Counter++;
	  p = p->Next;
	}

  return NULL;
}
/*--------------------------------------------------------------------------------*/  
CDBField* CDBFieldList::GetFirstItem()
{
  p = Items;

  if (p->Next != NULL)
	{
      return p->Next->Element;
	}
  else
	return NULL;
}
/*--------------------------------------------------------------------------------*/  
CDBField* CDBFieldList::GetItem()
{
  p = Items;
  if (p->Next != NULL)
	{
	  return p->Next->Element;
	}
  else
	return NULL;
}
/*--------------------------------------------------------------------------------*/  
CDBField* CDBFieldList::GetNextItem()
{
  p = p->Next;
  if (p->Next != NULL)
	{
      return p->Next->Element;
	}
  else
	return NULL;
}
/*--------------------------------------------------------------------------------*/  
void CDBFieldList::SetNull()
{  

return;

  p = Items;


  while(p->Next != NULL)
	{
      switch (((CDBField*) p->Next->Element)->Type)
		{
		  case FT_STRING:
		  case FT_MEMO:
			memset (((CDBField*)p->Next->Element)->String, '\0', ((CDBField*)p->Next->Element)->StringLength);
			break;
		  case FT_LONG :
 		  case FT_BOOLEAN :
			memset (&((CDBField*)p->Next->Element)->Long, '\0', sizeof(((CDBField*)p->Next->Element)->Long));
			break;
		  case FT_DOUBLE : 
			memset (&((CDBField*)p->Next->Element)->Double, '\0', sizeof(((CDBField*)p->Next->Element)->Double));
			break;
		  case FT_DATE : 
			memset (&((CDBField*)p->Next->Element)->Date, '\0', sizeof(((CDBField*)p->Next->Element)->Date));
			break;
		  case FT_TIME : 
			memset (&((CDBField*)p->Next->Element)->Time, '\0', sizeof(((CDBField*)p->Next->Element)->Time));
			break;
		  case FT_TIMESTAMP : 
			memset (&((CDBField*)p->Next->Element)->TimeStamp, '\0', sizeof(((CDBField*)p->Next->Element)->TimeStamp));
			break;
		}

	  p = p->Next;
	}
}
/*********************************************************************************/
// CAdabas
/*********************************************************************************/
CEngineDB::CEngineDB()
{
int Counter;

  numwidth = 8;
  LastError = 0;

  for (Counter = 0; Counter < MAX_SQLSELECT+1; Counter++)
	{
/*
      mysqlda[Counter].sqlmax = MAX_SQLVAR;
*/
	  Row[Counter] = 0;
	  NumRows[Counter] = 0;
	  EndOfFile[Counter] = 0;
	}
}
/*--------------------------------------------------------------------------------*/
void CEngineDB::SQLError(Cda_Def *cda)
{
text msg[512];
sword n;

  LastError = (int) oerhms(&lda, cda->rc, msg, (sword) sizeof (msg));

  #ifndef VSADMIN
    msg[strlen((char*) msg)-1] = '\0';
    RuntimeError4((char*) msg,SQLVarQueryString);
  #endif

  //cerr << endl <<  msg << " ["<< SQLVarQueryString << "] "<< endl;


}
/*--------------------------------------------------------------------------------*/
int CEngineDB::GetLastError()
{
  return LastError;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::Connect(char* ServerDB, char* Username, char* Password, char* Servernode)
{


int Counter;

  memset(hda,'\0',sizeof(hda));
  
  if (olog(&lda, (ub1 *)hda, (text*) Username, -1, (text*) Password, -1,(text *) ServerDB, -1, (ub4)OCI_LM_DEF))
    return 0;
  else
    {
      for (Counter = 0; Counter < MAX_SQLSELECT+1; Counter++)
        {
          if (oopen(&cda[Counter], &lda, (text *) 0, -1, -1, (text *) 0, -1))
            {
              ologof(&lda);
              return 0;
            }
        }
    }
    return 1;
return 0;
}
/*--------------------------------------------------------------------------------*/
CEngineDB::~CEngineDB()
{
int Counter;

  for (Counter = 0; Counter < MAX_SQLSELECT+1; Counter++)
        {
          oclose(&cda[Counter]);
        }

   ologof(&lda);
}
/*--------------------------------------------------------------------------------*/
void CEngineDB::Close()
{
int Counter;

  for (Counter = 0; Counter < MAX_SQLSELECT+1; Counter++)
        {
          oclose(&cda[Counter]);
        }

   ologof(&lda);
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::Commit()
{
  if (ocom(&lda))
    return 0;
  return 1;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::CreateSession(char* ID)
{
char Date[9];
char Time[7];
time_t TimeT;
tm* Today;
char Referer[500];

  time(&TimeT);

  Today = localtime(&TimeT);

  sprintf(Date,"%i%.2i%.2i",Today->tm_year+1900,Today->tm_mon+1,Today->tm_mday);
  sprintf(Time,"%.2i%.2i%.2i",Today->tm_hour,Today->tm_min,Today->tm_sec);

#ifndef VSADMIN
  strcpy(Referer,cgiReferer);
  strcat(Referer,"");
  if (strlen(Referer) > 119)
    Referer[118]='\0';
  

  sprintf(SQLVarQueryString,"insert into SESSIONS (VSINDEX,ENTRYTIME,LASTUSAGE,REFERER,TOTALPAGES,USERIP,GATEWAY,OLD) values ('%s','%s%s','%s%s','%s',1,'%s','%s',0)",
    ID,Date,Time,Date,Time,Referer,cgiRemoteAddr,cgiRemoteHost);

#endif 

  if (oparse(&cda[MAX_SQLSELECT], (text *) SQLVarQueryString, (sb4) -1,(sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  if (oexec(&cda[MAX_SQLSELECT]))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }


  sprintf(SQLVarQueryString,"insert into SESSIONVAR (VSINDEX,NAME,VALUE) values ('%s','LOGIN','ANONYM')",ID);

  if (oparse(&cda[MAX_SQLSELECT], (text *) SQLVarQueryString, (sb4) -1,(sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  if (oexec(&cda[MAX_SQLSELECT]))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }




  return 1;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::UpdateSession(char* ID)
{
char Date[9];
char Time[6];
time_t TimeT;
tm* Today;
CDBField* DBField;

  time(&TimeT);

  Today = localtime(&TimeT);

  sprintf(Date,"%i%.2i%.2i",Today->tm_year+1900,Today->tm_mon+1,Today->tm_mday);
  sprintf(Time,"%.2i%.2i%.2i",Today->tm_hour,Today->tm_min,Today->tm_sec);

  sprintf(SQLVarQueryString,"update SESSIONS set LASTUSAGE='%s%s',TOTALPAGES=TOTALPAGES+1 where VSINDEX='%s' AND OLD=0 ", Date,Time,ID);

  //cerr << endl <<  SQLVarQueryString << endl;
  if (oparse(&cda[MAX_SQLSELECT], (text *) SQLVarQueryString, (sb4) -1,(sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  if (oexec(&cda[MAX_SQLSELECT]))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }
  //cerr << endl <<  "UpdateSession returned 1" << endl;
  return cda[MAX_SQLSELECT].rpc > 0;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::UpdateVar(char* ID,char* VarName,char* VarValue)
{
  
  sprintf(SQLVarQueryString,"update SESSIONVAR set VALUE='%s' where VSINDEX='%s' and  NAME='%s'",VarValue,ID,VarName);
//  cerr << endl <<  SQLVarQueryString << endl;

  if (oparse(&cda[MAX_SQLSELECT], (text *) SQLVarQueryString, (sb4) -1,(sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  if (oexec(&cda[MAX_SQLSELECT]))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  return cda[MAX_SQLSELECT].rpc > 0;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::InsertVar(char* ID,char* VarName,char* VarValue)
{
  if (strlen(VarValue) < 150)
	{
      sprintf(SQLVarQueryString,"insert into SESSIONVAR (VSINDEX,NAME,VALUE) values ('%s','%s','%s')",ID,VarName,VarValue);

      //cerr << endl <<  SQLVarQueryString << endl;
      
      if (oparse(&cda[MAX_SQLSELECT], (text *) SQLVarQueryString, (sb4) -1,(sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
        {
          SQLError(&cda[MAX_SQLSELECT]);
          return 0;
        }

      if (oexec(&cda[MAX_SQLSELECT]))
        {
          SQLError(&cda[MAX_SQLSELECT]);
          return 0;
        }

      return cda[MAX_SQLSELECT].rpc > 0;;
	}
  else
	return 0;
}
/*--------------------------------------------------------------------------------*/
char* CEngineDB::GetVar(char* ID,char* VarName)
{
CDBField* DBField;

  sprintf(SQLVarQueryString,"select VALUE from SESSIONVAR where VSINDEX='%s' AND NAME='%s'",ID,VarName);

  if (SQLSelect(MAX_SQLSELECT,SQLVarQueryString) == 1)
    {
      DBField = GetField(MAX_SQLSELECT,1);
    }
  else
    return NULL;
  
  return DBField->String;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::DescribeFields(int Index, Cda_Def *cda)
{
CDBField* DBField;
sword col, deflen, deftyp;
static ub1 *defptr;

  DBFieldList[Index].Clear();

  /* Describe the select-list items. */
  for (col = 0; col < MAX_SELECT_LIST_SIZE; col++)
    {
        DBField = new CDBField();
        DBField->buflen = MAX_NAMELENGTH;
        if (odescr(cda, col + 1, &DBField->dbsize,
                   &DBField->dbtype, (sb1*) &DBField->Name[0],
                   &DBField->buflen, &DBField->dsize,
                   &DBField->precision, &DBField->scale,
                   &DBField->nullok))


        {
            delete DBField;
            /* Break on end of select list. */
            if (cda->rc == VAR_NOT_IN_LIST)
                break;
            else
            {
                SQLError(cda);
                return 0;
            }
        }

        DBField->Name[strcspn(DBField->Name," ")] = '\0';

        switch (DBField->dbtype)
        {
        case NUMBER_TYPE:
            DBField->dbsize = numwidth;
            /* Handle NUMBER with scale as float. */
            if (DBField->scale != 0)
            {
                defptr = (ub1 *) &DBField->flt_buf;
                deflen = (sword) sizeof(float);
                deftyp = FLOAT_TYPE;
                DBField->dbtype = FLOAT_TYPE;
				DBField->Type = FT_DOUBLE;
				DBField->Col = col;
            }
            else
            {
                defptr = (ub1 *) &DBField->int_buf;
                deflen = (sword) sizeof(sword);
                deftyp = INT_TYPE;
                DBField->dbtype = INT_TYPE;
				DBField->Type = FT_LONG;
				DBField->Col = col;
            }
            break;
		  case DATE_TYPE:
			deftyp = SQLT_DAT;

            DBField->String = (char*) malloc(7 + 1);
            DBField->StringLength = 7 + 1;
            
			defptr = (ub1 *) DBField->String;

            deflen = 7;
			DBField->dbtype = SQLT_DAT;
			DBField->Type = FT_DATE;
			DBField->Col = col;
			break;
        default:
            if (DBField->dbtype == ROWID_TYPE)
              DBField->dbsize = 18;

            DBField->String = (char*) malloc(DBField->dbsize + 1);
            DBField->StringLength = DBField->dbsize + 1;
            deflen = DBField->dbsize+1;
            
			defptr = (ub1 *) DBField->String;
            deftyp = STRING_TYPE;

 		    DBField->Type = FT_STRING;
			DBField->Col = col;

            break;
        }
        if (odefin(cda, col + 1,
                   defptr, deflen, deftyp,
                   -1, &DBField->indp, (text *) 0, -1, -1,
                   &DBField->col_retlen,
                   &DBField->col_retcode))
        {
            SQLError(cda);
            return 0;
        }

      DBFieldList[Index].Add(DBField);
    }
    return col;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::SQLSelect(int Index, char* SQLString)
{
  strcpy(SQLVarQueryString,SQLString);

  while (strstr(SQLVarQueryString,"=''") != NULL)
    {
      strcpy(SQLVarQueryString,Replace(SQLVarQueryString,"=''"," IS NULL"));
    }
  
  while (strstr(SQLVarQueryString,"= ''") != NULL)
    {
      strcpy(SQLVarQueryString,Replace(SQLVarQueryString,"= ''"," IS NULL"));
    }

  //cerr << endl <<  "SQL[" << Index << "][" << SQLVarQueryString << "]" << endl;

  Row[Index] = 0;
  EndOfFile[Index] = 1;

  if (oparse(&cda[Index], (text *) SQLVarQueryString, (sb4) -1,(sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
    {
      SQLError(&cda[Index]);
      return 0;
    }

  if ((NumCols[Index] = DescribeFields(Index, &cda[Index])) == -1)
    return 0;

  if (oexec(&cda[Index]))
    {
      SQLError(&cda[Index]);
      return 0;
    }

  if (ofetch(&cda[Index]))
    {
       //cerr << endl << "RETURN 0" << endl;
      if (cda[Index].rc != 0)
        return 0;
    }

  EndOfFile[Index] = 0;
  
  NumRows[Index] = cda[Index].rpc;
  Row[Index] = cda[Index].rpc;

  //cerr << endl << "SQL-RPC[" << cda[Index].rpc << "]" <<  endl;

  return 1;
}
int CEngineDB::SQL(char* SQLString)
{
return SQLUpdate(SQLString);
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::SQLUpdate(char* SQLString)
{
char* Ptr;
int i;
char Buffer[1000];

  strcpy(SQLVarQueryString, SQLString);
  i = 0;
  while(SQLVarQueryString[i]) 
    {
      SQLVarQueryString[i]=tolower(SQLVarQueryString[i]);
      i++;
    }

  if ((Ptr = strstr (SQLVarQueryString, "where")) != NULL)
    {
      strcpy(SQLVarQueryString, SQLString);
      strcpy(Buffer, Ptr);

      while (strstr(Buffer,"=''") != NULL)
        {
          strcpy(Buffer,Replace(Buffer,"=''"," IS NULL"));
        }
  
      while (strstr(Buffer,"= ''") != NULL)
        {
          strcpy(Buffer,Replace(Buffer,"= ''"," IS NULL"));
        }

      strcpy(Ptr, Buffer);

    }
  else
    strcpy(SQLVarQueryString, SQLString);

  if (oparse(&cda[MAX_SQLSELECT], (text *) SQLVarQueryString, (sb4) -1,(sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  if (oexec(&cda[MAX_SQLSELECT]))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  return 1;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::SQLInsert(char* SQLString,char* ID)
{
long t;
int Ret;
#ifndef HPUX
long c;
#endif

  time(&t); 

#ifndef HPUX
	
	#ifdef WIN32
	  c=_getpid() % 100;
	#else
	  c=getpid() % 100;
	#endif

    sprintf(ID,"%ld%ld",t,c);
#else
    sprintf(ID,"%ld",t);

#endif

  do	  
	{
      sprintf(SQLVarQueryString,"insert into %s (VSINDEX) values ('%s')",SQLString,ID);

      if (oparse(&cda[MAX_SQLSELECT], (text *) SQLVarQueryString, (sb4) -1,(sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
        {
          SQLError(&cda[MAX_SQLSELECT]);
          return 0;
        }

      Ret = oexec(&cda[MAX_SQLSELECT]);

	  if (Ret)
		{
		  t += 1;
              #ifndef HPUX
                sprintf(ID,"%ld%ld",t,c);
              #else
                sprintf(ID,"%ld",t);
              #endif
		}
	}
  while(Ret);

  return 1;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::SQLCreate(char* TableName,char* OldTable)
{
  sprintf(SQLVarQueryString,"create table %s as select * from  %s",TableName,OldTable);

  if (oparse(&cda[MAX_SQLSELECT], (text *) SQLVarQueryString, (sb4) -1,(sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  if (oexec(&cda[MAX_SQLSELECT]))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  return 1;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::SQLDrop(char* TableName)
{
  sprintf(SQLVarQueryString,"drop table %s",TableName);

  if (oparse(&cda[MAX_SQLSELECT], (text *) SQLVarQueryString, (sb4) -1,(sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  if (oexec(&cda[MAX_SQLSELECT]))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  return 1;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::SQLDelete(char* TableName, char* Index)
{
int Ret;

  sprintf(SQLVarQueryString,"delete from %s where VSINDEX='%s'",TableName,Index);

  if (oparse(&cda[MAX_SQLSELECT], (text *) SQLVarQueryString, (sb4) -1,(sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }

  if (oexec(&cda[MAX_SQLSELECT]))
    {
      SQLError(&cda[MAX_SQLSELECT]);
      return 0;
    }
  return 1;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::IsEOF(int SelectIndex)
{
  return EndOfFile[SelectIndex];
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::GetFirst(int SelectIndex)
{
  return 0;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::GetLast(int SelectIndex)
{
 return 0;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::GetNext(int SelectIndex)
{
  if (ofetch(&cda[SelectIndex]))
    {
      if (cda[SelectIndex].rc != 0)
        {
          EndOfFile[SelectIndex] = 1;
          return 0;
        }
    }

  Row[SelectIndex] = cda[SelectIndex].rpc;

  return 1;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::GetPrev(int SelectIndex)
{
  return 0;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::GetAt(int SelectIndex, int Position)
{
  return 0;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::GetCurrentRow(int SelectIndex)
{
  return cda[SelectIndex].rpc;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::GetNumRows(int SelectIndex)
{
  // return NumRows[SelectIndex];
  return cda[SelectIndex].rpc;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::GetNumCols(int SelectIndex)
{
  return NumCols[SelectIndex];
}
/*--------------------------------------------------------------------------------*/
CDBField* CEngineDB::SetField(CDBField* DBField, int Index)
{
struct tm TM;
char TimeBuffer[25];


  if (DBField != NULL)
	{
/*
      if (DBField->indp < 0)
        DBField->Indicator = -1;
*/
     DBField->Indicator = 0;

	  switch (DBField->Type)
		{
		  case FT_STRING:
                        //cerr << endl <<  " FeldInhalt von "<<DBField->Name<<"  : ["<< DBField->String << "] "<< endl;
                        
			break;
		  case FT_DATE:
		
            memcpy(&DBField->DateEx,DBField->String,7);
            if (DBField->DateEx._year < 100) 
              {
                DBField->DateEx.year = 100*(100-DBField->DateEx._century) + (100-DBField->DateEx._year);
              } 
            else 
              {
                DBField->DateEx.year = 100*(DBField->DateEx._century-100) + (DBField->DateEx._year-100);
              }
       		sprintf(DBField->Time,"%02d%02d%02d", DBField->DateEx.hour,
			        DBField->DateEx.minute,
			        DBField->DateEx.second);
		    sprintf(DBField->Date,"%04d%02d%02d", DBField->DateEx.year,
			        DBField->DateEx.month,
			        DBField->DateEx.day);
		    sprintf(DBField->TimeStamp,"%04d%02d%02d%02d%02d%02d000000", DBField->DateEx.year,
			        DBField->DateEx.month,
			        DBField->DateEx.day,
			        DBField->DateEx.hour,
			        DBField->DateEx.minute,
			        DBField->DateEx.second);

		    sprintf(TimeBuffer,"%04d%02d%02d%02d%02d%02d", DBField->DateEx.year,
			        DBField->DateEx.month,
			        DBField->DateEx.day,
			        DBField->DateEx.hour,
			        DBField->DateEx.minute,
			        DBField->DateEx.second);

            //strptime(TimeBuffer,"%Y%m%d%H%M%S",&TM);
            DBField->TimeInSeconds = mktime(&TM);

			break;
		  case FT_DOUBLE: 
                        if (DBField->indp < 0)
                          DBField->Double = 0;
                        else
			  DBField->Double = DBField->flt_buf;
			break;
		  case FT_LONG: 
                        if (DBField->indp < 0)
                          DBField->Long = 0;
                        else
			  DBField->Long = DBField->int_buf;
			break;
		}
	}
  return DBField;
}
/*--------------------------------------------------------------------------------*/
CDBField* CEngineDB::GetField(int SelectIndex, char* FieldName)
{
  return SetField(DBFieldList[SelectIndex].Seek(FieldName), SelectIndex);
}
/*--------------------------------------------------------------------------------*/
CDBField* CEngineDB::GetField(int SelectIndex, int Index)
{
  return SetField(DBFieldList[SelectIndex].Seek(Index), SelectIndex);
}
/*--------------------------------------------------------------------------------*/
CDBField* CEngineDB::GetFirstField(int SelectIndex)
{
  return SetField(DBFieldList[SelectIndex].GetFirstItem(), SelectIndex);
}
/*--------------------------------------------------------------------------------*/
CDBField* CEngineDB::GetNextField(int SelectIndex)
{
  return SetField(DBFieldList[SelectIndex].GetNextItem(), SelectIndex);
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::GetFirstListField(char* Category,char* ColumnName,
                                       char* Name, char* Value)
{

CDBField* ListField;

  strcpy(SQLVarQueryString,"SELECT LISTS.VSINDEX,LISTS.CATEGORY, LISTS.COLUMNNAME, LISTITEMS.LISTINDEX, LISTITEMS.NAME, ");
  strcat(SQLVarQueryString,"LISTITEMS.VALUE FROM LISTS,LISTITEMS WHERE");
  strcat(SQLVarQueryString,"(LISTS.VSINDEX = LISTITEMS.LISTINDEX) AND (LISTS.CATEGORY='");
  strcat(SQLVarQueryString,Category);
  strcat(SQLVarQueryString,"') AND (LISTS.COLUMNNAME='");
  strcat(SQLVarQueryString,ColumnName);
  strcat(SQLVarQueryString,"')");

  if (SQLSelect(MAX_SQLSELECT,SQLVarQueryString) == 0)
	return 0;

  if ((ListField = GetField(MAX_SQLSELECT,5)) != NULL)
	{
	  strcpy(Name,ListField->String);

      if ((ListField = GetField(MAX_SQLSELECT,6)) != NULL)
	    {
		  strcpy(Value,ListField->String);

          return 1;
		}
	 }

  return 0;
}
/*--------------------------------------------------------------------------------*/
int CEngineDB::GetNextListField(char* Name, char* Value)
{
CDBField* ListField;

  if (GetNext(MAX_SQLSELECT) == 0)
	return 0;

  if ((ListField = GetField(MAX_SQLSELECT,5)) != NULL)
	{
	  strcpy(Name,ListField->String);

      if ((ListField = GetField(MAX_SQLSELECT,6)) != NULL)
	    {
		  strcpy(Value,ListField->String);

          return 1;
		}
	 }

  return 0;
}
/*--------------------------------------------------------------------------------*/

#ifndef VSADMIN
CEngineDB* EngineDB;
#endif
