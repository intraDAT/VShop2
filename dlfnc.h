#ifndef DLFCN_H
#define DLFCN_H

#ifdef HPUX
  #include "dlfcn_hpux.h"
#else
#ifdef WIN32
  #include "dlfcn_win32.h"
#else
  #include <dlfcn.h>
#endif

#endif

