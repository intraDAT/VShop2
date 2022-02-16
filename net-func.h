#ifdef WIN32

  #include "vsl.h"

  extern "C" _declspec(dllexport) t_Value base_sendmail(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_tomailbegin(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_tomailend(t_Value *arg_list);
#endif