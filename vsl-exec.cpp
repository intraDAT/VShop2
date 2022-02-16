#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include "m_code.h"
#include "vsl.h"
#include <malloc.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "error.h"


#ifndef DB_ORACLE
  #include "adabas.h"
#else
  #include "oracle.h"
#endif


//#include "adabas.h"
#include "julian.h"
#include "vslanguage.h"

#ifdef HPUX
  #include "dlfnc_hpux.h"
  #include <sys/uio.h>
  #include <unistd.h>
  #include <sys/wait.h>
#elif WIN32
  #include "dlfcn_win32.h"
  #include <io.h>
#else
  #include <unistd.h>
  #include <dlfcn.h>
  #include <sys/wait.h>
#endif

#include "db.h"
#include "hfct.h"
#include "cgic.h"
#include "vslanguage.h"

long Magic;

#ifdef __cplusplus 
  extern "C" {
#endif

void *dlLibrary = NULL;

void *dlLibraryEx = NULL;

char *seg_text;
unsigned char *seg_exec;
long *label_tbl;

long SP = 0;
long PC=0;
long accu = 0;
long _accu=0;
long seg_text_ptr, label;
long last_for_value;

#define MAXSP 1000

t_Value stack[MAXSP];

/*--------------------------------------------------------------------------------*/
void ShowType(char* Text1,Id_type Typ1,char* Text2,Id_type Typ2, char* Text3)
{
  fprintf(cgiOut,Text1);
  switch(Typ1)
    {
      case Bool_Value:
        fprintf(cgiOut,"BOOL");
        break;
      case Int_Value:
        fprintf(cgiOut,"INTEGER");
        break;
      case Float_Value:
        fprintf(cgiOut,"FLOAT");
        break;
      case String_Value:
        fprintf(cgiOut,"STRING");
        break;
      case Time_Value:
        fprintf(cgiOut,"TIME");
        break;
      case Date_Value:
        fprintf(cgiOut,"DATE");
        break;
      case Timestamp_Value:
        fprintf(cgiOut,"TIMESTAMP");
        break;
      case Not_Set:
        fprintf(cgiOut,"NOTSET");
        break;
      case Unknown:
        fprintf(cgiOut,"UNKNOWN");
    }
  fprintf(cgiOut,Text2);
  switch(Typ2)
    {
      case Bool_Value:
        fprintf(cgiOut,"BOOL");
        break;
      case Int_Value:
        fprintf(cgiOut,"INTEGER");
        break;
      case Float_Value:
        fprintf(cgiOut,"FLOAT");
        break;
      case String_Value:
        fprintf(cgiOut,"STRING");
        break;
      case Time_Value:
        fprintf(cgiOut,"TIME");
        break;
      case Date_Value:
        fprintf(cgiOut,"DATE");
        break;
      case Timestamp_Value:
        fprintf(cgiOut,"TIMESTAMP");
        break;
      case Not_Set:
        fprintf(cgiOut,"NOTSET");
        break;
      case Unknown:
        fprintf(cgiOut,"UNKNOWN");
    }
  fprintf(cgiOut,Text3);
}
/*--------------------------------------------------------------------------------*/
void parse_call()
{
  /* Call of an external function */
  t_Value proc_stack[30];
  t_Value (*func)(t_Value *arg_list);
  void (*func2)(CBaseEngineDB *EngineDB);
  
  char proc_name[256];
  int cnt=0;
  int i=0;
  long args;

  char Buffer[256];
  strcpy(Buffer,PROGRAMNAME);
  i=0;
  while(Buffer[i]) 
    {
      Buffer[i]=tolower(Buffer[i]);
      i++;
    }

  if (!dlLibrary)
    {
      #ifdef HPUX
        dlLibrary = dlopen(DL_LIBRARY_NAME, RTLD_NOW);
      #elif WIN32
        dlLibrary = dlopen(Buffer, RTLD_NOW | RTLD_GLOBAL);
      #else 
        dlLibrary = dlopen(DL_LIBRARY_NAME, RTLD_NOW | RTLD_GLOBAL);
      #endif
    }
  strcpy(proc_name, stack[SP].sValue);

  free(stack[SP].sValue);
  SP--;
  args = stack[SP].nValue;
  while(args) 
    {
      proc_stack[cnt]=stack[SP-args];
      /* if (proc_stack[cnt].typ == String_Value)
      if (proc_stack[cnt].sValue == NULL) 
        {
	        proc_stack[cnt].sValue = malloc(strlen(seg_text + proc_stack[cnt].cValue)+1);
	        strcpy(proc_stack[cnt].sValue, seg_text + proc_stack[cnt].cValue);
        } 
      */
      args--;
      cnt++;
    }
  proc_stack[cnt].typ = Not_Set;
  SP -= stack[SP].nValue;
  i=0;
  while(proc_name[i]) 
    {
      proc_name[i]=tolower(proc_name[i]);
      i++;
    }

  //printf("<B>%s</B><BR>",proc_name);

  if (dlLibrary) 
    {
      #ifdef WIN32
        func = (t_Value (*)(t_Value *arg_list)) dlsym((HMODULE) dlLibrary, proc_name);
      #elif HPUX 
        func = (t_Value (*)(t_Value *arg_list)) dlsym(dlLibrary, proc_name);
      #elif SUN
        func = (t_Value (*)(t_Value *arg_list)) dlsym(dlLibrary, proc_name);
      #else 
        func = (t_Value (*)(t_Value *arg_list)) dlsym(dlLibrary, proc_name);
      #endif

      if (func) 
        stack[SP] = (*func)(proc_stack);
      else 
        {
          if (!dlLibraryEx)
            {
              strcat(Buffer,".vsm");

              #ifdef HPUX
                dlLibraryEx = dlopen(Buffer, RTLD_NOW);
              #else 
                if ((dlLibraryEx = dlopen(Buffer, RTLD_NOW | RTLD_GLOBAL)) != NULL)
                  {
                    #ifdef WIN32
                      func2 = (void (*)(CBaseEngineDB *)) dlsym((HMODULE) dlLibrary, "initadabas__FP13CBaseEngineDB");
                      if (func2 != NULL)
                    #elif SUN
                       if ((func2 = (void (*)(CBaseEngineDB *)) dlsym(dlLibraryEx, "initadabas__FP13CBaseEngineDB")) != NULL)
                    #else
                      if ((func2 = (void (*)(CBaseEngineDB *)) dlsym(dlLibraryEx, "initadabas__FP13CBaseEngineDB")) != NULL)
                    #endif
                      {
                        (*func2)(EngineDB);
                      }
                    else
                      fprintf(cgiOut,"<B>Function 'initadabas' not found</B><BR>");
                  }
              #endif
            }
          if (dlLibraryEx) 
            {
              #ifdef DEBUG
                DebugToFile2("Calling external function",proc_name);
              #endif

              #ifdef WIN32
                func = (t_Value (*)(t_Value *arg_list)) dlsym((HMODULE)dlLibraryEx, proc_name);
              #elif HPUX 
                func = (t_Value (*)(t_Value *arg_list)) dlsym(dlLibraryEx, proc_name); 
              #elif SUN
                func = (t_Value (*)(t_Value *arg_list)) dlsym(dlLibraryEx, proc_name);
              #else
                func = (t_Value (*)(t_Value *arg_list)) dlsym(dlLibraryEx, proc_name);
              #endif
          
              if (func) 
                stack[SP] = (*func)(proc_stack);
              else 
                {
                  fprintf(cgiOut,"<B>No Function : %s found</B><BR>",proc_name);
                  stack[SP].typ = Not_Set;
                }
            }
          else
            {
              stack[SP].typ = Not_Set;
              #ifdef WIN32
                fprintf(cgiOut,"<B>VShopExtensionModule : (%s)</B><BR>",proc_name);
              #else
                fprintf(cgiOut,"<B>VShopExtensionModule : %s - (%s) </B><BR>",proc_name,dlerror());
              #endif
            }
        }
    } 
  else 
    {
      stack[SP].nValue = -1;
      stack[SP].typ = Int_Value;
    }

  for(i=0;i<cnt;i++) 
    {
      if (proc_stack[i].typ == String_Value && proc_stack[i].sValue) 
        {
          free(proc_stack[i].sValue);
          proc_stack[i].sValue = NULL;
        }
    } 

}
/*--------------------------------------------------------------------------------*/
void parse_opcode(unsigned char op)
{
float* f;
float f2;
struct tm* Today;
char Buffer[51];

  /* one Byte, one command to execute */
  unsigned char mod;
  mod = op & 15;
  switch (op>>4) {
  case CALL: 
    parse_call();
    PC++;
    break;
  case PFX:
    _accu = (_accu << 4) | mod;
    PC++;
    break;
  case LDC:
    accu = (_accu << 4) | mod;
    _accu = 0;
    PC++;
    break;
  case PUSH:
    if (SP >= MAXSP)
      {
        RuntimeError1("Internal error : Sack overflow");
        if (dlLibraryEx)
          dlclose(dlLibraryEx);
        return;
      }
    SP++;
    #ifdef WIN32
      stack[SP].typ = (Id_type) mod;
    #elif HPUX
      stack[SP].typ = (Id_type) mod;
    #else 
      stack[SP].typ = (enum Id_type) mod;
    #endif
    switch (mod) {
    case Bool_Value:
    case Int_Value: /* int value */
      stack[SP].nValue = accu;
      break;
    case String_Value: /* string offset */
      stack[SP].sValue = (char*) malloc(strlen(seg_text + accu)+1);
      strcpy(stack[SP].sValue, seg_text+accu);
      break;
    case Float_Value: /* float value */
 
      f = &f2;
      *(long*) f=accu;
      stack[SP].dValue = (double) *f;
      break; 
    }
    PC++;          
    break;                  

  case PRINT:
/*MEDICOMP*/
    
      {
        switch (stack[SP].typ) 
          {
            case Int_Value:
              if (stack[SP].Indicator == -1)
                fprintf(cgiOut,"%s","?");
              else
                fprintf(cgiOut,"%ld", stack[SP].nValue);
              break;

            case String_Value:
/*
            if (stack[SP].Indicator == -1)
              fprintf(cgiOut,"?");
*/
              fprintf(cgiOut,"%s", stack[SP].sValue);

              free(stack[SP].sValue);
              stack[SP].sValue = NULL;
              break;


            case Float_Value:
              {
                char Buffer[30];

                if (stack[SP].Indicator == -1)
                  fprintf(cgiOut,"%s","?");
                else
                  {
                    sprintf(Buffer,"%.2f", stack[SP].dValue);
                    Replace(Buffer,".",DECIMALSYMBOL_STRING);
                    fprintf(cgiOut,"%s", Buffer);
                  }
                break;
              }

            case Time_Value:
              if (stack[SP].Indicator == -1)
                fprintf(cgiOut,"%s","?");
              else
                {
                  Today = localtime(&(stack[SP].tValue));
                  if (strftime(Buffer,50,TIMEFORMAT,Today) > 0)
                    fprintf(cgiOut,Buffer);
                  else
                    {
                      /* Runtime Error */
                    }
                }
              break;
            case Date_Value:
              if (stack[SP].Indicator == -1)
                fprintf(cgiOut,"%s","?");
              else
                {
                  Today = localtime(&(stack[SP].tValue));
                  if (strftime(Buffer,50,DATEFORMAT,Today) > 0)
                    fprintf(cgiOut,Buffer);
                  else
                    {
                      /* Runtime Error */
                    }
                }
              break;
            case Timestamp_Value:
              if (stack[SP].Indicator == -1)
                fprintf(cgiOut,"%s","?");
              else
                {
                  Today = localtime(&(stack[SP].tValue));
                  if (strftime(Buffer,50,TIMESTAMPFORMAT,Today) > 0)
                    fprintf(cgiOut,Buffer);
                  else
                    {
                      /* Runtime Error */
                    }
                }
              break;
            default:
              break;
          }
      }
    SP--;
    PC++;
    break;
  case FUNC:
    switch (mod) 
      {
        /*-----------------------------------------------------------------*/
        case OP_LT:
          switch(stack[SP-1].typ)
            {
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].nValue < stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].nValue < stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  {
                    Replace(stack[SP].sValue, DECIMALSYMBOL_STRING, ".");
                    stack[SP-1].nValue = stack[SP-1].nValue < atof(stack[SP].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    ShowType("<BR><B>NOT IMPLEMENTED LOWER ",stack[SP-1].typ,"<",stack[SP].typ,"</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Float_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].dValue < stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].dValue < stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  {
                    Replace(stack[SP].sValue, DECIMALSYMBOL_STRING, ".");
                    stack[SP-1].nValue = stack[SP-1].dValue < atof(stack[SP].sValue);
                  }
                else
                  {
                    ShowType("<BR><B>NOT IMPLEMENTED LOWER ",stack[SP-1].typ,"<",stack[SP].typ,"</B><BR>");
                    /* Runtime Error */
                    stack[SP-1].nValue = 0;
                  }
                break;
              case String_Value:
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = (strcmp(stack[SP-1].sValue,stack[SP].sValue) < 0);
                else
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    Replace(stack[SP-1].sValue, DECIMALSYMBOL_STRING, ".");
                    stack[SP-1].nValue = atof(stack[SP-1].sValue) < stack[SP].nValue;
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    Replace(stack[SP-1].sValue, DECIMALSYMBOL_STRING, ".");
                    stack[SP-1].nValue = atof(stack[SP-1].sValue) < stack[SP].dValue;
                  }
                else
                  {
                    /* Runtime Error */
                    ShowType("<BR><B>NOT IMPLEMENTED LOWER ",stack[SP-1].typ,"<",stack[SP].typ,"</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Date_Value:
                if (stack[SP].typ == Date_Value)
                  {
                    stack[SP-1].nValue = stack[SP-1].tValue < stack[SP].tValue;
                  }
                else
                  {
                    ShowType("<BR><B>NOT IMPLEMENTED LOWER ",stack[SP-1].typ,"<",stack[SP].typ,"</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              default:
                /* Runtime Error */
                ShowType("<BR><B>NOT IMPLEMENTED LOWER ",stack[SP-1].typ,"<",stack[SP].typ,"</B><BR>");
                stack[SP-1].nValue = 0;
                break;
            }
          stack[SP-1].typ = Bool_Value;
          SP--;
          break;
        /*-----------------------------------------------------------------*/
        case OP_LE    :
          switch(stack[SP-1].typ)
            {
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].nValue <= stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].nValue <= stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  {
                    Replace(stack[SP].sValue, DECIMALSYMBOL_STRING, ".");
                    stack[SP-1].nValue = stack[SP-1].nValue <= atof(stack[SP].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED LOWER/EQUAL</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Float_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].dValue <= stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].dValue <= stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = stack[SP-1].dValue <= atof(stack[SP].sValue);
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED LOWER/EQUAL</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case String_Value:
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = (strcmp(stack[SP-1].sValue,stack[SP].sValue) <= 0);
                else
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = atof(stack[SP-1].sValue) <= stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = atof(stack[SP-1].sValue) <= stack[SP].dValue;
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED LOWER/EQUAL</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              default:
                /* Runtime Error */
                fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED LOWER/EQUAL</B><BR>");
                stack[SP-1].nValue = 0;
                break;
            }
          stack[SP-1].typ = Bool_Value;
          SP--;
          break;
        /*-----------------------------------------------------------------*/
        case OP_EQ:
          switch(stack[SP-1].typ)
            {
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].nValue == stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].nValue == stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = stack[SP-1].nValue == atof(stack[SP].sValue);
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED EQUAL (INTEGER = ?)</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Float_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].dValue == stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].dValue == stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = stack[SP-1].dValue == atof(stack[SP].sValue);
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED EQUAL (FLOAT = ?)</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case String_Value:
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = (strcmp(stack[SP-1].sValue,stack[SP].sValue) == 0);
                else
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = atof(stack[SP-1].sValue) == stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = atof(stack[SP-1].sValue) == stack[SP].dValue;
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED EQUAL (STRING = ?)</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              default:
                ShowType("<BR><B>NOT IMPLEMENTED EQUAL ",stack[SP-1].typ,"=",stack[SP].typ,"</B><BR>");
                /* Runtime Error */
                stack[SP-1].nValue = 0;
                break;
            }
          stack[SP-1].typ = Bool_Value;
          SP--;
          break;
        /*-----------------------------------------------------------------*/
        case OP_GE:
          switch(stack[SP-1].typ)
            {
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].nValue >= stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].nValue >= stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = stack[SP-1].nValue >= atof(stack[SP].sValue);
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED GREATER/EQUAL</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Float_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].dValue >= stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].dValue >= stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = stack[SP-1].dValue >= atof(stack[SP].sValue);
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED GREATER/EQUAL</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case String_Value:
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = (strcmp(stack[SP-1].sValue,stack[SP].sValue) >= 0);
                else
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = atof(stack[SP-1].sValue) >= stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = atof(stack[SP-1].sValue) >= stack[SP].dValue;
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED GREATER/EQUAL</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              default:
                /* Runtime Error */
                fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED GREATER/EQUAL</B><BR>");
                stack[SP-1].nValue = 0;
                break;
            }
          stack[SP-1].typ = Bool_Value;
          SP--;
          break;
        /*-----------------------------------------------------------------*/
        case OP_GT:
          switch(stack[SP-1].typ)
            {
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].nValue > stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].nValue > stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = stack[SP-1].nValue > atof(stack[SP].sValue);
                else
                  {
                    /* Runtime Error */
                    ShowType("<BR><B>NOT IMPLEMENTED GREATER ",stack[SP-1].typ,">",stack[SP].typ,"</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Float_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].dValue > stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].dValue > stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = stack[SP-1].dValue > atof(stack[SP].sValue);
                else
                  {
                    /* Runtime Error */
                    ShowType("<BR><B>NOT IMPLEMENTED GREATER ",stack[SP-1].typ,">",stack[SP].typ,"</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case String_Value:
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = (strcmp(stack[SP-1].sValue,stack[SP].sValue) > 0);
                else
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = atof(stack[SP-1].sValue) > stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = atof(stack[SP-1].sValue) > stack[SP].dValue;
                else
                  {
                    /* Runtime Error */
                    ShowType("<BR><B>NOT IMPLEMENTED GREATER ",stack[SP-1].typ,">",stack[SP].typ,"</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Date_Value:
                if (stack[SP].typ == Date_Value)
                  {
                    stack[SP-1].nValue = stack[SP-1].tValue > stack[SP].tValue;
                    fprintf(cgiOut,"%ld - %ld",stack[SP-1].tValue,stack[SP].tValue);
                  }
                else
                  {
                    ShowType("<BR><B>NOT IMPLEMENTED GREATER",stack[SP-1].typ,">",stack[SP].typ,"</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              default:
                /* Runtime Error */
                ShowType("<BR><B>NOT IMPLEMENTED GREATER ",stack[SP-1].typ,">",stack[SP].typ,"</B><BR>");
                stack[SP-1].nValue = 0;
                break;
            }
          stack[SP-1].typ = Bool_Value;
          SP--;
          break;
        /*-----------------------------------------------------------------*/
        case OP_NE :
          switch(stack[SP-1].typ)
            {
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].nValue != stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].nValue != stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = stack[SP-1].nValue != atof(stack[SP].sValue);
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED NOT EQUAL</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Float_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].dValue != stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = stack[SP-1].dValue != stack[SP].dValue;
                else
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = stack[SP-1].dValue != atof(stack[SP].sValue);
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED NOT EQUAL</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              case String_Value:
                if (stack[SP].typ == String_Value)
                  stack[SP-1].nValue = (strcmp(stack[SP-1].sValue,stack[SP].sValue) != 0);
                else
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = atof(stack[SP-1].sValue) != stack[SP].nValue;
                else
                if (stack[SP].typ == Float_Value)
                  stack[SP-1].nValue = atof(stack[SP-1].sValue) != stack[SP].dValue;
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED NOT EQUAL</B><BR>");
                    stack[SP-1].nValue = 0;
                  }
                break;
              default:
                /* Runtime Error */
                fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED NOT EQUAL</B><BR>");
                stack[SP-1].nValue = 0;
                break;
            }
          stack[SP-1].typ = Bool_Value;
          SP--;
          break;
        /*-----------------------------------------------------------------*/
        case OP_PLUS  :
          switch(stack[SP-1].typ)
            {
              case Date_Value:
                if (stack[SP].typ == Int_Value)
                  {
                    stack[SP-1].tValue = JPlus(stack[SP-1].tValue,stack[SP].nValue);
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].tValue = JPlus(stack[SP-1].tValue,(long) stack[SP].dValue);
                  }
                else
                if (stack[SP].typ == String_Value)
                  {
                    stack[SP-1].tValue = JPlus(stack[SP-1].tValue,(long) atoi(stack[SP].sValue));
                  }
                
                break;
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Int_Value;
                    stack[SP-1].nValue += stack[SP].nValue;
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = stack[SP-1].nValue + stack[SP].dValue;
                  }
                else
                if (stack[SP].typ == String_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = stack[SP-1].nValue + atof(stack[SP].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED PLUS</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Float_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue += stack[SP].nValue;
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue += stack[SP].dValue;
                  }
                else
                if (stack[SP].typ == String_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue += atof(stack[SP].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED PLUS</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              case String_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = atol(stack[SP-1].sValue) + stack[SP].nValue;
                    free(stack[SP-1].sValue);
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue += atof(stack[SP].sValue);
                    free(stack[SP-1].sValue);
                  }
                else
                if (stack[SP].typ == String_Value)
                  {
                    char Buffer[5000];
                    strcpy(Buffer,stack[SP-1].sValue);
                    strcat(Buffer,stack[SP].sValue);

                    free(stack[SP-1].sValue);
                    stack[SP-1].sValue = (char*) malloc(strlen(Buffer)+1);
                    strcpy(stack[SP-1].sValue,Buffer);
                  }
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED PLUS</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              default:
                /* Runtime Error */
                fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED PLUS</B><BR>");
                stack[SP-1].nValue = 0;
                break;
            }
          SP--;		 
          break;		 
        /*-----------------------------------------------------------------*/
        case OP_MINUS :	 
          switch(stack[SP-1].typ)
            {
              case Date_Value:
                if (stack[SP].typ == Int_Value)
                  {
                    stack[SP-1].tValue = JMinus(stack[SP-1].tValue,stack[SP].nValue);
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].tValue = JMinus(stack[SP-1].tValue,(long) stack[SP].dValue);
                  }
                else
                if (stack[SP].typ == String_Value)
                  {
                    stack[SP-1].tValue = JMinus(stack[SP-1].tValue,(long) atoi(stack[SP].sValue));
                  }
                
                break;
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Int_Value;
                    stack[SP-1].nValue -= stack[SP].nValue;
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = stack[SP-1].nValue - stack[SP].dValue;
                  }
                else
                if (stack[SP].typ == String_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = stack[SP-1].nValue - atof(stack[SP].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED MINUS</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Float_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue -= stack[SP].nValue;
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue -= stack[SP].dValue;
                  }
                else
                if (stack[SP].typ == String_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue -= atof(stack[SP].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED MINUS</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              case String_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = atol(stack[SP-1].sValue) - stack[SP].nValue;
                    free(stack[SP-1].sValue);
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue -= atof(stack[SP].sValue);
                    free(stack[SP-1].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED MINUS</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              default:
                /* Runtime Error */
                fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED MINUS</B><BR>");
                stack[SP-1].nValue = 0;
                break;
            }
          SP--;		 
          break;		 
        /*-----------------------------------------------------------------*/
        case OP_TIMES:	
          switch(stack[SP-1].typ)
            {
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Int_Value;
                    stack[SP-1].nValue *= stack[SP].nValue;
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = stack[SP-1].nValue * stack[SP].dValue;
                  }
                else
                if (stack[SP].typ == String_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = stack[SP-1].nValue * atof(stack[SP].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    ShowType("<BR><B>NOT IMPLEMENTED MUL ",stack[SP-1].typ,"*",stack[SP].typ,"</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Float_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue *= stack[SP].nValue;
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue *= stack[SP].dValue;
                  }
                else
                if (stack[SP].typ == String_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue *= atof(stack[SP].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    ShowType("<BR><B>NOT IMPLEMENTED MUL ",stack[SP-1].typ,"*",stack[SP].typ,"</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              case String_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = atol(stack[SP-1].sValue) * stack[SP].nValue;
                    free(stack[SP-1].sValue);
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue *= atof(stack[SP].sValue);
                    free(stack[SP-1].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    ShowType("<BR><B>NOT IMPLEMENTED MUL ",stack[SP-1].typ,"*",stack[SP].typ,"</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              default:
                /* Runtime Error */
                ShowType("<BR><B>NOT IMPLEMENTED MUL ",stack[SP-1].typ,"*",stack[SP].typ,"</B><BR>");
                stack[SP-1].nValue = 0;
                break;
            }
          SP--;		 
          break;		 
        /*-----------------------------------------------------------------*/
        case OP_DIV:	 
          switch(stack[SP-1].typ)
            {
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Int_Value;
                    stack[SP-1].nValue /= stack[SP].nValue;
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = stack[SP-1].nValue / stack[SP].dValue;
                  }
                else
                if (stack[SP].typ == String_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = stack[SP-1].nValue / atof(stack[SP].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED DIV</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              case Float_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue /= stack[SP].nValue;
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue /= stack[SP].dValue;
                  }
                else
                if (stack[SP].typ == String_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue /= atof(stack[SP].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED DIV</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              case String_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue = atol(stack[SP-1].sValue) / stack[SP].nValue;
                    free(stack[SP-1].sValue);
                  }
                else
                if (stack[SP].typ == Float_Value)
                  {
                    stack[SP-1].typ = Float_Value;
                    stack[SP-1].dValue /= atof(stack[SP].sValue);
                    free(stack[SP-1].sValue);
                  }
                else
                  {
                    /* Runtime Error */
                    fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED DIV</B><BR>");
                    stack[SP-1].typ = Bool_Value;
                    stack[SP-1].nValue = 0;
                  }
                break;
              default:
                /* Runtime Error */
                fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED DIV</B><BR>");
                stack[SP-1].nValue = 0;
                break;
            }
          SP--;		 
          break;		 
        /*-----------------------------------------------------------------*/
        case OP_MOD:	 
          if ((stack[SP-1].typ == Int_Value) && (stack[SP].typ == Int_Value))
            {
              stack[SP-1].nValue %= stack[SP].nValue;
            }
          else
            {
              /* Runtime Error */
              fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED MOD</B><BR>");
              stack[SP-1].typ = Int_Value;
              stack[SP-1].nValue = 0;
            }
          SP--;		 
          break;		 
        /*-----------------------------------------------------------------*/
        case OP_NEG:
          if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
            stack[SP].nValue =  -stack[SP].nValue;
          else
          if (stack[SP].typ == Float_Value)
            stack[SP].dValue =  -stack[SP].dValue;
          else
            {
              fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED NEG</B><BR>");
              /* Runtime Error */
            }
          break;
        /*-----------------------------------------------------------------*/
        case OP_AND:
          switch(stack[SP-1].typ)
            {
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].nValue && stack[SP].nValue;
                break;
              default:
                fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED AND</B><BR>");
                /* Runtime Error */
                break;
            }
          SP--;
          break;
        /*-----------------------------------------------------------------*/
        case OP_OR:
          switch(stack[SP-1].typ)
            {
              case Int_Value:
              case Bool_Value:
                if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
                  stack[SP-1].nValue = stack[SP-1].nValue || stack[SP].nValue;
                break;
              default:
                fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED OR</B><BR>");
                /* Runtime Error */
                break;
            }
          SP--;
          break;
        /*-----------------------------------------------------------------*/
        case OP_NOT :
          if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
            stack[SP].nValue = !stack[SP].nValue;
          else
            {
              fprintf(cgiOut,"<BR><B>NOT IMPLEMENTED NOT</B><BR>");
              /* Runtime Error */
            }
          break;
      }
    PC++;
    break;
  /*-----------------------------------------------------------------*/
  case JMP:
    switch (mod) 
      {
        case JMP_UNC:
          PC=label_tbl[accu];
          break;
        case JMP_Z:
          if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
            {
              if (!stack[SP].nValue) 
	              PC=label_tbl[accu];
              else
	              PC++;
            }
          else
          if (stack[SP].typ == String_Value)
            {
              if (strlen(stack[SP].sValue) == 0) 
	              PC=label_tbl[accu];
              else
	              PC++;
            }
          SP--;
          break;
        case JMP_NZ:
          if ((stack[SP].typ == Int_Value) || (stack[SP].typ == Bool_Value))
            {
              if (stack[SP].nValue) 
	              PC=label_tbl[accu];
              else
	              PC++;
            }
          else
          if (stack[SP].typ == String_Value)
            {
              if (strlen(stack[SP].sValue) > 0) 
	              PC=label_tbl[accu];
              else
	              PC++;
            }
          SP--;
          break;
      }
    break;

  case SPECIAL:

    switch (mod) 
      {
        case PRINT_STACK:
          {
            int Count;
            printf("StackPointer : %ld<BR>",SP);
            for(Count = 0; Count <= SP; Count++)
              {
                switch(stack[Count].typ)
                  {
                    case Bool_Value:
                      printf("%i BOOLEAN %ld<BR>",Count,stack[Count].nValue);
                      break;
                    case Int_Value:
                      printf("%i INTEGER %ld<BR>",Count,stack[Count].nValue);
                      break;
                    case Float_Value:
                      printf("%i FLOAT %f<BR>",Count,stack[Count].dValue);
                      break;
                    case String_Value:
                      printf("%i FLOAT %s<BR>",Count,stack[Count].sValue);
                      break;
                    case Time_Value:
                      printf("%i TIME %ld<BR>",Count,stack[Count].tValue);
                      break;
                    case Date_Value:
                      printf("%i DATE %ld<BR>",Count,stack[Count].tValue);
                      break;
                    case Timestamp_Value:
                      printf("%i TIMESTAMP %ld<BR>",Count,stack[Count].tValue);
                      break;
                    case Not_Set:
                      printf("%i NOTSET <BR>",Count);
                      break;
                    case Unknown:
                      printf("%i UNKNOWN <BR>",Count);
                      break;
                  }
              }
            PC++;
            break;
          }
        case FOR_TEST:
          SP++;
          last_for_value = stack[SP-1].nValue;

          if (stack[SP-2].nValue > 0) 
	          stack[SP].nValue = (stack[SP-1].nValue <= stack[SP-3].nValue);
          else 
	          stack[SP].nValue = (stack[SP-1].nValue >= stack[SP-3].nValue);
          stack[SP].typ = Int_Value;
          PC++;
          break;
        case FOR_INIT: 
          {
            long start, end, step;
            start=stack[SP-2].nValue;
            end  =stack[SP-1].nValue;
            step =stack[SP].nValue;
            stack[SP].nValue   = start;
            stack[SP-1].nValue = step;
            stack[SP-2].nValue = end;
            PC++;
          }	
          break;
        case FOR_LOOP:
          stack[SP].nValue += stack[SP-1].nValue;
          PC++;
          break;
        case FOR_VAL:
          /* TODO: stack for last_for_value */
          SP++;
          stack[SP].nValue = last_for_value;
          stack[SP].typ = Int_Value;
          stack[SP].Indicator = 0;
          PC++;
          break;
        case PRODUCT_INIT: 
          {
            long end;

            if (DBListSelect(stack[SP-2].sValue,stack[SP-1].nValue) == 0)
              {
                stack[SP].nValue   = 2;             
                stack[SP-1].nValue = 1;
                stack[SP-2].nValue = 1;
              }
            else
              {
                end  =stack[SP-1].nValue;
                stack[SP].nValue   = 1;
                stack[SP-1].nValue = 1;
                stack[SP-2].nValue = end;
              }
            PC++;
          }	
          break;
        case PRODUCT_INIT2: 
          {
            long end;

            if (DBListSelect("",stack[SP-1].nValue) == 0)
              {
                stack[SP].nValue   = 2;             
                stack[SP-1].nValue = 1;
                stack[SP-2].nValue = 1;
              }
            else
              {
                end  =stack[SP-1].nValue;
                stack[SP].nValue   = 1;
                stack[SP-1].nValue = 1;
                stack[SP-2].nValue = end;
              }
            PC++;
          }	
          break;
        case PRODUCT_LOOP:
          {
            if (DBListNext() == 0)
              {
                stack[SP].nValue += 10000;
              }
            else
              stack[SP].nValue += stack[SP-1].nValue;
            PC++;
          }
          break;
        case CATALOG_INIT: 
          {
            long end;

            if (stack[SP-2].typ == Int_Value)
              end = stack[SP-2].nValue;
            else
            if (stack[SP-1].typ == Int_Value)
              end = stack[SP-1].nValue;
            else
              end = 1000;

            if (DBCatalogSelect(stack[SP-2],stack[SP-1]) == 0)
              {
                stack[SP].nValue   = 2;
                stack[SP-1].nValue = 1;
                stack[SP-2].nValue = 1;
              }
            else
              {
                stack[SP].nValue   = 1;
                stack[SP-1].nValue = 1;
                stack[SP-2].nValue = end;
              }
            PC++;
          }	
          break;
        case CATALOG_LOOP:
          {
            if (DBCatalogNext() == 0)
              {
                stack[SP].nValue += 10000;
              }
            else
              stack[SP].nValue += stack[SP-1].nValue;
            PC++;
          }
          break;
        case BASKET_INIT: 
          {
            long end;

            end = stack[SP-2].nValue;
            if (DBBasketSelect(stack[SP-2]) == 0)
              {
                stack[SP].nValue   = 2;
                stack[SP-1].nValue = 1;
                stack[SP-2].nValue = 1;
              }
            else
              {
                stack[SP].nValue   = 1;
                stack[SP-1].nValue = 1;
                stack[SP-2].nValue = end;
              }
            PC++;
          }	
          break;
        case BASKET_LOOP:
          {
            char VSBS[20];
            if (DBBasketNext() == 0)
              {
                sprintf(VSBS,"%ld",stack[SP].nValue);
                InsertEntry("VSBS",VSBS);

                stack[SP].nValue += 10000;
              }
            else
              stack[SP].nValue += stack[SP-1].nValue;
            PC++;
          }
          break;

      case PROMOTION_INIT: 
        {
          long end;

          end = stack[SP-2].nValue;

          if (DBPromotionSelect(stack[SP-1]) == 0)
            {
              stack[SP].nValue   = 2;
              stack[SP-1].nValue = 1;
              stack[SP-2].nValue = 1;
            }
          else
            {
              stack[SP].nValue   = 1;
              stack[SP-1].nValue = 1;
              stack[SP-2].nValue = end;
            }
         
          PC++;
        }	
        break;
      case PROMOTION_LOOP:
        {
          if (DBPromotionNext() == 0)
            {
              stack[SP].nValue += 10000;
            }
          else
            stack[SP].nValue += stack[SP-1].nValue;
          PC++;
        }
        break;

      case RELEVANT_INIT: 
        {
          long end;

          end = stack[SP-2].nValue;

          
          if (DBRelevantSelect() == 0)
            {
              stack[SP].nValue   = 2;
              stack[SP-1].nValue = 1;
              stack[SP-2].nValue = 1;
            }
          else
            {
              stack[SP].nValue   = 1;
              stack[SP-1].nValue = 1;
              stack[SP-2].nValue = end;
            }
         
          PC++;
        }	
        break;
      case RELEVANT_LOOP:
        {
          if (DBRelevantNext() == 0)
            {
              stack[SP].nValue += 10000;
            }
          else
            stack[SP].nValue += stack[SP-1].nValue;
          PC++;
        }
        break;
      }
    break;
  case POP:
    if (stack[SP].typ == String_Value && stack[SP].sValue) 
      {
        free(stack[SP].sValue);
        stack[SP].sValue = NULL;
      }
    SP--;
    PC++;
    break;
  case EXIT:
#ifdef DEBUG
    fprintf(cgiOut, "Program ended\n");
#endif
    PC=-1;
    break;
  default:
#ifdef DEBUG
    fprintf(cgiOut, "Unresolved %c %ld %x\n", mod, PC, seg_exec[PC]);
    exit(0);
#endif
    PC++;
  }
}
/*--------------------------------------------------------------------------------*/
void run_program()
{
  PC = 0;
  while(PC>=0) {
    parse_opcode(seg_exec[PC]);
  }
}
/*--------------------------------------------------------------------------------*/
void ShowError(char* Filename)
{
char Buffer[500];
FILE* Pipe;
int Counter = 0;

  #ifdef WIN32
    sprintf(Buffer,"vsl-compile %s DEBUG",Filename); 
    Pipe = _popen(Buffer,"rt");
  #else
      sprintf(Buffer,"%s/vsl-compile %s DEBUG",PROGRAMPATH,Filename); 
    Pipe = popen(Buffer,"r");
  #endif

  if (Pipe != NULL)
    {
      fprintf(cgiOut,"<HTML>");
      fprintf(cgiOut,"<BODY>");
      fprintf(cgiOut,"<B>Fehler beim compilieren der Datei %s :</B><BR>",Filename);
      fprintf(cgiOut,"<HR>");
      
      while(!feof(Pipe))
        {
          Counter ++;
          if(fgets( Buffer, 499, Pipe) != NULL )
            fprintf(cgiOut, "<B>%i :</b>%s",Counter,Buffer );
        }

      fprintf(cgiOut,"</BODY>");
      fprintf(cgiOut,"</HTML>");
      #ifdef WIN32
        _pclose(Pipe);
      #else
        pclose(Pipe);
      #endif
    }
}
/*--------------------------------------------------------------------------------*/
int VSLMain(char* Filename, int Flag)
{
char CompilerName[300];
p_exec_head p_header;
int fd;
#ifdef DEBUG
  int i;
#endif
char *out_file_name;
char* Ptr;
char Buffer[501];
struct stat source_file_stat, dest_file_stat;
#ifndef WIN32
  int status;
#endif



  out_file_name = (char*) malloc(strlen(Filename)+3);
  strcpy(out_file_name, Filename);


  if ((strstr(Filename,".mtl") != NULL) || (strstr(Filename,".MTL") != NULL))
    {
      #ifdef WIN32
        strcpy(CompilerName,"mtl-compile.exe");
      #else
        sprintf(CompilerName,"%s/mtl-compile",PROGRAMPATH);
      #endif
    }
  else
    {
      #ifdef WIN32
        strcpy(CompilerName,"vsl-compile.exe");
      #else
        sprintf(CompilerName,"%s/vsl-compile",PROGRAMPATH);
      #endif
    }

  strcat(out_file_name, ".p");
  stat(Filename, &source_file_stat);



  if ((stat(out_file_name, &dest_file_stat) != 0) || (source_file_stat.st_mtime > dest_file_stat.st_mtime)) 
    {
      #ifndef WIN32
        if (!fork()) 
          {
            char *arg_list[]={CompilerName, Filename, NULL};
            execv(arg_list[0], arg_list);
          }
        
        wait(&status);

        if ((WEXITSTATUS(status) != 0))
          {
            cgiHeaderContentType("text/html");
/*
            if (cgiFormString("DEBUG",Buffer,20) != cgiFormNotFound)
              {
                ShowError(Filename);
              }
            else
              {
                if ((Ptr = strrchr(Filename,'/')) != NULL)
                  strcpy(Buffer,Ptr+1);
                else
                  strcpy(Buffer,Filename);

                ShowError(Buffer);
              }
            exit(0);
*/
               ShowError(Filename);
               return 0;          
          }
        #else
          char CommandLine[501];
          DWORD ExitCode;
          unsigned int Count;

          for (Count = 0; Count < strlen(Filename); Count++)
            if(Filename[Count] == '/')
              Filename[Count] = '\\';

            sprintf(CommandLine,"%s %s",CompilerName,Filename);
            STARTUPINFO StartupInfo = {0};
            StartupInfo.cb = sizeof(StartupInfo);
            PROCESS_INFORMATION ProcessInfo;
		        StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
		        StartupInfo.wShowWindow = SW_HIDE;

            ExitCode = 0;

		        if (::CreateProcess(NULL,CommandLine,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&StartupInfo,&ProcessInfo))
  		        {
  			        ::WaitForSingleObject(ProcessInfo.hProcess,INFINITE);
	  	          ::GetExitCodeProcess(ProcessInfo.hProcess,&ExitCode);
  			      }
            if (ExitCode != 0)
              {
                cgiHeaderContentType("text/html");
                ShowError(Filename);
                exit(0);
               }

          #endif
    }
  #ifndef WIN32
    if ((fd = open(out_file_name, O_RDONLY)) == -1)
  #else
    if ((fd = _open(out_file_name, O_RDONLY | _O_BINARY )) == -1)
  #endif 
      {
        cgiHeaderContentType("text/html");
        sprintf(Buffer,TEXT_ERROR10,out_file_name);
        HTMLError(Buffer);
        return 0;
      }

  #ifndef WIN32
    read(fd, &p_header, sizeof(p_header));
  #else
    _read(fd, &p_header, sizeof(p_header));
  #endif
 
  Magic = p_header.magic;
  label_tbl = (long*) malloc(p_header.n_labels * sizeof(long));
  seg_text = (char*) malloc(p_header.n_strings + 32768);
  seg_exec = (unsigned char *) malloc(p_header.n_exec);

  #ifndef WIN32
    read(fd, label_tbl, (label = p_header.n_labels) * sizeof(*label_tbl));
    read(fd, seg_text, seg_text_ptr = p_header.n_strings);
    read(fd, seg_exec, PC = p_header.n_exec);
    close(fd);
  #else
    _read(fd, label_tbl, (label = p_header.n_labels) * sizeof(*label_tbl));
    _read(fd, seg_text, seg_text_ptr = p_header.n_strings);
    _read(fd, seg_exec, PC = p_header.n_exec);
    _close(fd);
  #endif
  
  
  #ifdef DEBUG
    fprintf(cgiOut, "  %ld Labels:\n\n", label);
    i=0;
    while(i<label) 
      {
        fprintf(cgiOut, "L%ld = PC %ld\n", i, label_tbl[i]);
        i++;
      }

    i=0;
    fprintf(cgiOut, "\n  %ld Strings:\n\n", seg_text_ptr);
    while(i<seg_text_ptr) 
      {
        fprintf(cgiOut, "%ld = (%s)\n", i, seg_text+i);
        i+=strlen(seg_text+i)+1;
      }

    i=0;
    fprintf(cgiOut, "\n  %ld Program:\n\n", PC);
    while(i<PC) 
      {
        fprintf(cgiOut, "%ld = 0x%x\n", i, seg_exec[i]);
        i++;
      }
  #endif

  if ((Magic == 0) && (Flag == 0))
    cgiHeaderContentType("text/html");

  run_program();

  if (dlLibraryEx)
    dlclose(dlLibraryEx);

  return 1;
}

/*--------------------------------------------------------------------------------*/
#ifdef __cplusplus 
}
#endif




