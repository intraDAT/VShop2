#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <curses.h>
#include <menu.h>
#include <form.h>
#include <pwd.h>
#ifndef DB_ORACLE
  #include "adabas.h"
#else
  #include "oracle.h"
#endif

/*------------------------------------------------------------------------------*/
/* TOOL Functions */
/*------------------------------------------------------------------------------*/
int Copy(char* Source, char* Destination)
{
char Buffer[200];

  sprintf(Buffer,"cp %s %s",Source,Destination);

  return (system(Buffer) == 0);
}
/*-------------------------------------------------------------------------------------*/
int SetPassword(char* DBUserName, char* OldDBPassword, char* NewDBPassword,char* ShopName)
{
FILE* File1;
FILE* File2;
char Buffer[201];
char Line[501];
char NewPasswordLine[201];
char* Ptr;

  EngineDB = new CEngineDB();
  EngineDB->Connect("vshop",DBUserName, OldDBPassword,"");

  sprintf(Buffer,"ALTER PASSWORD %s TO %s",OldDBPassword,NewDBPassword);

  if (EngineDB->SQLUpdate(Buffer) == 0)
    {
      printf("Fehler beim Žndern des Passwortes in der Datenbank.\n");
      return 6;
    }

  delete EngineDB;

  sprintf(Buffer,"%s_DBPASSWORD",ShopName);
  
  cryptpassword (&Ptr, NewDBPassword, "VSHOPDBPASSWORD");
  sprintf(NewPasswordLine,"%s_DBPASSWORD %s",ShopName,Ptr);

  Copy("/etc/vshop.conf", "/etc/vshop.conf.bak");

  if ((File1 = fopen("/etc/vshop.conf.bak","r")) != NULL)
    {
      if ((File2 = fopen("/etc/vshop.conf","w+")) != NULL)
        {
          while (fgets(Line,500,File1) != NULL)
            {
              if (strncasecmp(Line,Buffer,strlen(Buffer)) == 0)
                {
                  fputs(NewPasswordLine,File2);
                  fputs("\n",File2);
                }
              else
                fputs(Line,File2);
            }
          fclose(File2);
        }
      else
        printf("Fehler beim ™ffnen von /etc/vshop.conf.\n");

      fclose(File1);
    }
  else
    printf("Fehler beim ™ffnen von /etc/vshop.conf.bak.\n");
}
/*-------------------------------------------------------------------------------------*/
int main(int argc, char *argv[]) 
{
char* Ptr;
char ShopName[10];
int Counter;
char Buffer[256];
char DBPASSWORD[201];
char DBUSER[201];
char Arg1[51];
char Arg2[51];

  if (argc <= 2) 
    {
      printf("\nNicht genuegend Parameter !\n");
      exit(1);
    }

  strcpy(Arg1,argv[1]);
  strcpy(Arg2,argv[2]);

  for (Counter = 0;Counter < (int) strlen(Arg1);Counter++)
    if (((int) Arg1[Counter] > 96) && ((int) Arg1[Counter] < 123))
      Arg1[Counter] -= 32;

  for (Counter = 0;Counter < (int) strlen(Arg2);Counter++)
    if (((int) Arg2[Counter] > 96) && ((int) Arg2[Counter] < 123))
      Arg2[Counter] -= 32;



  if ((Ptr = strrchr(argv[0],'/')) != NULL)
    strcpy(ShopName,Ptr+1);
  else
    strcpy(ShopName,argv[0]);

  ShopName[strlen(ShopName)-6] = '\0';

  for (Counter = 0;Counter < (int) strlen(ShopName);Counter++)
    if (((int) ShopName[Counter] > 96) && ((int) ShopName[Counter] < 123))
      ShopName[Counter] -= 32;

  OpenIniFile("/etc/vshop.conf");

  if((Ptr = GetFirstParam("DBROOT")) != NULL)
    {
      setenv("DBROOT",Ptr,1);
    }
  else
    {
      printf("Parameter DBROOT in /etc/vshop.conf nicht gefunden.\n");
      exit(2);
    }

  sprintf(Buffer,"%s_DBUSER",ShopName);
  if((Ptr = GetFirstParam(Buffer)) != NULL)
    {
      strcpy(DBUSER,Ptr);
    }
  else
    {
      printf("Parameter %s_DBUSER in /etc/vshop.conf nicht gefunden.\n",ShopName);
      exit(3);
    }

  sprintf(Buffer,"%s_DBPASSWORD",ShopName);
  if((Ptr = GetFirstParam(Buffer)) != NULL)
    {
      strcpy(DBPASSWORD,Ptr);
    }
  else
    {
      printf("Parameter %s_DBPASSWORD in /etc/vshop.conf nicht gefunden.\n",ShopName);
      exit(4);
    }

  CloseIniFile();

  decryptpassword (&Ptr, DBPASSWORD, "VSHOPDBPASSWORD");

  if (argc != 4)
    {
      if (strcmp(Ptr,Arg1) != 0)
        {
          printf("Das alte Password ist nicht korrekt.\n");
          exit(5);
        }
    }
  else
    {
      if (strcmp(DBPASSWORD,Arg1) != 0)
        {
          printf("Das alte Password ist nicht korrekt.\n");
          exit(5);
        }
    }

  if (argc != 4)
    return SetPassword(DBUSER,Ptr,Arg2,ShopName);
  else
    return SetPassword(DBUSER,DBPASSWORD,Arg2,ShopName);
}
