#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#ifdef WIN32
  #include <process.h>
#endif

#include "hfct.h"


#ifdef __cplusplus 
extern "C" {
#endif

/*---------------------------------------------------------------------------------*/
void ipaddr2str(char* buffer,unsigned int ipaddr)
{
	int	addr_byte[4];
	int	i;
	unsigned int	xbyte;

	for(i = 0;i < 4;i++) {
		xbyte = ipaddr >> (i*8);
		xbyte = xbyte & (unsigned int)0x000000FF;
		addr_byte[i] = xbyte;
	}
	sprintf(buffer, "%u.%u.%u.%u", addr_byte[2], addr_byte[3],
		addr_byte[0], addr_byte[1]);
}
/*---------------------------------------------------------------------------------*/
unsigned int ipstr2long(char* ip_str)
{
	char	buf[6];
	char	*ptr;
	int	i;
	int	count;
	unsigned int	ipaddr;
	int	cur_byte;

	ipaddr = (unsigned int)0;
	for(i = 0;i < 4;i++) {
		ptr = buf;
		count = 0;
		*ptr = '\0';
		while(*ip_str != '.' && *ip_str != '\0' && count < 4) {
			if(!isdigit(*ip_str)) {
				return((unsigned int)0);
			}
			*ptr++ = *ip_str++;
			count++;
		}
		if(count >= 4 || count == 0) {
			return((unsigned int)0);
		}
		*ptr = '\0';
		cur_byte = atoi(buf);
		if(cur_byte < 0 || cur_byte > 255) {
			return((unsigned int)0);
		}
		ip_str++;
		ipaddr = ipaddr << 8 | (unsigned int)cur_byte;
	}
	return(ipaddr);
}
/*---------------------------------------------------------------------------------*/
void milisec(char* datumstr)
{
long t;
long c;

  time(&t); 

  #ifdef WIN32
    c=_getpid() % 100;
  #else
    c=getpid() % 100;
  #endif
    sprintf(datumstr,"%ld%ld",t,c);
}
/*---------------------------------------------------------------------------------*/
void sec(char* datumstr)
{
long t;

    /* Sekunden und clock-Zeiten holen */
    time(&t); 

    /* In String wandenln */
    sprintf(datumstr,"%ld",t);

    /* String krzen und als Ergebnis der Methode zurckgeben */
}
/*---------------------------------------------------------------------------------*/
void Mid(char *d, char *s, int n)
{
  strncpy(d,s,n); 
  d[n]=0;
}
/*---------------------------------------------------------------------------------*/
/*
void Replace(char* String1, char Char1, char Char2)
{
unsigned int Count;

  for (Count = 0; Count < strlen(String1); Count++)
    if(String1[Count] == Char1)
      String1[Count] = Char2;
}
*/
char* Replace(char* QuellString,char* SuchString,char* ErsetzString)
{
char Buffer[8192];
char *ptr,*ptr2=QuellString;
char *dest=Buffer;

   if (strstr(QuellString,SuchString))
   {
     *dest=0;
     ptr=strstr(ptr2,SuchString);
     while ( ptr )
       {
         while (ptr2<ptr) *dest++=*ptr2++;
  
         strcpy(dest,ErsetzString);
         dest= (char*)dest+strlen(ErsetzString);

         ptr2=(char*)ptr+strlen(SuchString);

         ptr=strstr(ptr2,SuchString);
        }
      if (ptr!=QuellString)
        {
          strcpy(dest,ptr2);
          strcpy(QuellString,Buffer);
        }
    }
   return(QuellString);
}

/*---------------------------------------------------------------------------------*/
int IsOfForm(char* String, char* FormString)
{
unsigned int Counter;

  if (strlen(String) != strlen(FormString))
    return 0;

  for(Counter = 0; Counter < strlen(FormString); Counter++)
    {
      switch(FormString[Counter])
        {
          case 'd':
            if (!isdigit(String[Counter]))
              return 0;
            break;
          default :
            if (String[Counter] != FormString[Counter])
              return 0;
            break;
        }
    }

  return 1;
}
/*---------------------------------------------------------------------------------*/
char* FillZeroFloat(char* Buffer,double Number, int Length)
{
char Buffer1[100];
unsigned int Count;

  sprintf(Buffer1,"%0*.2f",Length+1,Number);

  for (Count = 0; Count < strlen(Buffer1); Count++)
    {
      if (Buffer1[Count] == '.')
        {
          strcpy(Buffer,Buffer1);
          Buffer[Count] = '\0';
          strcat(Buffer,&Buffer1[Count+1]);
          break;
        }
    }

  return Buffer;
}
/*---------------------------------------------------------------------------------*/
char* FillZeroInt(char* Buffer,int Number, int Length)
{
  sprintf(Buffer,"%0*i",Length,Number);

  return Buffer;

}
/*---------------------------------------------------------------------------------*/
struct tm StringToTime(char* TimeString)
{
struct tm Time;
char Buffer[10];


  Time.tm_hour =  0;
  Time.tm_isdst = 0;
  Time.tm_mday =  0;
  Time.tm_min =   0;
  Time.tm_mon =   1;
  Time.tm_sec =   0;
  Time.tm_wday =  0;
  Time.tm_yday =  2;
  Time.tm_year =  70;

  if (IsOfForm(TimeString, "dd:dd"))
    {
      Mid(Buffer, TimeString,2);
      Time.tm_hour = atoi(Buffer);
      Mid(Buffer, &TimeString[3],2);
      Time.tm_min = atoi(Buffer);
    }
  else
  if (IsOfForm(TimeString, "d:dd"))
    {
      Mid(Buffer, TimeString,1);
      Time.tm_hour = atoi(Buffer);
      Mid(Buffer, &TimeString[2],2);
      Time.tm_min = atoi(Buffer);

    }

  return Time;
}
/*---------------------------------------------------------------------------------*/
struct tm StringToDate(char* DateString)
{
struct tm Time;
char Buffer[15];

  Time.tm_hour = 0;
  Time.tm_isdst = 0;
  Time.tm_mday = 0;
  Time.tm_min = 0;
  Time.tm_mon = 0;
  Time.tm_sec = 0;
  Time.tm_wday = 0;
  Time.tm_yday = 0;
  Time.tm_year = 0;


  #ifdef ENGLISH
    if (IsOfForm(DateString, "dd/d/d"))
      {
        Mid(Buffer, DateString,2);
        Time.tm_year = atoi(Buffer);
        Mid(Buffer, &DateString[3],1);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[5],1);
        Time.tm_mday = atoi(Buffer);
      }
    else
    if (IsOfForm(DateString, "dddd/d/d"))
      {
        Mid(Buffer, DateString,4);
        Time.tm_year = atoi(Buffer)-1900;
        Mid(Buffer, &DateString[5],1);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[7],1);
        Time.tm_mday = atoi(Buffer);
      }
    else                           
    if (IsOfForm(DateString, "dd/dd/d"))
      {
        Mid(Buffer, DateString,2);
        Time.tm_year = atoi(Buffer);
        Mid(Buffer, &DateString[3],2);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[6],1);
        Time.tm_mday = atoi(Buffer);
      }
    else
    if (IsOfForm(DateString, "dddd/dd/d"))
      {
        Mid(Buffer, DateString,4);
        Time.tm_year = atoi(Buffer)-1900;
        Mid(Buffer, &DateString[5],2);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[8],1);
        Time.tm_mday = atoi(Buffer);
      }
    else
    if (IsOfForm(DateString, "dd/d/dd"))
      {
        Mid(Buffer, DateString,2);
        Time.tm_year = atoi(Buffer);
        Mid(Buffer, &DateString[3],1);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[5],2);
        Time.tm_mday = atoi(Buffer);
      }
    else
    if (IsOfForm(DateString, "dddd/d/dd"))
      {
        Mid(Buffer, DateString,4);
        Time.tm_year = atoi(Buffer)-1900;
        Mid(Buffer, &DateString[5],1);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[7],2);
        Time.tm_mday = atoi(Buffer);
      }
    else
    if (IsOfForm(DateString, "dd/dd/dd"))
      {
        Mid(Buffer, DateString,2);
        Time.tm_year = atoi(Buffer);
        Mid(Buffer, &DateString[3],2);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[6],2);
        Time.tm_mday = atoi(Buffer);
      }
    else
    if (IsOfForm(DateString, "dddd/dd/dd"))
      {
        Mid(Buffer, DateString,4);
        Time.tm_year = atoi(Buffer)-1900;
        Mid(Buffer, &DateString[5],2);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[8],2);
        Time.tm_mday = atoi(Buffer);
      }
    else
    if (IsOfForm(DateString, "dddddddd"))
      {
        Mid(Buffer, DateString,4);
        Time.tm_year = atoi(Buffer)-1900;
        Mid(Buffer, &DateString[4],2);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[6],2);
        Time.tm_mday = atoi(Buffer);
      }
  #else
    if (IsOfForm(DateString, "d.d.dd"))
      {
        Mid(Buffer, DateString,1);
        Time.tm_mday = atoi(Buffer);
        Mid(Buffer, &DateString[2],1);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[4],2);
        Time.tm_year = atoi(Buffer);
      }
    else
    if (IsOfForm(DateString, "d.d.dddd"))
      {
        Mid(Buffer, DateString,1);
        Time.tm_mday = atoi(Buffer);
        Mid(Buffer, &DateString[2],1);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[4],4);
        Time.tm_year = atoi(Buffer)-1900;
      }
    else                           
    if (IsOfForm(DateString, "d.dd.dd"))
      {
        Mid(Buffer, DateString,1);
        Time.tm_mday = atoi(Buffer);
        Mid(Buffer, &DateString[2],2);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[5],2);
        Time.tm_year = atoi(Buffer);
      }
    else
    if (IsOfForm(DateString, "d.dd.dddd"))
      {
        Mid(Buffer, DateString,1);
        Time.tm_mday = atoi(Buffer);
        Mid(Buffer, &DateString[2],2);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[5],4);
        Time.tm_year = atoi(Buffer)-1900;
      }
    else
    if (IsOfForm(DateString, "dd.d.dd"))
      {
        Mid(Buffer, DateString,2);
        Time.tm_mday = atoi(Buffer);
        Mid(Buffer, &DateString[3],1);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[5],2);
        Time.tm_year = atoi(Buffer);
      }
    else
    if (IsOfForm(DateString, "dd.d.dddd"))
      {
        Mid(Buffer, DateString,2);
        Time.tm_mday = atoi(Buffer);
        Mid(Buffer, &DateString[3],1);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[5],4);
        Time.tm_year = atoi(Buffer)-1900;
      }
    else
    if (IsOfForm(DateString, "dd.dd.dd"))
      {
        Mid(Buffer, DateString,2);
        Time.tm_mday = atoi(Buffer);
        Mid(Buffer, &DateString[3],2);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[6],2);
        Time.tm_year = atoi(Buffer);
      }
    else
    if (IsOfForm(DateString, "dd.dd.dddd"))
      {
        Mid(Buffer, DateString,2);
        Time.tm_mday = atoi(Buffer);
        Mid(Buffer, &DateString[3],2);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[6],4);
        Time.tm_year = atoi(Buffer)-1900;
      }
    else
    if (IsOfForm(DateString, "dddddddd"))
      {
        Mid(Buffer, DateString,4);
        Time.tm_year = atoi(Buffer)-1900;
        Mid(Buffer, &DateString[4],2);
        Time.tm_mon = atoi(Buffer)-1;
        Mid(Buffer, &DateString[6],2);
        Time.tm_mday = atoi(Buffer);
      }
  #endif

  return Time;
}
/*---------------------------------------------------------------------------------*/
struct tm StringToTimestamp(char* TimestampString)
{
struct tm Time;
char Buffer[30];

  Time.tm_hour = 0;
  Time.tm_isdst = 0;
  Time.tm_mday = 0;
  Time.tm_min = 0;
  Time.tm_mon = 0;
  Time.tm_sec = 0;
  Time.tm_wday = 0;
  Time.tm_yday = 0;
  Time.tm_year = 0;

  Mid(Buffer, TimestampString,2);
  Time.tm_mday = atoi(Buffer);
  Mid(Buffer, &TimestampString[3],2);
  Time.tm_mon = atoi(Buffer)-1;
  Mid(Buffer, &TimestampString[6],4);
  Time.tm_year = atoi(Buffer)-1900;
  Mid(Buffer, &TimestampString[11],2);
  Time.tm_hour = atoi(Buffer);
  Mid(Buffer, &TimestampString[14],2);
  Time.tm_min = atoi(Buffer);
  Mid(Buffer, &TimestampString[17],2);
  Time.tm_sec = atoi(Buffer);

return Time;
}

#ifdef __cplusplus 
}
#endif

/*---------------------------------------------------------------------------------*/
