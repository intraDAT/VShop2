#define _XOPEN_SOURCE       
#include <unistd.h>


/* stefan added */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* end stefan added */


#ifdef WINDOWSNT
  #include "skey.h"
#endif

#ifdef VSHOP_DEMO
  #define SHOPVERSION 0021
#elif  VSHOP_FREE
  #define SHOPVERSION 0121
#elif  VSL_ENGINE
  #define SHOPVERSION 0221
#elif  VSHOP_PROFESSIONAL
  #ifdef SUN
    #define SHOPVERSION 2321
  #elif  HPUX
    #define SHOPVERSION 1321
  #else
    #define SHOPVERSION 0321
  #endif
#elif  VSHOP_PARTNER
  #ifdef SUN
    #define SHOPVERSION 2421
  #elif  HPUX
    #define SHOPVERSION 1421
  #else
    #define SHOPVERSION 0421
  #endif
#elif  VSHOP_PROVIDER
  #ifdef SUN
    #define SHOPVERSION 2521
  #elif  HPUX
    #define SHOPVERSION 1521
  #else
    #define SHOPVERSION 0521
  #endif
#elif  VSHOP_BASIC
    #define SHOPVERSION 0621
#endif


unsigned int ipstr2long(char *ip_str)
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

int main(int argc, char **argv)
{
char Buffer1[41];
char Buffer2[41];
unsigned int IP;
#if defined(VSHOP_PROVIDER) || defined(VSHOP_PARTNER)
  int i=0;
#endif

#ifdef WINDOWSNT
char* Ptr;
#endif

  #if defined(VSHOP_PROVIDER) || defined(VSHOP_PARTNER)
    if (argc > 2)
  #else
    if (argc > 1)
  #endif
    {
      if ((IP = ipstr2long(argv[1])) > 0)
        {
          IP -= SHOPVERSION;
          sprintf(Buffer1,"%10u",IP);
          #if defined(VSHOP_PROVIDER) || defined(VSHOP_PARTNER)
            strcpy(Buffer2,argv[2]);
            while(Buffer2[i]) 
              {
                Buffer2[i]=toupper(Buffer2[i]);
                i++;
              }
            strcat(Buffer1,Buffer2);
          #else
            strcat(Buffer1,"VSHOP");
          #endif

          #ifndef WINDOWSNT
            strcpy(Buffer2,&Buffer1[8]);
            Buffer1[8]='\0';
            strcpy(Buffer1,crypt(Buffer1,"21"));
            strcpy(Buffer2,crypt(Buffer2,"00"));
            printf("%s%s",Buffer1,Buffer2);
          #else
            cryptpassword (&Ptr, Buffer1, "BS20031998CW");
            printf("%s",Ptr);
            free(Ptr);
          #endif
        }
    }
  else
    {
    }
  return 0;
}
