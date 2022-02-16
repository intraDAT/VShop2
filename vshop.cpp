#include <stdlib.h>


#ifndef WIN32
  #ifdef SUN
    #include <sys/time.h>
    #include <sys/resource.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
  #else
    #include <unistd.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
  #endif
#else
  #include <windows.h>
  #include "win32tools.h"
  #include <winreg.h>
#endif

#include <locale.h>
#include <string.h>
#include "vsl.h"
#include "cgic.h"
#include "vsl-exec.h"

#ifndef DB_ORACLE
  #include "adabas.h"
#else
  #include "oracle.h"
#endif

#include "hfct.h"
#include "error.h"
#include "base-func.h"
#include "db.h"
#ifndef VSL_ENGINE
  #include "shop-func.h"
#endif
#include "vslanguage.h"
#include "inifile.h"
#include "skey.h"

#include "vshop.h"
//#include "carp.h"

#ifndef WIN32
  #ifdef VSL_ENGINE
    #define INIFILE "/etc/vsl.conf"
  #else
    #define INIFILE "/etc/vshop.conf"
  #endif
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

int NCVersion;
int Remote = 0;

/*---------------------------------------------------------------------------------*/
void SetVariables(char* ID)
{
cgiFormEntry* Entry;
char Buffer[200];

  if ((Entry = cgiFormEntryGetFirst()) != NULL)
    {
      do
        {
          if (strcmp(Entry->attr,"ID") != 0)
            {
              sprintf(Buffer,"%s",Entry->value);
              if (!EngineDB->UpdateVar(ID,Entry->attr,Buffer))
                EngineDB->InsertVar(ID,Entry->attr,Entry->value);
            }
        }
      while ((Entry = cgiFormEntryGetNext()) != NULL);
    }
}
/*---------------------------------------------------------------------------------*/
int ISValidKey(char* Key)
{
#ifndef HPUX
unsigned int IP;
char IPAddr[20];
char NewKey[40];
char Buffer1[41];
char Buffer2[41];
char Buffer1b[41];
char Buffer2b[41];
char ProgramName[100];

#ifdef WIN32
  HOSTENT* Hostent;
  char HostName[101];
  char* Ptr;
  WORD wVersionRequested;      
  WSADATA wsaData;
#endif


#ifdef WIN32

  wVersionRequested = MAKEWORD( 2, 0 );
  WSAStartup( wVersionRequested, &wsaData ) ;
      
  if (gethostname(HostName,100) != 0)
    {
      printf(" Error : %i",WSAGetLastError());

      return 0;
    }

  if ((Hostent = gethostbyname (HostName)) == NULL)
    return 0;

  IP = ipstr2long(inet_ntoa (*(struct in_addr *)*Hostent->h_addr_list));
  ipaddr2str(IPAddr,IP);
  IP = ipstr2long(IPAddr);

  WSACleanup( );  

  strcpy(ProgramName,PROGRAMNAME);
  if ((Ptr = strchr(ProgramName,'.')) != NULL)
    *Ptr = '\0';

#else
  IP = gethostid();
  strcpy(ProgramName,PROGRAMNAME);
#endif

  if (strlen(ProgramName) > 6)
    {
      printf(ProgramName);
      HTMLError("Der Name der Engine ist ungültig lang !");
      return 0;
    }

  ipaddr2str(IPAddr,IP);
  IP = ipstr2long(IPAddr);


  IP -= SHOPVERSION;

  sprintf(Buffer1,"%10u",IP);
  strcat(Buffer1,ProgramName);
  strcpy(Buffer2,&Buffer1[8]);
  Buffer1[8]='\0';

  #ifndef WIN32
    strcpy(Buffer1b,crypt(Buffer1,"21"));
    strcpy(Buffer2b,crypt(Buffer2,"00"));
    strcpy(NewKey,Buffer1b);
    strcat(NewKey,Buffer2b);

    if (strcmp(NewKey,Key) == 0)
      return 1;

    strcpy(Buffer1b,crypt(Buffer1,"21"));
    strcpy(Buffer2b,crypt(Buffer2,"01"));
    strcpy(NewKey,Buffer1b);
    strcat(NewKey,Buffer2b);

    if (strcmp(NewKey,Key) == 0)
      {
        Remote = 1;
        return 1;
      }

    cgiHeaderContentType("text/html");
    char ErrorBuffer[400];
    sprintf(ErrorBuffer,TEXT_ERROR11,TEXT_VERSION,IPAddr);
    HTMLError(ErrorBuffer);

  #else
    strcat(Buffer1,Buffer2);
    if (cryptpassword (&Ptr, Buffer1, "BS20031998CW") != 0)
      return 1;
    strcpy(NewKey,Ptr);
    free(Ptr);
  #endif
    

#endif
  return 0;
}
/*---------------------------------------------------------------------------------*/
int IsExpired(char* Key)
{
time_t TimeT;
tm* Today;
char TimeBuffer[20];
int Ret;
char* Ptr;

  #ifndef VSHOP_DEMO
    return 1;
  #endif

  if ((Key[0] == 'N') && (Key[1]=='C'))
    {
      if (ISValidKey(&Key[2]))
        {
          NCVersion = 1;
          cgiHeaderContentType("text/html");
          fprintf(cgiOut,"<!----------------------------------------->\n"); 
          fprintf(cgiOut,"<!-- VShop 2.1 NC  - http://www.vshop.de -->\n");
          fprintf(cgiOut,"<!--    for non commercial use only !    -->\n");
          fprintf(cgiOut,"<!----------------------------------------->\n"); 
          return 0;
        }
      return 1;
    }
  else
    {
      if (decryptpassword (&Ptr, &Key[2], "cwulter") != 0)
        return 1;

      time(&TimeT);
      Today = localtime(&TimeT);
      strftime(TimeBuffer,19,"%Y%m%d",Today);
      Ret = (atol(TimeBuffer) > atol(Ptr));
      free(Ptr);
    }

  return Ret;

}
/*---------------------------------------------------------------------------------*/
int CheckIPAddr()
{
unsigned long ulIP;
struct hostent	*hHost;
struct in_addr	sin;

  ulIP = inet_addr(cgiRemoteAddr);
  sin.s_addr = ulIP;
  if ((hHost = gethostbyaddr((char*)&sin, 4, AF_INET)) == NULL)
    return 0;

  if (strstr(hHost->h_name,".intradat.com") == NULL)
    {
      printf ("Funktion fuer %s nicht zugelassen",hHost->h_name);
      return 0;
    }

  return 1;
}
/*---------------------------------------------------------------------------------*/
void ShowVersionInfo()
{
char SQL[501];
CDBField* SETTING;
CDBField* VALUE;

  sprintf(SQL,"select SETTING,VALUE from SYSTEMPREFERENCES where KEYWORD='VERSIONINFO'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      do
        {
          SETTING = EngineDB->GetField(MAX_SQLSELECT-1,"SETTING");
          VALUE = EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
          printf("%s:%s\n",SETTING->String,VALUE->String);
        }
      while(EngineDB->GetNext(MAX_SQLSELECT-1));
    }
}
/*---------------------------------------------------------------------------------*/
void ShowProductcategories()
{
char SQL[501];
char SQL2[501];
CDBField* INDEX;
CDBField* NAMEOFCATEGORY;
CDBField* NAME;
CDBField* DESCRIPTION;
CDBField* TAX;
double Tax;

  sprintf(SQL,"select NAMEOFCATEGORY,NAME,DESCRIPTION,VSINDEX FROM PRODUCTCATEGORIES WHERE HAS_SUBGROUPS=0");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      do
        {
          INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
          NAMEOFCATEGORY = EngineDB->GetField(MAX_SQLSELECT-1,"NAMEOFCATEGORY");
          NAME = EngineDB->GetField(MAX_SQLSELECT-1,"NAME");
          DESCRIPTION = EngineDB->GetField(MAX_SQLSELECT-1,"DESCRIPTION");

          Tax = 0;
          sprintf(SQL2,"SELECT TAX FROM TAXES WHERE PROFILEID='0' AND CATEGORYID='%s'",INDEX->String);
          if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQL2) != 0)                      
            {
              TAX = EngineDB->GetField(MAX_SQLSELECT-2,"TAX");
              Tax = TAX->Double;
            }
          printf("%s[:]%s[:]%s[:]%s[:]%f[:]\n",INDEX->String,NAMEOFCATEGORY->String,NAME->String,DESCRIPTION->String,Tax);
        }
      while(EngineDB->GetNext(MAX_SQLSELECT-1));
    }
}
/*---------------------------------------------------------------------------------*/
void ShowShopInfo()
{
char SQL[501];
char Buffer[2500];
CDBField* VALUE;
int First = 1;

  strcpy(Buffer,"");
  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='FRONTEND' AND SETTING='PRIMARY_CURRENCY'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='FRONTEND' AND SETTING='SECONDARY_CURRENCY'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='FRONTEND' AND SETTING='XCHANGE_PRIMARY_SECONDARY'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  strcat(Buffer,"\n");
  printf(Buffer);

}
/*---------------------------------------------------------------------------------*/
void ShowCompanyInfo()
{
char SQL[501];
char Buffer[2500];
CDBField* VALUE;
int First = 1;

  strcpy(Buffer,"");
  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='CINF' AND SETTING='SHOPNAME'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='CINF' AND SETTING='SLOGAN'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='CINF' AND SETTING='COMPANY'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='CINF' AND SETTING='STREET'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='CINF' AND SETTING='ZIP'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='CINF' AND SETTING='CITY'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='CINF' AND SETTING='PHONE'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='CINF' AND SETTING='FAX'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='CINF' AND SETTING='CONTACT'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  sprintf(SQL,"select VALUE FROM SYSTEMPREFERENCES WHERE KEYWORD='CINF' AND SETTING='CONTACTEMAIL'");
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      VALUE=EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
      strcat(Buffer,VALUE->String);
      strcat(Buffer,"[:]");
    }

  strcat(Buffer,"\n");
  printf(Buffer);

}
/*---------------------------------------------------------------------------------*/
void SearchProduct(char* Tablename, char* Keyword)
{
char SQL[501];
CDBField* INDEX;
CDBField* NAME;
CDBField* SHORTDESCR;
CDBField* DESCRIPTION;
CDBField* PRICE;
CDBField* PROMOTION;

  sprintf(SQL,"select * FROM %s WHERE ACTIVE=1 AND (",Tablename);
  strcat(SQL,Keyword);
  strcat(SQL,")");

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      do
        {
          INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");

          NAME = EngineDB->GetField(MAX_SQLSELECT-1,"NAME");
          SHORTDESCR = EngineDB->GetField(MAX_SQLSELECT-1,"SHORTDESCR");
          DESCRIPTION = EngineDB->GetField(MAX_SQLSELECT-1,"DESCRIPTION");
          PRICE = EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");
          PROMOTION = EngineDB->GetField(MAX_SQLSELECT-1,"PROMOTION");

          printf("%s[:]%s[:]%s[:]%s[:]%f[:]%i[:]\n",INDEX->String,NAME->String,SHORTDESCR->String,DESCRIPTION->String,
                  PRICE->Double,PROMOTION->Long);
        }
      while(EngineDB->GetNext(MAX_SQLSELECT-1));
    }
}
/*---------------------------------------------------------------------------------*/
void Maintenance(int Minutes)
{
time_t TimeT;
tm* Today;
char Date[15];
char SQL[501];
CDBField* INDEX;
CDBField* CATEGORYID;
CDBField* PRODUCTID;
CDBField* QUANTITY;


  time(&TimeT);
  TimeT -= (60* 60*24*30);
  Today = localtime(&TimeT);
  sprintf(Date,"%i%.2i%.2i%.2i%.2i",Today->tm_year+1900,Today->tm_mon+1,
          Today->tm_mday,Today->tm_hour,Today->tm_min);

  sprintf(SQL,"delete from SESSIONS where LASTUSAGE < '%s' AND OLD=1",Date);
  EngineDB->SQLUpdate(SQL);

  time(&TimeT);
  TimeT -= (60* Minutes);

  Today = localtime(&TimeT);

  sprintf(Date,"%i%.2i%.2i%.2i%.2i",Today->tm_year+1900,Today->tm_mon+1,
          Today->tm_mday,Today->tm_hour,Today->tm_min);

  sprintf(SQL,"select VSINDEX from SESSIONS where LASTUSAGE < '%s' AND OLD=0",Date);

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      do
        {
          INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
          sprintf(SQL,"delete from SESSIONVAR where VSINDEX='%s'",INDEX->String);
          EngineDB->SQLUpdate(SQL);
          sprintf(SQL,"select CATEGORYID,PRODUCTID,QUANTITY from BASKETITEMS where BASKETID='%s'",INDEX->String);
          if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQL) != 0)                      
            {
              do
                {
                  CATEGORYID = EngineDB->GetField(MAX_SQLSELECT-2,"CATEGORYID");
                  PRODUCTID = EngineDB->GetField(MAX_SQLSELECT-2,"PRODUCTID");
                  QUANTITY = EngineDB->GetField(MAX_SQLSELECT-2,"QUANTITY");
#if defined(VSHOP_BASIC) || defined(VSHOP_BASIC)
                  sprintf(SQL,"update INVENTORYCONTROL set ONSTOCK=ONSTOCK+%ld where CATEGORY='%s' AND PRODUCTINDEX='%s'",
                          QUANTITY->Long,CATEGORYID->String,PRODUCTID->String);
                  EngineDB->SQLUpdate(SQL);
#endif
                }
               while(EngineDB->GetNext(MAX_SQLSELECT-2));

               sprintf(SQL,"delete from BASKETITEMS where BASKETID='%s'",INDEX->String);
               EngineDB->SQLUpdate(SQL);

	       sprintf(SQL,"delete from BASKETOPTION where BASKETID='%s'",INDEX->String);
               EngineDB->SQLUpdate(SQL);
            }
          sprintf(SQL,"update SESSIONS set OLD=1 where VSINDEX='%s'",INDEX->String);
          EngineDB->SQLUpdate(SQL);
        }
  
      while(EngineDB->GetNext(MAX_SQLSELECT-1));
    }
}
/*---------------------------------------------------------------------------------*/
int GetAdmin()
{
char Login[40];
char ID[40];
char* Value;
char SQL[200];
char PASSWORD[50];
char USERNAME[50];
char Buffer[250];
char Buffer2[250];
char* Ptr;
char* Ptr2;
int IPOK = 0;
int Pos;

  if ((Ptr = DBGetConfig("ADMINIP")) != NULL)
    {
      strcpy(Buffer2,Ptr);
      do
        {
          strcpy(Buffer,Buffer2);

          if ((Ptr = strchr(Buffer,',')) != NULL)
            {
              strcpy(Buffer2,Ptr+1);
              *Ptr = '\0';
            }
          else
            strcpy(Buffer2,"");

          if ((strcmp(Buffer,cgiRemoteAddr) == 0) || (strcmp(Buffer,"*") == 0))
            {
              IPOK = 1;
              break;
            }

          if ((Ptr2 = strchr(Buffer,'*')) != NULL)
            {
              Pos = Ptr2-Buffer;
              if (strncmp(Buffer,cgiRemoteAddr,Pos) == 0)
                {
                  IPOK = 1;
                  break;
                }
            }
        }
      while (strcmp(Buffer2,"") != NULL);


      if (IPOK != 1)
        return -1;
    }

  if (cgiFormString("USERNAME",USERNAME,41) == cgiFormNotFound)
    {
      if     (cgiFormString("ID",ID,20) != cgiFormNotFound)
        {
          if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
            {
              strcpy(Login,Value);
              sprintf(SQL,"select VSINDEX from CUSTOMERS where LOGIN='%s' and VSGROUP='1'",Login);

              return (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL));
            }
          else
            return 0;
        }
      else
        return 0;
    }
  else
    {
      if (cgiFormString("PASSWORD",PASSWORD,41) != cgiFormNotFound)
        {
          sprintf(SQL,"select VSINDEX from CUSTOMERS where LOGIN='%s' and PASSWORD='%s' and VSGROUP='1'",USERNAME,PASSWORD);
          if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)
            {
              cgiFormString("ID",ID,20);
              if (!EngineDB->UpdateVar(ID,"LOGIN",USERNAME))
                EngineDB->InsertVar(ID,"LOGIN",USERNAME);
              return 1;
            }
          else
            return 0;
        }
      return 0;
    }

}
/*---------------------------------------------------------------------------------*/
int cgiMain(int Flag)
{
char DBUser[50];
char DBPassword[50];
char DBName[50];
char Servernode[50];
char PageName[MAX_HTMLPAGE];
char Page[MAX_HTMLPAGE + MAX_TEMPLATEDIR];
char ID[20];
int Minutes;
char Buffer[400];
int UseIniFile = 0;
char DBErrorURL[255];
char TimeoutURL[255];
char MaintenanceURL[255];
int Demoversion = 0;
#ifdef WIN32
  char RegistryKey[15];
	DWORD dw;
    HKEY  m_hKey;
	DWORD dwType;
	DWORD dwSize = 400;
#endif
int Ret;
int ConnectTimeout = 0;
char* Ptr;
NCVersion = Flag;
char KEY[51];
char Tablename[20];
char Keyword[100];

/*
  //
  // setup log file
  //
  ofstream log( "mylog.txt", ios::app );
  Carp::setStream( &log );
  Carp::showDate( true );
*/


  /* -------------------------- Windows-Conf --------------------------*/

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
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR0);
      return 0;
    }

  dwSize = 400;
  if (RegQueryValueEx (m_hKey, "DBPASSWORD", NULL,&dwType, (BYTE*) Buffer, &dwSize) != ERROR_SUCCESS)
    {
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR5);
      RegCloseKey (m_hKey);
      return 0;
    }
  else
    strcpy(DBPassword,Buffer);

  dwSize = 400;
  if (RegQueryValueEx (m_hKey, "DBUSER", NULL,&dwType, (BYTE*) Buffer, &dwSize) != ERROR_SUCCESS)
    {
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR4);
      RegCloseKey (m_hKey);
      return 0;
    }
  else
    strcpy(DBUser,Buffer);
                    
  dwSize = 400;
  if (RegQueryValueEx (m_hKey, "DBNODE", NULL,&dwType, (BYTE*) Buffer, &dwSize) != ERROR_SUCCESS)
    strcpy(Servernode,"");
  else
    strcpy(Servernode,Buffer);

  dwSize = 400;
  if (RegQueryValueEx (m_hKey, "DBNAME", NULL,&dwType, (BYTE*) Buffer, &dwSize) != ERROR_SUCCESS)
    strcpy(DBName,"VSHOP");
  else
    strcpy(DBName,Buffer);

  dwSize = 400;
  if (RegQueryValueEx (m_hKey, "DBERRORURL", NULL,&dwType, (BYTE*) Buffer, &dwSize) != ERROR_SUCCESS)
    strcpy(DBErrorURL,"");
  else
    strcpy(DBErrorURL,Buffer);

  dwSize = 400;
  if (RegQueryValueEx (m_hKey, "TEMPLATEROOT", NULL,&dwType, (BYTE*) Buffer, &dwSize) != ERROR_SUCCESS)
    {
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR3);
      RegCloseKey (m_hKey);
      return 0;
    }
  else
    {
      strcpy(TEMPLATEROOT,Buffer);
    }

  dwSize = 400;
  if (RegQueryValueEx (m_hKey, "BASEREF", NULL,&dwType, (BYTE*) Buffer, &dwSize) != ERROR_SUCCESS)
    {
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR3);
      RegCloseKey (m_hKey);
      return 0;
    }
  else
    {
      strcpy(BASEREF,Buffer);
    }

  dwSize = 400;
  if (RegQueryValueEx (m_hKey, "SECUREBASEREF", NULL,&dwType, (BYTE*) Buffer, &dwSize) != ERROR_SUCCESS)
    strcpy(SECUREBASEREF,BASEREF);
  else
    strcpy(BASEREF,Buffer);

  dwSize = 400;
  if (RegQueryValueEx (m_hKey, "CGIBIN", NULL,&dwType, (BYTE*) Buffer, &dwSize) != ERROR_SUCCESS)
    {
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR3);
      RegCloseKey (m_hKey);
      return 0;
    }
  else
    {
      strcpy(CGIBIN,Buffer);
    }

  dwSize = 400;
  if (RegQueryValueEx (m_hKey, "SECURECGIBIN", NULL,&dwType, (BYTE*) Buffer, &dwSize) != ERROR_SUCCESS)
    strcpy(SECURECGIBIN,CGIBIN);
  else
    strcpy(CGIBIN,Buffer);


  RegCloseKey (m_hKey);
