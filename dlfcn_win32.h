#include <windows.h>

#ifndef DLFCN_WIN32
  #define DLFCN_WIN32

  #define RTLD_LAZY 1
  #define RTLD_NOW  2
  #define RTLD_GLOBAL 0
  HMODULE dlopen(const char *, int);
  void *dlsym(HMODULE, const char *);
  int dlclose(void *);

#endif