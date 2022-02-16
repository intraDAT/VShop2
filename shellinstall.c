#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>


#include <sys/stat.h>


#include "vslanguage.h"
#include "inifile.h"

char DBROOT[200];
char HTTPDCONF[200];
char CGIBIN[200];
char CGIBINPATH[200];
char HTDOCS[200];
char SERVERNAME[200];
char DBUSER[200];
char DBPASSWORD[200];
char INSTALLPATH[200];
char INDEXHTML[200];
char SRM[200];
char KEY[50];
char WWWUSER[200];


#define BUFFERLENGTH 1024

/*------------------------------------------------------------------------------*/
/* TOOL Functions */
/*------------------------------------------------------------------------------*/
int IsFileWriteable(char* File)
{
  if (access(File,W_OK) != 0)
    {
      return 0;
    }
  return 1;
}
/*------------------------------------------------------------------------------*/
int FileExists(char* File)
{
  if (access(File,W_OK) != 0)
    {
      return 0;
    }
  return 1;
}
/*------------------------------------------------------------------------------*/
int UnZip(char* Path, char* File)
{
char Buffer[200];

  if (Path[strlen(Path)-1] != '/')
    sprintf(Buffer,"%s/gzip -d %s/%s",INSTALLPATH,Path,File);
  else
    sprintf(Buffer,"%s/gzip -d %s%s",INSTALLPATH,Path,File);

  system(Buffer);
  return 1;
}
/*------------------------------------------------------------------------------*/
int UnZipExt(char* Path, char* File)
{
char Buffer[200];

  if (Path[strlen(Path)-1] != '/')
    sprintf(Buffer,"%s/gzip -dc %s/%s | cpio -icvBdmu",INSTALLPATH,Path,File);
  else
    sprintf(Buffer,"%s/gzip -dc %s%s | cpio -icvBdmu",INSTALLPATH,Path,File);

  system(Buffer);

  return 1;
}
/*------------------------------------------------------------------------------*/
int Untar(char* Path, char* File)
{
char Buffer[200];

  #ifdef SUN
    if (Path[strlen(Path)-1] != '/')
      sprintf(Buffer,"tar xf %s/%s",Path,File);
    else
      sprintf(Buffer,"tar xf %s%s",Path,File);
  #else
    if (Path[strlen(Path)-1] != '/')
      sprintf(Buffer,"tar xzf %s/%s",Path,File);
    else
      sprintf(Buffer,"tar xzf %s%s",Path,File);
  #endif

  system(Buffer);

  return 1;
}
/*------------------------------------------------------------------------------*/
int Copy(char* Source, char* Destination)
{
char Buffer[200];

  sprintf(Buffer,"cp %s %s",Source,Destination);
  
  return (system(Buffer) == 0);
}
/*------------------------------------------------------------------------------*/
int ChangeMod(char* File)
{
char Buffer[200];

  sprintf(Buffer,"chmod -R 775 %s",File);
  
  return (system(Buffer) == 0);
}
/*------------------------------------------------------------------------------*/
int ChangeModRW(char* File)
{
char Buffer[200];

  sprintf(Buffer,"chmod -R a+rw %s",File);
  
  return (system(Buffer) == 0);
}
/*------------------------------------------------------------------------------*/
int ChangeModX(char* File)
{
char Buffer[200];

  sprintf(Buffer,"chmod -R a+x %s",File);
  
  return (system(Buffer) == 0);
}
/*------------------------------------------------------------------------------*/
int ChangeOwner(char* File, char* Owner)
{
struct passwd *Passwd;
char Buffer[200];

  Passwd = getpwnam(Owner);
  if (Passwd == NULL)
    return 0;

  sprintf(Buffer,"chown -R %i:%i %s",Passwd->pw_uid,Passwd->pw_gid,File);
  
  return (system(Buffer) == 0);
}
/*------------------------------------------------------------------------------*/
int Delete(char* Path,char* File)
{
char Buffer[500];

  if (Path[strlen(Path)-1] != '/')
    sprintf(Buffer,"%s/%s",Path,File);
  else
    sprintf(Buffer,"%s%s",Path,File);

  return (remove(Buffer) == 0);
}
/*------------------------------------------------------------------------------*/
void Init()
{
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
  printf(TEXT_VERSION);
  printf("\n");
  printf("\n");
  printf(TEXT_INSTALLDIALOG1);
  printf("\n");
  printf(TEXT_INSTALLDIALOG2);
  printf("\n");
  printf(TEXT_INSTALLDIALOG3);
  printf("\n");
  printf("\n");
}
/*------------------------------------------------------------------------------*/
int Ueberschreiben()
{
/*

char Char[5];

  if (FileExists("/etc/vshop.conf"))
    {
      printf(STEXT_UEBERSCHREIBEN1);
      printf(STEXT_UEBERSCHREIBEN2);
      printf(STEXT_UEBERSCHREIBEN3);
      printf("\n");
      printf(STEXT_UEBERSCHREIBEN4);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
      return 0;
    }
*/
  return 1;
}
/*------------------------------------------------------------------------------*/
int IsRoot()
{
  return getuid() == 0; 
}
/*------------------------------------------------------------------------------*/
/* ADABAS Functions */
/*------------------------------------------------------------------------------*/
int IsAdabasInstalled()
{
char Char[5];

  printf(STEXT_INSTALLADABAS1);
  printf("\n");
  scanf("%s",Char);
  printf("\n");
      
  if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
    {
      return 1;
    }
  return 0;
}
/*------------------------------------------------------------------------------*/
int IsUser()
{
char Char[5];

  printf(STEXT_INSTALLADABAS2);
  printf("\n");
  scanf("%s",Char);
  printf("\n");
      
  if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
    {
      return 1;
    }

  return 0;
}
/*------------------------------------------------------------------------------*/
int GetNewDBUSER()
{
char Char[5];

  if (strcmp(DBUSER,"") != 0)
    {
      printf("%s%s%s",STEXT_IS,DBUSER,STEXT_INSTALLADABAS3);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
    }
  
  printf("\n");
  printf(STEXT_INSTALLADABAS4);
  scanf("%s",DBUSER);
  printf("\n");

  return 0;
}
/*------------------------------------------------------------------------------*/
int GetDBUSER()
{
char Char[5];

  if (strcmp(DBUSER,"") != 0)
    {
      printf("%s%s%s",STEXT_IS,DBUSER,STEXT_INSTALLADABAS3);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
    }
  
  printf("\n");
  printf(STEXT_INSTALLADABAS4B);
  scanf("%s",DBUSER);
  printf("\n");

  return 0;
}
/*------------------------------------------------------------------------------*/
int GetInstallDBROOT()
{
char Char[5];

  if (strcmp(DBROOT,"") != 0)
    {
      printf("%s%s%s",STEXT_IS,DBROOT,STEXT_INSTALLADABAS5);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
    }
  
  printf("\n");
  printf(STEXT_INSTALLADABAS6);
  scanf("%s",DBROOT);
  printf("\n");

  return 0;
}
/*------------------------------------------------------------------------------*/
int InstallAdabas()
{
char Buffer[200];
char Hostname[200];
struct passwd *Passwd;
int Return;

  printf(STEXT_INSTALLADABAS7);
  printf(STEXT_INSTALLADABAS8);
  printf(STEXT_INSTALLADABAS9);

  if (IsUser() == 0)
    {
      printf(STEXT_INSTALLADABAS10);
      exit(1);
    }

  while(GetNewDBUSER() != 1)
    {
      printf("\n");
    }

  while (getpwnam(DBUSER) == NULL)
    {
      printf(STEXT_INSTALLADABAS11,DBUSER);
      strcpy(DBUSER,"");

      while(GetNewDBUSER() != 1)
        {
          printf("\n");
        }
    }

  while(GetInstallDBROOT() != 1)
    {
      printf("\n");
    }

  printf(STEXT_INSTALLADABAS12);

  mkdir(DBROOT,775);


  #if defined (SUN) || defined (HPUX)
    if (Copy("adabas/install/adad6116.gz", DBROOT) == 0)
      exit(2);
  #elif DEMOVERSION
    if (Copy("adabas/install/adabas11.tgz", DBROOT) == 0)
      exit(2);
  #elif CEBITPATCH
    if (Copy("../adabas/unix/linux/aad/v110/adabas1106.tgz", DBROOT) == 0)
      exit(2);
  #else
    if (Copy("adabas/install/adad1005.tgz", DBROOT) == 0)
      exit(2);
  #endif

#ifndef CEBITPATCH
  #ifdef ENGLISH
    if (Copy("adabas/install/docu.tgz", DBROOT) == 0);
  #else
    if (Copy("adabas/install/doku.tgz", DBROOT) == 0);
  #endif
#endif


  printf(STEXT_INSTALLADABAS13);

  chdir(DBROOT);

  #if defined (SUN) || defined (HPUX)
    if (UnZipExt(DBROOT,"adad6116.gz") == 0)
      exit(2);
  #elif DEMOVERSION
    if (Untar(DBROOT,"adabas11.tgz") == 0)
      exit(2);
  #elif CEBITPATCH
    if (Untar(DBROOT,"adabas1106.tgz") == 0)
      exit(2);
  #else
    if (Untar(DBROOT,"adad1005.tgz") == 0)
      exit(2);
  #endif

#ifndef CEBITPATCH
  #ifdef ENGLISH
    if (Untar(DBROOT,"docu.tgz") == 0);
  #else
    if (Untar(DBROOT,"doku.tgz") == 0);
  #endif
#endif

  printf(STEXT_INSTALLADABAS13);

  gethostname(Hostname,200);

  Passwd = getpwnam(DBUSER);

  sprintf(Buffer,"bin/x_install -r %s -o %i -g %i -n %s",DBROOT,Passwd->pw_uid,Passwd->pw_gid,Hostname);

  Return = system(Buffer);
  #if defined (SUN) || defined (HPUX)
    Delete(DBROOT,"adad6116");
  #elif DEMOVERSION
    Delete(DBROOT,"adabas11.tgz");
  #elif CEBITPATCH
    Delete(DBROOT,"adabas1106.tgz");
  #else
    Delete(DBROOT,"adad1005.tgz");
  #endif

  printf("\n\n");
  return (Return == 0);
}
/*------------------------------------------------------------------------------*/
int GetDBROOT()
{
char Char[5];

  if (strcmp(DBROOT,"") != 0)
    {
      printf(STEXT_INSTALLADABAS15,DBROOT);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
    }
  
  printf("\n");
  printf(STEXT_INSTALLADABAS16);
  scanf("%s",DBROOT);
  printf("\n");

  return 0;
}
/*------------------------------------------------------------------------------*/
/* WWW-SERVER Functions */
/*------------------------------------------------------------------------------*/
int IsServerApache()
{
char Char[5];


  printf(STEXT_INSTALLWWW1);
  scanf("%s",Char);
  printf("\n");
      
  if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
    {
      return 1;
    }

  return 0;
}
/*------------------------------------------------------------------------------*/
void TestHTTPDConf()
{
  if (access("/etc/httpd/httpd.conf",W_OK) == 0)
    {
      strcpy(HTTPDCONF,"/etc/httpd/httpd.conf");
    }
  else
  if (access("/etc/httpsd/httpd.conf",W_OK) == 0)
    {
      strcpy(HTTPDCONF,"/etc/httpsd/httpd.conf");
    }
  else
  if (access("/etc/httpd/conf/httpd.conf",W_OK) == 0)
    {
      strcpy(HTTPDCONF,"/etc/httpd/conf/httpd.conf");
    }
  else
  if (access("/etc/httpsd/conf/httpd.conf",W_OK) == 0)
    {
      strcpy(HTTPDCONF,"/etc/httpsd/conf/httpd.conf");
    }
  else
    {
      printf(STEXT_INSTALLWWW2);
      strcpy(HTTPDCONF,"");
    }
}
/*------------------------------------------------------------------------------*/
int GetSRM()
{
char Char[5];

  if (strcmp(SRM,"") != 0)
    {
      printf(STEXT_INSTALLWWW3,SRM);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
    }
  
  printf("\n");
  printf(STEXT_INSTALLWWW4);
  scanf("%s",SRM);
  printf("\n");

  return 0;
}
/*------------------------------------------------------------------------------*/
int GetHTTPDConf()
{
char Char[5];

  if (strcmp(HTTPDCONF,"") != 0)
    {
      printf(STEXT_INSTALLWWW5,HTTPDCONF);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
    }
  
  printf("\n");
  printf(STEXT_INSTALLWWW6);
  scanf("%s",HTTPDCONF);
  printf("\n");

  return 0;
}
/*------------------------------------------------------------------------------*/
int GetSERVERNAME()
{
char Char[5];

  if (strcmp(SERVERNAME,"") != 0)
    {
      printf(STEXT_INSTALLWWW7,SERVERNAME,SERVERNAME);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
    }
  
  printf("\n");
  printf(STEXT_INSTALLWWW8);
  scanf("%s",SERVERNAME);
  printf("\n");

  return 0;
}
/*------------------------------------------------------------------------------*/
int GetHTDOCS()
{
char Char[5];

  if (strcmp(HTDOCS,"") != 0)
    {
      printf(STEXT_INSTALLWWW9,HTDOCS);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
    }
  
  printf("\n");
  printf(STEXT_INSTALLWWW10);
  scanf("%s",HTDOCS);
  printf("\n");

  return 0;
}
/*------------------------------------------------------------------------------*/
int GetCGIBIN()
{
char Char[5];

  if (strcmp(CGIBIN,"") != 0)
    {
      printf(STEXT_INSTALLWWW11,CGIBIN,SERVERNAME,CGIBIN);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
    }
  
  printf("\n");
  printf(STEXT_INSTALLWWW12);
  scanf("%s",CGIBIN);
  printf("\n");

  return 0;
}
/*------------------------------------------------------------------------------*/
int GetKEY()
{
char Char[5];

  if (strcmp(KEY,"") != 0)
    {
      printf(STEXT_INSTALL1,KEY);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
    }
  
  printf("\n");
  printf(STEXT_INSTALL2);
  scanf("%s",KEY);
  printf("\n");

  return 0;
}
/*------------------------------------------------------------------------------*/
int GetCGIBINPATH()
{
char Char[5];

  if (strcmp(CGIBINPATH,"") != 0)
    {
      printf(STEXT_INSTALLWWW13,CGIBINPATH);
      scanf("%s",Char);
      printf("\n");
      
      if ((strcmp(Char,STEXT_YES1) == 0) || (strcmp(Char,STEXT_YES2) == 0))
        {
          return 1;
        }
    }
  
  printf("\n");
  printf(STEXT_INSTALLWWW14);
  scanf("%s",CGIBINPATH);
  printf("\n");

  return 0;
}
/*------------------------------------------------------------------------------*/
int ServerIsApache()
{
char* Ptr; 

  TestHTTPDConf();

  while(GetHTTPDConf() != 1)
    {
      printf("\n");
    }

  while (IsFileWriteable(HTTPDCONF) == 0)
    {
      printf(STEXT_INSTALLWWW15,HTTPDCONF);
      strcpy(HTTPDCONF,"");
      GetHTTPDConf();
    }

  OpenIniFile(HTTPDCONF);

  if((Ptr = GetFirstParam("ServerName")) != NULL)
    {
      strcpy(SERVERNAME,Ptr);
    }
  else
    {
      strcpy(SERVERNAME,"");
    }

  while(GetSERVERNAME() != 1)
    {
      printf("\n");
    }

  if((Ptr = GetFirstParam("WWWUSER")) != NULL)
    {
      strcpy(WWWUSER,Ptr);
    }
  else
    {
      strcpy(WWWUSER,DBUSER);
    }

      if((Ptr = GetFirstParam("ResourceConfig")) != NULL)
        {
          strcpy(SRM,Ptr);
        }
      else
        {
          strcpy(SRM,HTTPDCONF);

          if (strrchr(SRM,'/') != NULL)
            {
              Ptr = strrchr(SRM,'/');
              *Ptr = 0;
              strcat(SRM,"/srm.conf");
            }

          if (IsFileWriteable(SRM) == 0)
            {
              strcpy(SRM,"");
              printf(STEXT_INSTALLWWW16);
              while(GetSRM() != 1)
                {
                  printf("\n");
                }
            }
        }

      CloseIniFile();
      OpenIniFile(SRM);

      if((Ptr = GetFirstParam("DirectoryIndex")) != NULL)
        {
          strcpy(INDEXHTML,Ptr);
          if ((Ptr = strchr(INDEXHTML,' ')) != NULL)
            *Ptr = 0;
        }

      if((Ptr = GetFirstParam("DocumentRoot")) != NULL)
        {
          if (*Ptr == '"')
            strcpy(HTDOCS,Ptr+1);
          else
            strcpy(HTDOCS,Ptr);

          if (HTDOCS[strlen(HTDOCS)-1] == '"')
            HTDOCS[strlen(HTDOCS)-1] = '\0';
        }
      while(GetHTDOCS() != 1)
        {
          printf("\n");
        }
      while (IsFileWriteable(HTDOCS) == 0)
        {
          printf(STEXT_INSTALLWWW17,HTDOCS);
          strcpy(HTDOCS,"");
          GetHTDOCS();
        }

      if((Ptr = GetFirstParam("ScriptAlias ")) != NULL)
        {
          if (*Ptr == '"')
            strcpy(CGIBIN,Ptr+1);
          else
            strcpy(CGIBIN,Ptr);

          Ptr = strchr(CGIBIN,' ');
          if (*(Ptr+1) == '"')
            strcpy(CGIBINPATH,Ptr+2);
          else
            strcpy(CGIBINPATH,Ptr+1);
          *Ptr = '\0';

          if (CGIBIN[strlen(CGIBIN)-1] == '"')
            CGIBIN[strlen(CGIBIN)-1] = '\0';

          if (CGIBINPATH[strlen(CGIBINPATH)-1] == '"')
            CGIBINPATH[strlen(CGIBINPATH)-1] = '\0';

        }
      while(GetCGIBIN() != 1)
        {
          printf("\n");
        }


      while(GetCGIBINPATH() != 1)
        {
          printf("\n");
        }
      while (IsFileWriteable(CGIBINPATH) == 0)
        {
          printf(STEXT_INSTALLWWW17,CGIBINPATH);
          strcpy(CGIBINPATH,"");
          GetCGIBINPATH();
        }

      CloseIniFile();

  return 1;
}
/*------------------------------------------------------------------------------*/
void ServerIsNotApache()
{
  strcpy(SERVERNAME,"");

  while(GetSERVERNAME() != 1)
    {
      printf("\n");
    }

  strcpy(HTDOCS,"");
  while(GetHTDOCS() != 1)
    {
      printf("\n");
    }
  while (IsFileWriteable(HTDOCS) == 0)
    {
      printf(STEXT_INSTALLWWW17,HTDOCS);
      strcpy(HTDOCS,"");
      GetHTDOCS();
    }

  strcpy(CGIBIN,"");
  while(GetCGIBIN() != 1)
    {
      printf("\n");
    }

  strcpy(CGIBINPATH,"");
  while(GetCGIBINPATH() != 1)
   {
     printf("\n");
   }
  while (IsFileWriteable(CGIBINPATH) == 0)
    {
      printf(STEXT_INSTALLWWW17,CGIBINPATH);
      strcpy(CGIBINPATH,"");
      GetCGIBINPATH();
    }
}
/*------------------------------------------------------------------------------*/
void MakeIndexhtm()
{
char Buffer[400];

FILE* File1;
FILE* File2;
char Line[501];

  if (CGIBIN[strlen(CGIBIN)-1] != '/')
    strcat(CGIBIN,"/");

  sprintf(Buffer,"%svshop/index.tmp",HTDOCS);
  if ((File1 = fopen(Buffer,"r")) != NULL)
    {
      sprintf(Buffer,"%svshop/%s",HTDOCS,INDEXHTML);
      if ((File2 = fopen(Buffer,"w")) != NULL)
        {
          while(!feof(File1))
            {
              if (fgets(Line,500,File1) != NULL)
                {
                  if (strncasecmp(Line,"REF1",4) == 0)
                    fprintf(File2,"<BASE HREF=\"http://%s/vshop/\">\n",SERVERNAME);
                  else
                  if (strncasecmp(Line,"REF2",4) == 0)
                    fprintf(File2,"<A HREF=\"%svshop?HTML=door.htm\">\n",CGIBIN);
                  else
                  if (strncasecmp(Line,"REF3",4) == 0)
                    fprintf(File2,"<A HREF=\"%svshop?HTML=admin%%2Flogin.htm\">\n",CGIBIN);
                  else
                  if (strncasecmp(Line,"REF4",4) == 0)
                    fprintf(File2,"<A HREF=\"%svshop?HTML=admin%%2Flogin.htm&CONFIG=TRUE\">\n",CGIBIN);
                  else
                    fprintf(File2,Line);
                }
            }
          fclose(File2);
        }
      fclose(File1);
    }
}
/*------------------------------------------------------------------------------*/
void MakeVSConf()
{
FILE* File;


  if ((File = fopen("/etc/vshop.conf","w")) != NULL)
    {
      fprintf(File,"DBROOT %s\n", DBROOT);
      fprintf(File,"VSHOP_DBUSER VSHOP\n");
      fprintf(File,"VSHOP_DBPASSWORD VSHOP\n");
      fprintf(File,"VSHOP_KEY %s\n", KEY);
      fprintf(File,"VSHOP_TEMPLATEROOT %svshop\n", HTDOCS);
      fprintf(File,"VSHOP_BASEREF http://%s/vshop\n", SERVERNAME);
      fprintf(File,"VSHOP_CGIBIN http://%s%svshop\n", SERVERNAME,CGIBIN);
      fprintf(File,"#VSHOP_SECUREBASEREF https://%s/vshop...\n", SERVERNAME);
      fprintf(File,"#VSHOP_SECURECGIBIN https://%s%svshop...\n", SERVERNAME,CGIBIN);
      fprintf(File,"VSHOP_PROFILING 1\n");
      fprintf(File,"VSHOP_TIMEOUT 20\n");
      fprintf(File,"#VSHOP_TIMEOUTURL http://%s/vshop/...\n", SERVERNAME);
      fprintf(File,"#VSHOP_DBERRORURL http://%s/vshop/...\n", SERVERNAME);
      fprintf(File,"VSHOP_RTF application/rtf\n");
      fprintf(File,"VSHOP_MAILDIR /var/tmp\n");
      fprintf(File,"#VSHOP_MAILSERVER your.mail.server\n");
      fprintf(File,"#VSHOP_MAILFROM vshop@%s\n",SERVERNAME);
      fprintf(File,"#VSHOP_MAILTO orders@%s\n",SERVERNAME);
      fprintf(File,"VSHOP_EURODM 0.5112918811962\n");
      fprintf(File,"VSHOP_EUROUSD 0.8637816360024\n");
      fclose(File);
    }
}
/*------------------------------------------------------------------------------*/
void MakeVSAdminConf()
{
FILE* File;

  
  if ((File = fopen("/opt/vshop/vsadmin.conf","w")) != NULL)
    {
      fprintf(File,"DBROOT %s\n", DBROOT);
      fprintf(File,"HTDOCS %s\n", HTDOCS);
      fprintf(File,"WWWUSER %s\n", WWWUSER);
      fprintf(File,"INDEXHTML %s\n", INDEXHTML);
      fprintf(File,"CGIBIN %s\n", CGIBIN);
      fprintf(File,"CGIBINPATH %s\n", CGIBINPATH);
      fprintf(File,"SERVERNAME %s\n", SERVERNAME);
      fprintf(File,"SHOPNAME vshop\n");
      fclose(File);
    }
}
/*------------------------------------------------------------------------------*/
void MakeVShopDB()
{
FILE* File;
char Buffer[400];

  sprintf(Buffer,"%svshopdb",DBROOT);
  if ((File = fopen(Buffer,"w")) != NULL)
    {
      fprintf(File,"#! /bin/sh\n");
      #if defined (SUN) || defined (HPUX)
      #else
        fprintf(File,"export DBROOT=%s\n",DBROOT);
      #endif
      fprintf(File,"cd %s\n",DBROOT);
      fprintf(File,"\n");
      fprintf(File,"case \"$1\" in\n");
	    fprintf(File,"\tstart)\n");
		  fprintf(File,"\t\t# echo \"Starting VShop ADABAS D remote SQL.\";\n");
      fprintf(File,"\t\t# su %s -c \"DBROOT=%s %sbin/x_server start\";\n",DBUSER,DBROOT,DBROOT);
		  fprintf(File,"\t\techo \"Starting VShop ADABAS D.\";\n");
      fprintf(File,"\t\t#su %s -c \"DBROOT=%s %sbin/x_clear vshop\";\n",DBUSER,DBROOT,DBROOT);
		  fprintf(File,"\t\tsu %s -c \"DBROOT=%s %sbin/x_start vshop\";\n",DBUSER,DBROOT,DBROOT);
		  fprintf(File,"\t\tsu %s -c \"DBROOT=%s %sbin/xutil -d vshop -u control,control restart\";\n",DBUSER,DBROOT,DBROOT);
	    fprintf(File,"\t;;\n");
	    fprintf(File,"\tstop)\n");
		  fprintf(File,"\t\t# echo \"Shutting down VShop ADABAS D remote SQL.\";\n");
		  fprintf(File,"\t\t# su %s -c \"%sbin/x_server stop\";\n",DBUSER,DBROOT);
		  fprintf(File,"\t\techo \"Shutting down VShop ADABAS D.\";\n");
		  fprintf(File,"\t\tsu %s -c \"DBROOT=%s %sbin/xutil -d vshop -u control,control shutdown\";\n",DBUSER,DBROOT,DBROOT);
		  fprintf(File,"\t\tsu %s -c \"DBROOT=%s %sbin/x_stop vshop\";\n",DBUSER,DBROOT,DBROOT);
	    fprintf(File,"\t;;\n");
	    fprintf(File,"\t*)\n");
	    fprintf(File,"\techo \"Usage $0 {start|stop}\";\n");
      fprintf(File,"esac\n");
      fprintf(File,"\n");
      fprintf(File,"exit 1\n");
      fclose(File);
    }
}
/*------------------------------------------------------------------------------*/
void Install()
{
  Init();

  chdir(INSTALLPATH);

  if (IsRoot() == 0)
    {
      printf(STEXT_INSTALL3);
      printf("\n");
      exit(1);
    }
}
/*------------------------------------------------------------------------------*/
void main(int argc, char *argv[])
{
char Buffer[200];
char SPVSHOP[200];
char SPVSLCOMPILE[200];
char* Ptr;

  strcpy(INSTALLPATH,argv[0]);
  Ptr = strrchr(INSTALLPATH,'/');
  if (Ptr != NULL)
    *(Ptr+1) = '\0';

  chdir(INSTALLPATH);
  strcpy(INSTALLPATH,getcwd(INSTALLPATH,199));

  if (IsRoot() == 0)
    {
      printf(TEXT_ERRORINSTALL6);
      printf("\n");
      exit(1);
    }

  Init();

  if (Ueberschreiben() == 0)
    exit(2);

#ifndef WITHOUT_ADABAS
  if (IsAdabasInstalled() == 1)
    {
#endif

      while(GetDBUSER() != 1)
        {
          printf("\n");
        }

      while (getpwnam(DBUSER) == NULL)
        {
          printf(STEXT_INSTALLADABAS11,DBUSER);
          strcpy(DBUSER,"");

         while(GetDBUSER() != 1)
          {
            printf("\n");
          }
        }

      while(GetDBROOT() != 1)
        {
          printf("\n");
        }

      while (IsFileWriteable(DBROOT) == 0)
        {
          printf(STEXT_INSTALLWWW17,DBROOT);
          strcpy(DBROOT,"");
          GetDBROOT();
        }
#ifndef WITHOUT_ADABAS
    }
  else
    {
      InstallAdabas();
    }
#endif

  chdir(INSTALLPATH);

  strcpy(INDEXHTML,"index.html");

  if (IsServerApache() == 1)
    {
      ServerIsApache();
    }
  else
    ServerIsNotApache();

  #ifndef DEMOVERSION
    while (GetKEY() == 0)
      {
      }
    if (strcmp(KEY,"") == 0)
      strcpy(KEY,"NO_KEY");
  #else
    /* Expired am 1.8.1999 */
    /*strcpy(KEY,"D-srnqvktgilcfggvh");*/

    /* Expired am 1.12.1999 */
    strcpy(KEY,"D-kmhftvijuicfggvh");

  #endif 

  chdir(INSTALLPATH);

  printf(TEXT_INSTALL5);
  printf("\n");

  if (CGIBINPATH[strlen(CGIBINPATH)-1] != '/')
    strcat(CGIBINPATH,"/");

  #if defined (SUN) || defined (HPUX)
    strcpy(SPVSHOP,"vshop/engine/sun/vshop");
    strcpy(SPVSLCOMPILE,"vshop/engine/sun/vsl-compile");
  #else
    if (FileExists("/lib/libc.so.6") == 1)
      {
        strcpy(SPVSHOP,"vshop/engine/libc6/vshop");
        strcpy(SPVSLCOMPILE,"vshop/engine/libc6/vsl-compile");
      }
    else
      {
        strcpy(SPVSHOP,"vshop/engine/libc5/vshop");
        strcpy(SPVSLCOMPILE,"vshop/engine/libc5/vsl-compile");
      }
  #endif

  if (Copy(SPVSHOP, CGIBINPATH) == 0)
    exit(2);
  
  sprintf(Buffer,"%svshop",CGIBINPATH);

  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);

  chdir(INSTALLPATH);

  if (Copy(SPVSLCOMPILE, CGIBINPATH) == 0)
    exit(2);
  sprintf(Buffer,"%svsl-compile",CGIBINPATH);
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);

  if (Copy("./vshop/vshop", "/usr/spool/sql/config/") == 0)
    exit(2);
  ChangeOwner("/usr/spool/sql/config/vshop",DBUSER);
  ChangeMod("/usr/spool/sql/config/vshop");


  if (DBROOT[strlen(DBROOT)-1] != '/')
    strcat(DBROOT,"/");

  if (Copy("./vshop/vsdb.tgz", DBROOT) == 0)
    exit(2);

  if (HTDOCS[strlen(HTDOCS)-1] != '/')
    strcat(HTDOCS,"/");

  if (Copy("vshop/vshtml.tgz", HTDOCS) == 0)
    exit(2);

  printf(TEXT_INSTALL6);
  printf("\n");

  chdir(DBROOT);

  #if defined (SUN) || defined (HPUX)
    if (UnZip(DBROOT,"vsdb.tgz") == 0)
      exit(2);
    if (Untar(DBROOT,"vsdb.tar") == 0)
      exit(2);
  #else
    if (Untar(DBROOT,"vsdb.tgz") == 0)
      exit(2);
  #endif


  sprintf(Buffer,"%swrk/",DBROOT);
  ChangeOwner(Buffer,DBUSER);
  ChangeMod(Buffer);

  sprintf(Buffer,"%swrk/vshop/",DBROOT);
  ChangeOwner(Buffer,DBUSER);
  ChangeMod(Buffer);

  Delete(DBROOT,"vsdb.tgz");
