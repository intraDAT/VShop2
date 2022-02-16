#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/* includes stefan */ 
#include <strings.h>

#ifdef WIN32
  #include "win32tools.h"
#endif

#include "inifile.h"

#ifdef __cplusplus 
extern "C" {
#endif

FILE* IniFile;
char IniFileLine[501];

int OpenIniFile(char* FileName)
{
  #ifdef WIN32
    return ((IniFile = fopen(FileName,"r+")) != NULL);
  #else
    return ((IniFile = fopen(FileName,"r")) != NULL);
  #endif
}
/*------------------------------------------------------------------------------*/
int CloseIniFile()
{
  return (fclose(IniFile) == 0);
}
/*------------------------------------------------------------------------------*/
char* GetFirstParam(char* Param)
{
int ParamLength;

  ParamLength = strlen(Param);
  
  rewind(IniFile);

  while (fgets(IniFileLine,500,IniFile) != NULL)
    {
      IniFileLine[strlen(IniFileLine)-1]='\0';
      if (strncasecmp(Param,IniFileLine,ParamLength) == 0)
        {
          while(IniFileLine[ParamLength] == ' ')
            ParamLength++;

          return &IniFileLine[ParamLength];
        }
    }
  return NULL;
}
/*------------------------------------------------------------------------------*/
char* GetNextParam(char* Param)
{
int ParamLength;

  ParamLength = strlen(Param);

  while (fgets(IniFileLine,500,IniFile) != NULL)
    {
      IniFileLine[strlen(IniFileLine)-1]='\0';
      if (strncasecmp(Param,IniFileLine,ParamLength) == 0)
        {
          while(IniFileLine[ParamLength] == ' ')
            ParamLength++;

          return &IniFileLine[ParamLength];
        }
    }
  return NULL;
}

#ifdef __cplusplus 
}
#endif


