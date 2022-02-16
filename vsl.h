#ifndef __VSL
#define __VSL

#include <time.h>
#include <ctype.h>

#define MAX_NUM_ARGS 30
#define DL_LIBRARY_NAME NULL

typedef enum { Bool_Value, Int_Value, Float_Value, String_Value, Time_Value, Date_Value, Timestamp_Value, Not_Set, Unknown} Id_type;

typedef struct t_Value {
  Id_type typ;
  long cValue;  /* Offset auf String im seg_text Segment */
  char *sValue; /* Pointer auf String im user space */
  long nValue;
  double dValue;
  time_t tValue;
  long Indicator;
} t_Value;

#endif