#else


  /* -------------------------- Linux-Conf --------------------------*/

  if (! OpenIniFile(INIFILE))
    {
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR12);
      return 0;
    }

  /* ----------------------------- DBROOT ---------------------------*/
  if ((Ptr = GetFirstParam("DBROOT")) != NULL)
    {
      #if defined (SUN) || defined (HPUX)
        sprintf(Buffer,"DBROOT=%s",Ptr);
        putenv(Buffer);
      #elif WIN32
        sprintf(Buffer,"DBROOT=%s",Ptr);
        _putenv(Buffer);
      #else
        setenv("TERM","vt100",1);
        setenv("DBROOT",Ptr,1);
      #endif
    }
  else
    {
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR2);
      CloseIniFile();
      return 0;
    }

  /* ----------------------------- TEMPLATEROOT ---------------------------*/
  sprintf(Buffer,"%s_TEMPLATEROOT",PROGRAMNAME);
  if ((Ptr = GetFirstParam(Buffer)) != NULL)
    strcpy(TEMPLATEROOT,Ptr);
  else
    {
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR3);
      CloseIniFile();
      return 0;
     }

  /* ----------------------------- BASEREF ---------------------------*/
  sprintf(Buffer,"%s_BASEREF",PROGRAMNAME);
  if ((Ptr = GetFirstParam(Buffer)) != NULL)
    strcpy(BASEREF,Ptr);
  else
    {
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR9);
      CloseIniFile();
      return 0;
    }
  
  /* ----------------------------- SECUREBASEREF ---------------------------*/
  sprintf(Buffer,"%s_SECUREBASEREF",PROGRAMNAME);
  if ((Ptr = GetFirstParam(Buffer)) != NULL)
    strcpy(SECUREBASEREF,Ptr);
  else
    strcpy(SECUREBASEREF,BASEREF);

  /* ----------------------------- CGIBIN ---------------------------*/
  sprintf(Buffer,"%s_CGIBIN",PROGRAMNAME);
  if ((Ptr = GetFirstParam(Buffer)) != NULL)
    strcpy(CGIBIN,Ptr);
  else
    {
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR8);
      CloseIniFile();
      return 0;
     }

  /* ----------------------------- SECURECGIBIN ---------------------------*/
  sprintf(Buffer,"%s_SECURECGIBIN",PROGRAMNAME);
  if ((Ptr = GetFirstParam(Buffer)) != NULL)
    strcpy(SECURECGIBIN,Ptr);
  else
    strcpy(SECURECGIBIN,CGIBIN);

  /* ----------------------------- DBUSER ---------------------------*/
  sprintf(Buffer,"%s_DBUSER",PROGRAMNAME);
  if ((Ptr = GetFirstParam(Buffer)) != NULL)
    strcpy(DBUser,Ptr);
  else
   {
     cgiHeaderContentType("text/html");
     HTMLError(TEXT_ERROR4);
     CloseIniFile();
     return 0;
   }

  /* ----------------------------- DBPASSWORD ---------------------------*/  
  sprintf(Buffer,"%s_DBPASSWORD",PROGRAMNAME);
  if ((Ptr = GetFirstParam(Buffer)) != NULL)
    strcpy(DBPassword,Ptr);
  else
    {
      cgiHeaderContentType("text/html");
      HTMLError(TEXT_ERROR5);
      CloseIniFile();
      return 0;
    }
  
  /* ----------------------------- DBNODE ---------------------------*/
  sprintf(Buffer,"%s_DBNODE",PROGRAMNAME);
  if ((Ptr = GetFirstParam(Buffer)) != NULL)
    strcpy(Servernode,Ptr);
  else
    strcpy(Servernode,"");

  /* ----------------------------- DBNAME ---------------------------*/
  sprintf(Buffer,"%s_DBNAME",PROGRAMNAME);
  if ((Ptr = GetFirstParam(Buffer)) != NULL)
    strcpy(DBName,Ptr);
  else
    strcpy(DBName,"vshop");

  /* ----------------------------- DBERRORURL ---------------------------*/
  sprintf(Buffer,"%s_DBERRORURL",PROGRAMNAME);
  if ((Ptr = GetFirstParam(Buffer)) != NULL)
    sprintf(DBErrorURL,"text/html\nLocation: %s",Ptr);
  else
    strcpy(DBErrorURL,"");

  CloseIniFile();

