#ifdef WIN32

  #include "vsl.h"

  extern "C" _declspec(dllexport) t_Value table_lookup(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_get(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_columnname(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_inputfield(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_next(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_prev(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_last(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_first(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_at(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_eof(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_bof(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_columns(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_rows(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_row(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_select(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_searchform(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_seek(t_Value *arg_list);

  extern "C" _declspec(dllexport) t_Value table_fileopen(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_fileclose(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_filewrite(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_filewriteline(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_filereadline(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value table_fileeof(t_Value *arg_list);

#endif
