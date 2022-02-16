#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>

#include <iostream.h>
#include <sstream>

char DBROOT[200];
char DBUSER[200];
char INSTALLPATH[200];

char HTTPDCONF[200];
char CGIBIN[200];
char CGIBINPATH[200];
char HTDOCS[200];
char INDEXHTML[200];
char SRM[200];
char SERVERNAME[200];
char WWWUSER[200];

char KEY[50];
int FORCE=0;
#define BUFFERLENGTH 1024

//
// Returns the system's hostname.
//
char *hostname() {
  static char buf[ 128 ];
  gethostname( buf, 128 );
  return buf;
}

//
// Returns the system's ip address as a string.
//
char *hostIp() {
  static char ipStr[ 16 ];

  //
  // get host ip
  //
  long ip = gethostid();

  //
  // convert it to a human readable string
  //
  int quads[ 4 ];
  for ( int i = 0; i < 4; i++ )
    quads[ i ] = ( ip >> ( i * 8 ) ) & ( unsigned long )0x000000FF;

  sprintf( ipStr,
           "%u.%u.%u.%u",
           quads[ 2 ], quads[ 3 ], quads[ 0 ], quads[ 1 ] );

  return ipStr;
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
int ChangeMod(char* File)
{
char Buffer[200];

  sprintf(Buffer,"chmod -R 775 %s",File);

  return (system(Buffer) == 0);
}

/*------------------------------------------------------------------------------*/
int Copy(char* Source, char* Destination)
{
char Buffer[200];

  sprintf(Buffer,"cp %s %s",Source,Destination);

  return (system(Buffer) == 0);
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
                    fprintf(File2,"http://%s/vshop/\n",SERVERNAME);
                  else
                  if (strncasecmp(Line,"REF2",4) == 0)
                    fprintf(File2,"%svshop?HTML=vshop.htm\n",CGIBIN);
                  else
                  if (strncasecmp(Line,"REF3",4) == 0)
                    fprintf(File2,"%svshop?HTML=admin%%2Flogin.htm\n",CGIBIN);
                  else
                  if (strncasecmp(Line,"REF4",4) == 0)
                    fprintf(File2,"%svshop?HTML=admin%%2Flogin.htm&NEXT=admin/wizards/adm_wizard_conf_list.htm\n",CGIBIN);
                  else
                  if (strncasecmp(Line,"REF5",4) == 0)
                    fprintf(File2,"<form action=\"%svshop?HTML=vshop.htm\" method=\"post\">\n",CGIBIN);
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
void MakeAdminIndexhtm()
{
char Buffer[400];

FILE* File1;
FILE* File2;
char Line[501];

  if (CGIBIN[strlen(CGIBIN)-1] != '/')
    strcat(CGIBIN,"/");

  sprintf(Buffer,"%svshop/admin/index.tmp",HTDOCS);
  if ((File1 = fopen(Buffer,"r")) != NULL)
    {
      sprintf(Buffer,"%svshop/admin/%s",HTDOCS,INDEXHTML);
      if ((File2 = fopen(Buffer,"w")) != NULL)
        {
          while(!feof(File1))
            {  
              if (fgets(Line,500,File1) != NULL)
                {
                  if (strncasecmp(Line,"REF1",4) == 0)
                    fprintf(File2,"http://%s/vshop/\n",SERVERNAME);
                  else
                  if (strncasecmp(Line,"REF2",4) == 0)
                    fprintf(File2,"%svshop?HTML=vshop.htm\n",CGIBIN);
                  else
                  if (strncasecmp(Line,"REF3",4) == 0)
                    fprintf(File2,"%svshop?HTML=admin%%2Flogin.htm\n",CGIBIN);
                  else
                  if (strncasecmp(Line,"REF4",4) == 0)
                    fprintf(File2,"%svshop?HTML=admin%%2Flogin.htm&NEXT=admin/wizards/adm_wizard_conf_list.htm\n",CGIBIN);
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
void MakeVSConf(char* Filename)
{
FILE* File;

  
  if ((File = fopen(Filename,"w")) != NULL)
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
      fprintf(File,"VSHOP_SECUREBASEREF http://%s/vshop\n", SERVERNAME);
      fprintf(File,"VSHOP_SECURECGIBIN http://%s/%s/vshop\n", SERVERNAME,CGIBIN);
      fclose(File);
    }
}
/*------------------------------------------------------------------------------*/
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
      fprintf(File,"#\n");
      fprintf(File,"# /etc/init.d/vshopdb\n");
      fprintf(File,"#\n");
      fprintf(File,"#\n");
      fprintf(File,"### BEGIN INIT INFO\n");
      fprintf(File,"# Provides:       vshopdb\n");
      fprintf(File,"# Required-Start: $remote_fs\n");
      fprintf(File,"# Required-Stop:  \n");
      fprintf(File,"# Default-Start:  2 3 5\n");
      fprintf(File,"# Default-Stop:\n");
      fprintf(File,"# Description:    Start the VShop database\n");
      fprintf(File,"### END INIT INFO\n");
      fprintf(File,"#\n");
      fprintf(File,"#\n");
      fprintf(File,". /etc/rc.status\n");
      fprintf(File,". /etc/rc.config\n");
      fprintf(File,"return=$rc_done\n");
      fprintf(File,"#\n");
      fprintf(File,"export DBROOT=%s\n",DBROOT);
      fprintf(File,"cd %s\n",DBROOT);
      fprintf(File,"\n");
      fprintf(File,"case \"$1\" in\n");
      fprintf(File,"\tstart)\n");
	  fprintf(File,"\t\t# echo \"Starting VShop ADABAS D remote SQL.\";\n");
      fprintf(File,"\t\t# su adabas -c \"%sbin/x_server start\";\n",DBROOT);
	  fprintf(File,"#\t\techo \"Starting VShop ADABAS D.\";\n");
      fprintf(File,"\t\t#su %s -c \"%sbin/x_clear vshop\";\n",DBUSER,DBROOT);
	  fprintf(File,"\t\tsu %s -c \"%sbin/x_start vshop \" || return=$rc_failed\n",DBUSER,DBROOT);
	  fprintf(File,"\t\tsu %s -c \"%sbin/xutil -d vshop -u control,control restart\" || return=$rc_failed\n",DBUSER,DBROOT);
      fprintf(File,"\t\techo -e \"$return\"\n");
      fprintf(File,"\t;;\n");
      fprintf(File,"\tstop)\n");
 	  fprintf(File,"\t\t# echo \"Shutting down VShop ADABAS D remote SQL.\";\n");
	  fprintf(File,"\t\t# su %s -c \"%sbin/x_server stop\";\n",DBUSER,DBROOT);
	  fprintf(File,"\t\techo -n \"Shutting down VShop ADABAS D.\"\n");
	  fprintf(File,"\t\tsu %s -c \"%sbin/xutil -d vshop -u control,control shutdown\" || return=$rc_failed;\n",DBUSER,DBROOT);
	  fprintf(File,"\t\tsu %s -c \"%sbin/x_stop vshop\" || return=$rc_failed\n",DBUSER,DBROOT);
      fprintf(File,"\t\techo -e \"$return\"\n");
      fprintf(File,"\t;;\n");
      fprintf(File,"\tclear)\n");
	  fprintf(File,"\t\t echo \"Clearing VShop ADABAS D.\";\n");
      fprintf(File,"\t\tsu %s -c \"%sbin/x_clear vshop\"  || return=$rc_failed;\n",DBUSER,DBROOT);
      fprintf(File,"\t\techo -e \"$return\"\n");
      fprintf(File,"\t;;\n");
      fprintf(File,"\t*)\n");
      fprintf(File,"\techo \"Usage $0 {start|stop|clear}\";\n");
      fprintf(File,"esac\n");           
      fprintf(File,"\n");
      fprintf(File,"test \"$return\" = \"$rc_done\" || exit 1\n");
      fprintf(File,"exit 0\n");
      fprintf(File,"\n");
      fprintf(File,"\n");
      fprintf(File,"\n");
      fclose(File);
   }
}
/*-------------------------------------------------------------------------------------*/






//
// mainline
//
int main( int argc, char **argv ) {

  //
  // set installation parameters
  //
  strcpy( DBROOT,     "/usr/lib/adabas/" );
  strcpy( DBUSER,     "adabas" );
  strcpy( HTTPDCONF,  "/etc" );
  strcpy( CGIBIN,     "/cgi-bin" );
  strcpy( CGIBINPATH, "/usr/local/httpd/cgi-bin/" );
  strcpy( HTDOCS,     "/usr/local/httpd/htdocs/" );
  strcpy( SERVERNAME, hostIp() );
  strcpy( WWWUSER,    "wwwrun" );
  strcpy( INDEXHTML,  "index.html" );

  //
  // call utility functions to do the dirty work
  //
  cout << "Generating index..." << flush;
  MakeIndexhtm();
  cout << "ok" << endl << flush;

  cout << "Generating admin index..." << flush;
  MakeAdminIndexhtm();
  cout << "ok" << endl << flush;

  cout << "Generating configuration file..." << flush;
  MakeVSConf( "/etc/vshop.conf" );
  ChangeOwner("/etc/vshop.conf",WWWUSER);
  ChangeMod("/etc/vshop.conf");
  cout << "ok" << endl << flush;

  cout << "Generating backup configuration file..." << flush;
  MakeVSConf( "/etc/vshop.conf.bak" );
  ChangeOwner("/etc/vshop.conf.bak",WWWUSER);
  ChangeMod("/etc/vshop.conf.bak");
  cout << "ok" << endl << flush;

  cout << "Generating service file..." << flush;
  MakeVShopDB();
  cout << "ok" << endl << flush;

  //
  // install service file
  //
  cout << "Installing service..." << flush;

  //
  // determine destination (do something better later)
  //
  char destination[ 128 ] = "/etc/init.d";
  strcpy(destination,"/etc/init.d");

  //
  // copy vshop service file to final destination
  //
  char fileName[ 128 ];
  sprintf( fileName, "%svshopdb", DBROOT );
  Copy( fileName, destination );

  //
  // create symbolic links
  //
  char cmd[ 256 ];

  //
  // start database
  //
  sprintf( cmd, "%s/vshopdb start", destination );
  system( cmd );
}
