
#include <time.h>
#include <stdio.h>
#include "error.h"
#include "cgic.h"
#ifndef DB_ORACLE
  #include "adabas.h"
#else
  #include "oracle.h"
#endif


#include "vslanguage.h"



/*---------------------------------------------------------------------------------*/
void DBShowVersionInfo()
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
          fprintf(cgiOut,"		  <LI><FONT SIZE=\"-1\" FACE=\"arial,helv,helvetica\">Modul %s (Version %s)</FONT></LI>\n",SETTING->String,VALUE->String);
        }
      while(EngineDB->GetNext(MAX_SQLSELECT-1));
    }
}
/*--------------------------------------------------------------------------------*/
void HTMLError(char* Text)
{
  fprintf(cgiOut,"<HTML>\n");
  fprintf(cgiOut,"<HEAD>\n");
  fprintf(cgiOut,"	<TITLE>%s</TITLE></HEAD>\n",TEXT_VERSION);
  fprintf(cgiOut,"<BODY>\n");
  fprintf(cgiOut,"<TABLE BORDER=\"0\" CELLPADDING=\"15\" CELLSPACING=\"0\">\n");
  fprintf(cgiOut,"<TR>\n");
  fprintf(cgiOut,"	<TD></TD>\n");
  fprintf(cgiOut,"	<TD></TD>\n");
  fprintf(cgiOut,"</TR>\n");
  fprintf(cgiOut,"<TR>\n");
  fprintf(cgiOut,"	<TD></TD>\n");
  fprintf(cgiOut,"	<TD>\n");
  fprintf(cgiOut,"		<FONT SIZE=\"3\" COLOR=\"#000080\" FACE=\"arial,helv,helvetica\"><B>%s</B></FONT><BR><BR>\n",TEXT_VERSION);
  fprintf(cgiOut,"		<FONT SIZE=\"-1\" FACE=\"arial,helv,helvetica\">%s</FONT><BR><BR>\n",Text);
  fprintf(cgiOut,"		<HR SIZE=\"1\" NOSHADE=\"NOSHADE\">\n");
  /*fprintf(cgiOut,"		<UL>\n");*/
  /*fprintf(cgiOut,"      <FONT SIZE=\"-1\" FACE=\"arial,helv,helvetica\">%s</FONT>\n",TEXT_VERSION);*/
  DBShowVersionInfo();
  /*fprintf(cgiOut,"		</UL>\n");*/
  fprintf(cgiOut,"	</TD>\n");
  fprintf(cgiOut,"</TR>\n");
  fprintf(cgiOut,"</TABLE>\n");
  fprintf(cgiOut,"</BODY>\n");
  fprintf(cgiOut,"</HTML>\n");
}
/*--------------------------------------------------------------------------------*/
void RuntimeError1(char* Text)
{
fprintf(cgiOut,"\n<SCRIPT LANGUAGE=\"JavaScript\">\n"); 
fprintf(cgiOut,"alert(\"VShop Runtime Error\\n");
fprintf(cgiOut,Text);
fprintf(cgiOut,"\")\n"); 
fprintf(cgiOut,"</SCRIPT>\n");

ErrorToFile(Text);
}
/*--------------------------------------------------------------------------------*/
void RuntimeError2(char* Text1, char* Text2)
{
char Buffer[500];

fprintf(cgiOut,"\n<SCRIPT LANGUAGE=\"JavaScript\">\n"); 
fprintf(cgiOut,"alert(\"VShop Runtime Error\\n");
fprintf(cgiOut,Text1);
fprintf(cgiOut,Text2);
fprintf(cgiOut,"\")\n"); 
fprintf(cgiOut,"</SCRIPT>\n");

sprintf(Buffer,"%s %s",Text1,Text2);
ErrorToFile(Buffer);
}
/*--------------------------------------------------------------------------------*/
void RuntimeError3(char* Text, long Value)
{
char Buffer[500];

fprintf(cgiOut,"\n<SCRIPT LANGUAGE=\"JavaScript\">\n"); 
fprintf(cgiOut,"alert(\"VShop Runtime Error\\n");
fprintf(cgiOut,Text);
fprintf(cgiOut,"%ld",Value);
fprintf(cgiOut,"\")\n"); 
fprintf(cgiOut,"</SCRIPT>\n");

sprintf(Buffer,"%s %ld",Text,Value);
ErrorToFile(Buffer);
}
/*--------------------------------------------------------------------------------*/
void RuntimeError4(char* Text1, char* Text2)
{
char Buffer[500];

fprintf(cgiOut,"\n<SCRIPT LANGUAGE=\"JavaScript\">\n"); 
/* stefan */
#ifndef DB_ORACLE
	fprintf(cgiOut,"alert(\"VShop Runtime Error - ADABAS\\n");
#else
	fprintf(cgiOut,"alert(\"VShop Runtime Error - ORACLE\\n");
#endif
fprintf(cgiOut,Text1);
fprintf(cgiOut,"\\n");
fprintf(cgiOut,Text2);
fprintf(cgiOut,"\")\n"); 
fprintf(cgiOut,"</SCRIPT>\n");

sprintf(Buffer,"%s %s",Text1,Text2);
ErrorToFile(Buffer);
}
/*--------------------------------------------------------------------------------*/

void DebugToHTML(char* DebugString)
{
  fprintf(cgiOut,"<SCRIPT>\n");
  fprintf(cgiOut,"window2=open('','VShopDebugoutput','scrollbars=yes,width=250,height=400');\n");
  fprintf(cgiOut,"window2.document.writeln('<HTML><BODY>');\n");
  fprintf(cgiOut,"window2.document.writeln('%s');\n",DebugString);
  fprintf(cgiOut,"window2.document.writeln('</BODY></HTML>');\n");
  fprintf(cgiOut,"</SCRIPT>");
}

void DebugToFile(char* DebugString)
{
FILE *fh;

  fh=fopen("debug.vsl","a");
  if (fh)
    {
      fprintf(fh,"%s\n",DebugString);
      fclose(fh);
    }
}

void DebugToFile2(char* DebugString1,char* DebugString2)
{
FILE *fh;

  fh=fopen("debug.vsl","a");
  if (fh)
    {
      fprintf(fh,"%s %s\n",DebugString1,DebugString2);
      fclose(fh);
    }
}

void ErrorToFile(char* DebugString)
{

FILE *fh;
char Buffer[200];
char ErrorBuffer[200];
time_t TimeT;
struct tm* Today;

  time(&TimeT);
  Today = localtime(&TimeT);
  sprintf(ErrorBuffer,"%i/%.2i/%.2i %.2i:%.2i:%.2i %s\n",Today->tm_year+1900,Today->tm_mon+1,Today->tm_mday
  ,Today->tm_hour,Today->tm_min,Today->tm_sec,DebugString);

  sprintf(Buffer,"%s.error",PROGRAMNAME);
  fh=fopen(Buffer,"a+");
  if (fh)
    {
      fprintf(fh,"%s\n",ErrorBuffer);
      fclose(fh);
    }
}

void EndHTMLDebug()
{
}
void StartHTMLDebug()
{
}


