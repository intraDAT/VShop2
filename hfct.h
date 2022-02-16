#ifndef HFCT_H
#define HFCT_H

#ifdef __cplusplus 
extern "C" {
#endif

/*---------------------------------------------------------------------------------*/

void ipaddr2str(char* buffer,unsigned int ipaddr);
unsigned int ipstr2long(char* ip_str);

void milisec(char* datumstr);
void sec(char* datumstr);
void Mid(char *d, char *s, int n);
char* Replace(char* QuellString,char* SuchString,char* ErsetzString);
int IsOfForm(char* String, char* FormString);

char* FillZeroFloat(char* Buffer,double Number, int Length);
char* FillZeroInt(char* Buffer,int Number, int Length);

struct tm StringToDate(char* DateString);
struct tm StringToTime(char* TimeString);
struct tm StringToTimestamp(char* TimeStampString);

#ifdef __cplusplus 
}
#endif
/*---------------------------------------------------------------------------------*/
#endif

