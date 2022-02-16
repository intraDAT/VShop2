#include <stdio.h>
#include <time.h>

#ifdef __cplusplus 
  extern "C" {
#endif

long ToJ(time_t Date);
int JDay(long j);
int JMonth(long j);
int JYear(long j);
time_t JMinus(time_t Date, long Value );
time_t JPlus(time_t Date, long Value );
long DaysUntil(time_t Date1, time_t Date2);

#ifdef __cplusplus 
}
#endif
