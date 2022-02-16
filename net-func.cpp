#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#include <dirent.h>
#elif HPUX 
#include "inifile.h"
#else
#include "win32tools.h"
#include "inifile.h"
//  #include <windows.h>
//  #include <winreg.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include "vsl.h"
#include "hfct.h"
#include "cgic.h"
#include "error.h"

#ifndef DB_ORACLE
  #include "adabas.h"
#else
  #include "oracle.h"
#endif

#include "db.h"
#include "vsl-exec.h"
#include "vslanguage.h"
#include "shop-func.h"
#include "inifile.h"
#include "skey.h"
#include "smtp.h"

#include "net-func.h"

#ifdef __cplusplus 
extern "C" {
#endif

#ifdef DEBUG
#undef DEBUG
#endif

  char MailFrom[80];
  char MailTo[80];
  char MailSubject[80];
  char MailServer[80];
  char MailFile[250];


  /*--------------------------------------------------------------------------------*/

t_Value base_getipaddr(t_Value *arg_list)
{
t_Value ret_val;
unsigned int IP;
char IPAddr[20];

  IP = gethostid();
  ipaddr2str(IPAddr,IP);

  ret_val.sValue = (char*) malloc(strlen(IPAddr)+1);
  strcpy(ret_val.sValue,IPAddr);
  ret_val.typ = String_Value;

  return ret_val;
}

  /*--------------------------------------------------------------------------------*/
  t_Value base_sendmail(t_Value *arg_list)
  {
    t_Value ret_val;
    FILE* File;
    char Buffer[100];
    char* MailBuffer;
    long ToRead;
    char* Ptr;
#ifdef WIN32
    DWORD dw;
    HKEY 	m_hKey;
    DWORD dwType;
    DWORD dwSize = 400;
    char RegistryKey[15];
#endif
    char DEBUG[5];
    int Debug;
 
    Debug = (cgiFormString("DEBUG",DEBUG,5) != cgiFormNotFound);

    ret_val.typ = Bool_Value;
    ret_val.nValue = 0;

    strcpy(MailFrom,"");
    strcpy(MailTo,"");
    strcpy(MailFile,"");
    strcpy(MailServer,"");

    if (arg_list[0].typ == String_Value)
      {
	strcpy(MailFile,arg_list[0].sValue);
	if (arg_list[1].typ == String_Value)
	  {
	    strcpy(MailSubject,arg_list[1].sValue);
	    if (arg_list[2].typ == String_Value)
	      {
		strcpy(MailTo,arg_list[2].sValue);
		if (arg_list[3].typ == String_Value)
		  {
		    strcpy(MailFrom,arg_list[3].sValue);
		    if (arg_list[4].typ == String_Value)
		      {
			strcpy(MailServer,arg_list[4].sValue);
		      }
		    else
		      {
			if ((Ptr = DBGetConfig("MAILSERVER")) != NULL)
			  strcpy(MailServer,Ptr);
                        else
                          RuntimeError1("%%%%sendmail[...] - Can't get MAILSERVER from SYSTEMPREFERENCES");
		      }
		  }
		else
		  {
		    if ((Ptr = DBGetConfig("MAILSERVER")) != NULL)
		      strcpy(MailServer,Ptr);
		    else
		      RuntimeError1("%%%%sendmail[...] - Can't get MAILSERVER from SYSTEMPREFERENCES");

		    if ((Ptr = DBGetConfig("MAILFROM")) != NULL)
		      strcpy(MailFrom,Ptr);
		    else
		      RuntimeError1("%%%%sendmail[...] - Can't get MAILFROM from SYSTEMPREFERENCES");
		  }
	      }
	    else
	      {
		if ((Ptr = DBGetConfig("MAILSERVER")) != NULL)
		  strcpy(MailServer,Ptr);
		else
		  RuntimeError1("%%%%sendmail[...] - Can't get MAILSERVER from SYSTEMPREFERENCES");

		if ((Ptr = DBGetConfig("MAILFROM")) != NULL)
		  strcpy(MailFrom,Ptr);
		else
		  RuntimeError1("%%%%sendmail[...] - Can't get MAILFROM from SYSTEMPREFERENCES");

		if ((Ptr = DBGetConfig("MAILTO")) != NULL)
		  strcpy(MailTo,Ptr);
		else
		  RuntimeError1("%%%%sendmail[...] - Can't get MAILTO from SYSTEMPREFERENCES");
	      }
	  }
	else
	  {
	    RuntimeError1("%%%%sendmail[PARAMETER1,PARAMETER2,(...)] - PARAMETER2 must be STRING");
	  }
      }
    else
      {
	RuntimeError1("%%%%sendmail[PARAMETER1,PARAMETER2,(...)] - PARAMETER1 must be STRING");
      }

    if ((strcmp(MailFrom,"") != 0) && (strcmp(MailTo,"") != 0) &&
	(strcmp(MailFile,"") != 0) && (strcmp(MailServer,"") != 0))
      {

    
#ifdef WIN32
	if ((File = fopen( MailFile, "rt")) != NULL)
#else
	  if ((File = fopen( MailFile, "r")) != NULL)
#endif
	    {

	      fseek(File,0,SEEK_END);
	      ToRead = ftell(File);

	      if ((MailBuffer = (char*) malloc(ToRead +1)) != NULL)
		{
		  memset(MailBuffer,'\0',ToRead +1);
		  fseek(File,0,SEEK_SET);
		  if (fread(MailBuffer,1,ToRead,File) == ToRead)
		    {
		      if (Debug == 1)
			ret_val.nValue =  (smtp_sm_mail (MailServer, MailFrom, MailTo, MailSubject, &MailBuffer,NULL,SMTP_8BIT,cgiOut) != -1);
		      else
			ret_val.nValue =  (smtp_sm_mail (MailServer, MailFrom, MailTo, MailSubject, &MailBuffer,NULL,SMTP_8BIT,NULL) != -1);
		    }

		  free(MailBuffer);
		}

	      ret_val.nValue = 1;
	      fclose(File);
	    }
	  else
	    {
	      RuntimeError1("%%%%sendmail[PARAMETER1,PARAMETER2,(...)] - File not found");
	    }
      }

    return ret_val;
  }
  /*--------------------------------------------------------------------------------*/
  t_Value base_tomailbegin(t_Value *arg_list)
  {
    t_Value ret_val;
    char Filename[255];
    char ID[20];
    char* Ptr;
    char Buffer[100];
#ifdef WIN32
    DWORD dw;
    HKEY 	m_hKey;
    DWORD dwType;
    DWORD dwSize = 400;
    char RegistryKey[15];
#endif

    cgiFormString("ID",ID,20);

    ret_val.typ = Bool_Value;
    ret_val.nValue = 0;

    strcpy(MailFrom,"");
    strcpy(MailTo,"");
    strcpy(MailFile,"");
    strcpy(MailServer,"");

#ifdef WIN32
    strcpy(RegistryKey,PROGRAMNAME);

    if ((Ptr = strchr(RegistryKey,'.')) != NULL)
      *Ptr = '\0';

#ifdef VSL_ENGINE
    sprintf(Buffer,"SOFTWARE\\intraDAT GmbH\\VSL\\%s",RegistryKey);
#else
    sprintf(Buffer,"SOFTWARE\\intraDAT GmbH\\VShop\\%s",RegistryKey);
#endif
    if (RegCreateKeyEx (HKEY_LOCAL_MACHINE, Buffer, 0L, NULL,	REG_OPTION_VOLATILE, KEY_READ, 
                        NULL, &m_hKey, &dw) != ERROR_SUCCESS)
      {
        RuntimeError1("%%%%sendmail[...] - Can't open Registry");
        return ret_val;
      }
#else
#ifdef VSL_ENGINE
    if (OpenIniFile("/etc/vsl.conf") != 1)
#else
      if (OpenIniFile("/etc/vshop.conf") != 1)
#endif
	{
	  RuntimeError1("%%%%sendmail[...] - Can't open configuration file");
	  return ret_val;
	}
#endif

    if (arg_list[0].typ == String_Value)
      {
	strcpy(MailSubject,arg_list[0].sValue);
	if (arg_list[1].typ == String_Value)
	  {
	    strcpy(MailTo,arg_list[1].sValue);
	    if (arg_list[2].typ == String_Value)
	      {
		strcpy(MailFrom,arg_list[2].sValue);
		if (arg_list[3].typ == String_Value)
		  {
		    strcpy(MailServer,arg_list[3].sValue);
		  }
		else
		  {
		    if ((Ptr = DBGetConfig("MAILSERVER")) != NULL)
		      strcpy(MailServer,Ptr);
		    else
		      RuntimeError1("%%%%sendmail[...] - Can't get MAILSERVER from SYSTEMPREFERENCES");
		  }
	      }
	    else
	      {
		if ((Ptr = DBGetConfig("MAILSERVER")) != NULL)
		  strcpy(MailServer,Ptr);
		else
		  RuntimeError1("%%%%sendmail[...] - Can't get MAILSERVER from SYSTEMPREFERENCES");

		if ((Ptr = DBGetConfig("MAILFROM")) != NULL)
		  strcpy(MailFrom,Ptr);
		else
		  RuntimeError1("%%%%sendmail[...] - Can't get MAILFROM from SYSTEMPREFERENCES");
	      }
	  }
	else
	  {
	    if ((Ptr = DBGetConfig("MAILSERVER")) != NULL)
	      strcpy(MailServer,Ptr);
	    else
	      RuntimeError1("%%%%sendmail[...] - Can't get MAILSERVER from SYSTEMPREFERENCES");

	    if ((Ptr = DBGetConfig("MAILFROM")) != NULL)
	      strcpy(MailFrom,Ptr);
	    else
	      RuntimeError1("%%%%sendmail[...] - Can't get MAILFROM from SYSTEMPREFERENCES");

	    if ((Ptr = DBGetConfig("MAILTO")) != NULL)
	      strcpy(MailTo,Ptr);
	    else
	      RuntimeError1("%%%%sendmail[...] - Can't get MAILTO from SYSTEMPREFERENCES");
	  }
      }
    else
      {
	RuntimeError1("%%%%tomailbegin[PARAMETER1,(...)] - PARAMETER1 must be STRING");
      }

#ifdef WIN32
    RegCloseKey (m_hKey);
#else
    CloseIniFile();
#endif

    if ((strcmp(MailFrom,"") != 0) && (strcmp(MailTo,"") != 0) && (strcmp(MailServer,"") != 0))
      {

	if (TEMPLATEROOT[strlen(TEMPLATEROOT)-1] != '/')
	  sprintf(Filename,"%s/%s.mail",TEMPLATEROOT,ID);
	else
	  sprintf(Filename,"%s%s.mail",TEMPLATEROOT,ID);

	if ((cgiOut = fopen( Filename, "w")) == NULL)
	  {
	    cgiOut = stdout;
	  }
	else
	  ret_val.nValue = 1;
      }

    return ret_val;
  }
  /*--------------------------------------------------------------------------------*/
  t_Value base_tomailend(t_Value *arg_list)
  {
    t_Value ret_val;
    char Filename[255];
    char ID[20];
    FILE* File;
    char* MailBuffer;
    long ToRead;
    char DEBUG[5];
    int Debug = 0;

    Debug = (cgiFormString("DEBUG",DEBUG,5) != cgiFormNotFound);

    ret_val.typ = Bool_Value;
    ret_val.nValue = 0;
    cgiFormString("ID",ID,20);

    if (cgiOut != stdout)
      {
	fclose(cgiOut);
	cgiOut = stdout;

	if (TEMPLATEROOT[strlen(TEMPLATEROOT)-1] != '/')
	  sprintf(Filename,"%s/%s.mail",TEMPLATEROOT,ID);
	else
	  sprintf(Filename,"%s%s.mail",TEMPLATEROOT,ID);

#ifdef WIN32
        if ((File = fopen( Filename, "rt")) != NULL)
#else
	  if ((File = fopen( Filename, "r")) != NULL)
#endif
	    {

	      fseek(File,0,SEEK_END);
	      ToRead = ftell(File);

	      if ((MailBuffer = (char*) malloc(ToRead +1)) != NULL)
		{
		  memset(MailBuffer,'\0',ToRead +1);
		  fseek(File,0,SEEK_SET);
		  if (fread(MailBuffer,1,ToRead,File) == ToRead)
		    {
		      if (Debug == 1)
                        ret_val.nValue =  (smtp_sm_mail (MailServer, MailFrom, MailTo, MailSubject, &MailBuffer,NULL,SMTP_8BIT,cgiOut) != -1);
		      else
			ret_val.nValue =  (smtp_sm_mail (MailServer, MailFrom, MailTo, MailSubject, &MailBuffer,NULL,SMTP_8BIT,NULL) != -1);
		    }
		  free(MailBuffer);
		}
	    }
	fclose(File);
	remove(Filename);
      }

    return ret_val;
  }


#ifdef __cplusplus 
}
#endif
