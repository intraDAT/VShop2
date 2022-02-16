#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef __cplusplus 
  extern "C" {
#endif


 /*****************************************************************
 *                                                                *
 * Funktion zur Umrechnung des Datums in das Julianische Datum    *
 *                                                                *
 *****************************************************************/
long ToJ(time_t Date)
{
int d;
int m;
int y;
long jd;
int c,ya;
struct tm* TMDate;

  TMDate = localtime(&Date);
  d = TMDate->tm_mday;
  m = TMDate->tm_mon+1;
  y = TMDate->tm_year+1900;

  if (m > 2)
    m = m -3;
  else
    {
      m = m + 9;
      y = y -1;
    }
   c = y / 100;
   ya = y-100 * c;
   jd =(146097*c) / 4 + (1461*ya) / 4 + (153*m+2) / 5 + d + 1721119;

   return jd;
}   

 /*****************************************************************
 *                                                                *
 * Funktion zur Umrechnung des Julianischen Datums in eine Tag    *
 *                                                                *
 *****************************************************************/
int JDay(long j)
{
long d,m,y;

  j = j-1721119;        
  y = (4*j-1) / 146097;
  j = 4*j-1-146097*y;   
  d = j / 4;
  j = (4*d+3) / 1461;
  d = 4*d+3-1461*j;
  d = (d+4) / 4;      
  m = (5*d-3) / 153;
  d = 5*d-3-153*m;      
  d = (d+5) / 5;
  y = 100*y+j;
  if (m<10)
    m = m + 3;
  else
    {
      m = m-9;
      y = y +1;
    }
  return d;
}
 /*****************************************************************
 *                                                                *
 * Funktion zur Umrechnung des Julianischen Datums in eine Monat  *
 *                                                                *
 *****************************************************************/
int JMonth(long j)
{
long d,m,y;

  j = j-1721119;        
  y = (4*j-1) / 146097;
  j = 4*j-1-146097*y;   
  d = j / 4;
  j = (4*d+3) / 1461;
  d = 4*d+3-1461*j;
  d = (d+4) / 4;      
  m = (5*d-3) / 153;
  d = 5*d-3-153*m;      
  d = (d+5) / 5;
  y = 100*y+j;
  if (m<10)
    m = m + 3;
  else
    {
      m = m-9;
      y = y +1;
    }
  return m;
}

/*****************************************************************
*                                                                *
* Funktion zur Umrechnung des Julianischen Datums in ein Jahr    *
*                                                                *
*****************************************************************/
int JYear(long j)
{
long d,m,y;

  j = j-1721119;        
  y = (4*j-1) / 146097;
  j = 4*j-1-146097*y;   
  d = j / 4;
  j = (4*d+3) / 1461;
  d = 4*d+3-1461*j;
  d = (d+4) / 4;      
  m = (5*d-3) / 153;
  d = 5*d-3-153*m;      
  d = (d+5) / 5;
  y = 100*y+j;
  if (m<10)
    m = m + 3;
  else
    {
      m = m-9;
      y = y +1;
    }
  return y;
}

/*****************************************************************
*                                                                *
* Funktion zur Subtraktion eines Wertes                          *
*                                                                *
*****************************************************************/
time_t JMinus(time_t Date, long Value )
{
long J;
struct tm TMDate;

  TMDate.tm_hour = 0;
  TMDate.tm_isdst = 0;
  TMDate.tm_mday = 0;
  TMDate.tm_min = 0;
  TMDate.tm_mon = 0;
  TMDate.tm_sec = 0;
  TMDate.tm_wday = 0;
  TMDate.tm_yday = 0;
  TMDate.tm_year = 0;

  J = ToJ(Date);
  J -= Value;

  TMDate.tm_year = JYear(J)-1900;
  TMDate.tm_mon = JMonth(J)-1;
  TMDate.tm_mday = JDay(J);

  Date = mktime(&TMDate);
  return Date;
}

/*****************************************************************
*                                                                *
* Funktion zur Subtraktion eines Wertes                          *
*                                                                *
*****************************************************************/
time_t JPlus(time_t Date, long Value )
{
long J;
struct tm TMDate;

  TMDate.tm_hour = 0;
  TMDate.tm_isdst = 0;
  TMDate.tm_mday = 0;
  TMDate.tm_min = 0;
  TMDate.tm_mon = 0;
  TMDate.tm_sec = 0;
  TMDate.tm_wday = 0;
  TMDate.tm_yday = 0;
  TMDate.tm_year = 0;

  J = ToJ(Date);
  J += Value;

  TMDate.tm_year = JYear(J)-1900;
  TMDate.tm_mon = JMonth(J)-1;
  TMDate.tm_mday = JDay(J);

  Date = mktime(&TMDate);
  return Date;
}

long DaysUntil(time_t Date1, time_t Date2)
{
long J1;
long J2;

  J1 = ToJ(Date1);
  J2 = ToJ(Date2);
  return J2 - J1;
}



#ifdef __cplusplus 
}
#endif
