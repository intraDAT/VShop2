#include <windows.h>
#include <stdio.h>
#include "dlfcn_win32.h"

static char errbuf[512];

HMODULE dlopen(const char *name, int mode)
{  
HMODULE hdll;

  hdll =  LoadLibrary(name);
  #ifdef _WIN32  
    if (! hdll) 
      {
        sprintf(errbuf, "error code %d loading library %s", GetLastError(), name);
        return NULL;  
      }
  #else  
    if ((UINT) hdll < 32) 
      {
        sprintf(errbuf, "error code %d loading library %s", (UINT) hdll, name);
        return NULL;  
      }
  #endif  
  
  return hdll;
}

void *dlsym(HMODULE lib, const char *name)
{  
HMODULE hdll = (HMODULE) lib;
void *symAddr;  

  symAddr = (void *) GetProcAddress(hdll, name);

  return symAddr;
}

int dlclose(void *lib)
{  
HMODULE hdll = (HMODULE) lib;

  #ifdef _WIN32  
    if (FreeLibrary(hdll))    
      return 0;  
    else 
      {
        sprintf(errbuf, "error code %d closing library", GetLastError());
        return -1;  
      }
  #else  
    FreeLibrary(hdll);  
    return 0;
  #endif
}