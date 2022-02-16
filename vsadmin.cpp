#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <curses.h>
#include <menu.h>
#include <form.h>
#include <pwd.h>

#ifndef DB_ORACLE
  #include "adabas.h"
#else
  #include "oracle.h"
#endif

#include "vsadmin.h"

/* stefan */
#ifndef VSADMIN
CEngineDB* EngineDB;
#endif


/* stefans includes */
#include "inifile.h"
#include "skey.h"

static void Exit(int sig);

char DOMAINPW[200];

/* stefan */
char ORACLESID[200];
char OraServernode[200];
char OraDBUser[200];

char DBROOT[200];
char CGIBIN[200];
char CGIBINPATH[200];
char HTDOCS[200];
char INDEXHTML[200];
char SERVERNAME[200];
char WWWUSER[200];

char SHOPNAME[200];

char KEY[40];

WINDOW* MessageBoxWindow;
/*------------------------------------------------------------------------------*/
/* TOOL Functions */
/*------------------------------------------------------------------------------*/
int IsRoot()
{
  return getuid() == 0; 
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
int FileExists(char* File)
{
  if (access(File,F_OK) != 0)
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
int ChangeModRW(char* File)
{
char Buffer[200];

  sprintf(Buffer,"chmod -R a+rw %s",File);
  
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
/*-------------------------------------------------------------------------------------*/
void HilfeKeyDialog()
{
WINDOW* Window;
int c = 0;


  Window=newwin(8,60,7,10);
  wbkgdset(Window,COLOR_PAIR(8)); 
  wattron(Window,A_BOLD);
  werase(Window);

  wborder(Window,0,0,0,0,0,0,0,0);

  mvwaddstr(Window,0,2,TEXT_DIALOGTOP1);

  refresh();
  wrefresh(Window);

  c = getch();

  delwin(Window);
}
/*-------------------------------------------------------------------------------------*/
int KeyDialog()
{
WINDOW* KeyWindow;
FIELD* Fields[2];
FORM* Form;
int Finish = 0;
char* Ptr;

  KeyWindow=newwin(5,40,8,20);
  wbkgdset(KeyWindow,COLOR_PAIR(4)); 
  werase(KeyWindow);

  Fields[0] = new_field(1,28,2,10,0,1);
  Fields[1] = NULL;

  field_opts_off(Fields[0],O_AUTOSKIP | O_STATIC);

  Form = new_form(&Fields[0]);

  set_form_win(Form,KeyWindow);
  post_form(Form);


  wborder(KeyWindow,0,0,0,0,0,0,0,0);
  mvwaddstr(KeyWindow,2,2,TEXT_KEYDIALOG2);
  mvwaddstr(KeyWindow,4,10,TEXT_DIALOGBOTTOM2);

  wbkgdset(KeyWindow,COLOR_PAIR(3)); 
  mvwaddstr(KeyWindow,0,2,TEXT_KEYDIALOG1);

  form_driver(Form,REQ_FIRST_FIELD);
  wrefresh(KeyWindow);

  do
    {
      int c = getch();
      switch(c)
        {
          case KEY_RIGHT:
            form_driver(Form,REQ_NEXT_CHAR);
            break;
          case KEY_LEFT:
            form_driver(Form,REQ_PREV_CHAR);
            break;
          case KEY_DC:
            form_driver(Form,REQ_DEL_CHAR);
            break;
          case KEY_BACKSPACE:
            form_driver(Form,REQ_DEL_PREV);
            break;
          case KEY_HOME:
            form_driver(Form,REQ_BEG_LINE);
            break;
          case KEY_END:
            form_driver(Form,REQ_END_LINE);
            break;
          case 27:
            Finish = -1;
            break;
          case KEY_F(1):
            HilfeKeyDialog();
            refresh();
            wrefresh(KeyWindow);
          case KEY_F(10):
          case 13:
            Finish = 1;
            form_driver(Form,REQ_FIRST_FIELD);
          default :
            form_driver(Form,c);
            break;   
        }
      if (Finish == 1)
        {
          strcpy(KEY,field_buffer(Fields[0],0));
          if ((Ptr = strchr(KEY,' ')) != NULL)
            *Ptr = 0;
        }

     wrefresh(KeyWindow);
    }
  while(Finish == 0);

  unpost_form(Form);
  free_form(Form);
  free_field(Fields[0]);
  
  delwin(KeyWindow);

  return Finish;
}
/*-------------------------------------------------------------------------------------*/
void HilfeNameDialog()
{
WINDOW* Window;
int c = 0;


  Window=newwin(8,60,7,10);
  wbkgdset(Window,COLOR_PAIR(8)); 
  wattron(Window,A_BOLD);
  werase(Window);

  wborder(Window,0,0,0,0,0,0,0,0);

  mvwaddstr(Window,0,2,TEXT_DIALOGTOP1);

  refresh();
  wrefresh(Window);

  c = getch();

  delwin(Window);
}
/*-------------------------------------------------------------------------------------*/
int NameDialog()
{
WINDOW* KeyWindow;
FIELD* Fields[2];
FORM* Form;
int Finish = 0;
char* Ptr;

  KeyWindow=newwin(5,40,8,20);
  wbkgdset(KeyWindow,COLOR_PAIR(4)); 
  werase(KeyWindow);

  Fields[0] = new_field(1,28,2,10,0,1);
  Fields[1] = NULL;

  field_opts_off(Fields[0],O_AUTOSKIP | O_STATIC);

  Form = new_form(&Fields[0]);

  set_form_win(Form,KeyWindow);
  post_form(Form);


  wborder(KeyWindow,0,0,0,0,0,0,0,0);
  mvwaddstr(KeyWindow,2,2,TEXT_NAMEDIALOG2);
  mvwaddstr(KeyWindow,4,10,TEXT_DIALOGBOTTOM2);

  wbkgdset(KeyWindow,COLOR_PAIR(3)); 
  mvwaddstr(KeyWindow,0,2,TEXT_NAMEDIALOG1);

  form_driver(Form,REQ_FIRST_FIELD);
  wrefresh(KeyWindow);

  do
    {
      int c = getch();
      switch(c)
        {
          case KEY_RIGHT:
            form_driver(Form,REQ_NEXT_CHAR);
            break;
          case KEY_LEFT:
            form_driver(Form,REQ_PREV_CHAR);
            break;
          case KEY_DC:
            form_driver(Form,REQ_DEL_CHAR);
            break;
          case KEY_BACKSPACE:
            form_driver(Form,REQ_DEL_PREV);
            break;
          case KEY_HOME:
            form_driver(Form,REQ_BEG_LINE);
            break;
          case KEY_END:
            form_driver(Form,REQ_END_LINE);
            break;
          case 27:
            Finish = -1;
            break;
          case KEY_F(1):
            HilfeKeyDialog();
            refresh();
            wrefresh(KeyWindow);
          case KEY_F(10):
          case 13:
            Finish = 1;
            form_driver(Form,REQ_FIRST_FIELD);
          default :
            form_driver(Form,c);
            break;   
        }
      if (Finish == 1)
        {
          strcpy(SHOPNAME,field_buffer(Fields[0],0));
          if ((Ptr = strchr(SHOPNAME,' ')) != NULL)
            *Ptr = 0;
        }

     wrefresh(KeyWindow);
    }
  while(Finish == 0);

  unpost_form(Form);
  free_form(Form);
  free_field(Fields[0]);
  
  delwin(KeyWindow);

  return Finish;
}
/*-------------------------------------------------------------------------------------*/
void HilfeDomainPWDialog()
{
WINDOW* Window;
int c = 0;


  Window=newwin(8,60,7,10);
  wbkgdset(Window,COLOR_PAIR(8)); 
  wattron(Window,A_BOLD);
  werase(Window);

  wborder(Window,0,0,0,0,0,0,0,0);

  mvwaddstr(Window,0,2,TEXT_DIALOGTOP1);

  refresh();
  wrefresh(Window);

  c = getch();

  delwin(Window);
}
/*-------------------------------------------------------------------------------------*/
int DomainPWDialog()
{
WINDOW* KeyWindow;
FIELD* Fields[2];
FORM* Form;
int Finish = 0;
char* Ptr;
int i;

  KeyWindow=newwin(5,40,8,20);
  wbkgdset(KeyWindow,COLOR_PAIR(4)); 
  werase(KeyWindow);

  Fields[0] = new_field(1,28,2,10,0,1);
  Fields[1] = NULL;

  field_opts_off(Fields[0],O_AUTOSKIP | O_STATIC);

  Form = new_form(&Fields[0]);

  set_form_win(Form,KeyWindow);
  post_form(Form);


  wborder(KeyWindow,0,0,0,0,0,0,0,0);
  mvwaddstr(KeyWindow,2,2,TEXT_DOMAINPWDIALOG2);
  mvwaddstr(KeyWindow,4,10,TEXT_DIALOGBOTTOM2);

  wbkgdset(KeyWindow,COLOR_PAIR(3)); 
  mvwaddstr(KeyWindow,0,2,TEXT_DOMAINPWDIALOG1);

  form_driver(Form,REQ_FIRST_FIELD);
  wrefresh(KeyWindow);

  do
    {
      int c = getch();
      switch(c)
        {
          case KEY_RIGHT:
            form_driver(Form,REQ_NEXT_CHAR);
            break;
          case KEY_LEFT:
            form_driver(Form,REQ_PREV_CHAR);
            break;
          case KEY_DC:
            form_driver(Form,REQ_DEL_CHAR);
            break;
          case KEY_BACKSPACE:
            form_driver(Form,REQ_DEL_PREV);
            break;
          case KEY_HOME:
            form_driver(Form,REQ_BEG_LINE);
            break;
          case KEY_END:
            form_driver(Form,REQ_END_LINE);
            break;
          case 27:
            Finish = -1;
            break;
          case KEY_F(1):
            HilfeKeyDialog();
            refresh();
            wrefresh(KeyWindow);
          case KEY_F(10):
          case 13:
            Finish = 1;
            form_driver(Form,REQ_FIRST_FIELD);
          default :
            form_driver(Form,c);
            break;   
        }
      if (Finish == 1)
        {
          strcpy(DOMAINPW,field_buffer(Fields[0],0));
          if ((Ptr = strchr(DOMAINPW,' ')) != NULL)
            *Ptr = 0;
        }

     wrefresh(KeyWindow);
    }
  while(Finish == 0);

  unpost_form(Form);
  free_form(Form);
  free_field(Fields[0]);
  
  delwin(KeyWindow);


  i=0;
  while(DOMAINPW[i]) 
    {
      DOMAINPW[i]=toupper(DOMAINPW[i]);
      i++;
    }

  return Finish;
}
/*-------- stefan -----------------------------------------------------------------------------*/
int OraclePWDialog()
{
WINDOW* KeyWindow;
FIELD* Fields[3];
FORM* Form;
int Finish = 0;
char* Ptr;
int i;

  KeyWindow=newwin(11,60,6,10);
  wbkgdset(KeyWindow,COLOR_PAIR(4)); 
  werase(KeyWindow);

  Fields[0] = new_field(1,29,2,27,0,1);
  Fields[1] = new_field(1,29,4,27,0,1);
  Fields[2] = NULL;

  field_opts_off(Fields[0],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[1],O_AUTOSKIP | O_STATIC);

  Form = new_form(&Fields[0]);

  set_form_win(Form,KeyWindow);
  post_form(Form);


  wborder(KeyWindow,0,0,0,0,0,0,0,0);
  mvwaddstr(KeyWindow,2,2,TEXT_ORACLEPWDIALOG2);
  mvwaddstr(KeyWindow,4,2,TEXT_ORACLEPWDIALOG3);

  mvwaddstr(KeyWindow,10,30,TEXT_DIALOGBOTTOM2);

  wbkgdset(KeyWindow,COLOR_PAIR(3)); 
  mvwaddstr(KeyWindow,0,2,TEXT_ORACLEPWDIALOG1);

  form_driver(Form,REQ_FIRST_FIELD);
  wrefresh(KeyWindow);

  do
    {
      int c = getch();
      switch(c)
        {
          case KEY_RIGHT:
            form_driver(Form,REQ_NEXT_CHAR);
            break;
          case KEY_LEFT:
            form_driver(Form,REQ_PREV_CHAR);
            break;
          case KEY_DC:
            form_driver(Form,REQ_DEL_CHAR);
            break;
          case KEY_BACKSPACE:
            form_driver(Form,REQ_DEL_PREV);
            break;
          case KEY_HOME:
            form_driver(Form,REQ_BEG_LINE);
            break;
          case KEY_END:
            form_driver(Form,REQ_END_LINE);
            break;
          case 9:
          case KEY_DOWN:
            if (field_index(current_field(Form)) == 3)
              form_driver(Form,REQ_FIRST_FIELD);
            else
              form_driver(Form,REQ_NEXT_FIELD);
            break;
          case KEY_CTAB:
          case KEY_UP:
            form_driver(Form,REQ_PREV_FIELD);
            break;			
          case 27:
            Finish = -1;
            break;
          case KEY_F(1):
            HilfeKeyDialog();
            refresh();
            wrefresh(KeyWindow);
          case KEY_F(10):
          case 13:
            Finish = 1;
            form_driver(Form,REQ_FIRST_FIELD);
          default :
            form_driver(Form,c);
            break;   
        }
      if (Finish == 1)
        {
          strcpy(DOMAINPW,field_buffer(Fields[0],0));
          if ((Ptr = strchr(DOMAINPW,' ')) != NULL)
            *Ptr = 0;

          strcpy(ORACLESID,field_buffer(Fields[1],0));
          if ((Ptr = strchr(ORACLESID,' ')) != NULL)
            *Ptr = 0;
        }

     wrefresh(KeyWindow);
    }
  while(Finish == 0);

  unpost_form(Form);
  free_form(Form);
  free_field(Fields[0]);
  
  delwin(KeyWindow);


  i=0;
  /*
  while(DOMAINPW[i]) 
    {
      DOMAINPW[i]=toupper(DOMAINPW[i]);
      i++;
    }
	*/

  return Finish;
}
/*-------------------------------------------------------------------------------------*/
void HilfeVShopParameterDialog()
{
WINDOW* Window;
int c = 0;

  Window=newwin(8,60,7,10);
  wbkgdset(Window,COLOR_PAIR(8)); 
  wattron(Window,A_BOLD);
  werase(Window);
  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,0,2,TEXT_DIALOGTOP1);
  refresh();
  wrefresh(Window);
  c = getch();
  delwin(Window);
}
/*-------------------------------------------------------------------------------------*/
void SetEnvWarning()
{
WINDOW* Window;
int c = 0;

  Window=newwin(8,60,7,10);
  wbkgdset(Window,COLOR_PAIR(8)); 
  wattron(Window,A_BOLD);
  werase(Window);
  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,0,2,TEXT_DIALOGTOP1);
  refresh();
  wrefresh(Window);
  c = getch();
  delwin(Window);
}
/*-------------------------------------------------------------------------------------*/
void WarnungCGIBINPATH()
{
WINDOW* Window;

  Window=newwin(7,40,8,20);
  wbkgdset(Window,COLOR_PAIR(7) | A_BOLD); 
  attron(A_BOLD);
  werase(Window);

  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2,TEXT_CGIBINERROR1);
  mvwaddstr(Window,3,2,CGIBINPATH);
  mvwaddstr(Window,4,2,TEXT_CGIBINERROR2);
  mvwaddstr(Window,0,2,TEXT_ERROR);
  wrefresh(Window);
  getch();
  delwin(Window);
}
/*-------------------------------------------------------------------------------------*/
void WarnungHTDOCS()
{
WINDOW* Window;

  Window=newwin(7,40,8,20);
  wbkgdset(Window,COLOR_PAIR(7) | A_BOLD); 
  attron(A_BOLD);
  werase(Window);
  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2,TEXT_HTDOCSERROR1);
  mvwaddstr(Window,3,2,HTDOCS);
  mvwaddstr(Window,4,2,TEXT_HTDOCSERROR2);
  mvwaddstr(Window,0,2,TEXT_ERROR);
  wrefresh(Window);
  getch();
  delwin(Window);
}
/*-------------------------------------------------------------------------------------*/
int VShopParameterDialog()
{
WINDOW* Window;
FIELD* Fields[5];
FORM* Form;
int Finish = 0;
char* Ptr;

  Window=newwin(11,60,6,10);
  wbkgdset(Window,COLOR_PAIR(4)); 
  werase(Window);

  Fields[0] = new_field(1,29,2,27,0,1);
  Fields[1] = new_field(1,29,4,27,0,1);
  Fields[2] = new_field(1,29,6,27,0,1);
  Fields[3] = new_field(1,29,8,27,0,1);
  Fields[4] = NULL;

  set_field_buffer(Fields[0],0,SERVERNAME);
  set_field_buffer(Fields[1],0,CGIBIN);
  set_field_buffer(Fields[2],0,HTDOCS);
  set_field_buffer(Fields[3],0,CGIBINPATH);

  field_opts_off(Fields[0],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[1],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[2],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[3],O_AUTOSKIP | O_STATIC);
  Form = new_form(&Fields[0]);
  set_form_win(Form,Window);

  set_field_fore(Fields[0],0 | A_BOLD);
  set_field_fore(Fields[1],0 | A_BOLD);
  set_field_fore(Fields[2],0 | A_BOLD);
  set_field_fore(Fields[3],0 | A_BOLD);

  post_form(Form);
  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2,TEXT_PARAMDIALOG1);
  mvwaddstr(Window,4,2,TEXT_PARAMDIALOG2);    
  mvwaddstr(Window,6,2,TEXT_PARAMDIALOG3);
  mvwaddstr(Window,8,2,TEXT_PARAMDIALOG4);
  mvwaddstr(Window,10,30,TEXT_DIALOGBOTTOM2);
  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2,TEXT_PARAMDIALOG);
  form_driver(Form,REQ_FIRST_FIELD);
  wrefresh(Window);
  do
    {
      int c = getch();
      switch(c)
        {
          case KEY_RIGHT:
            form_driver(Form,REQ_NEXT_CHAR);
            break;
          case KEY_LEFT:
            form_driver(Form,REQ_PREV_CHAR);
            break;
          case KEY_DC:
            form_driver(Form,REQ_DEL_CHAR);
            break;
          case KEY_BACKSPACE:
            form_driver(Form,REQ_DEL_PREV);
            break;
          case KEY_HOME:
            form_driver(Form,REQ_BEG_LINE);
            break;
          case KEY_END:
            form_driver(Form,REQ_END_LINE);
            break;
          case 9:
          case KEY_DOWN:
            if (field_index(current_field(Form)) == 3)
              form_driver(Form,REQ_FIRST_FIELD);
            else
              form_driver(Form,REQ_NEXT_FIELD);
            break;
          case KEY_CTAB:
          case KEY_UP:
            form_driver(Form,REQ_PREV_FIELD);
            break;
          case 27:
            form_driver(Form,REQ_FIRST_FIELD);
            Finish = -1;
            break;
          case KEY_F(10):
          case 13:
            form_driver(Form,REQ_FIRST_FIELD);
            Finish = 1;
            break;
          case KEY_F(1):
            HilfeVShopParameterDialog();
            refresh();
            break;
          default :
            form_driver(Form,c);
            break;   
        }

    if (Finish == 1)
      {
        strcpy(SERVERNAME,field_buffer(Fields[0],0));
        if ((Ptr = strchr(SERVERNAME,' ')) != NULL)
          *Ptr = 0;

        strcpy(CGIBIN,field_buffer(Fields[1],0));
        if ((Ptr = strchr(CGIBIN,' ')) != NULL)
          *Ptr = 0;

        strcpy(HTDOCS,field_buffer(Fields[2],0));
        if ((Ptr = strchr(HTDOCS,' ')) != NULL)
          *Ptr = 0;

        strcpy(CGIBINPATH,field_buffer(Fields[3],0));
        if ((Ptr = strchr(CGIBINPATH,' ')) != NULL)
          *Ptr = 0;

       if (IsFileWriteable(CGIBINPATH) == 0)
          {
            WarnungCGIBINPATH();
            refresh();
            Finish =0;
          }
       if (IsFileWriteable(HTDOCS) == 0)
          {
            WarnungHTDOCS();
            refresh();
            Finish =0;
          }
      }

     wrefresh(Window);
    }
  while(Finish == 0);

  unpost_form(Form);
  free_form(Form);
  free_field(Fields[0]);
  
  delwin(Window);

  refresh();

  return Finish;
}
/*-------------------------------------------------------------------------------------*/
void Init()
{
  initscr();
  keypad(stdscr,TRUE);
  nonl();
  cbreak();
  noecho();

  if (has_colors())
    {
      start_color();
      init_pair(0,COLOR_WHITE,COLOR_BLACK);
      init_pair(1,COLOR_WHITE,COLOR_BLUE);
      init_pair(2,COLOR_YELLOW,COLOR_BLUE);
      init_pair(3,COLOR_BLUE,COLOR_WHITE);
      init_pair(4,COLOR_BLACK,COLOR_WHITE);
      init_pair(5,COLOR_BLUE,COLOR_BLACK);
      init_pair(6,COLOR_BLACK,COLOR_BLUE);
      init_pair(7,COLOR_YELLOW,COLOR_RED);
      init_pair(8,COLOR_BLACK,COLOR_GREEN);
    }

  bkgdset(COLOR_PAIR(1));
  erase();
  border(0,0,0,0,0,0,0,0);
  bkgdset(COLOR_PAIR(2));
  attron(A_BOLD);
  #ifdef DOWNLOAD
    mvaddstr(0,2,TEXT_VERSIONDOWNLOAD);
  #else
    mvaddstr(0,2,TEXT_VERSIONFULL);
  #endif
}
/*-------------------------------------------------------------------------------------*/
static void Exit(int sig)
{
  bkgdset(COLOR_PAIR(0));
  erase();
  refresh();
  endwin();

  exit(sig);
}
/*-------------------------------------------------------------------------------------*/
void DoErrorBox(char* Message)
{
WINDOW* Window;

  Window=newwin(5,50,8,10);
  wbkgdset(Window,COLOR_PAIR(7) | A_BOLD); 
  attron(A_BOLD);
  werase(Window);

  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,0,2,"Error");
  mvwaddstr(Window,2,2,Message);
  wrefresh(Window);
  getch();
  delwin(Window);
  refresh();
}
/*-------------------------------------------------------------------------------------*/
void DoModalMessageBox(char* Message)
{
  MessageBoxWindow=newwin(5,40,8,20);
  wbkgdset(MessageBoxWindow,COLOR_PAIR(4) | A_BOLD); 
  attron(A_BOLD);
  werase(MessageBoxWindow);
  wborder(MessageBoxWindow,0,0,0,0,0,0,0,0);
  mvwaddstr(MessageBoxWindow,2,1,Message);
  wrefresh(MessageBoxWindow);
}
/*-------------------------------------------------------------------------------------*/
void EndModalMessageBox()
{
  delwin(MessageBoxWindow);
  refresh();
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

  sprintf(Buffer,"%s%s/index.tmp",HTDOCS,SHOPNAME);
  if ((File1 = fopen(Buffer,"r")) != NULL)
    {
      sprintf(Buffer,"%s%s/%s",HTDOCS,SHOPNAME,INDEXHTML);
      if ((File2 = fopen(Buffer,"w")) != NULL)
        {
          while(!feof(File1))
            {  
              if (fgets(Line,500,File1) != NULL)
                {
                  if (strncasecmp(Line,"REF1",4) == 0)
                    fprintf(File2,"http://%s/%s/\n",SERVERNAME,SHOPNAME);
                  else
                  if (strncasecmp(Line,"REF2",4) == 0)
                    fprintf(File2,"%s%s?HTML=vshop.htm\n",CGIBIN,SHOPNAME);
                  else
                  if (strncasecmp(Line,"REF3",4) == 0)
                    fprintf(File2,"%s%s?HTML=admin%%2Flogin.htm\n",CGIBIN,SHOPNAME);
                  else
                  if (strncasecmp(Line,"REF4",4) == 0)
                    fprintf(File2,"%s%s?HTML=admin%%2Flogin.htm&NEXT=admin/wizards/adm_wizard_conf_list.htm\n",CGIBIN,SHOPNAME);
                  else
                  if (strncasecmp(Line,"REF5",4) == 0)
                    fprintf(File2,"<form action=\"%s%s?HTML=vshop.htm\" method=\"post\">\n",CGIBIN,SHOPNAME);
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

  sprintf(Buffer,"%s%s/admin/index.tmp",HTDOCS,SHOPNAME);
  if ((File1 = fopen(Buffer,"r")) != NULL)
    {
      sprintf(Buffer,"%s%s/admin/%s",HTDOCS,SHOPNAME,INDEXHTML);
      if ((File2 = fopen(Buffer,"w")) != NULL)
        {
          while(!feof(File1))
            {  
              if (fgets(Line,500,File1) != NULL)
                {
                  if (strncasecmp(Line,"REF1",4) == 0)
                    fprintf(File2,"http://%s/%s/\n",SERVERNAME,SHOPNAME);
                  else
                  if (strncasecmp(Line,"REF2",4) == 0)
                    fprintf(File2,"%s%s?HTML=vshop.htm\n",CGIBIN,SHOPNAME);
                  else
                  if (strncasecmp(Line,"REF3",4) == 0)
                    fprintf(File2,"%s%s?HTML=admin%%2Flogin.htm\n",CGIBIN,SHOPNAME);
                  else
                  if (strncasecmp(Line,"REF4",4) == 0)
                    fprintf(File2,"%s%s?HTML=admin%%2Flogin.htm&NEXT=admin/wizards/adm_wizard_conf_list.htm\n",CGIBIN,SHOPNAME);
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
char buffer[20];
int i = 0;
char* Ptr;

  strcpy(buffer,SHOPNAME);
  while(buffer[i]) 
    {
      buffer[i]=toupper(buffer[i]);
      i++;
    }

  if ((File = fopen("/etc/vshop.conf","a+")) != NULL)
    {
      fprintf(File,"\n\n");
      fprintf(File,"%s_DBUSER %s\n",buffer,buffer);

      cryptpassword (&Ptr, buffer, "VSHOPDBPASSWORD");

      fprintf(File,"%s_DBPASSWORD %s\n",buffer,Ptr);
      fprintf(File,"%s_DBERRORURL http://%s/%s/dberror.htm\n", buffer,SERVERNAME,SHOPNAME);
      fprintf(File,"%s_TEMPLATEROOT %s%s\n",buffer,HTDOCS,SHOPNAME);
      fprintf(File,"%s_BASEREF http://%s/%s\n",buffer,SERVERNAME,SHOPNAME);
      fprintf(File,"%s_CGIBIN http://%s%s%s\n",buffer,SERVERNAME,CGIBIN,SHOPNAME);
      fprintf(File,"%s_CGIBINPATH %s\n",buffer,CGIBINPATH);
      fprintf(File,"%s_%s %s%s\n", buffer,buffer,CGIBINPATH,SHOPNAME);
      fprintf(File,"%s_CHDBPASSWD %s%s/admin/bin/%spasswd\n", buffer,HTDOCS,SHOPNAME,SHOPNAME);
      fprintf(File,"%s_SECUREBASEREF http://%s/%s\n", buffer,SERVERNAME,SHOPNAME);
      fprintf(File,"%s_SECURECGIBIN http://%s%s%s\n", buffer,SERVERNAME,CGIBIN,SHOPNAME);
      fclose(File);
    }
}
/*-------- stefan ----------------------------------------------------------------------*/
void MakeVSConfOracle()
{
FILE* File;
char buffer[20];
int i = 0;
char* Ptr;

  strcpy(buffer,SHOPNAME);
  while(buffer[i]) 
    {
      buffer[i]=toupper(buffer[i]);
      i++;
    }

  if ((File = fopen("/etc/vshop.conf","a+")) != NULL)
    {
      fprintf(File,"\n\n");
	  fprintf(File,"%s_DBNAME %s\n",buffer,ORACLESID);
      fprintf(File,"%s_DBUSER %s\n",buffer,buffer);
      fprintf(File,"%s_DBPASSWORD %s\n",buffer,buffer);
      fprintf(File,"%s_DBERRORURL http://%s/%s/dberror.htm\n", buffer,SERVERNAME,SHOPNAME);
      fprintf(File,"%s_TEMPLATEROOT %s%s\n",buffer,HTDOCS,SHOPNAME);
      fprintf(File,"%s_BASEREF http://%s/%s\n",buffer,SERVERNAME,SHOPNAME);
      fprintf(File,"%s_CGIBIN http://%s%s%s\n",buffer,SERVERNAME,CGIBIN,SHOPNAME);
      fprintf(File,"%s_CGIBINPATH %s\n",buffer,CGIBINPATH);
      fprintf(File,"%s_%s %s%s\n", buffer,buffer,CGIBINPATH,SHOPNAME);
      fprintf(File,"%s_CHDBPASSWD %s%s/admin/bin/%spasswd\n", buffer,HTDOCS,SHOPNAME,SHOPNAME);
      fprintf(File,"%s_SECUREBASEREF http://%s/%s\n", buffer,SERVERNAME,SHOPNAME);
      fprintf(File,"%s_SECURECGIBIN http://%s%s%s\n", buffer,SERVERNAME,CGIBIN,SHOPNAME);
      fclose(File);
    }
}
/*------------------------------------------------------------------------------*/
void DeleteFromVSConf()
{
FILE* File1;
FILE* File2;
char Buffer[50];
char Buffer2[50];
char Line[501];

  sprintf(Buffer,"%s_",SHOPNAME);
  sprintf(Buffer2,"#%s_",SHOPNAME);
  if ((File1 = fopen("/etc/vshop.conf","r")) != NULL)
    {
      if ((File2 = fopen("/etc/vshop.conf.bak","w+")) != NULL)
        {
          while (fgets(Line,500,File1) != NULL)
            {
              if (strncasecmp(Line,Buffer,strlen(Buffer)) != 0)
                if (strncasecmp(Line,Buffer2,strlen(Buffer2)) != 0)
                  fputs(Line,File2);
            }
          fclose(File2);
          Copy("/etc/vshop.conf.bak", "/etc/vshop.conf");
        }
      fclose(File1);
    }
}
/*------------------------------------------------------------------------------*/
void DeleteFromVSAConf()
{
FILE* File1;
FILE* File2;
char Buffer[50];
char Line[501];

  sprintf(Buffer,"SHOPNAME %s",SHOPNAME);
  if ((File1 = fopen("/opt/vshop/vsadmin.conf","r")) != NULL)
    {
      if ((File2 = fopen("/opt/vshop/vsadmin.conf.bak","w+")) != NULL)
        {
          while (fgets(Line,500,File1) != NULL)
            {
              if (strncasecmp(Line,Buffer,strlen(Buffer)) != 0)
                fputs(Line,File2);
            }
          fclose(File2);
          Copy("/opt/vshop/vsadmin.conf.bak", "/opt/vshop/vsadmin.conf");
        }
      fclose(File1);
    }
}
/*------------------------------------------------------------------------------*/
void AddToVSAConf()
{
FILE* File1;
FILE* File2;
char Buffer[50];
char Line[501];

  sprintf(Buffer,"SHOPNAME %s\n",SHOPNAME);
  if ((File1 = fopen("/opt/vshop/vsadmin.conf","r")) != NULL)
    {
      if ((File2 = fopen("/opt/vshop/vsadmin.conf.bak","w+")) != NULL)
        {
          while (fgets(Line,500,File1) != NULL)
            {
              fputs(Line,File2);
            }
          fputs(Buffer,File2);
          fclose(File2);
          Copy("/opt/vshop/vsadmin.conf.bak", "/opt/vshop/vsadmin.conf");
        }
      fclose(File1);
    }
}
/*-------------------------------------------------------------------------------------*/


void NewShop()
{
char SPVSHOP[200];
char SPVSLCOMPILE[200];
char SPMTLCOMPILE[200];
char SPVSPASSWD[200];
char Buffer[200];

  //Shopname abfragen
  if (NameDialog() == -1)
    return;

  if (strlen(SHOPNAME) >6)
    {
      DoErrorBox(ATEXT_MESSAGE16);
      return;
    }
  
  //Parameter abfragen
  if (VShopParameterDialog() == -1)
    return;
/*     
  // Key abfragen
  if (KeyDialog() == -1)
    return;
*/
  // Passwort abfragen
  #ifndef DB_ORACLE
	  if (DomainPWDialog() == -1)
    	return;
  #else
	  if (OraclePWDialog() == -1)
    	return;
  #endif

  //DoModalMessageBox(ATEXT_MESSAGE1);

/* stefan geaendert */  
  CEngineDB *EngineDB = new CEngineDB();

  /* stefan */
  #ifndef DB_ORACLE
	  EngineDB->Connect("vshop","ADMIN", DOMAINPW,"");
	  sprintf(Buffer,"CREATE USER %s PASSWORD %s LIKE DOMAIN",SHOPNAME,SHOPNAME);
	  if (EngineDB->SQLUpdate(Buffer) == 0)
	    {
	      EndModalMessageBox();
	      DoErrorBox(ATEXT_MESSAGE2);
	      return;
	    }
	
	  sprintf(Buffer,"ALTER USER %s NOT EXCLUSIVE",SHOPNAME);
	  if (EngineDB->SQLUpdate(Buffer) == 0)
	    {
	    }
	
	  delete EngineDB;
  #else
	  EngineDB->Connect(ORACLESID,"sys", DOMAINPW,"");
 	  sprintf(Buffer,"create user %s identified by %s",SHOPNAME,SHOPNAME);
	  if (EngineDB->SQLUpdate(Buffer) == 0)
	    {
	      EndModalMessageBox();
	      DoErrorBox(ATEXT_MESSAGE2);
	      return;
	    }
	
	  sprintf(Buffer,"grant connect to %s",SHOPNAME);
	  if (EngineDB->SQLUpdate(Buffer) == 0)
	    {
	    }

	  sprintf(Buffer,"grant resource to %s",SHOPNAME);
	  if (EngineDB->SQLUpdate(Buffer) == 0)
	    {
	    }
	
	  delete EngineDB;
  #endif
	  
  EndModalMessageBox();
  DoModalMessageBox(ATEXT_MESSAGE3);

  //Parameter ueberpruefen
  if (CGIBINPATH[strlen(CGIBINPATH)-1] != '/')
    strcat(CGIBINPATH,"/");

  if (HTDOCS[strlen(HTDOCS)-1] != '/')
    strcat(HTDOCS,"/");

  strcpy(SPVSHOP,"/opt/vshop/engine/vshop");
  strcpy(SPVSLCOMPILE,"/opt/vshop/engine/vsl-compile");
  strcpy(SPMTLCOMPILE,"/opt/vshop/engine/mtl-compile");
  strcpy(SPVSPASSWD,"/opt/vshop/engine/vshoppasswd");

  // vshop kopieren
  sprintf(Buffer,"%s%s",CGIBINPATH,SHOPNAME);
  if (Copy(SPVSHOP, Buffer) == 0)
    {
      EndModalMessageBox();
      DoErrorBox(ATEXT_MESSAGE4);
      return;
    }
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);


  //vsl-compile kopieren
  sprintf(Buffer,"%svsl-compile",CGIBINPATH);
  if (Copy(SPVSLCOMPILE, Buffer) == 0)
    {
      EndModalMessageBox();
      DoErrorBox(ATEXT_MESSAGE4);
      return;
    }
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);

  //mtl-compile kopieren
  sprintf(Buffer,"%smtl-compile",CGIBINPATH);
  if (Copy(SPMTLCOMPILE, Buffer) == 0)
    {
      EndModalMessageBox();
      DoErrorBox(ATEXT_MESSAGE4);
      return;
    }
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);

#ifndef ENGLISH
  sprintf(Buffer,"%spips",CGIBINPATH);
  if (Copy("/opt/vshop/engine/pips", Buffer) == 0)
    {
      EndModalMessageBox();
      DoErrorBox(ATEXT_MESSAGE4);
      return;
    }
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);
#endif


  EndModalMessageBox();
  DoModalMessageBox(ATEXT_MESSAGE5);


  sprintf(Buffer,"%s%s/",HTDOCS,SHOPNAME);
  mkdir(Buffer,775);

  //HTML-Seiten kopieren
  if (Copy("/opt/vshop/vshtml.tgz", Buffer) == 0)
    {
      EndModalMessageBox();
      DoErrorBox(ATEXT_MESSAGE6);
      return;
    }

  //HTML-Seiten entpacken
  chdir(Buffer);
  if (Untar(Buffer,"vshtml.tgz") == 0)
    {
      EndModalMessageBox();
      DoErrorBox(ATEXT_MESSAGE6);
      return;
    }

  ChangeOwner(Buffer,WWWUSER);
  ChangeModRW(Buffer);
  ChangeModX(Buffer);
  
  Delete(Buffer,"vshtml.tgz");

  //index.html erstellen
  MakeIndexhtm();
  MakeAdminIndexhtm();

  //vshoppasswd kopieren
  sprintf(Buffer,"%s/%s/admin/bin/%spasswd",HTDOCS,SHOPNAME,SHOPNAME);
  if (Copy(SPVSPASSWD, Buffer) == 0)
    {
      EndModalMessageBox();
      DoErrorBox(ATEXT_MESSAGE4);
      return;
    }
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);
  ChangeModX(Buffer);

  // /etc/vshop.conf anpassen
  #ifndef DB_ORACLE
	  MakeVSConf();
  #else
	  MakeVSConfOracle();  
  #endif
  AddToVSAConf();

  EndModalMessageBox();
  DoModalMessageBox(ATEXT_MESSAGE7);

  #ifndef DB_ORACLE
  	sprintf(Buffer,"%s/bin/xload -d vshop -u %s,%s -S ADABAS -b /opt/vshop/createtabs",DBROOT,SHOPNAME,SHOPNAME);
  	system(Buffer);
  #else
    sprintf(Buffer,"%s/bin/sqlplus %s/%s@%s < /opt/vshop/createtabs > /dev/null 2>&1",DBROOT,SHOPNAME,SHOPNAME,ORACLESID);
  	system(Buffer);
  #endif
	
  EndModalMessageBox();
}
/*-------------------------------------------------------------------------------------*/
int ShopWirklichLoeschen()
{
ITEM* MenuItems[10];
MENU* Menu;
WINDOW* Window;
WINDOW* MenuWindow;
int Finish = 0;

  MenuItems[0] = new_item("1.",ATEXT_MESSAGE8);
  MenuItems[1] = new_item("2.",ATEXT_MESSAGE9);
  MenuItems[2] = NULL;

  Window = newwin(6,50,5,5);
  wbkgdset(Window,COLOR_PAIR(4));
  werase(Window);
  wborder(Window,0,0,0,0,0,0,0,0);

  MenuWindow = newwin(3,40,7,7);
  wbkgdset(MenuWindow,COLOR_PAIR(4));
  wattron(MenuWindow,A_BOLD);
  werase(MenuWindow);

  mvwaddstr(Window,6,20,"[RETURN/F10=OK][ESC=Cancel]");

  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2,ATEXT_MESSAGE10);

  Menu = new_menu(&MenuItems[0]);
  set_menu_fore(Menu,COLOR_PAIR(1));
  set_menu_back(Menu,COLOR_PAIR(3));
  set_menu_win(Menu,MenuWindow);
  post_menu(Menu);

  refresh();
  wrefresh(Window);
  wrefresh(MenuWindow);

  do
    {
      int c = getch();
      switch(c)
        {
          case KEY_DOWN:
              menu_driver(Menu,REQ_DOWN_ITEM);
            break;
          case KEY_UP:
            menu_driver(Menu,REQ_UP_ITEM);
            break;
          case 27:
            Finish = -1;
            break;
          case KEY_F(10):
          case 10:
          case 13:
              Finish = item_index(current_item(Menu))+1;
            break;
          default :
            menu_driver(Menu,c);
            menu_driver(Menu,REQ_CLEAR_PATTERN);
        }
      wrefresh(MenuWindow );
    }
  while(Finish == 0);
  
  unpost_menu(Menu);
  free_menu(Menu);
  free_item(MenuItems[0]);
  free_item(MenuItems[1]);

  delwin(MenuWindow);
  delwin(Window);
  refresh();

  return Finish;
}
/*-------------------------------------------------------------------------------------*/
void ShopLoeschen()
{
char ShopName[20];
char Buffer[200];
int i =0;
char* Ptr;


  if (ShopWirklichLoeschen() != 2)
    return;

  // Key abfragen
  if (DomainPWDialog() == -1)
    return;

  strcpy(ShopName,SHOPNAME);
  while(ShopName[i]) 
    {
      ShopName[i]=toupper(ShopName[i]);
      i++;
    }

  DoModalMessageBox(ATEXT_MESSAGE12);

  CEngineDB* EngineDB = new CEngineDB();
  EngineDB->Connect("vshop","ADMIN", DOMAINPW,"");

  sprintf(Buffer,"DROP USER %s",SHOPNAME);
  if (!EngineDB->SQLUpdate(Buffer))
    {
      EndModalMessageBox();
      DoErrorBox(ATEXT_MESSAGE2);
      return;
    }

  delete EngineDB;
  EndModalMessageBox();


  OpenIniFile("/etc/vshop.conf");

  DoModalMessageBox(ATEXT_MESSAGE13);

  sprintf(Buffer,"%s_TEMPLATEROOT",ShopName);
  if((Ptr = GetFirstParam(Buffer)) != NULL)
    {
      sprintf(Buffer,"rm -r %s",Ptr);
      system(Buffer);
    }

  EndModalMessageBox();
  DoModalMessageBox(ATEXT_MESSAGE14);

  sprintf(Buffer,"%s_CGIBINPATH",ShopName);
  if((Ptr = GetFirstParam(Buffer)) != NULL)
    {
      sprintf(Buffer,"rm %s/%s",Ptr,SHOPNAME);
      system(Buffer);
    }

  CloseIniFile();

  DeleteFromVSConf();
  DeleteFromVSAConf();

  EndModalMessageBox();
}
/*--------------- stefan ----------------------------------------------------------------------*/
void ShopLoeschenOracle()
{
char ShopName[20];
char Buffer[200];
int i =0;
char* Ptr;


  if (ShopWirklichLoeschen() != 2)
    return;

  // Key abfragen
  if (OraclePWDialog() == -1)
    return;

  strcpy(ShopName,SHOPNAME);
  while(ShopName[i]) 
    {
      ShopName[i]=toupper(ShopName[i]);
      i++;
    }

  DoModalMessageBox(ATEXT_MESSAGE12);

  CEngineDB* EngineDB = new CEngineDB();
  EngineDB->Connect(ORACLESID,"sys", DOMAINPW,"");

  sprintf(Buffer,"drop user %s cascade",SHOPNAME);
  if (!EngineDB->SQLUpdate(Buffer))
    {
      EndModalMessageBox();
      DoErrorBox(ATEXT_MESSAGE2);
      return;
    }

  delete EngineDB;
  EndModalMessageBox();


  OpenIniFile("/etc/vshop.conf");

  DoModalMessageBox(ATEXT_MESSAGE13);

  sprintf(Buffer,"%s_TEMPLATEROOT",ShopName);
  if((Ptr = GetFirstParam(Buffer)) != NULL)
    {
      sprintf(Buffer,"rm -r %s",Ptr);
      system(Buffer);
    }

  EndModalMessageBox();
  DoModalMessageBox(ATEXT_MESSAGE14);

  sprintf(Buffer,"%s_CGIBINPATH",ShopName);
  if((Ptr = GetFirstParam(Buffer)) != NULL)
    {
      sprintf(Buffer,"rm %s/%s",Ptr,SHOPNAME);
      system(Buffer);
    }

  CloseIniFile();

  DeleteFromVSConf();
  DeleteFromVSAConf();

  EndModalMessageBox();
}
/*-------------------------------------------------------------------------------------*/
int ShopsListen()
{
  /* --- oli
    ITEM* MenuItems[21];
  --- */

ITEM* MenuItems[1024];
MENU* Menu;
WINDOW* MenuWindow;
int Finish = 0;
int Counter = 0;

/* --- oli
char Buffer[21][21];
char PtrBuffer[21][21];
--- */

char Buffer[1024][21];
char PtrBuffer[1024][21];
char* Ptr;

  OpenIniFile("/opt/vshop/vsadmin.conf");

  if((Ptr = GetFirstParam("SHOPNAME")) != NULL)
    {
      do
        {
          sprintf(Buffer[Counter],"%i.",Counter+1);
          strcpy(PtrBuffer[Counter],Ptr);
          MenuItems[Counter] = new_item(&Buffer[Counter][0],&PtrBuffer[Counter][0]);
          Counter++;                                    
        }
      while ((Ptr = GetNextParam("SHOPNAME")) != NULL);
    }

  sprintf(Buffer[Counter],"%i.",Counter+1);
  MenuItems[Counter] = new_item(&Buffer[Counter][0],ATEXT_MESSAGE15);
  Counter++;
  MenuItems[Counter] = NULL;

  MenuWindow = newwin(5+Counter,50,5,5);
  wbkgdset(MenuWindow,COLOR_PAIR(1));
  wattron(MenuWindow,A_BOLD);
  werase(MenuWindow);

  Menu = new_menu(&MenuItems[0]);
  set_menu_fore(Menu,COLOR_PAIR(3));
  set_menu_back(Menu,COLOR_PAIR(1));
  set_menu_win(Menu,MenuWindow);
  post_menu(Menu);
  refresh();
  wrefresh(MenuWindow);
                             
  do
    {
      int c = getch();
      switch(c)
        {
          case KEY_DOWN:
            menu_driver(Menu,REQ_DOWN_ITEM);
            break;
          case KEY_UP:
            menu_driver(Menu,REQ_UP_ITEM);
            break;
          case 10:
          case 13:
            if (item_index(current_item(Menu)) == (Counter-1))
              Finish = -1;
            else
              {
                strcpy(SHOPNAME,item_description(current_item(Menu)));
                Finish = 1;
              }
            break;
          default :
            menu_driver(Menu,c);
            menu_driver(Menu,REQ_CLEAR_PATTERN);
            break;   
        }
      wrefresh(MenuWindow);
    }
  while(Finish == 0);
  
  unpost_menu(Menu);
  free_menu(Menu);
  while (Counter > 0)
    {
      Counter--;
      free_item(MenuItems[Counter]);
    }

  return Finish;
}
/*-------------------------------------------------------------------------------------*/
void MainMenu()
{
ITEM* MenuItems[6];
MENU* Menu;
WINDOW* MenuWindow;
int Finish = 0;

  MenuItems[0] = new_item("1.",TEXT_MAINMENU1);
  MenuItems[1] = new_item("2.",TEXT_MAINMENU2);
  MenuItems[2] = new_item("3.",TEXT_MAINMENU3);
  MenuItems[3] = new_item("4.",TEXT_MAINMENU4);
  MenuItems[4] = NULL;

  MenuWindow = newwin(10,50,5,5);
  wbkgdset(MenuWindow,COLOR_PAIR(1));
  wattron(MenuWindow,A_BOLD);
  werase(MenuWindow);

  Menu = new_menu(&MenuItems[0]);
  set_menu_fore(Menu,COLOR_PAIR(3));
  set_menu_back(Menu,COLOR_PAIR(1));
  set_menu_win(Menu,MenuWindow);
  post_menu(Menu);
  refresh();
  wrefresh(MenuWindow);
  
  do
    {
      int c = getch();
      switch(c)
        {
          case KEY_DOWN:
            menu_driver(Menu,REQ_DOWN_ITEM);
            break;
          case KEY_UP:
            menu_driver(Menu,REQ_UP_ITEM);
            break;
          case 10:
          case 13:
            switch(item_index(current_item(Menu)))
              {
                case 0:
                  ShopsListen();
                  menu_driver(Menu,REQ_CLEAR_PATTERN);
                  refresh();
                  wrefresh(MenuWindow);
                  break;
                case 1:
				  /* stefan */
				  #ifndef DB_ORACLE
                    if (ShopsListen() == 1)
                      ShopLoeschen();
				  #else
                    if (ShopsListen() == 1)
                      ShopLoeschenOracle();
				  #endif
                  menu_driver(Menu,REQ_CLEAR_PATTERN);
                  refresh();
                  wrefresh(MenuWindow);
                  break;
                case 2:
                  NewShop();
                  menu_driver(Menu,REQ_CLEAR_PATTERN);
                  refresh();
                  wrefresh(MenuWindow);
                  break;
                case 3:
                  Finish = 1;
                  break;
               }
            break;
          default :
            menu_driver(Menu,c);
            menu_driver(Menu,REQ_CLEAR_PATTERN);
            break;   
        }
      wrefresh(MenuWindow);
    }
  while(Finish == 0);
  
  unpost_menu(Menu);
  free_menu(Menu);
  free_item(MenuItems[0]);
  free_item(MenuItems[1]);
}
/*-------------------------------------------------------------------------------------*/
int main(int argc, char *argv[]) 
{
char* Ptr;
int Counter;
/* stefan */
char Buffer[250];

  if (IsRoot() == 0)
    {
      printf(TEXT_ERROR6);
      printf("\n");
      exit(1);
    }

  OpenIniFile("/opt/vshop/vsadmin.conf");

  if((Ptr = GetFirstParam("DBROOT")) != NULL)
    {
      strcpy(DBROOT,Ptr);
      /* stefan */
      sprintf(Buffer,"DBROOT=%s",Ptr);
      putenv(Buffer);
      //setenv("DBROOT",Ptr,1);
    }

  if((Ptr = GetFirstParam("SERVERNAME")) != NULL)
    {
      strcpy(SERVERNAME,Ptr);
    }

  if((Ptr = GetFirstParam("HTDOCS")) != NULL)
     {
       strcpy(HTDOCS,Ptr);
     }

  if((Ptr = GetFirstParam("CGIBINPATH")) != NULL)
    {
      strcpy(CGIBINPATH,Ptr);
    }

  if((Ptr = GetFirstParam("CGIBIN")) != NULL)
    {
      strcpy(CGIBIN,Ptr);
    }

  if((Ptr = GetFirstParam("WWWUSER")) != NULL)
    {
      strcpy(WWWUSER,Ptr);
    }

  if((Ptr = GetFirstParam("INDEXHTML")) != NULL)
    {
      strcpy(INDEXHTML,Ptr);
    }

  CloseIniFile();

  if (argc >= 2) 
    {
      if (strcmp(argv[1],"-l") == 0)
        {
            OpenIniFile("/opt/vshop/vsadmin.conf");
            if ((Ptr = GetFirstParam("SHOPNAME")) != NULL)
              {
                Counter = 1;
                printf ("Eingetragene Shops : \n\n");
                do
                  {
                    printf("%i. %s\n",Counter,Ptr);
                    Counter++;                                    
                  }
                while ((Ptr = GetNextParam("SHOPNAME")) != NULL);
                printf ("\n");
              }
        }
      else
      if (strcmp(argv[1],"-d") == 0)
        {
          strcpy(SHOPNAME,argv[2]);
          signal(SIGINT, Exit);
          Init();
		  /* stefan */
          #ifndef DB_ORACLE
            ShopLoeschen();		  
		  #else
            ShopLoeschenOracle();
		  #endif
          bkgdset(COLOR_PAIR(0));
          erase();
          refresh();
          endwin();
        }
    }
  else
    {
      signal(SIGINT, Exit);
      Init();

      MainMenu();
 
      bkgdset(COLOR_PAIR(0));
      erase();
      refresh();
      endwin();
  }

  return 0;
}