/*
  #if defined (SUN) || defined (HPUX)
    Delete(DBROOT,"vsdb.tgz");
  #else
    Delete(DBROOT,"vsdb.tar");
  #endif
*/
  printf(TEXT_INSTALL7);
  printf("\n");

  sprintf(Buffer,"DBROOT=%s %spgm/putparam vshop RUNDIRECTORY %swrk/vshop",DBROOT,DBROOT,DBROOT);
  system(Buffer);

  sprintf(Buffer,"DBROOT=%s %sbin/x_param -d vshop -u control,control BCHECK",DBROOT,DBROOT);
  system(Buffer);

  printf(TEXT_INSTALL8);
  printf("\n");

  chdir(HTDOCS);
  #if defined (SUN) || defined (HPUX)
    if (UnZip(HTDOCS,"vshtml.tgz") == 0)
      exit(2);

    if (Untar(HTDOCS,"vshtml.tar") == 0)
      exit(2);
  #else
    if (Untar(HTDOCS,"vshtml.tgz") == 0)
      exit(2);
  #endif


  MakeVSConf();
  MakeIndexhtm();
  MakeVShopDB();

  sprintf(Buffer,"%svshopdb",DBROOT);
  ChangeModX(Buffer);

  #if defined (SUN) || defined (HPUX)
    if (Copy(Buffer, "/etc/rc2.d/S99vshopdb") == 0)
      exit(2);
    if (Copy(Buffer, "/etc/rc2.d/K99vshopdb") == 0)
      exit(2);
    if (Copy(Buffer, "/etc/rc3.d/S99vshopdb") == 0)
      exit(2);
    if (Copy(Buffer, "/etc/rc3.d/K99vshopdb") == 0)
      exit(2);

    ChangeMod("/etc/rc2.d/S99vshopdb");
    ChangeMod("/etc/rc2.d/K99vshopdb");
    ChangeModX("/etc/rc2.d/S99vshopdb");
    ChangeModX("/etc/rc2.d/K99vshopdb");

    ChangeMod("/etc/rc3.d/S99vshopdb");
    ChangeMod("/etc/rc3.d/K99vshopdb");
    ChangeModX("/etc/rc3.d/S99vshopdb");
    ChangeModX("/etc/rc3.d/K99vshopdb");

  #else
    if (IsFileWriteable("/sbin/init.d/"))
      {
        if (Copy(Buffer, "/sbin/init.d/") == 0)
          exit(2);

        system("ln -f -s /sbin/init.d/vshopdb /sbin/init.d/rc2.d/S99vshopdb");
        system("ln -f -s /sbin/init.d/vshopdb /sbin/init.d/rc2.d/K99vshopdb");
        system("ln -f -s /sbin/init.d/vshopdb /sbin/init.d/rc3.d/S99vshopdb");
        system("ln -f -s /sbin/init.d/vshopdb /sbin/init.d/rc3.d/K99vshopdb");


        ChangeMod("/sbin/init.d/rc2.d/S99vshopdb");
        ChangeMod("/sbin/init.d/rc2.d/K99vshopdb");
        ChangeMod("/sbin/init.d/rc3.d/S99vshopdb");
        ChangeMod("/sbin/init.d/rc3.d/K99vshopdb");
        ChangeMod("/sbin/init.d/vshopdb");
        ChangeModX("/sbin/init.d/rc2.d/S99vshopdb");
        ChangeModX("/sbin/init.d/rc2.d/K99vshopdb");
        ChangeModX("/sbin/init.d/rc3.d/S99vshopdb");
        ChangeModX("/sbin/init.d/rc3.d/K99vshopdb");
        ChangeModX("/sbin/init.d/vshopdb");
      }
    else
    if (IsFileWriteable("/etc/rc.d/init.d/"))
      {
        if (Copy(Buffer, "/etc/rc.d/init.d/") == 0)
          exit(2);

        system("ln -f -s /etc/rc.d/init.d/vshopdb /etc/rc.d/rc2.d/S99vshopdb");
        system("ln -f -s /etc/rc.d/init.d/vshopdb /etc/rc.d/rc2.d/K99vshopdb");
        system("ln -f -s /etc/rc.d/init.d/vshopdb /etc/rc.d/rc3.d/S99vshopdb");
        system("ln -f -s /etc/rc.d/init.d/vshopdb /etc/rc.d/rc3.d/K99vshopdb");
        ChangeMod("/etc/rc.d/rc2.d/S99vshopdb");
        ChangeMod("/etc/rc.d/rc2.d/K99vshopdb");
        ChangeMod("/etc/rc.d/rc3.d/S99vshopdb");
        ChangeMod("/etc/rc.d/rc3.d/K99vshopdb");
        ChangeMod("/etc/rc.d/init.d/vshopdb");
        ChangeModX("/etc/rc.d/rc2.d/S99vshopdb");
        ChangeModX("/etc/rc.d/rc2.d/K99vshopdb");
        ChangeModX("/etc/rc.d/rc3.d/S99vshopdb");
        ChangeModX("/etc/rc.d/rc3.d/K99vshopdb");
        ChangeModX("/etc/rc.d/init.d/vshopdb");
      }
    #endif
  
  sprintf(Buffer,"%svshop/",HTDOCS);
  ChangeOwner(Buffer,WWWUSER);
  ChangeModRW(Buffer);
  
  #if defined (SUN) || defined (HPUX)
    Delete(HTDOCS,"vshtml.tar");
  #else
    Delete(HTDOCS,"vshtml.tgz");
  #endif

  #ifdef VSHOP_PROVIDER

    chdir(INSTALLPATH);

    mkdir("/opt",775);
    mkdir("/opt/vshop",775);
    mkdir("/opt/vshop/engine",775);

    if (FileExists("/opt/vshop"))
      {
        if (Copy(SPVSHOP, "/opt/vshop/engine") == 0)
          exit(2);
        ChangeOwner("/opt/vshop/engine/vshop",WWWUSER);
        ChangeMod("/opt/vshop/engine/vshop");
        ChangeModX("/opt/vshop/engine/vshop");
        
        if (Copy(SPVSLCOMPILE, "/opt/vshop/engine") == 0)
          exit(2);
        ChangeOwner("/opt/vshop/engine/vsl-compile",WWWUSER);
        ChangeMod("/opt/vshop/engine/vsl-compile");
        ChangeModX("/opt/vshop/engine/vsl-compile");

        if (Copy("vshop/vsadmin", "/opt/vshop") == 0)
          exit(2);

        if (Copy("vshop/createtabs", "/opt/vshop") == 0)
          exit(2);

        if (Copy("vshop/vshtmlp.tgz", "/opt/vshop/vshtml.tgz") == 0)
          exit(2);

        if (! FileExists("/opt/vshop/vsadmin.conf"))
          MakeVSAdminConf();
      }
  #endif

  sprintf(Buffer,"%svshopdb start",DBROOT);
  system(Buffer);
  printf(TEXT_ERRORINSTALL3);

  sprintf(Buffer,TEXT_ENDE1,DBROOT);
  printf(Buffer);
  sprintf(Buffer,TEXT_ENDE2,HTDOCS);
  printf(Buffer);
  sprintf(Buffer,TEXT_ENDE3,SERVERNAME);
  printf(Buffer);
  printf(TEXT_ENDE4);
  printf(TEXT_ENDE5);
  printf(TEXT_ENDE6);
  printf(TEXT_ENDE7);
  printf("\n");
}