#endif

  /* ------------------------ Aender der BASEREF ------------------------ */

  if (strlen(cgiPathInfo) >0)
    {
      if (TEMPLATEROOT[strlen(TEMPLATEROOT)-1] != '/')
        strcat(TEMPLATEROOT,"/");
      strcat(TEMPLATEROOT,cgiPathInfo);

      if (BASEREF[strlen(BASEREF)-1] != '/')
        strcat(BASEREF,"/");
      strcat(BASEREF,cgiPathInfo);

      strcat(CGIBIN,cgiPathInfo);
    }

  /* ------------------------ HTML-Seitenname setzen ------------------------ */

  cgiFormString("HTML",PageName,MAX_HTMLPAGE);
  if ((strlen(PageName) == 0) || (strstr(PageName,"../") != NULL))
    {
      strcpy(PageName,"door.htm");
    }

  if (TEMPLATEROOT[strlen(TEMPLATEROOT)-1] != '/')
    sprintf(Page,"%s/%s",TEMPLATEROOT,PageName);
  else
    sprintf(Page,"%s%s",TEMPLATEROOT,PageName);

  /* ------------------------ Verbinden zur Datenbank ------------------------ */

    //decryptpassword (&DBPasswordPtr, DBPassword, "VSHOPDBPASSWORD");
  
  EngineDB = new CEngineDB();

  if (EngineDB->Connect(DBName,DBUser, DBPassword,Servernode) != 1)
    {
      if (strcmp(DBErrorURL,"") != 0)
        cgiHeaderContentType(DBErrorURL);
      else
        {
          cgiHeaderContentType("text/html");
          HTMLError("Datenbankfehler");
        }
      delete EngineDB;
      return 0;
    }
  
  
  /* org 2.1.2 senglert geaendert
  decryptpassword (&Ptr, DBPassword, "VSHOPDBPASSWORD");

  ConnectTimeout = 1;
  EngineDB = new CEngineDB();
  while (EngineDB->Connect(DBName,DBUser, Ptr,Servernode) != 1)
    {
      if (ConnectTimeout == 10) 
        {
          if (strcmp(DBErrorURL,"") != 0)
            cgiHeaderContentType(DBErrorURL);
          else
            {
              cgiHeaderContentType("text/html");
              HTMLError("Datenbankfehler");
            }
          delete EngineDB;
          return 0;
        }
      else
        {
          sleep(1);
          ConnectTimeout++;
        }
    };
*/
  /* ----------------- VARIABLEN AUS DATENBANK LESEN -----------------*/

  /* ----------------------------- TIMEOUT ---------------------------*/
  if ((Ptr = DBGetConfig("TIMEOUT")) != NULL)
    Minutes = atoi(Ptr);
  else
    Minutes = 20;

  /* ----------------------------- TIMEOUTURL ---------------------------*/
  if ((Ptr = DBGetConfig("TIMEOUTURL")) != NULL)
    sprintf(TimeoutURL,"text/html\nLocation: %s",Ptr);
  else
    strcpy(TimeoutURL,"");

  /* ----------------------------- PROFILING ---------------------------*/
  if ((Ptr = DBGetConfig("PROFILING")) != NULL)
    VSHOP_PROFILING = atoi(Ptr);
  else
    VSHOP_PROFILING = 0;

  /* ------------------------ Session Management ----------------------------- */

  #ifndef VSL_ENGINE
    Maintenance(Minutes);

    cgiFormString("ID",ID,20);
    if (strlen(ID) == 0)
      {
        milisec(ID);
        while (EngineDB->CreateSession(ID) == 200)
          {         
            #ifndef WIN32
              sleep(1);
            #endif
            milisec(ID);     
          }
        InsertEntry("ID", ID);
      }
    else
      if (EngineDB->UpdateSession(ID) == 0)
        {
          if (strcmp(TimeoutURL,"") != 0)
            cgiHeaderContentType(TimeoutURL);
          else
            {
              cgiHeaderContentType("text/html");
              sprintf(Buffer,"%s<A HREF=\"%s/\" TARGET=_parent>%s</A>",TEXT_ERROR6,BASEREF,TEXT_ERROR7);
              HTMLError(Buffer);
            }
          delete EngineDB;
          return 0;
        }
  #endif

  /* ------------------------ Ueberpruefen der Rechte ----------------------------- */

  if (strstr(Page,"admin/") != NULL)
    {
      Ret = GetAdmin();
      if (Ret == 0)
        {
          if (TEMPLATEROOT[strlen(TEMPLATEROOT)-1] != '/')
            sprintf(Page,"%s/admin/login.htm",TEMPLATEROOT);
          else
            sprintf(Page,"%sadmin/login.htm",TEMPLATEROOT);
        }
      else
      if (Ret == -1)
        {

          WriteEvent(0,"Falsche IPAdresse",cgiRemoteAddr);

          cgiHeaderContentType("text/html");
          char ErrorBuffer[400];
          sprintf(ErrorBuffer,TEXT_ERROR14,cgiRemoteAddr);
          HTMLError(ErrorBuffer);
          delete EngineDB;
          return 0;
        }
    }
  else
    {
      /* ---------------------------- Maintenance --------------------------- */
      if ((Ptr = DBGetConfig("MAINTENANCE")) != NULL)
        {
          if (strcmp(Ptr,"TRUE") == 0)
            {
              if ((Ptr = DBGetConfig("MAINTENANCEURL")) != NULL)
                {
                  sprintf(MaintenanceURL,"text/html\nLocation: %s",Ptr);
                  cgiHeaderContentType(MaintenanceURL);
                }
            }
        }
      /* ---------------------------- Key --------------------------- */
      if ((Ptr = DBGetConfig("KEY")) != NULL)
        strcpy(KEY,Ptr);
      else
        strcpy(KEY,"");

      if (IsExpired(KEY))
        {
          if (!ISValidKey(KEY))
            {
              delete EngineDB;
              return 0;
            }
        }
    }
  /* ---------------- Sondefunktionen ----------------------- */

  if (Remote == 1)
    {
      if (cgiFormString("VSHOPSHOWVERSIONINFO",Buffer,20) != cgiFormNotFound)
        {
          cgiHeaderContentType("text/html");
          if (CheckIPAddr() == 1)
            ShowVersionInfo();
          delete EngineDB;
          return 0;
        }

      if (cgiFormString("VSHOPSHOWCOMPANYINFO",Buffer,20) != cgiFormNotFound)
        {
          cgiHeaderContentType("text/html");
          if (CheckIPAddr() == 1)
            ShowCompanyInfo();
          delete EngineDB;
          return 0;
        }

      if (cgiFormString("VSHOPSHOWSHOPINFO",Buffer,20) != cgiFormNotFound)
        {
          cgiHeaderContentType("text/html");
          if (CheckIPAddr() == 1)
            ShowShopInfo();
          delete EngineDB;
          return 0;
        }

      if (cgiFormString("VSHOPSHOWPRODUCTCATEGORIES",Buffer,20) != cgiFormNotFound)
        {
          cgiHeaderContentType("text/html");
          if (CheckIPAddr() == 1)
            ShowProductcategories();
          delete EngineDB;
          return 0;
        }

      if ((cgiFormString("VSHOPSEARCHPRODUCT",Buffer,20) != cgiFormNotFound) &&
          (cgiFormString("CATEGORY",Tablename,20) != cgiFormNotFound) && 
          (cgiFormString("SEARCHSTRING",Keyword,100) != cgiFormNotFound))
        {
          cgiHeaderContentType("text/html");
          if (CheckIPAddr() == 1)
            SearchProduct(Tablename,Keyword);
          delete EngineDB;
          return 0;
        }
    }

  /* ------------------------ GO ----------------------------- */

  setlocale(LC_TIME, "German");  
  VSLMain(Page, NCVersion);
  
  /* ------------------------ STOP ----------------------------- */                          

/*
  //
  // close log file
  //
  log.close();
*/

  delete EngineDB;
  return 0;
}


