#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#ifndef WIN32
  #include <grp.h>
  #include <pwd.h>
  #include <unistd.h>
  #include <dirent.h>
  #include "inifile.h"
#else
  #include <direct.h>
  #include <io.h>
  #include "win32tools.h"
  #include <windows.h>
  #include <winreg.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include "vsl.h"
#include "hfct.h"
#include "cgic.h"
#include "error.h"
#ifndef DB_ORACLE
  #include "adabas.h"
#else
  #include "oracle.h"
#endif
#include "db.h"
#include "vsl-exec.h"
#include "vslanguage.h"
#include "base-func.h"
#include "julian.h"
#include "skey.h"

#ifdef SUN
  #include "inifile.h"
#endif



#define INIFILE "/etc/vshop.conf"

#ifndef S_IFMT
  #define S_IFMT 00170000
#endif

#ifndef S_IFLNK
#define S_IFLNK 0120000
#endif

#ifdef __cplusplus 
extern "C" {
#endif

#define MAX_SQLLEN 8192

  #define MAX_FILENAME 300
#ifndef WIN32
  #define MAX_PATH 400
#endif

  typedef struct DirEntryStruct 
    {
      #ifndef WIN32
        off_t Size;
      #else
        _fsize_t Size; 
      #endif
      time_t ATime;
      time_t CTime;
      time_t MTime;
      unsigned int Mode;
      long UID;
      long GID;
      char FileName[MAX_FILENAME];
      char Path[MAX_PATH];
    } DirEntryStruct;

  DirEntryStruct DirEntry;

#ifndef WIN32
  DIR* Dir;
#else
  long Dir; 
#endif

/*--------------------------------------------------------------------------------*/  
t_Value base_eval(t_Value *arg_list)  
{                             
  t_Value ret_val;            

  if (arg_list[0].typ == Bool_Value)
    arg_list[0].typ = Int_Value;

  if (arg_list[0].typ == String_Value)
    {
      ret_val.sValue = (char*) malloc(strlen(arg_list[0].sValue)+1);
      strcpy(ret_val.sValue,arg_list[0].sValue);
      ret_val.typ = String_Value;
    }
  else
    ret_val=arg_list[0];        

  return ret_val;             
}                             

/*--------------------------------------------------------------------------------*/  
int nRandom_Seed=0;

t_Value base_random(t_Value *arg_list)
{
t_Value ret_val;

  if (nRandom_Seed==0) 
    {
      #ifndef WIN32
        srand(getpid()+time(NULL));
      #else
        srand(time(NULL));
      #endif

      nRandom_Seed=rand();
    }

  if (arg_list[0].nValue == 0 || arg_list[0].typ == Not_Set) 
    {
      RuntimeError1("%%%%random[PARAMETER] - PARAMETER must be INTEGER0");
      fprintf(stderr, "random requieres one argument (integer != 0)");
      ret_val.typ=Not_Set;
    } 
  else 
    {
      ret_val.nValue=rand() % arg_list[0].nValue;
      ret_val.dValue=ret_val.nValue;
      ret_val.typ = Int_Value;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_env(t_Value *arg_list)
{
t_Value ret_val;
char* Env;
char Buffer[200];

  if (arg_list[0].typ == String_Value)
    {

      if ((strstr(arg_list[0].sValue,"DBUSER") != NULL) ||
          (strstr(arg_list[0].sValue,"DBPASSWORD") != NULL))
        {
          ret_val.sValue = (char*) malloc(1);
          ret_val.sValue[0] = '\0';
          ret_val.typ = String_Value;
          return ret_val;
        }

      /*-----------------------------------------------------*/
      if (strcasecmp(arg_list[0].sValue,"VSHOP_CGIBIN") == 0)
        {
          sprintf(Buffer,"%s_CGIBIN",PROGRAMNAME);
        }
      else
      if (strcasecmp(arg_list[0].sValue,"VSHOP_BASEREF") == 0)
        {
          sprintf(Buffer,"%s_BASEREF",PROGRAMNAME);
        }
      else
        strcpy(Buffer,arg_list[0].sValue);
      /*-----------------------------------------------------*/
      Env = getenv(Buffer);
      if (Env != NULL)
        {
          ret_val.sValue = (char*) malloc(strlen(Env)+1);
          strcpy(ret_val.sValue,Env);
          ret_val.typ = String_Value;
        }
      else
        {
          ret_val.sValue = (char*) malloc(1);
          ret_val.sValue[0] = '\0';
          ret_val.typ = String_Value;
        }
    }
  else
    {
      RuntimeError1("%%%%env[PARAMETER] - PARAMETER must be STRING");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_id(t_Value *arg_list)
{
t_Value ret_val;
char ID[21];
char Buffer[40];

  if (cgiFormString("ID",ID,20) == cgiFormNotFound)
    {
      ret_val.typ = Not_Set;
    }
  else
    {
      sprintf(Buffer,"ID=%s",ID);
      ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
      strcpy(ret_val.sValue,Buffer);
      ret_val.typ = String_Value;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_var(t_Value *arg_list)
{
t_Value ret_val;
char* Value;
char ID[21];

  if (arg_list[0].typ == String_Value) 
    {

      Value = cgiString(arg_list[0].sValue);

      if (Value == NULL)
        {
          if (cgiFormString("ID",ID,20) != cgiFormNotFound)
            {

              if ((Value = EngineDB->GetVar(ID,arg_list[0].sValue)) != NULL)
                {
                  ret_val.sValue = (char*) malloc(strlen(Value)+1);
                  strcpy(ret_val.sValue,Value);
                  ret_val.typ = String_Value;
                }
              else                   
                {
                  switch (arg_list[1].typ)
                    {

                      case String_Value:
                        ret_val.sValue = (char*) malloc(strlen(arg_list[1].sValue)+1);
                        strcpy(ret_val.sValue,arg_list[1].sValue);
                        ret_val.typ = String_Value;
                        break;
                      case Int_Value:
                      case Bool_Value:
                        ret_val.nValue = arg_list[1].nValue;
                        ret_val.typ = arg_list[1].typ;
                        break;
                      case Float_Value:
                        ret_val.dValue = arg_list[1].dValue;
                        ret_val.typ = Float_Value;
                        break;
                      default:
                        ret_val.sValue = (char*) malloc(1);
                        ret_val.sValue[0] = '\0';
                        ret_val.typ = String_Value;
                        break;
                    }
                }

            }

        }
      else
        {
          ret_val.sValue = (char*) malloc(strlen(Value)+1);
          strcpy(ret_val.sValue,Value);
          ret_val.typ = String_Value;
        }
    }
  else
    {
      RuntimeError1("%%%%var[PARAMETER1(,PARAMETER2)] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_setvar(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[501];
char ID[20];

  ret_val.typ=Not_Set;

  if (arg_list[0].typ == String_Value) 
    {
     switch(arg_list[1].typ)
        {
          case String_Value: 
            InsertEntry(arg_list[0].sValue,arg_list[1].sValue);
            strcpy(Buffer,arg_list[1].sValue);
            DBReplace(Buffer);
            break;
          case Bool_Value:
          case Int_Value:
            sprintf(Buffer,"%ld",arg_list[1].nValue);
            InsertEntry(arg_list[0].sValue,Buffer);
            break;
          case Float_Value:
            sprintf(Buffer,"%f",arg_list[1].dValue);
            InsertEntry(arg_list[0].sValue,Buffer);
            break;
          default:
            RuntimeError1("%%%%setvar[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING/INTEGER/FLOAT");
            break;
        }

      cgiFormString("ID",ID,20);

      if (!EngineDB->UpdateVar(ID,arg_list[0].sValue,Buffer))
        EngineDB->InsertVar(ID,arg_list[0].sValue,Buffer);
     }
  else
    {
      RuntimeError1("%%%%setvar[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_tmpvar(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[100];
char ID[20];

  ret_val.typ=Not_Set;

  if (arg_list[0].typ == String_Value) 
    {
     switch(arg_list[1].typ)
        {
          case String_Value: 
            InsertEntry(arg_list[0].sValue,arg_list[1].sValue);
            break;
          case Bool_Value:
          case Int_Value:
            sprintf(Buffer,"%ld",arg_list[1].nValue);
            InsertEntry(arg_list[0].sValue,Buffer);
            break;
          case Float_Value:
            sprintf(Buffer,"%f",arg_list[1].dValue);
            InsertEntry(arg_list[0].sValue,Buffer);
            break;
          default:
            RuntimeError1("%%%%tmpvar[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING/INTEGER/FLOAT");
            break;
        }

      cgiFormString("ID",ID,20);
     }
  else
    {
      RuntimeError1("%%%%tmpvar[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_strcmp(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          ret_val.nValue = (strcmp(arg_list[0].sValue,arg_list[1].sValue) == 0);
        }
      else
        {
          RuntimeError1("%%%%strcmp[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%strcmp[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_strcut(t_Value *arg_list)
{
t_Value ret_val;
char* Ptr;
char* Buffer1;
char* Buffer2;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          if (arg_list[2].typ == String_Value)
            {
              Buffer1 = (char*) malloc(strlen(arg_list[0].sValue)+1);
              Buffer2 = (char*) malloc(strlen(arg_list[0].sValue)+1);

              strcpy(Buffer1,arg_list[0].sValue);
              if ((Ptr = strstr(Buffer1,arg_list[1].sValue)) != NULL)
                {
                  strcpy(Buffer2,Ptr+strlen(arg_list[1].sValue));
                  if ((Ptr = strstr(Buffer2,arg_list[2].sValue)) != NULL)
                    {
                     *Ptr = '\0';
                    }
                }
              else
                {
                  strcpy(Buffer2,Buffer1);
                }

              ret_val.sValue = (char*) malloc(strlen(Buffer2)+1);
              strcpy(ret_val.sValue,Buffer2);
              free(Buffer1);
              free(Buffer2);
              ret_val.typ = String_Value;
            }
          else
            {
              RuntimeError1("%%%%strcut[PARAM1,PARAM2,PARAM3] - PARAMETER3 must be STRING");
              ret_val.typ = Not_Set;
            }
        }
      else
        {
          RuntimeError1("%%%%strcut[PARAM1,PARAM2,PARAM3] - PARAMETER2 must be STRING");
          ret_val.typ = Not_Set;
        }
    }
  else
    {
      RuntimeError1("%%%%strcut[PARAM1,PARAM2,PARAM3] - PARAMETER1 must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_left(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[2000];

  if (arg_list[0].typ == String_Value)
    {
      if ((arg_list[1].typ == Int_Value) || (arg_list[1].typ == Float_Value))
        {
          if (arg_list[1].typ == Int_Value)
            {
              if ((arg_list[1].nValue < (int) strlen(arg_list[0].sValue)) && (arg_list[1].nValue >= 0))
                {
                  strcpy(Buffer,arg_list[0].sValue);
                  Buffer[arg_list[1].nValue] = '\0';
                  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
                  strcpy(ret_val.sValue,Buffer);
                  ret_val.typ = String_Value;
                }
              else
                {
                  ret_val.sValue = (char*) malloc(strlen(arg_list[0].sValue)+1);
                  strcpy(ret_val.sValue,arg_list[0].sValue);
                  ret_val.typ = String_Value;
                }
            }
          else
            {
              if (((int) arg_list[1].dValue < (int) strlen(arg_list[0].sValue)) && (arg_list[1].dValue >= 0))
                {
                  strcpy(Buffer,arg_list[0].sValue);
                  Buffer[(int) arg_list[1].dValue] = '\0';
                  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
                  strcpy(ret_val.sValue,Buffer);
                  ret_val.typ = String_Value;
                }
              else
                {
                  ret_val.sValue = (char*) malloc(strlen(arg_list[0].sValue)+1);
                  strcpy(ret_val.sValue,arg_list[0].sValue);
                  ret_val.typ = String_Value;
                }
            }
        }
      else
        {
          RuntimeError1("%%%%left[PARAMETER1,PARAMETER2] - PARAMETER2 must be INTEGER/FLOAT");
          ret_val.typ = Not_Set;
        }
    }
  else
    {
      RuntimeError1("%%%%left[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_right(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[2000];
int Ptr;


  if (arg_list[0].typ == String_Value)
    {
      if ((arg_list[1].typ == Int_Value) || (arg_list[1].typ == Float_Value))
        {
          if (arg_list[1].typ == Int_Value)
            {
              if ((arg_list[1].nValue < (int) strlen(arg_list[0].sValue)) && (arg_list[1].nValue >= 0))
                {
                  Ptr = (strlen(arg_list[0].sValue) - arg_list[1].nValue);
                  strcpy(Buffer,(arg_list[0].sValue + Ptr));

                  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
                  strcpy(ret_val.sValue,Buffer);
                  ret_val.typ = String_Value;
                }
              else
                {
                  ret_val.sValue = (char*) malloc(strlen(arg_list[0].sValue)+1);
                  strcpy(ret_val.sValue,arg_list[0].sValue);
                  ret_val.typ = String_Value;
                }
            }
          else
            {
              if (((int) arg_list[1].dValue < (int) strlen(arg_list[0].sValue)) && (arg_list[1].dValue >= 0))
                {
                  Ptr = (strlen(arg_list[0].sValue) - (int) arg_list[1].dValue);
                  strcpy(Buffer,(arg_list[0].sValue + Ptr));

                  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
                  strcpy(ret_val.sValue,Buffer);
                  ret_val.typ = String_Value;
                }
              else
                {
                  ret_val.sValue = (char*) malloc(strlen(arg_list[0].sValue)+1);
                  strcpy(ret_val.sValue,arg_list[0].sValue);
                  ret_val.typ = String_Value;
                }
            }
        }
      else
        {
          RuntimeError1("%%%%right[PARAMETER1,PARAMETER2] - PARAMETER2 must be INTEGER/FLOAT");
          ret_val.typ = Not_Set;
        }
    }
  else
    {
      RuntimeError1("%%%%right[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_mid(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[2000];

  if (arg_list[0].typ == String_Value)
    {
      if ((arg_list[1].typ == Int_Value) || (arg_list[1].typ == Float_Value))
        {
          if (arg_list[2].typ != Not_Set)
            {
              if ((arg_list[2].typ == Int_Value) || (arg_list[2].typ == Float_Value))
                {
                  if (arg_list[2].typ == Int_Value)
                    {
                      strcpy(Buffer,(arg_list[0].sValue + (arg_list[1].nValue-1)));
                      if (arg_list[2].nValue < (int) strlen(Buffer))
                        Buffer[arg_list[2].nValue] = '\0';
                      ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
                      strcpy(ret_val.sValue,Buffer);
                      ret_val.typ = String_Value;
                    }
                  else
                    {
                      strcpy(Buffer,(arg_list[0].sValue + ((int) arg_list[1].dValue-1)));
                      if ((int) arg_list[2].dValue < (int) strlen(Buffer))
                        Buffer[(int) arg_list[2].dValue] = '\0';
                      ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
                      strcpy(ret_val.sValue,Buffer);
                      ret_val.typ = String_Value;
                    }
                }
              else
                {
                  RuntimeError1("%%%%mid[PARAMETER1,PARAMETER2(,PARAMETER3)] - PARAMETER3 must be INTEGER/FLOAT");
                  ret_val.typ = Not_Set;
                }
            }
          else
            {
              if (arg_list[1].typ == Int_Value)
                {
                  if (arg_list[1].nValue >= (int) strlen(Buffer))
                    strcpy(Buffer,arg_list[0].sValue);
                  else
                    strcpy(Buffer,(arg_list[0].sValue + (arg_list[1].nValue-1)));
                  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
                  strcpy(ret_val.sValue,Buffer);
                  ret_val.typ = String_Value;
                }
              else
                {
                  if (arg_list[1].nValue >= (int) strlen(Buffer))
                    strcpy(Buffer,arg_list[0].sValue);
                  else
                    strcpy(Buffer,(arg_list[0].sValue + (int) (arg_list[1].dValue-1)));

                  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
                  strcpy(ret_val.sValue,Buffer);
                  ret_val.typ = String_Value;
                }
            }
        }
      else
        {
          RuntimeError1("%%%%mid[PARAMETER1,PARAMETER2(,PARAMETER3)] - PARAMETER2 must be INTEGER/FLOAT");
          ret_val.typ = Not_Set;
        }
    }
  else
    {
      RuntimeError1("%%%%mid[PARAMETER1,PARAMETER2(,PARAMETER3)] - PARAMETER1 must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_upper(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
int Counter;

  if (arg_list[0].typ == String_Value)
    {
      strcpy(Buffer,arg_list[0].sValue);
      for (Counter = 0;Counter < (int) strlen(Buffer);Counter++)
        {
          if (((int) Buffer[Counter] > 96) && ((int) Buffer[Counter] < 123))
            Buffer[Counter] -= 32;
          else
            {
              switch(Buffer[Counter])
                {
                  case '÷':
                    Buffer[Counter] = '™';
                    break;
                  case '„':
                    Buffer[Counter] = 'Ž';
                    break;
                  case '':
                    Buffer[Counter] = 'š';
                    break;
                }
            }
        }
      ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
      strcpy(ret_val.sValue,Buffer);
      ret_val.typ = String_Value;
    }
  else
    {
      RuntimeError1("%%%%upper[PARAMETER] - PARAMETER must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_lower(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
int Counter;

  if (arg_list[0].typ == String_Value)
    {
      strcpy(Buffer,arg_list[0].sValue);
      for (Counter = 0;Counter < (int) strlen(Buffer);Counter++)
        {
          if (((int) Buffer[Counter] > 64) && ((int) Buffer[Counter] < 91))
            Buffer[Counter] += 32;
          else
            {
              switch(Buffer[Counter])
                {
                  case '™':
                    Buffer[Counter] = '÷';
                    break;
                  case 'Ž':
                    Buffer[Counter] = '„';
                    break;
                  case 'š':
                    Buffer[Counter] = '';
                    break;
                }
            }
        }
      
      ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
      strcpy(ret_val.sValue,Buffer);
      ret_val.typ = String_Value;
    }
  else
    {
      RuntimeError1("%%%%lower[PARAMETER] - PARAMETER must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_length(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[51];

  ret_val.typ = Int_Value;

  if (arg_list[0].typ == Int_Value)
    {
      sprintf(Buffer,"%i",arg_list[0].nValue);
      ret_val.nValue = strlen(Buffer);
    }
  else
  if (arg_list[0].typ == Float_Value)
    {
      sprintf(Buffer,"%.2f",arg_list[0].dValue);
      ret_val.nValue = strlen(Buffer);
    }
  else
  if (arg_list[0].typ == String_Value)
    {
      ret_val.typ = Int_Value;
      ret_val.nValue = strlen(arg_list[0].sValue);
    }
  else
    {
      RuntimeError1("%%%%length[PARAMETER] - PARAMETER must be STRING/INTEGER/FLOAT");
      ret_val.typ = Not_Set;
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_pos(t_Value *arg_list)
{
t_Value ret_val;
char *pdest;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          pdest = strchr(arg_list[0].sValue,arg_list[1].sValue[0]);
          ret_val.typ = Int_Value;
          if (pdest == NULL)
            ret_val.nValue = 0;
          else
            ret_val.nValue = pdest - arg_list[0].sValue + 1;
        }
      else
        {
          RuntimeError1("%%%%pos[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
          ret_val.typ = Not_Set;
        }
    }
  else
    {
      RuntimeError1("%%%%pos[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_in(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          ret_val.typ = Bool_Value;
          ret_val.nValue = (strstr(arg_list[0].sValue,arg_list[1].sValue) != NULL);
        }
      else
        {
          RuntimeError1("%%%%in[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
          ret_val.typ = Not_Set;
        }
    }
  else
    {
      RuntimeError1("%%%%in[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filter(t_Value *arg_list)
{
t_Value ret_val;
char *ptr,*ptr2=arg_list[0].sValue;
char strbuffer[500];
char *dest=strbuffer;


  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          /* Muss man ueberhaupt etwas ersetzen ? */
           if (strstr(arg_list[0].sValue,arg_list[1].sValue))
           {
            /* "was" darf nicht leer sein. */
            if (strlen(arg_list[1].sValue) != 0)
            {
                *dest=0;
                ptr=strstr(arg_list[0].sValue,arg_list[1].sValue);
                while ( ptr )
                {
                    /* Kopiere alle Zeichen bis zum "was"-text */
                    while (ptr2<ptr) 
                      *dest++=*ptr2++;

                    /* Ueberspringe den "was"-text */
                    ptr2=(char*)ptr+strlen(arg_list[1].sValue);

                    /* Suche naechsten "was"-text im "string" */ 
                    ptr=strstr(ptr2,arg_list[1].sValue);
                }
                if (ptr!=arg_list[0].sValue)
                {
                    strcpy(dest,ptr2);
                }
            }
          else
            strcpy(strbuffer,arg_list[0].sValue);

          }
          else
            strcpy(strbuffer,arg_list[0].sValue);

           ret_val.sValue = (char*) malloc(strlen(strbuffer)+1);
           strcpy(ret_val.sValue,strbuffer);
           ret_val.typ = String_Value;

        }
      else
        {
          RuntimeError1("%%%%filter[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
          ret_val.typ = Not_Set;
        }
    }
  else
    {
      RuntimeError1("%%%%filter[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_include(t_Value *arg_list)
{
t_Value ret_val;
FILE* File;
char Line[2001];
int Erstazzeichen = 1;

  ret_val.typ=Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if ((File = fopen(arg_list[0].sValue,"r")) != NULL)
        {
          while (fgets(Line,2000,File) != NULL)
            {

              if (strncasecmp(Line,"<begin>",7) == 0)
                {
                  Erstazzeichen = 0;
                  fprintf(cgiOut,"%s",Line);
                }
              else
              if (strncasecmp(Line,"<end>",5) == 0)
                {
                  Erstazzeichen = 1;
                  fprintf(cgiOut,"%s",Line);
                }
              else
                {
                  if ((arg_list[1].typ == String_Value) && (Erstazzeichen == 1))
                    {
                      fprintf(cgiOut,"%s%s",Line,arg_list[1].sValue);
                    }
                  else
                    fprintf(cgiOut,"%s",Line);
                }
            }
          fclose(File);
        }
      else
        {
          RuntimeError2("%%%%include[PARAMETER] - File not found ",arg_list[0].sValue);
        }
    }
  else
    {
      RuntimeError1("%%%%include[PARAMETER] - PARAMETER must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_includecss(t_Value *arg_list)
{
t_Value ret_val;
FILE* File;
char Line[2001];
char Filename[255];

  ret_val.typ=Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if (TEMPLATEROOT[strlen(TEMPLATEROOT)-1] != '/')
        sprintf(Filename,"%s/%s",TEMPLATEROOT,arg_list[0].sValue);
      else
        sprintf(Filename,"%s%s",TEMPLATEROOT,arg_list[0].sValue);

      if ((File = fopen(Filename,"r")) != NULL)
        {
          while (fgets(Line,2000,File) != NULL)
            {
              fprintf(cgiOut,"%s",Line);
            }
          fclose(File);
        }
      else
        {
          RuntimeError2("%%%%includescss[PARAMETER] - File not found ",Filename);
        }
    }
  else
    {
      RuntimeError1("%%%%includecss[PARAMETER] - PARAMETER must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_tofloat(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == Int_Value)
    {
      ret_val.dValue = arg_list[0].nValue;
      ret_val.typ = Float_Value;
    }

  if (arg_list[0].typ == String_Value)
    {
      Replace(arg_list[0].sValue,",",".");

      ret_val.dValue = atof(arg_list[0].sValue);
      ret_val.typ = Float_Value;
    }
  else
    {
      RuntimeError1("%%%%tofloat[PARAMETER] - PARAMETER must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_toint(t_Value *arg_list)
{
t_Value ret_val;

  if ((arg_list[0].typ == String_Value) || (arg_list[0].typ == Float_Value) || (arg_list[0].typ == Int_Value))
    {
      if (arg_list[0].typ == Int_Value)
        {
          ret_val.nValue = arg_list[0].nValue;
          ret_val.typ = Int_Value;
        }
      if (arg_list[0].typ == String_Value)
        {
          ret_val.nValue = atol(arg_list[0].sValue);
          ret_val.typ = Int_Value;
        }
      else
        {
          ret_val.nValue = (int) arg_list[0].dValue;
          ret_val.typ = Int_Value;
        }
    }
  else
    {
      RuntimeError1("%%%%toint[PARAMETER] - PARAMETER must be STRING/FLOAT");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_showfloat(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[20];
double Value = 0;

  if ((arg_list[0].typ == String_Value) || (arg_list[0].typ == Float_Value))
    {

      if (arg_list[0].typ == String_Value)
        {
          strcpy(Buffer,arg_list[0].sValue);
          Replace(Buffer, ",", ".");
          Value = atof(Buffer);
        }
      else
        Value = arg_list[0].dValue;

      if (arg_list[1].typ == Int_Value)
        {
          sprintf(Buffer,"%.*f",(int) arg_list[1].nValue,Value);

          Replace(Buffer, ".", ",");
          ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
          strcpy(ret_val.sValue,Buffer);
          ret_val.typ = String_Value;
        }
      else
        {
          RuntimeError1("%%%%showfloat[PARAMETER1,PARAMETER2] - PARAMETER2 must be INTEGER");
          ret_val.typ = Not_Set;
        }
    }
  else
    {
      RuntimeError1("%%%%showfloat[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING/FLOAT");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_round(t_Value *arg_list)
{
double Value;
int Stellen;
t_Value ret_val;
char Buffer[40];


   ret_val.typ = Not_Set;

   if (arg_list[0].typ == String_Value) 
    {
      Value = atof(arg_list[0].sValue);
    }
  else
    if (arg_list[0].typ == Float_Value)
      {
        Value = arg_list[0].dValue;
      }
    else
      {
        RuntimeError1("%%%%round[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING/FLOAT");
        ret_val.typ = Not_Set;
        return ret_val;        
      }

   if (arg_list[1].typ == String_Value) 
     {
       Stellen = atoi(arg_list[1].sValue);
     }
   else
   if (arg_list[1].typ == Int_Value) 
     {
       Stellen = arg_list[1].nValue;
       ret_val.typ = Float_Value;
     }
   else
    {
      Stellen = 2;
    }

   if (Stellen == 0)
     ret_val.dValue = (int) Value;
   else
     {
       sprintf(Buffer,"%.*f",(int) Stellen,Value);
       ret_val.dValue = atof(Buffer);
     }

  return ret_val;        
}
/*--------------------------------------------------------------------------------*/
t_Value base_version(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[20];

  strcpy(Buffer,"2.1.2");

  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
  strcpy(ret_val.sValue,Buffer);
  ret_val.typ = String_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_year(t_Value *arg_list)
{
t_Value ret_val;
time_t TimeT;
tm* Today;

  time(&TimeT);

  Today = localtime(&TimeT);

  ret_val.nValue = Today->tm_year+1900;
  ret_val.typ = Int_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_month(t_Value *arg_list)
{
t_Value ret_val;
time_t TimeT;
tm* Today;

  time(&TimeT);

  Today = localtime(&TimeT);

  ret_val.nValue = Today->tm_mon +1;
  ret_val.typ = Int_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_daysuntil(t_Value *arg_list)
{
time_t TimeT;
t_Value ret_val;

  ret_val.typ = Int_Value;
  ret_val.nValue = -1;

  if (arg_list[0].typ == Date_Value)
    {
      if (arg_list[1].typ == Date_Value)
        {
          ret_val.nValue = DaysUntil(arg_list[0].tValue, arg_list[1].tValue);
          ret_val.typ = Int_Value;
        }
      else
        {
          time(&TimeT);
          ret_val.nValue = DaysUntil(TimeT, arg_list[0].tValue);
        }
     }
   else
     {
        RuntimeError1("%%%%daysuntil[PARAM1,PARAM2] - Parameter1 must be Date");
        ret_val.typ = Not_Set;
      }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_monthsuntil(t_Value *arg_list)
{
t_Value ret_val;
time_t Date1;
time_t Date2;
tm* TM1;
tm* TM2;
int Years;
int Months;
int Mon1;
int Mon2;
int Year1;
int Year2;

  ret_val.typ = Int_Value;
  ret_val.nValue = -1;

  if (arg_list[0].typ == Date_Value)
    {
      if (arg_list[1].typ == Date_Value)
        {
          Date1 = arg_list[0].tValue;
          Date2 = arg_list[1].tValue;
        }
      else
        {
          time(&Date1);
          Date2 = arg_list[0].tValue;

        }

     TM1 = localtime(&Date1);
     Mon1 = TM1->tm_mon+1;
     Year1 = TM1->tm_year;

     TM2 = localtime(&Date2);
     Mon2 = TM2->tm_mon+1;
     Year2 = TM2->tm_year;

     Years = Year2 - Year1;                      
     Months = Mon2 - Mon1;                      
     Months += 12* Years;
     ret_val.nValue = Months;


     }
   else
     {
        RuntimeError1("%%%%monthsuntil[PARAM1,(PARAM2)] - Parameter1 must be Date");
        ret_val.typ = Not_Set;
      }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_day(t_Value *arg_list)
{
t_Value ret_val;
time_t TimeT;
tm* Today;

  time(&TimeT);

  Today = localtime(&TimeT);

  ret_val.nValue = Today->tm_mday;
  ret_val.typ = Int_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_weekday(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[21];
time_t TimeT;
tm* Today;

  time(&TimeT);

  Today = localtime(&TimeT);

  switch(Today->tm_wday)
    {
      case 0: strcpy(Buffer,WEEKDAY1); break;
      case 1: strcpy(Buffer,WEEKDAY2); break;
      case 2: strcpy(Buffer,WEEKDAY3); break;
      case 3: strcpy(Buffer,WEEKDAY4); break;
      case 4: strcpy(Buffer,WEEKDAY5); break;
      case 5: strcpy(Buffer,WEEKDAY6); break;
      case 6: strcpy(Buffer,WEEKDAY7); break;
    }

  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
  strcpy(ret_val.sValue,Buffer);
  ret_val.typ = String_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_date(t_Value *arg_list)
{
t_Value ret_val;
time_t TimeT;
tm* Today;

  time(&TimeT);

  Today = localtime(&TimeT);

  ret_val.tValue = TimeT;
  ret_val.typ = Date_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_convertdate(t_Value *arg_list)
{
t_Value ret_val;
tm* Today;
char Buffer[60];

  if ((arg_list[0].typ == Date_Value) || (arg_list[0].typ == Time_Value) || (arg_list[0].typ == Timestamp_Value))
    {
      if (arg_list[1].typ == String_Value)
        {

          Today = localtime(&arg_list[0].tValue);

          if (strftime(Buffer,60,arg_list[1].sValue,Today) > 0)
            {
              ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
              strcpy(ret_val.sValue,Buffer);
              ret_val.typ = String_Value;
            }
          else
            {
              RuntimeError1("%%%%convertdate : wrong date format");
              ret_val.typ = Not_Set;
            }
        }
      else
        {
          RuntimeError1("%%%%convertdate[PARAMETER1, PARAMETER2] - PARAMETER2 must be STRING");
          ret_val.typ = Not_Set;
        }
    }
  else
    {
      RuntimeError1("%%%%convertdate[PARAMETER1, PARAMETER2] - PARAMETER1 must be DATE/TIME/TIMESTAMP");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_time(t_Value *arg_list)
{
t_Value ret_val;
time_t TimeT;
tm* Today;

  time(&TimeT);

  Today = localtime(&TimeT);

  ret_val.tValue = TimeT;
  ret_val.typ = Time_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_timestamp(t_Value *arg_list)
{
t_Value ret_val;
time_t TimeT;
tm* Today;

  time(&TimeT);

  Today = localtime(&TimeT);

  ret_val.tValue = TimeT;
  ret_val.typ = Timestamp_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_monthname(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[41];
time_t TimeT;
tm* Today;

  time(&TimeT);

  Today = localtime(&TimeT);
  
 	switch (Today->tm_mon)
	{
		case 0: strcpy(Buffer,MONTHNAME1);break;
		case 1: strcpy(Buffer,MONTHNAME2);break;
		case 2: strcpy(Buffer,MONTHNAME3);break;
		case 3: strcpy(Buffer,MONTHNAME4);break;
		case 4: strcpy(Buffer,MONTHNAME5);break;
		case 5: strcpy(Buffer,MONTHNAME6);break;
		case 6: strcpy(Buffer,MONTHNAME7);break;
		case 7: strcpy(Buffer,MONTHNAME8);break;
		case 8: strcpy(Buffer,MONTHNAME9);break;
		case 9: strcpy(Buffer,MONTHNAME10);break;
		case 10: strcpy(Buffer,MONTHNAME11);break;
		case 11: strcpy(Buffer,MONTHNAME12);break;
	}

  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
  strcpy(ret_val.sValue,Buffer);
  ret_val.typ = String_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_lastref(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.sValue = (char*) malloc(strlen(cgiReferer)+1);
  strcpy(ret_val.sValue,cgiReferer);
  ret_val.typ = String_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_username(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.sValue = (char*) malloc(strlen(cgiRemoteUser)+1);
  strcpy(ret_val.sValue,cgiRemoteUser);
  ret_val.typ = String_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_todate(t_Value *arg_list)
{
tm Time;
t_Value ret_val;
char Buffer[10];

  if (arg_list[0].typ == String_Value)
    {
      Time.tm_hour = 0;
      Time.tm_isdst = 0;
      Time.tm_mday = 0;
      Time.tm_min = 0;
      Time.tm_mon = 0;
      Time.tm_sec = 0;
      Time.tm_wday = 0;
      Time.tm_yday = 0;
      Time.tm_year = 0;


      #ifdef ENGLISH
        if (IsOfForm(arg_list[0].sValue, "dd/d/d"))
          {
            Mid(Buffer, arg_list[0].sValue,2);
            Time.tm_year = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[3],1);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[5],1);
            Time.tm_mday = atoi(Buffer);
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dddd/d/d"))
          {
            Mid(Buffer, arg_list[0].sValue,4);
            Time.tm_year = atoi(Buffer)-1900;
            Mid(Buffer, &arg_list[0].sValue[5],1);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[7],1);
            Time.tm_mday = atoi(Buffer);
          }
        else                           
        if (IsOfForm(arg_list[0].sValue, "dd/dd/d"))
          {
            Mid(Buffer, arg_list[0].sValue,2);
            Time.tm_year = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[3],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[6],1);
            Time.tm_mday = atoi(Buffer);
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dddd/dd/d"))
          {
            Mid(Buffer, arg_list[0].sValue,4);
            Time.tm_year = atoi(Buffer)-1900;
            Mid(Buffer, &arg_list[0].sValue[5],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[8],1);
            Time.tm_mday = atoi(Buffer);
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dd/d/dd"))
          {
            Mid(Buffer, arg_list[0].sValue,2);
            Time.tm_year = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[3],1);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[5],2);
            Time.tm_mday = atoi(Buffer);
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dddd/d/dd"))
          {
            Mid(Buffer, arg_list[0].sValue,4);
            Time.tm_year = atoi(Buffer)-1900;
            Mid(Buffer, &arg_list[0].sValue[5],1);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[7],2);
            Time.tm_mday = atoi(Buffer);
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dd/dd/dd"))
          {
            Mid(Buffer, arg_list[0].sValue,2);
            Time.tm_year = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[3],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[6],2);
            Time.tm_mday = atoi(Buffer);
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dddd/dd/dd"))
          {
            Mid(Buffer, arg_list[0].sValue,4);
            Time.tm_year = atoi(Buffer)-1900;
            Mid(Buffer, &arg_list[0].sValue[5],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[8],2);
            Time.tm_mday = atoi(Buffer);
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dddddddd"))
          {
            Mid(Buffer, arg_list[0].sValue,4);
            Time.tm_year = atoi(Buffer)-1900;
            Mid(Buffer, &arg_list[0].sValue[4],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[6],2);
            Time.tm_mday = atoi(Buffer);
          }
        else
          {
            ret_val.typ = Not_Set;
            return ret_val;
          }
      #else
        if (IsOfForm(arg_list[0].sValue, "d.d.dd"))
          {
            Mid(Buffer, arg_list[0].sValue,1);
            Time.tm_mday = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[2],1);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[4],2);
            Time.tm_year = atoi(Buffer);
          }
        else
        if (IsOfForm(arg_list[0].sValue, "d.d.dddd"))
          {
            Mid(Buffer, arg_list[0].sValue,1);
            Time.tm_mday = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[2],1);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[4],4);
            Time.tm_year = atoi(Buffer)-1900;
          }
        else                           
        if (IsOfForm(arg_list[0].sValue, "d.dd.dd"))
          {
            Mid(Buffer, arg_list[0].sValue,1);
            Time.tm_mday = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[2],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[5],2);
            Time.tm_year = atoi(Buffer);
          }
        else
        if (IsOfForm(arg_list[0].sValue, "d.dd.dddd"))
          {
            Mid(Buffer, arg_list[0].sValue,1);
            Time.tm_mday = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[2],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[5],4);
            Time.tm_year = atoi(Buffer)-1900;
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dd.d.dd"))
          {
            Mid(Buffer, arg_list[0].sValue,2);
            Time.tm_mday = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[3],1);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[5],2);
            Time.tm_year = atoi(Buffer);
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dd.d.dddd"))
          {
            Mid(Buffer, arg_list[0].sValue,2);
            Time.tm_mday = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[3],1);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[5],4);
            Time.tm_year = atoi(Buffer)-1900;
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dd.dd.dd"))
          {
            Mid(Buffer, arg_list[0].sValue,2);
            Time.tm_mday = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[3],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[6],2);
            Time.tm_year = atoi(Buffer);
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dd.dd.dddd"))
          {
            Mid(Buffer, arg_list[0].sValue,2);
            Time.tm_mday = atoi(Buffer);
            Mid(Buffer, &arg_list[0].sValue[3],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[6],4);
            Time.tm_year = atoi(Buffer)-1900;
          }
        else
        if (IsOfForm(arg_list[0].sValue, "dddddddd"))
          {
            Mid(Buffer, arg_list[0].sValue,4);
            Time.tm_year = atoi(Buffer)-1900;
            Mid(Buffer, &arg_list[0].sValue[4],2);
            Time.tm_mon = atoi(Buffer)-1;
            Mid(Buffer, &arg_list[0].sValue[6],2);
            Time.tm_mday = atoi(Buffer);
          }
        else
          {
            ret_val.typ = Not_Set;
            return ret_val;
          }
      #endif

      ret_val.tValue = mktime(&Time);
      ret_val.typ = Date_Value;
    }
  else
    {
      RuntimeError1("%%%%todate[PARAMETER] - PARAMETER must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_totime(t_Value *arg_list)
{
tm Time;
t_Value ret_val;
char Buffer[10];


  if (arg_list[0].typ == String_Value)
    {
      Time.tm_hour =  0;
      Time.tm_isdst = 0;
      Time.tm_mday =  0;
      Time.tm_min =   0;
      Time.tm_mon =   1;
      Time.tm_sec =   0;
      Time.tm_wday =  0;
      Time.tm_yday =  2;
      Time.tm_year =  70;

      if (IsOfForm(arg_list[0].sValue, "dd:dd"))
        {
          Mid(Buffer, arg_list[0].sValue,2);
          Time.tm_hour = atoi(Buffer);
          Mid(Buffer, &arg_list[0].sValue[3],2);
          Time.tm_min = atoi(Buffer);
        }
      else
      if (IsOfForm(arg_list[0].sValue, "d:dd"))
        {
          Mid(Buffer, arg_list[0].sValue,1);
          Time.tm_hour = atoi(Buffer);
          Mid(Buffer, &arg_list[0].sValue[2],2);
          Time.tm_min = atoi(Buffer);

        }
      else
        {
          ret_val.typ = Not_Set;
          return ret_val;
        }

      ret_val.tValue = mktime(&Time);
      ret_val.typ = Time_Value;

    }
  else
    {
      RuntimeError1("%%%%totime[PARAMETER] - PARAMETER must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_tohref(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];

  if (arg_list[0].typ == String_Value)
    {
      strcpy(Buffer,arg_list[0].sValue);
      String2HREF(Buffer);
  
      ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
      strcpy(ret_val.sValue,Buffer);
      ret_val.typ = String_Value;
    }
  else
    {
      RuntimeError1("%%%%tohref[PARAMETER] - PARAMETER must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_toprice(t_Value *arg_list)
{
t_Value ret_val;
char* Ptr;
char Buffer1[200];
char Buffer2[200];
char Buffer3[200];
int Start;
unsigned int Counter;

  if (arg_list[0].typ == String_Value)
    {
      strcpy(Buffer1,arg_list[0].sValue);
    }
  else
  if (arg_list[0].typ == Float_Value)
    {
      sprintf(Buffer1,"%.2f",arg_list[0].dValue);
    }
  else
  if (arg_list[0].typ == Int_Value)
    {
      sprintf(Buffer1,"%ld",arg_list[0].nValue);
      strcat(Buffer1,".00");
    }
  else
    {
      RuntimeError1("%%%%toprice[PARAMETER] - PARAMETER must be STRING/FLOAT/INTEGER");
      ret_val.typ = Not_Set;
      return ret_val;
    }

  Ptr=strchr(Buffer1,'.'); 

  if (Ptr)
      {
        Ptr++;
        if (strlen(Ptr)==0) strcat(Buffer1,"00");
      	else 
          if (strlen(Ptr)==1) strcat(Buffer1,"0");
          else 
            Ptr[2]=0;
      }
    else 
      strcat(Buffer1,".00");

  if (DECIMALSYMBOL != '.')
  while((Ptr = strchr(Buffer1,'.')) != NULL)
    *Ptr=DECIMALSYMBOL;

  if ((Ptr = strchr(Buffer1,DECIMALSYMBOL)) != NULL)
    {
      strcpy(Buffer3,"");


      Start = strlen(Buffer1) -3;
      if (Start > 2)
        {

          Start = (Start % 3);

          Mid(Buffer2, &Buffer1[0], Start);
          strcat(Buffer3,Buffer2);

      if ((Start != 0) && strlen(Buffer1) > 6)
        strcat(Buffer3,DIGITSYMBOL_STRING);


      for(Counter = 3;Counter < (strlen(Buffer1)-5); Counter+=3)
        {
          Mid(Buffer2,&Buffer1[Start],3);
          Start += 3;
          strcat(Buffer3,Buffer2);

          strcat(Buffer3,DIGITSYMBOL_STRING);
        }
      strcat(Buffer3,&Buffer1[Start]);
      strcpy(Buffer1,Buffer3);
        }

    }

  ret_val.sValue = (char*) malloc(strlen(Buffer1)+1);
  strcpy(ret_val.sValue,Buffer1);
  ret_val.typ = String_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_totimestamp(t_Value *arg_list)
{
tm Time;
t_Value ret_val;
char Buffer[20];


  if (arg_list[0].typ == String_Value)
    {
      Time.tm_hour = 0;
      Time.tm_isdst = 0;
      Time.tm_mday = 0;
      Time.tm_min = 0;
      Time.tm_mon = 0;
      Time.tm_sec = 0;
      Time.tm_wday = 0;
      Time.tm_yday = 0;
      Time.tm_year = 0;

      Mid(Buffer, arg_list[0].sValue,4);
      Time.tm_year = atoi(Buffer)-1900;
      Mid(Buffer, &arg_list[0].sValue[4],2);
      Time.tm_mon = atoi(Buffer)-1;
      Mid(Buffer, &arg_list[0].sValue[6],2);
      Time.tm_mday = atoi(Buffer);
      Mid(Buffer, &arg_list[0].sValue[8],2);
      Time.tm_hour = atoi(Buffer)-1;
      Mid(Buffer, &arg_list[0].sValue[10],2);
      Time.tm_min = atoi(Buffer);
      Mid(Buffer, &arg_list[0].sValue[12],2);
      Time.tm_sec = atoi(Buffer);

      ret_val.tValue = mktime(&Time);
      ret_val.typ = Timestamp_Value;
    }
  else
    {
      RuntimeError1("%%%%totimestamp[PARAMETER] - PARAMETER must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_exec(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
char Buffer2[25];
int Counter;
FILE* Pipe;
char* Ptr;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if (!OpenIniFile(INIFILE))
        {
          RuntimeError1("%%%%exec[PARAMETER] - CAN'T OPEN CONF-FILE");
          return ret_val;
        }
     
      sprintf(Buffer,"%s_%s",PROGRAMNAME,arg_list[0].sValue);
      if ((Ptr = GetFirstParam(Buffer)) != NULL)
        strcpy(Buffer,Ptr);
      else
        {
          RuntimeError1("%%%%exec[PARAMETER] - COMMAND NOT FOUND");
          return ret_val;
        }
      
      CloseIniFile();

      Counter = 1;
      while (arg_list[Counter].typ != Not_Set)
        {
          switch(arg_list[Counter].typ)
            {
              case String_Value:
                strcat(Buffer," ");
                strcat(Buffer,arg_list[Counter].sValue);
                break;
              case Bool_Value:
              case Int_Value:
                sprintf(Buffer2,"%ld",arg_list[Counter].nValue);
                strcat(Buffer," ");
                strcat(Buffer,Buffer2);
                break;
              case Float_Value:
                sprintf(Buffer2,"%d",arg_list[Counter].dValue);
                strcat(Buffer," ");
                strcat(Buffer,Buffer2);
                break;
              default:
                RuntimeError3("%%%%exec[PARAMETER1(,PARAMETER2,...)] - not supported PARAMETER",Counter);
                break;
            }

          Counter++;
        }

        EngineDB->Commit();

        #ifdef WIN32
          Pipe = _popen(Buffer,"rt");
        #else
          Pipe = popen(Buffer,"r");
        #endif

        if (Pipe != NULL)
          {
            while(!feof(Pipe))
              {
                if(fgets( Buffer, 499, Pipe) != NULL )
                  fprintf(cgiOut,Buffer );
              }
            #ifdef WIN32
              _pclose(Pipe);
            #else
              pclose(Pipe);
            #endif
           }
    }
  else
    {
      RuntimeError1("%%%%exec[PARAMETER] - PARAMETER must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_execs(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[256];
char Buffer2[25];
int Counter;
FILE* Pipe;
char* Ptr;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if (!OpenIniFile(INIFILE))
        {
          RuntimeError1("%%%%execs[PARAMETER] - CAN'T OPEN CONF-FILE");
          return ret_val;
        }
     
      sprintf(Buffer,"%s_%s",PROGRAMNAME,arg_list[0].sValue);
      if ((Ptr = GetFirstParam(Buffer)) != NULL)
        strcpy(Buffer,Ptr);
      else
        {
          RuntimeError1("%%%%execs[PARAMETER] - COMMAND NOT FOUND");
          return ret_val;
        }
      
      CloseIniFile();

      Counter = 1;
      while (arg_list[Counter].typ != Not_Set)
        {
          switch(arg_list[Counter].typ)
            {
              case String_Value:
                strcat(Buffer," ");
                strcat(Buffer,arg_list[Counter].sValue);
                break;
              case Bool_Value:
              case Int_Value:
                sprintf(Buffer2,"%ld",arg_list[Counter].nValue);
                strcat(Buffer," ");
                strcat(Buffer,Buffer2);
                break;
              case Float_Value:
                sprintf(Buffer2,"%d",arg_list[Counter].dValue);
                strcat(Buffer," ");
                strcat(Buffer,Buffer2);
                break;
              default:
                RuntimeError3("%%%%execs[PARAMETER1(,PARAMETER2,...)] - not supported PARAMETER",Counter);
                break;
            }

          Counter++;
        }

        EngineDB->Commit();

        #ifdef WIN32
          Pipe = _popen(Buffer,"rt");
        #else
          Pipe = popen(Buffer,"r");
        #endif

        strcpy(Buffer,"");
        if (Pipe != NULL)
          fgets( Buffer, 255, Pipe);

        ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
        strcpy(ret_val.sValue,Buffer);
        ret_val.typ = String_Value;

        #ifdef WIN32
          _pclose(Pipe);
        #else
          pclose(Pipe);
        #endif
    }
  else
    {
      RuntimeError1("%%%%execs[PARAMETER] - PARAMETER must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_execi(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
char Buffer2[25];
int Counter;
char* Ptr;

  ret_val.typ = Int_Value;

  if (arg_list[0].typ == String_Value)
    {
      if (!OpenIniFile(INIFILE))
        {
          RuntimeError1("%%%%execi[PARAMETER] - CAN'T OPEN CONF-FILE");
          return ret_val;
        }
     
      sprintf(Buffer,"%s_%s",PROGRAMNAME,arg_list[0].sValue);
      if ((Ptr = GetFirstParam(Buffer)) != NULL)
        strcpy(Buffer,Ptr);
      else
        {
          RuntimeError1("%%%%execi[PARAMETER] - COMMAND NOT FOUND");
          return ret_val;
        }
      
      CloseIniFile();

      Counter = 1;
      while (arg_list[Counter].typ != Not_Set)
        {
          switch(arg_list[Counter].typ)
            {
              case String_Value:
                strcat(Buffer," ");
                strcat(Buffer,arg_list[Counter].sValue);
                break;
              case Bool_Value:
              case Int_Value:
                sprintf(Buffer2,"%ld",arg_list[Counter].nValue);
                strcat(Buffer," ");
                strcat(Buffer,Buffer2);
                break;
              case Float_Value:
                sprintf(Buffer2,"%d",arg_list[Counter].dValue);
                strcat(Buffer," ");
                strcat(Buffer,Buffer2);
                break;
              default:
                RuntimeError3("%%%%execi[PARAMETER1(,PARAMETER2,...)] - not supported PARAMETER",Counter);
                break;
            }

          Counter++;
        }

      EngineDB->Commit();

      ret_val.nValue = system(Buffer);

    }
  else
    {
      RuntimeError1("%%%%execi[PARAMETER] - PARAMETER must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_merge(t_Value *arg_list)
{
t_Value ret_val;
char* Buffer;
char Buffer2[100];
int Counter;
int Length;
 
  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ != Not_Set)
        {
          Counter = 0;
          Length = 0;
          while (arg_list[Counter].typ != Not_Set)
            {
              switch(arg_list[Counter].typ)
                {
                  case String_Value:
                    Length += strlen(arg_list[Counter].sValue);
                    break;
                  case Bool_Value:
                  case Int_Value:
                    sprintf(Buffer2,"%ld",arg_list[Counter].nValue);
                    Length += strlen(Buffer2);
                    break;
                  case Float_Value:
                    sprintf(Buffer2,"%f",arg_list[Counter].dValue);
                    Length += strlen(Buffer2);
                    break;
                  default:
                    RuntimeError3("%%%%merge[PARAMETER1(,PARAMETER2,...)] - not supported PARAMETER",Counter);
                    break;
                }
              Counter++;
            }
          
          ret_val.typ=String_Value;
          ret_val.sValue = (char*) malloc(Length+1);

          strcpy(ret_val.sValue,arg_list[0].sValue);
          Counter = 1;
          while (arg_list[Counter].typ != Not_Set)
            {
              switch(arg_list[Counter].typ)
                {
                  case String_Value:
                    strcat(ret_val.sValue,arg_list[Counter].sValue);
                    break;
                  case Bool_Value:
                  case Int_Value:
                    sprintf(Buffer2,"%ld",arg_list[Counter].nValue);
                    strcat(ret_val.sValue,Buffer2);
                    break;
                  case Float_Value:
                    sprintf(Buffer2,"%f",arg_list[Counter].dValue);
                    strcat(ret_val.sValue,Buffer2);
                    break;
                  default:
                    RuntimeError3("%%%%merge[PARAMETER1(,PARAMETER2,...)] - not supported PARAMETER",Counter);
                    break;
                }
              Counter++;
            }
        }
      else
        {
          RuntimeError1("%%%%merge[PARAMETER1,PARAMETER2(,PARAMETER3)] - merger requires 2 parameters");
          ret_val.typ = Not_Set;
        }
    }
  else
    {
      RuntimeError1("%%%%merge[PARAMETER1,PARAMETER2(,PARAMETER3)] - PARAMETER1 must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
/* DB Functions */
/*--------------------------------------------------------------------------------*/
t_Value base_dbcopytable(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          /* ToDo - Rausfiltern von dummen Zeichen */
          return DBCopyTable(arg_list[0].sValue,arg_list[1].sValue);
        }
      else
        {
          RuntimeError1("%%%%dbcopytable[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
          ret_val.typ = Bool_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%dbcopytable[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dbinsert(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      return DBInsert(arg_list[0].sValue);
    }
  else
    {
      RuntimeError1("%%%%dbinsert[PARAMETER] - PARAMETER must be STRING");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dbdelete(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[0].typ == String_Value)
        {
          return DBDelete(arg_list[0].sValue,arg_list[1].sValue);
        }
      else
        {
          RuntimeError1("%%%%dbdelete[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
          ret_val.typ = Bool_Value;
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%dbdelete[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dbdrop(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      return DBDrop(arg_list[0].sValue);
    }
  else
    {
      RuntimeError1("%%%%dbdrop[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dbsaveform(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      return DBSaveForm(arg_list[0].sValue);
    }
  else
    {
      RuntimeError1("%%%%dbsaveform[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dbupdate(t_Value *arg_list)
{
t_Value ret_val;
int ArgCounter;
char* Ptr;
char* SQLPtr;
char* CurrentPtr;
char SQL[MAX_SQLLEN];
char Buffer[MAX_SQLLEN];
struct tm* Today;

  if (arg_list[0].typ == String_Value) 
    {
      
      SQLPtr = &(SQL[0]);

      if ((Ptr = strchr(arg_list[0].sValue,'?')) == NULL)
        {
          strcpy(SQL, arg_list[0].sValue);    
        }
      else
        {
          ArgCounter = 1;
          CurrentPtr = arg_list[0].sValue;
          do
            {
              while (CurrentPtr<Ptr)
                *SQLPtr++=*CurrentPtr++;

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
                    RuntimeError1("%%%%dbupdate[PARAMETER1,(PARAMETER2,...)] - Not enough parameters");
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
      return DBUpdate(SQL);
    }
  else
    {
      RuntimeError1("%%%%dbupdate[PARAMETER1,(PARAMETER2,...)] - PARAMETER1 must be STRING");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dbsql(t_Value *arg_list)
{
t_Value ret_val;
int ArgCounter;
char* Ptr;
char* SQLPtr;
char* CurrentPtr;
char SQL[MAX_SQLLEN];
char Buffer[MAX_SQLLEN];
struct tm* Today;

  if (arg_list[0].typ == String_Value) 
    {
      
      SQLPtr = &(SQL[0]);

      if ((Ptr = strchr(arg_list[0].sValue,'?')) == NULL)
        {
          strcpy(SQL, arg_list[0].sValue);    
        }
      else
        {
          ArgCounter = 1;
          CurrentPtr = arg_list[0].sValue;
          do
            {
              while (CurrentPtr<Ptr)
                *SQLPtr++=*CurrentPtr++;

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
                    RuntimeError1("%%%%dbsql[PARAMETER1,(PARAMETER2,...)] - Not enough parameters");
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
      return DBSQL(SQL);
    }
  else
    {
      RuntimeError1("%%%%dbsql[PARAMETER1,(PARAMETER2,...)] - PARAMETER1 must be STRING");
      ret_val.typ = Bool_Value;
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_cgibin(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = String_Value;

  ret_val.sValue = (char*) malloc(strlen(CGIBIN)+1);
  strcpy(ret_val.sValue,CGIBIN);

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_securecgibin(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = String_Value;

  ret_val.sValue = (char*) malloc(strlen(SECURECGIBIN)+1);
  strcpy(ret_val.sValue,SECURECGIBIN);

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_baseref(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = String_Value;
  ret_val.sValue = (char*) malloc(strlen(BASEREF)+1);
  strcpy(ret_val.sValue,BASEREF);

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_securebaseref(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = String_Value;
  ret_val.sValue = (char*) malloc(strlen(SECUREBASEREF)+1);
  strcpy(ret_val.sValue,SECUREBASEREF);

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
/* Directory functions  UNIX */
/*--------------------------------------------------------------------------------*/
#ifndef WIN32
t_Value base_dirreadfirst(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
struct dirent *DirEnt;
struct stat Stat;

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      if ((Dir = opendir(arg_list[0].sValue)) != NULL)
        {
          do
            {
              if ((DirEnt = readdir(Dir)) != NULL)
                {
                  strcpy(Buffer,arg_list[0].sValue);

                  if (Buffer[strlen(Buffer)] != '/')
                    strcat(Buffer,"/");

                  strcpy(DirEntry.Path,Buffer);

                  strcat(Buffer,DirEnt->d_name);

                  if (stat(Buffer,&Stat) != 0)
                    {
                      ret_val.nValue = 0;
                      return ret_val;
                    }
                 }
              else
                {
                  ret_val.nValue = 0;
                  return ret_val;
                }

            if (strcmp(DirEnt->d_name,".") != 0)
              {
                if (S_ISDIR(Stat.st_mode))
                  break;
              }
            }
          while(1);
        
          strcpy(DirEntry.FileName,DirEnt->d_name);

          DirEntry.ATime = Stat.st_atime;
          DirEntry.CTime = Stat.st_ctime;
          DirEntry.MTime = Stat.st_mtime;
          DirEntry.UID   = Stat.st_uid;
          DirEntry.GID   = Stat.st_gid;
          DirEntry.Mode  = Stat.st_mode;
          DirEntry.Size  = Stat.st_size;

		  ret_val.nValue = 1;
        }
      else
        {
          RuntimeError2("%%%%dirreadfirst[PARAMETER] - error opening directory ",arg_list[0].sValue);
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%dirreadfirst[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirreadnext(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
struct dirent *DirEnt;
struct stat Stat;

  if (arg_list); /* Dummy */

  ret_val.typ = Bool_Value;
  do
    {
      if ((DirEnt = readdir(Dir)) != NULL)
        {

          strcpy(Buffer,DirEntry.Path);

          strcat(Buffer,DirEnt->d_name);

          if (stat(Buffer,&Stat) == -1)
            {
              /* Runtime Error */
			  ret_val.nValue = 0;
              return ret_val;
            }
         }
      else
        {
		  
          /* Runtime Error */
		  /* stefan */
		  //closedir(Dir); bleibt die engine stecken
          ret_val.nValue = 0;
          return ret_val;
        }

      if (strcmp(DirEnt->d_name,".") != 0)
        {
          if (S_ISDIR(Stat.st_mode))
            break;
        }
    }
  while(1);

  strcpy(DirEntry.FileName,DirEnt->d_name);

  DirEntry.ATime = Stat.st_atime;
  DirEntry.CTime = Stat.st_ctime;
  DirEntry.MTime = Stat.st_mtime;
  DirEntry.UID   = Stat.st_uid;
  DirEntry.GID   = Stat.st_gid;
  DirEntry.Mode  = Stat.st_mode;
  DirEntry.Size  = Stat.st_size;

  ret_val.nValue = 1;
  return ret_val;
}
#else
/*--------------------------------------------------------------------------------*/
/* Directory functions  WIN32 */
/*--------------------------------------------------------------------------------*/
t_Value base_dirreadfirst(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
_finddata_t DirEnt;

  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  if (arg_list[0].typ == String_Value)
    {
	  strcpy(Buffer,arg_list[0].sValue);
      if (Buffer[strlen(Buffer)] != '*')
		if (Buffer[strlen(Buffer)] != '\\')
          strcat(Buffer,"\\*");
		else
		  strcat(Buffer,"*");

    strcpy(DirEntry.Path,Buffer);

    if ((Dir = _findfirst(Buffer,&DirEnt)) != -1)
		  {
        do
          {
            if (DirEnt.attrib & _A_SUBDIR)
              if (strcmp(DirEnt.name,".") != 0)
                break;

            if( _findnext(Dir,&DirEnt) == -1)
              return ret_val;
          }
        while(1);

        strcpy(DirEntry.FileName,DirEnt.name);
        DirEntry.Time = DirEnt.time_create;
        DirEntry.Size = 0;

        ret_val.nValue = 1;
      }
    else
      {
        RuntimeError2("%%%%dirreadfirst[PARAMETER] - error reading directory ",arg_list[0].sValue);
        ret_val.nValue = 0;
      }
    }
  else
    {
      RuntimeError1("%%%%dirreadfirst[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirreadnext(t_Value *arg_list)
{
t_Value ret_val;
_finddata_t DirEnt;

  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  do
    {
      if (_findnext(Dir,&DirEnt) == -1)
        return ret_val;

      if (DirEnt.attrib & _A_SUBDIR)
        if (strcmp(DirEnt.name,".") != 0)
          break;
    }
  while(1);

  strcpy(DirEntry.FileName,DirEnt.name);
  DirEntry.Time = DirEnt.time_create;
  DirEntry.Size = 0;

  ret_val.nValue = 1;
  return ret_val;
}
#endif
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetname(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = String_Value;

  ret_val.sValue = (char*) malloc(strlen(DirEntry.FileName)+1);
  strcpy(ret_val.sValue,DirEntry.FileName);
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgettime(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Time_Value;
  ret_val.tValue = DirEntry.MTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetacsesstime(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Time_Value;
  ret_val.tValue = DirEntry.ATime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetcreationtime(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Time_Value;
  ret_val.tValue = DirEntry.CTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetdate(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Date_Value;
  ret_val.tValue = DirEntry.MTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetaccessdate(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Date_Value;
  ret_val.tValue = DirEntry.ATime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetcreationdate(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Date_Value;
  ret_val.tValue = DirEntry.CTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgettimestamp(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Timestamp_Value;
  ret_val.tValue = DirEntry.MTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetaccesstimestamp(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Timestamp_Value;
  ret_val.tValue = DirEntry.ATime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetcreationtimestamp(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Timestamp_Value;
  ret_val.tValue = DirEntry.CTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetuid(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Int_Value;
  ret_val.nValue = DirEntry.UID;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetuser(t_Value *arg_list)
{
t_Value ret_val;
passwd *Passwd;

  ret_val.typ = String_Value;

  if ((Passwd = getpwuid(DirEntry.UID)) != NULL)
    {
      ret_val.sValue = (char*) malloc(strlen(Passwd->pw_name)+1);
      strcpy(ret_val.sValue,Passwd->pw_name);
    }
  else
    {
      ret_val.sValue = (char*) malloc(strlen("")+1);
      strcpy(ret_val.sValue,"");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetgid(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Int_Value;
  ret_val.nValue = DirEntry.GID;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetgroup(t_Value *arg_list)
{
t_Value ret_val;
group *Group;

  ret_val.typ = String_Value;

  if ((Group = getgrgid(DirEntry.GID)) != NULL)
    {
      ret_val.sValue = (char*) malloc(strlen(Group->gr_name)+1);
      strcpy(ret_val.sValue,Group->gr_name);
    }
  else
    {
      ret_val.sValue = (char*) malloc(strlen("")+1);
      strcpy(ret_val.sValue,"");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetmode(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Int_Value;
  ret_val.nValue = DirEntry.Mode;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirgetstrmode(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[21];
long Mode;

/*
  if (S_ISLNK(DirEntry.Mode))
    strcpy(Buffer,"l");
  else
    strcpy(Buffer,"d");
*/
  strcpy(Buffer,"");

  Mode = DirEntry.Mode & S_IRWXU;
  if ((Mode & S_IRUSR) ==S_IRUSR)
    strcat(Buffer,"r");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IWUSR) ==S_IWUSR)
    strcat(Buffer,"w");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IXUSR) ==S_IXUSR)
    strcat(Buffer,"x");
  else
    strcat(Buffer,"-");

  Mode = DirEntry.Mode & S_IRWXG;
  if ((Mode & S_IRGRP) ==S_IRGRP)
    strcat(Buffer,"r");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IWGRP) ==S_IWGRP)
    strcat(Buffer,"w");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IXGRP) ==S_IXGRP)
    strcat(Buffer,"x");
  else
    strcat(Buffer,"-");

  Mode = DirEntry.Mode & S_IRWXO;
  if ((Mode & S_IROTH) ==S_IROTH)
    strcat(Buffer,"r");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IWOTH) ==S_IWOTH)
    strcat(Buffer,"w");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IXOTH) ==S_IXOTH)
    strcat(Buffer,"x");
  else
    strcat(Buffer,"-");

  ret_val.typ = String_Value;
  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
  strcpy(ret_val.sValue,Buffer);

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirchange(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      #ifndef WIN32
        ret_val.nValue = (chdir(arg_list[0].sValue) == 0);
      #else
        ret_val.nValue = (_chdir(arg_list[0].sValue) == 0);
      #endif
    }
  else
    {
      RuntimeError1("%%%%dirchange[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dircreate(t_Value *arg_list)
{
t_Value ret_val;

    ret_val.typ=Bool_Value;
    ret_val.nValue = 0;
    
  if (arg_list[0].typ == String_Value)
    {
      #ifdef WIN32
        ret_val.nValue = (_mkdir(arg_list[0].sValue) == 0);
      #else
        ret_val.nValue = (mkdir(arg_list[0].sValue,0) == 0);
      #endif
    }
  else
    {
      RuntimeError1("%%%%dircreate[PARAMETER1] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_dirdelete(t_Value *arg_list)
{
t_Value ret_val;

    ret_val.typ=Bool_Value;
    ret_val.nValue = 0;
    
  if (arg_list[0].typ == String_Value)
    {
      #ifdef WIN32
        ret_val.nValue = (_rmdir(arg_list[0].sValue) == 0);
      #else
        ret_val.nValue = (rmdir(arg_list[0].sValue) == 0);
      #endif
    }
  else
    {
      RuntimeError1("%%%%dirdelete[PARAMETER1] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
/* File functions UNIX */
/*--------------------------------------------------------------------------------*/

#ifndef WIN32
t_Value base_filereadfirst(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
struct dirent *DirEnt;
struct stat Stat;

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      if ((Dir = opendir(arg_list[0].sValue)) != NULL)
        {
          do
            {
              if ((DirEnt = readdir(Dir)) != NULL)
                {
                  strcpy(Buffer,arg_list[0].sValue);

                  if (Buffer[strlen(Buffer)] != '/')
                    strcat(Buffer,"/");

                  strcpy(DirEntry.Path,Buffer);

                  strcat(Buffer,DirEnt->d_name);

                  if (stat(Buffer,&Stat) != 0)
                    {
                      ret_val.nValue = 0;
                      return ret_val;
                    }
                 }
              else
                {
                  ret_val.nValue = 0;
                  return ret_val;
                }
            }
          while(! S_ISREG(Stat.st_mode));      

          strcpy(DirEntry.FileName,DirEnt->d_name);

          DirEntry.ATime = Stat.st_atime;
          DirEntry.CTime = Stat.st_ctime;
          DirEntry.MTime = Stat.st_mtime;
          DirEntry.UID   = Stat.st_uid;
          DirEntry.GID   = Stat.st_gid;
          DirEntry.Mode  = Stat.st_mode;
          DirEntry.Size  = Stat.st_size;

          ret_val.nValue = 1;
        }
      else
        {
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%filereadfirst[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }
  
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filereadnext(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
struct dirent *DirEnt;
struct stat Stat;

  if (arg_list); /* Dummy */

  ret_val.typ = Bool_Value;

  do
    {
      if ((DirEnt = readdir(Dir)) != NULL)
        {
          strcpy(Buffer,DirEntry.Path);

          strcat(Buffer,DirEnt->d_name);

          if (stat(Buffer,&Stat) != 0)
            {
              /* Runtime Error */
			  /* stefan */
			  //closedir(Dir);
              ret_val.nValue = 0;
              return ret_val;
            }

         }
      else
        {
          /* Runtime Error */
		  /* stefan */
		  //closedir(Dir);
          ret_val.nValue = 0;
          return ret_val;
        }
    }
  while(! S_ISREG(Stat.st_mode));

  strcpy(DirEntry.FileName,DirEnt->d_name);
  DirEntry.ATime = Stat.st_atime;
  DirEntry.CTime = Stat.st_ctime;
  DirEntry.MTime = Stat.st_mtime;
  DirEntry.UID   = Stat.st_uid;
  DirEntry.GID   = Stat.st_gid;
  DirEntry.Mode  = Stat.st_mode;
  DirEntry.Size  = Stat.st_size;
  
  ret_val.nValue = 1;
  return ret_val;
}
#else
/*--------------------------------------------------------------------------------*/
/* File functions WIN32 */
/*--------------------------------------------------------------------------------*/
t_Value base_filereadfirst(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
_finddata_t DirEnt;

  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  if (arg_list[0].typ == String_Value)
    {
	    strcpy(Buffer,arg_list[0].sValue);
      if (Buffer[strlen(Buffer)] != '*')
		    if ((Buffer[strlen(Buffer)] != '\\') || (Buffer[strlen(Buffer)] != '/'))
          strcat(Buffer,"\\*");
		else
		  strcat(Buffer,"*");

    strcpy(Buffer,DirEntry.Path);
 
    if ((Dir = _findfirst(Buffer,&DirEnt)) != -1)
		  {
        do
          {
            if (!(DirEnt.attrib & _A_SUBDIR))
              break;

            if( _findnext(Dir,&DirEnt) == -1)
              return ret_val;
          }
        while(1);

        strcpy(DirEntry.FileName,DirEnt.name);
        DirEntry.Time = DirEnt.time_create;
        DirEntry.Size = DirEnt.size;

        ret_val.nValue = 1;
      }
    else
      {
        ret_val.nValue = 0;
      }
    }
  else
    {
      RuntimeError1("%%%%filereadfirst[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }
  
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filereadnext(t_Value *arg_list)
{
t_Value ret_val;
_finddata_t DirEnt;

  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  do
    {
      if (_findnext(Dir,&DirEnt) == -1)
        return ret_val;

      if (!(DirEnt.attrib & _A_SUBDIR))
        break;
    }
  while(1);

  strcpy(DirEntry.FileName,DirEnt.name);
  DirEntry.Time = DirEnt.time_create;
  DirEntry.Size = DirEnt.size;

  ret_val.nValue = 1;
  return ret_val;
}
#endif
/*--------------------------------------------------------------------------------*/
t_Value base_filegetname(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = String_Value;

  ret_val.sValue = (char*) malloc(strlen(DirEntry.FileName)+1);
  strcpy(ret_val.sValue,DirEntry.FileName);
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetsize(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Int_Value;

  ret_val.nValue = DirEntry.Size;
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegettime(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Time_Value;
  ret_val.tValue = DirEntry.MTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetaccesstime(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Time_Value;
  ret_val.tValue = DirEntry.ATime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetcreationtime(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Time_Value;
  ret_val.tValue = DirEntry.CTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetdate(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Date_Value;
  ret_val.tValue = DirEntry.MTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetaccessdate(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Date_Value;
  ret_val.tValue = DirEntry.ATime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetcreationdate(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Date_Value;
  ret_val.tValue = DirEntry.CTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegettimestamp(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Timestamp_Value;
  ret_val.tValue = DirEntry.MTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetaccesstimestamp(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Timestamp_Value;
  ret_val.tValue = DirEntry.ATime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetcreationtimestamp(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Timestamp_Value;
  ret_val.tValue = DirEntry.CTime;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetuid(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Int_Value;
  ret_val.nValue = DirEntry.UID;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetuser(t_Value *arg_list)
{
t_Value ret_val;
passwd *Passwd;

  ret_val.typ = String_Value;

  if ((Passwd = getpwuid(DirEntry.UID)) != NULL)
    {
      ret_val.sValue = (char*) malloc(strlen(Passwd->pw_name)+1);
      strcpy(ret_val.sValue,Passwd->pw_name);
    }
  else
    {
      ret_val.sValue = (char*) malloc(strlen("")+1);
      strcpy(ret_val.sValue,"");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetgid(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Int_Value;
  ret_val.nValue = DirEntry.GID;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetgroup(t_Value *arg_list)
{
t_Value ret_val;
group *Group;

  ret_val.typ = String_Value;

  if ((Group = getgrgid(DirEntry.GID)) != NULL)
    {
      ret_val.sValue = (char*) malloc(strlen(Group->gr_name)+1);
      strcpy(ret_val.sValue,Group->gr_name);
    }
  else
    {
      ret_val.sValue = (char*) malloc(strlen("")+1);
      strcpy(ret_val.sValue,"");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetmode(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Int_Value;
  ret_val.nValue = DirEntry.Mode;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filegetstrmode(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[21];
unsigned int Mode;

/*
  if ((DirEntry.Mode & S_IFMT & S_IFLNK) == S_IFLNK)
    strcpy(Buffer,"l");
  else
    strcpy(Buffer,"-");
*/
  strcpy(Buffer,"");

  Mode = DirEntry.Mode & S_IRWXU;
  if ((Mode & S_IRUSR) ==S_IRUSR)
    strcat(Buffer,"r");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IWUSR) ==S_IWUSR)
    strcat(Buffer,"w");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IXUSR) ==S_IXUSR)
    strcat(Buffer,"x");
  else
    strcat(Buffer,"-");

  Mode = DirEntry.Mode & S_IRWXG;
  if ((Mode & S_IRGRP) ==S_IRGRP)
    strcat(Buffer,"r");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IWGRP) ==S_IWGRP)
    strcat(Buffer,"w");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IXGRP) ==S_IXGRP)
    strcat(Buffer,"x");
  else
    strcat(Buffer,"-");

  Mode = DirEntry.Mode & S_IRWXO;
  if ((Mode & S_IROTH) ==S_IROTH)
    strcat(Buffer,"r");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IWOTH) ==S_IWOTH)
    strcat(Buffer,"w");
  else
    strcat(Buffer,"-");
  if ((Mode & S_IXOTH) ==S_IXOTH)
    strcat(Buffer,"x");
  else
    strcat(Buffer,"-");

  ret_val.typ = String_Value;
  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
  strcpy(ret_val.sValue,Buffer);

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_templateroot(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = String_Value;
  ret_val.sValue = (char*) malloc(strlen(TEMPLATEROOT)+1);
  strcpy(ret_val.sValue,TEMPLATEROOT);

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_upload(t_Value *arg_list)
{
t_Value ret_val;
char ID[21];

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
              cgiFormString("ID",ID,20);
              fprintf(cgiOut,"<FORM ENCTYPE=\"multipart/form-data\" ACTION=\"%s?ID=%s&PATH=%s&HTML=%s\" METHOD=POST>\n",CGIBIN,ID,arg_list[0].sValue,arg_list[1].sValue); 
              fprintf(cgiOut,"<INPUT NAME=\"USERFILE1\" TYPE=\"file\">\n"); 
              if (arg_list[2].typ == String_Value)
                fprintf(cgiOut,"<INPUT TYPE=\"submit\" VALUE=\"%s\">\n",arg_list[2].sValue); 
              else
                fprintf(cgiOut,"<INPUT TYPE=\"submit\" VALUE=\"Send File\">\n"); 
              fprintf(cgiOut,"</FORM>");
        }
      else
        RuntimeError1("%%%%upload[PARAMETER1,PARAMETER2,PARAMETER3] - PARAMETER2 must be STRING");
    }
  else
    RuntimeError1("%%%%upload[PARAMETER1,PARAMETER2,PARAMETER3] - PARAMETER1 must be STRING");
  
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_uploadex(t_Value *arg_list)
{
t_Value ret_val;
char ID[21];

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
              cgiFormString("ID",ID,20);
              fprintf(cgiOut,"<FORM ENCTYPE=\"multipart/form-data\" ACTION=\"%s?ID=%s&PATH=%s&HTML=%s\" METHOD=POST>\n",CGIBIN,ID,arg_list[0].sValue,arg_list[1].sValue); 
              fprintf(cgiOut,"<INPUT NAME=\"USERFILE1\" TYPE=\"file\">\n"); 
              if (arg_list[2].typ == String_Value)
                fprintf(cgiOut,"<INPUT TYPE=\"submit\" VALUE=\"%s\">\n",arg_list[2].sValue); 
              else
                fprintf(cgiOut,"<INPUT TYPE=\"submit\" VALUE=\"Send File\">\n"); 
        }
      else
        RuntimeError1("%%%%upload[PARAMETER1,PARAMETER2,PARAMETER3] - PARAMETER2 must be STRING");
    }
  else
    RuntimeError1("%%%%upload[PARAMETER1,PARAMETER2,PARAMETER3] - PARAMETER1 must be STRING");
  
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
t_Value base_importfile(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          if (arg_list[2].typ == String_Value)
            return DBImportFile(arg_list[0].sValue,arg_list[1].sValue,arg_list[2].sValue[0]);
          else
            return DBImportFile(arg_list[0].sValue,arg_list[1].sValue,';');
        }
      else
        RuntimeError1("%%%%importfile[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
    }
  else
    RuntimeError1("%%%%importfile[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");

  return ret_val;
}
#endif
/*--------------------------------------------------------------------------------*/
t_Value base_itemcount(t_Value *arg_list)
{
char SQL[200];
t_Value ret_val;
CDBField* COUNT;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      sprintf(SQL,"select COUNT(*) ANZAHL from %s",arg_list[0].sValue);
    }
  else
    {
      RuntimeError1("%%%%itemcount[PARAMETER1] - PARAMETER1 must be STRING");
      return ret_val;
    }

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) == 0)
    return ret_val;

  if ((COUNT = EngineDB->GetField(MAX_SQLSELECT-1,"ANZAHL")) != NULL)
    {
      ret_val.nValue = COUNT->Long;
      ret_val.typ = Int_Value;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_replace(t_Value *arg_list)
{
char Buffer[501];
char Buffer2[2];
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    if (arg_list[1].typ == String_Value)
      if (arg_list[2].typ == String_Value)
        {
          strcpy(Buffer,"");
          Buffer2[1] = '\0';

          for(unsigned int Counter = 0;Counter < strlen(arg_list[0].sValue);Counter++)
            {
              if (strncmp(&arg_list[0].sValue[Counter],arg_list[1].sValue,strlen(arg_list[1].sValue)) == 0)
                {
                  strcat(Buffer,arg_list[2].sValue);
                  Counter += (strlen(arg_list[1].sValue)-1);
                }
              else
                {
                  Buffer2[0] = arg_list[0].sValue[Counter];
                  strcat(Buffer,Buffer2);
                }
              ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
              strcpy(ret_val.sValue,Buffer);
              ret_val.typ = String_Value;
            }
        }
      else RuntimeError1("%%%%replace[PARAMETER1,PARAMETER2,PARAMETER3] - PARAMETER3 must be STRING");
    else RuntimeError1("%%%%replace[PARAMETER1,PARAMETER2,PARAMETER3] - PARAMETER2 must be STRING");
  else RuntimeError1("%%%%replace[PARAMETER1,PARAMETER2,PARAMETER3] - PARAMETER1 must be STRING");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_setcookie(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[100];

  ret_val.typ = Not_Set;
  if (arg_list[0].typ == String_Value)
    if (arg_list[1].typ == String_Value)
      {
        if (arg_list[2].typ == String_Value)
          sprintf(Buffer,"text/html\nSet-Cookie: %s=%s; %s",arg_list[0].sValue,arg_list[1].sValue,arg_list[2].sValue);
        else
          sprintf(Buffer,"text/html\nSet-Cookie: %s=%s;",arg_list[0].sValue,arg_list[1].sValue);
        cgiHeaderContentType(Buffer);
      }
    else 
      {
        cgiHeaderContentType("text/html");
        RuntimeError1("%%%%setcookie[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
      }
  else 
    {
      cgiHeaderContentType("text/html");
      RuntimeError1("%%%%setcookie[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_getcookie(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[100];
char* Pos;

  if (arg_list[0].typ == String_Value)
    {
      if ((Pos = strstr(cgiCookie,arg_list[0].sValue)) != NULL)
        strcpy(Buffer,Pos + strlen(arg_list[0].sValue)+1);
      else
        strcpy(Buffer,"");

      ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
      strcpy(ret_val.sValue,Buffer);
      ret_val.typ = String_Value;

    }
  else 
    {
      RuntimeError1("%%%%getcookie[PARAMETER1] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_redirect(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[100];

  ret_val.typ = Not_Set;
  if (arg_list[0].typ == String_Value)
    {
        sprintf(Buffer,"text/html\nLocation: %s",arg_list[0].sValue);
        cgiHeaderContentType(Buffer);
    }
  else 
    {
      cgiHeaderContentType("text/html");
      RuntimeError1("%%%%redirect[PARAMETER1] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_mimetype(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Not_Set;
  if (arg_list[0].typ == String_Value)
    {
      cgiHeaderContentType(arg_list[0].sValue);
    }
  else 
    {
      cgiHeaderContentType("text/html");
      RuntimeError1("%%%%mimetype[PARAMETER1] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_tofilebegin(t_Value *arg_list)
{
t_Value ret_val;
char Filename[255];

  
  ret_val.typ = Bool_Value;
  if (arg_list[0].typ == String_Value)
    {
      if (TEMPLATEROOT[strlen(TEMPLATEROOT)-1] != '/')
        sprintf(Filename,"%s/%s",TEMPLATEROOT,arg_list[0].sValue);
      else
        sprintf(Filename,"%s%s",TEMPLATEROOT,arg_list[0].sValue);

      if ((cgiOut = fopen( Filename, "w")) == NULL)
        {
          ret_val.nValue = 0;
          cgiOut = stdout;
        }
      else
        ret_val.nValue = 1;
    }
  else 
    {
      RuntimeError1("%%%%tofilebegin[PARAMETER] - PARAMETER must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_tofileend(t_Value *arg_list)
{
t_Value ret_val;
  
  ret_val.typ = Not_Set;

  if (cgiOut != stdout)
    {
      fclose(cgiOut);
      cgiOut = stdout;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_sendfile(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[16000];
long Read;
FILE* File;

  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          if ((File = fopen(arg_list[0].sValue,"r")) == NULL)
            {
              cgiHeaderContentType("text/html");
              RuntimeError1("%%%%sendfile : Kann Datei nicht finden");
              return ret_val;
            }
          cgiHeaderContentType(arg_list[1].sValue);
          while ((Read = fread(Buffer,1,16000,File)) == 16000)
            {
              fwrite(Buffer,1,Read,cgiOut);
            }
          fwrite(Buffer,1,Read,cgiOut);
          fclose(File);    
        }
      else
        {
          cgiHeaderContentType("text/html");
          RuntimeError1("%%%%sendfile[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
          return ret_val;
        }
    }
  else 
    {
      cgiHeaderContentType("text/html");
      RuntimeError1("%%%%sendfile[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      return ret_val;
    }

  ret_val.nValue = 1;
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filedelete(t_Value *arg_list)
{
t_Value ret_val;

    ret_val.typ=Bool_Value;
    ret_val.nValue = 0;
    
  if (arg_list[0].typ == String_Value)
    {
      ret_val.nValue = (remove(arg_list[0].sValue) == 0);
    }
  else
    {
      RuntimeError1("%%%%filedelete[PARAMETER1] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_filecopy(t_Value *arg_list)
{
t_Value ret_val;
FILE* SourceFile;
FILE* DestinationFile;
char Buffer[8192];
size_t Size;

  ret_val.typ=Bool_Value;
  ret_val.nValue = 0;
    
  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          #ifdef WIN32
            if ((SourceFile = fopen(arg_list[0].sValue, "rb")) == NULL)
          #else
            if ((SourceFile = fopen(arg_list[0].sValue, "r")) == NULL)
          #endif
            {
              return ret_val;
            }

          #ifdef WIN32
            if ((DestinationFile = fopen(arg_list[1].sValue, "wb")) == NULL)
          #else
            if ((DestinationFile = fopen(arg_list[1].sValue, "w")) == NULL)
          #endif
            {
              fclose(SourceFile);
              return ret_val;
            }

          while((Size = fread(Buffer,1,8192,SourceFile)) > 0)
            {
              if (fwrite(Buffer,1,Size,DestinationFile) != Size)
                {
                  fclose(SourceFile);
                  fclose(DestinationFile);
                  remove(arg_list[1].sValue);
                  return ret_val;
                }
            }

          fclose(SourceFile);
          fclose(DestinationFile);
          ret_val.nValue = 1;
        }
      else
       {
         RuntimeError1("%%%%filecopy[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
       }
    }
  else
    {
      RuntimeError1("%%%%filecopy[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_crypt(t_Value *arg_list)
{
char* Ptr;
t_Value ret_val;

  cryptpassword (&Ptr, arg_list[0].sValue, arg_list[1].sValue);

  ret_val.sValue = (char*) Ptr;
  ret_val.typ = String_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_decrypt(t_Value *arg_list)
{
char* Ptr;
t_Value ret_val;

  decryptpassword (&Ptr, arg_list[0].sValue, arg_list[1].sValue);

  ret_val.sValue = (char*) Ptr;
  ret_val.typ = String_Value;

  return ret_val; 
}
/*--------------------------------------------------------------------------------*/
t_Value base_fetch(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          if (arg_list[2].typ == String_Value)
            {
              ret_val = DBFetch(arg_list[0].sValue, arg_list[1].sValue,arg_list[2].sValue);
            }
          else
            RuntimeError1("%%%%fetch[PARAMETER1,PARAMETER2,PARAMETER3] - PARAMETER3 must be STRING");
        }
      else
        RuntimeError1("%%%%fetch[PARAMETER1,PARAMETER2,PARAMETER3] - PARAMETER2 must be STRING");
    }
  else
    RuntimeError1("%%%%fetch[PARAMETER1,PARAMETER2,PARAMETER3] - PARAMETER1 must be STRING");

  return ret_val;

}
/*--------------------------------------------------------------------------------*/
t_Value base_isnull(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ != Not_Set)
    {
      ret_val.typ = Bool_Value;
      ret_val.nValue = (arg_list[0].Indicator == -1);
    }
  else
    RuntimeError1("%%%%isnull[PARAMETER1] - PARAMETER1 must be SET");

  return ret_val; 
}
/*--------------------------------------------------------------------------------*/
t_Value base_daysofmonth(t_Value *arg_list)
{
t_Value ret_val;
time_t TimeT;
tm* Today;
int Year;
int Switch;

  time(&TimeT);
  Today = localtime(&TimeT);


  ret_val.typ = Int_Value;
  ret_val.nValue = 0;

  time(&TimeT);
  Today = localtime(&TimeT);


  if (arg_list[0].typ == Int_Value)
    Switch = arg_list[0].nValue;
  else
    Switch = Today->tm_mon+1;

    switch(arg_list[0].nValue)
    {
      case 1:
        ret_val.nValue = 31;
        break;
      case 2:
        if (arg_list[1].typ == Int_Value)
          Year = arg_list[1].nValue;
        else
          Year = Today->tm_year+1900;
        if (((Year % 4) == 0) && (((Year % 100) != 0) || ((Year % 400) == 0)))
          ret_val.nValue = 29;
        else
          ret_val.nValue = 28;
        break;
      case 3:
        ret_val.nValue = 31;
        break;
      case 4:
        ret_val.nValue = 30;
        break;
      case 5:
        ret_val.nValue = 31;
        break;
      case 6:
        ret_val.nValue = 30;
        break;
      case 7:
        ret_val.nValue = 31;
        break;
      case 8:
        ret_val.nValue = 31;
        break;
      case 9:
        ret_val.nValue = 30;
        break;
      case 10:
        ret_val.nValue = 31;
        break;
      case 11:
        ret_val.nValue = 30;
        break;
      case 12:
        ret_val.nValue = 31;
        break;
    }

  return ret_val; 
}
/*--------------------------------------------------------------------------------*/
t_Value base_daysofyear(t_Value *arg_list)
{
t_Value ret_val;
time_t TimeT;
tm* Today;
int Year;
int February;

  time(&TimeT);
  Today = localtime(&TimeT);


  ret_val.typ = Int_Value;
  ret_val.nValue = 0;

  if (arg_list[0].typ == Int_Value)
    Year = arg_list[0].nValue;
  else
    {
      time(&TimeT);
      Today = localtime(&TimeT);
      Year = Today->tm_year+1900;
    }
 
  if (((Year % 4) == 0) && (((Year % 100) != 0) || ((Year % 400) == 0)))
    February = 29;
  else
    February = 28;

  ret_val.nValue = 31 + February + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31;

  return ret_val; 
}
/*--------------------------------------------------------------------------------*/
t_Value base_commit(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Not_Set;

  EngineDB->Commit();

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_enginepath(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
char Buffer2[256];
int i = 0;


  strcpy(Buffer2,ENGINEPATH);
  while(Buffer2[i]) 
    {
      Buffer2[i]=tolower(Buffer2[i]);
      i++;
    }

  sprintf(Buffer,"%s/%s",ENGINEPATH,Buffer2);

  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
  strcpy(ret_val.sValue,Buffer2);
  ret_val.typ = String_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_topassword(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[50];

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if (strcmp(arg_list[0].sValue,"") != 0)
        strcpy(Buffer,crypt(arg_list[0].sValue,"VS"));
      else
        strcpy(Buffer,"");
    
      ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
      strcpy(ret_val.sValue,Buffer);
      ret_val.typ = String_Value;
    }
  else
    RuntimeError1("%%%%topassword[PARAMETER1] - PARAMETER1 must be STRING");

  return ret_val; 
}
/*--------------------------------------------------------------------------------*/
t_Value base_strrepeat(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[501];
int Counter;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == Int_Value)
        {
          strcpy(Buffer,"");
          for(Counter = 0; Counter < arg_list[1].nValue;Counter++)
            {
              strcat(Buffer,arg_list[0].sValue);
            }
    
          ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
          strcpy(ret_val.sValue,Buffer);
          ret_val.typ = String_Value;

        }
      else
        RuntimeError1("%%%%strrepeat[PARAM1,PARAM2] - PARAMETER2 must be INTEGER");
    }
  else
    RuntimeError1("%%%%strrepeat[PARAM1,PARAM2] - PARAMETER1 must be STRING");

  return ret_val; 
}
/*--------------------------------------------------------------------------------*/
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
t_Value base_exporttable(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          if (arg_list[2].typ == Int_Value)
            {
              if (arg_list[3].typ == String_Value)
                return DBExportTable(arg_list[0].sValue,arg_list[1].sValue,arg_list[2].nValue,arg_list[3].sValue);
              else
                return DBExportTable(arg_list[0].sValue,arg_list[1].sValue,arg_list[2].nValue,";");
            }
          else
            return DBExportTable(arg_list[0].sValue,arg_list[1].sValue,0,";");
        }
      else
        RuntimeError1("%%%%exporttable[PARAM1,PARAM2,...] - PARAMETER2 must be STRING");
    }
  else
    RuntimeError1("%%%%exporttable[PARAM1,PARAM2,...] - PARAMETER1 must be STRING");

  return ret_val;
}
#endif
/*--------------------------------------------------------------------------------*/
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
t_Value base_exportdb(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == Int_Value)
        return DBExportDB(arg_list[0].sValue,arg_list[1].nValue);
      else
        return DBExportDB(arg_list[0].sValue,0);
    }
  else
    RuntimeError1("%%%%exportdb[PARAM1] - PARAMETER1 must be STRING");

  return ret_val;
}
#endif
/*--------------------------------------------------------------------------------*/
#ifdef __cplusplus 
}
#endif




