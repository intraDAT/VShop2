#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pwd.h>
#include <sys/stat.h>


#include "language.h"
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
char KEY[20];
char WWWUSER[200];


#define BUFFERLENGTH 1024

/*------------------------------------------------------------------------------*/
/* TOOL Functions */
/*------------------------------------------------------------------------------*/
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
int Untar(char* Path, char* File)
{
char Buffer[200];
int Return;


  if (Path[strlen(Path)-1] != '/')
    sprintf(Buffer,"tar xzf %s/%s",Path,File);
  else
    sprintf(Buffer,"tar xzf %s%s",Path,File);
  Return = system(Buffer);

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
int IsRoot()
{
  return getuid() == 0; 
}
/*------------------------------------------------------------------------------*/
int ServerIsApache()
{
char* Ptr; 

  OpenIniFile(HTTPDCONF);

  if((Ptr = GetFirstParam("ServerName")) != NULL)
    {
      strcpy(SERVERNAME,Ptr);
    }
  else
    {
      char IPAddr[20];
      unsigned int IP;


      IP = gethostid();
      ipaddr2str(IPAddr,IP);
      strcpy(SERVERNAME,IPAddr);
    }

  if((Ptr = GetFirstParam("User")) != NULL)
    {
      strcpy(WWWUSER,Ptr);
    }
  else
    {
      strcpy(WWWUSER,"wwwrun");
    }

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
  else
    strcpy(HTDOCS,"/usr/local/httpd/htdocs");



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
  else
    {
      strcpy(CGIBIN,"/cgi-bin/");
      strcpy(CGIBINPATH,"/usr/local/httpd/cgi-bin/");
    }

  CloseIniFile();

  return 1;
}
/*------------------------------------------------------------------------------*/
void MakeVSConf()
{
FILE* File;

  if ((File = fopen("/etc/vshop.conf","w")) != NULL)
    {
      fprintf(File,"DBROOT %s\n", DBROOT);
      fprintf(File,"VSHOP_DBUSER VSHOP\n");
      fprintf(File,"VSHOP_DBPASSWORD esjduhplgbp\n");
      fprintf(File,"VSHOP_DBERRORURL http://%s/vshop/dberror.htm\n", SERVERNAME);
      fprintf(File,"VSHOP_TEMPLATEROOT %svshop\n", HTDOCS);
      fprintf(File,"VSHOP_BASEREF http://%s/vshop\n", SERVERNAME);
      fprintf(File,"VSHOP_CGIBIN http://%s%svshop\n", SERVERNAME,CGIBIN);
      fprintf(File,"VSHOP_VSHOP %svshop\n", CGIBINPATH); 
      fprintf(File,"VSHOP_CHDBPASSWD %svshop/admin/bin/vshoppasswd\n", HTDOCS); 
      fprintf(File,"#VSHOP_SECUREBASEREF https://%s/vshop...\n", SERVERNAME);
      fprintf(File,"#VSHOP_SECURECGIBIN https://%s%svshop...\n", SERVERNAME,CGIBIN);
      fclose(File);
    }
}
/*------------------------------------------------------------------------------*/
void MakeVShopDB()
{
FILE* File;

  if ((File = fopen("/sbin/init.d/vshopdb","w")) != NULL)
    {
      fprintf(File,"#! /bin/sh\n");
      fprintf(File,"# Copyright (c) 1999 intraDAT Gmbh, Germany.  All rights reserved.\n");
      fprintf(File,"#\n");
      fprintf(File,"# /sbin/init.d/vshopdb\n");
      fprintf(File,"#\n");
      fprintf(File,"\n");
      fprintf(File,". /etc/rc.config\n");
      fprintf(File,"\n");
      fprintf(File,"# Determine the base and follow a runlevel link name.\n");
      fprintf(File,"base=${0##*/}\n");
      fprintf(File,"link=${base#*[SK][0-9][0-9]}\n");
      fprintf(File,"\n");
      fprintf(File,"\n");
      fprintf(File,"# Force execution if not called by a runlevel directory.\n");
      fprintf(File,"test $link = $base && START_VSHOPDB=\"yes\"\n");
      fprintf(File,"test \"$START_VSHOPDB\" = yes || exit 0\n");
      fprintf(File,"\n");
      fprintf(File,"\n");
      fprintf(File,"# The echo return value for success (defined in /etc/rc.config).\n");
      fprintf(File,"return=$rc_done\n");
      fprintf(File,"\n");
      fprintf(File,"\n");
      fprintf(File,"export DBROOT=%s\n",DBROOT);
      fprintf(File,"cd %s\n",DBROOT);
      fprintf(File,"\n");
      fprintf(File,"case \"$1\" in\n");
      fprintf(File,"\tstart)\n");
	  fprintf(File,"\t\t# echo \"Starting VShop ADABAS D remote SQL.\";\n");
      fprintf(File,"\t\t# su adabas -c \"%sbin/x_server start\";\n",DBROOT);
	  fprintf(File,"\t\techo \"Starting VShop ADABAS D.\";\n");
      fprintf(File,"\t\t#su %s -c \"%sbin/x_clear vshop\";\n",DBUSER,DBROOT);
	  fprintf(File,"\t\tsu %s -c \"%sbin/x_start vshop\";\n",DBUSER,DBROOT);
	  fprintf(File,"\t\tsu %s -c \"%sbin/xutil -d vshop -u control,control restart\";\n",DBUSER,DBROOT);
      fprintf(File,"\t\techo -e \"$return\"\n");
      fprintf(File,"\t;;\n");
      fprintf(File,"\tstop)\n");
	  fprintf(File,"\t\t# echo \"Shutting down VShop ADABAS D remote SQL.\";\n");
	  fprintf(File,"\t\t# su %s -c \"%sbin/x_server stop\";\n",DBUSER,DBROOT);
	  fprintf(File,"\t\techo \"Shutting down VShop ADABAS D.\";\n");
	  fprintf(File,"\t\tsu %s -c \"%sbin/xutil -d vshop -u control,control shutdown\";\n",DBUSER,DBROOT);
	  fprintf(File,"\t\tsu %s -c \"%sbin/x_stop vshop\";\n",DBUSER,DBROOT);
      fprintf(File,"\t\techo -e \"$return\"\n");
      fprintf(File,"\t;;\n");
      fprintf(File,"\t*)\n");
      fprintf(File,"\techo \"Usage $0 {start|stop}\";\n");
      fprintf(File,"esac\n");
      fprintf(File,"# Inform the caller not only verbosely and set an exit status.\n");
      fprintf(File,"test \"$return\" = \"$rc_done\" || exit 1\n");
      fprintf(File,"exit 0\n");
      fprintf(File,"exit 0\n");
      fclose(File);
    }
}
/*------------------------------------------------------------------------------*/
void main(int argc, char *argv[])
{
char Buffer[200];
char SPVSHOP[200];
char SPVSLCOMPILE[200];
char SPMTLCOMPILE[200];

  strcpy(INSTALLPATH,getcwd(INSTALLPATH,199));

  if (argc > 3)
    {
      strcpy(DBROOT,argv[1]);
      strcpy(DBUSER,argv[2]);
      strcpy(HTTPDCONF,argv[3]);
    }
  else
    {
      strcpy(DBROOT,"/usr/lib/adabas");
      strcpy(DBUSER,"adabas");
      if (IsFileWriteable("/etc/httpd/httpd.conf"))
        strcpy(HTTPDCONF,"/etc/httpd/httpd.conf");
      else
        strcpy(HTTPDCONF,"/etc/httpsd/httpd.conf");
    }

  strcpy(INDEXHTML,"index.html");

  ServerIsApache();

  chdir(INSTALLPATH);

  if (CGIBINPATH[strlen(CGIBINPATH)-1] != '/')
    strcat(CGIBINPATH,"/");

  strcpy(SPVSHOP,"vshop/engine/libc6/vshop");
  strcpy(SPVSLCOMPILE,"vshop/engine/libc6/vsl-compile");
  strcpy(SPMTLCOMPILE,"vshop/engine/libc6/mtl-compile");

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


  if (Copy(SPMTLCOMPILE, CGIBINPATH) == 0)
    exit(2);
  sprintf(Buffer,"%smtl-compile",CGIBINPATH);
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);


  if (Copy("./vshop/vshop", "/usr/spool/sql/config/") == 0)
    exit(2);

  ChangeOwner("/usr/spool/sql/config/vshop",DBUSER);
  ChangeMod("/usr/spool/sql/config/vshop");

  Copy("./vshop/rc.config.vshop", "/etc/");

  if (DBROOT[strlen(DBROOT)-1] != '/')
    strcat(DBROOT,"/");

  if (Copy("./vshop/vsdb.tgz", DBROOT) == 0)
    exit(2);

  if (HTDOCS[strlen(HTDOCS)-1] != '/')
    strcat(HTDOCS,"/");

  if (Copy("vshop/vshtml.tgz", HTDOCS) == 0)
    exit(2);

  chdir(DBROOT);
  if (Untar(DBROOT,"vsdb.tgz") == 0)
    exit(2);

  sprintf(Buffer,"%swrk/vshop/",DBROOT);
  ChangeOwner(Buffer,DBUSER);
  ChangeMod(Buffer);
  Delete(DBROOT,"vsdb.tgz");

  sprintf(Buffer,"DBROOT=%s %spgm/putparam vshop RUNDIRECTORY %swrk/vshop",DBROOT,DBROOT,DBROOT);
  system(Buffer);

  sprintf(Buffer,"DBROOT=%s %sbin/x_param -d vshop -u control,control BCHECK",DBROOT,DBROOT);
  system(Buffer);

  chdir(HTDOCS);
  if (Untar(HTDOCS,"vshtml.tgz") == 0)
    exit(2);

  sprintf(Buffer,"%svshop/",HTDOCS);
  ChangeOwner(Buffer,WWWUSER);
  ChangeModRW(Buffer);
  Delete(HTDOCS,"vshtml.tgz");

  MakeVSConf();

  MakeVShopDB();

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

  sprintf(Buffer,"/sbin/init.d/vshopdb start");
  system(Buffer);
}

