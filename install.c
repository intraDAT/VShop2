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
#include <netdb.h>
/* stefan */
#include <strings.h>


#include "inifile.h"
#include "vslanguage.h"

static void Exit(int sig);

char DBROOT[200];
char DBUSER[200];

/* stefan */
char DBPASSWORD[200];
char ORACLESID[200];
char ORACLEDBA[200];

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
/*------------------------------------------------------------------------------*/
/* TOOL Functions */
/*------------------------------------------------------------------------------*/

/*************************************************************************/
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
int Libc5WarnungDialog()
{
WINDOW* Window;
int Finish = 0;
int c = 0;
char Buffer[80];
FILE* ConfFile;
char Line[401];
char* Ptr;


  if ((ConfFile = fopen("/etc/ld.so.conf","r")) != NULL)
    {
      while (fgets(Line,400,ConfFile) != NULL)
        {
          if ((Ptr = strrchr(Line,'=')) != NULL)
            *Ptr = '\0';
          else
            Line[strlen(Line)-1] = '\0';

          if (Line[strlen(Line)-1] != '/')
            strcat(Line,"/");

          strcat(Line,"libc.so.6");

          if (access(Line,R_OK) == 0)
            {
              fclose(ConfFile);
              return 0;
            }
        }
      fclose(ConfFile);
      if (access("/lib/libc.so.6",R_OK) == 0)
        return 0;
      if (access("/usr/lib/libc.so.6",R_OK) == 0)
        return 0;
    }
  else
    return 0;

  Window=newwin(10,60,7,10);
  wbkgdset(Window,COLOR_PAIR(4)); 
  werase(Window);

  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2,TEXT_LIBC5DIALOG1);
  mvwaddstr(Window,3,2,TEXT_LIBC5DIALOG2);
  mvwaddstr(Window,4,2,TEXT_LIBC5DIALOG3);
  mvwaddstr(Window,5,2,TEXT_LIBC5DIALOG4);
  mvwaddstr(Window,6,2,TEXT_LIBC5DIALOG5);
  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,7,2,TEXT_LIBC5DIALOG6);
  wbkgdset(Window,COLOR_PAIR(4)); 


  mvwaddstr(Window,9,39,TEXT_DIALOGBOTTOM1);

  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2," Warnung ");

  refresh();
  wrefresh(Window);

  while(Finish == 0)
    {

      c = getch();

      switch(c)
        {
          case 27:
            Finish = -1;
            break;
          case KEY_F(10):
          case 13:
            Finish = 1;
          default :
            break;   
        }
    }

  delwin(Window);

  return Finish;
}
/*-------------------------------------------------------------------------------------*/
int InstallDialog()
{
WINDOW* Window;
int Finish = 0;
int c = 0;
char Buffer[80];
unsigned int IP;
char IPAddr[20];


  IP = gethostid();
  ipaddr2str(IPAddr,IP);

  Window=newwin(10,60,7,10);
  wbkgdset(Window,COLOR_PAIR(4)); 
  werase(Window);

  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2,TEXT_INSTALLDIALOG1);
  mvwaddstr(Window,3,2,TEXT_INSTALLDIALOG2);
  mvwaddstr(Window,4,2,TEXT_INSTALLDIALOG3);
  mvwaddstr(Window,5,2,TEXT_INSTALLDIALOG4);
/*  mvwaddstr(Window,6,2,TEXT_INSTALLDIALOG5a);
*/
  /*wbkgdset(Window,COLOR_PAIR(3)); 
*/
/*  mvwaddstr(Window,6,3,IPAddr);
*/
/*  wbkgdset(Window,COLOR_PAIR(4)); 
*/
  mvwaddstr(Window,6,2,TEXT_INSTALLDIALOG5b);

  mvwaddstr(Window,7,2,TEXT_INSTALLDIALOG6);



  mvwaddstr(Window,9,39,TEXT_DIALOGBOTTOM1);

  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2,TEXT_DIALOGTOP1);

  refresh();
  wrefresh(Window);

  while(Finish == 0)
    {

      c = getch();

      switch(c)
        {
          case 27:
            Finish = -1;
            break;
          case KEY_F(10):
          case 13:
            Finish = 1;
          default :
            break;   
        }
    }

  delwin(Window);

  return Finish;
}
/*-------------------------------------------------------------------------------------*/
int UeberschreibenDialog()
{
WINDOW* Window;
int Finish = 0;
int c = 0;


  Window=newwin(8,60,7,10);
  wbkgdset(Window,COLOR_PAIR(4)); 
  werase(Window);

  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2,TEXT_UEBERSCHREIBENDIALOG1);
  mvwaddstr(Window,3,2,TEXT_UEBERSCHREIBENDIALOG2);
  mvwaddstr(Window,4,2,TEXT_UEBERSCHREIBENDIALOG3);
  mvwaddstr(Window,7,39,TEXT_DIALOGBOTTOM1);

  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2,TEXT_DIALOGTOP1);

  refresh();
  wrefresh(Window);

  while(Finish == 0)
    {
      c = getch();

      switch(c)
        {
          case 27:
            Finish = -1;
            break;
          case KEY_F(10):
            Finish = 1;
          default :
            break;   
        }
    }

  delwin(Window);

  return Finish;
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
void HilfeIstAdabasInstalliert()
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
int IstAdabasInstalliert()
{
ITEM* MenuItems[10];
MENU* Menu;
WINDOW* Window;
WINDOW* MenuWindow;
int Finish = 0;

  MenuItems[0] = new_item("1.",TEXT_ADABASMENU1);
  MenuItems[1] = new_item("2.",TEXT_ADABASMENU2);
  MenuItems[2] = new_item("3.",TEXT_ADABASMENU3);
  MenuItems[3] = NULL;

  Window = newwin(7,50,5,5);
  wbkgdset(Window,COLOR_PAIR(4));
  werase(Window);
  wborder(Window,0,0,0,0,0,0,0,0);

  MenuWindow = newwin(4,40,7,7);
  wbkgdset(MenuWindow,COLOR_PAIR(4));
  wattron(MenuWindow,A_BOLD);
  werase(MenuWindow);

  mvwaddstr(Window,6,20,TEXT_DIALOGBOTTOM2);

  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2,TEXT_ADABASDIALOG1);

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
          case 13:
            if (item_index(current_item(Menu)) == 2)
              Finish = -1;
            else
              Finish = item_index(current_item(Menu))+1;
            break;
          case KEY_F(1):
            HilfeIstAdabasInstalliert();
            refresh();
            wrefresh(Window);
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
  free_item(MenuItems[2]);

  delwin(MenuWindow);
  delwin(Window);

  return Finish;
}
/*-------------------------------------------------------------------------------------*/
void WarnungAdabasBenutzer()
{
WINDOW* Window;
char Buffer[100];

  Window=newwin(5,40,8,20);
  wbkgdset(Window,COLOR_PAIR(7) | A_BOLD); 
  attron(A_BOLD);
  werase(Window);
  wborder(Window,0,0,0,0,0,0,0,0);
  strcpy(Buffer,TEXT_ADABASUSERERROR1);
  strcat(Buffer,DBUSER);
  strcat(Buffer,TEXT_ADABASUSERERROR2);
  mvwaddstr(Window,2,1,Buffer);
  mvwaddstr(Window,0,2,TEXT_ERRORINSTALL);
  wrefresh(Window);
  getch();
  delwin(Window);
}
/*-------------------------------------------------------------------------------------*/
void WarnungAdabasVerzeichnis()
{
WINDOW* Window;

  Window=newwin(7,40,8,20);
  wbkgdset(Window,COLOR_PAIR(7) | A_BOLD); 
  attron(A_BOLD);
  werase(Window);
  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2,TEXT_ADABASDIRERROR1);
  mvwaddstr(Window,3,2,DBROOT);
  mvwaddstr(Window,4,2,TEXT_ADABASDIRERROR2);
  mvwaddstr(Window,0,2,TEXT_ERRORINSTALL);
  wrefresh(Window);
  getch();
  delwin(Window);
}
/*----- stefan -------------------------------------------------------------------------------*/
void WarnungOracleVerzeichnis()
{
WINDOW* Window;

  Window=newwin(7,40,8,20);
  wbkgdset(Window,COLOR_PAIR(7) | A_BOLD); 
  attron(A_BOLD);
  werase(Window);
  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2,TEXT_ORACLEDIRERROR1);
  mvwaddstr(Window,3,2,DBROOT);
  mvwaddstr(Window,4,2,TEXT_ORACLEDIRERROR2);
  mvwaddstr(Window,0,2,TEXT_ERRORINSTALL);
  wrefresh(Window);
  getch();
  delwin(Window);
}
/*-------------------------------------------------------------------------------------*/
void HilfeAdabasParameterDialog()
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
/*------- stefan ------------------------------------------------------------------------------*/
void HilfeOracleParameterDialog()
{
WINDOW* Window;
int c = 0;

  Window=newwin(7,50,7,16);
  wbkgdset(Window,COLOR_PAIR(8)); 
  wattron(Window,A_BOLD);
  werase(Window);

  wborder(Window,0,0,0,0,0,0,0,0);

  mvwaddstr(Window,1,2,TEXT_ORACLEPARAMHELP1);
  mvwaddstr(Window,2,2,TEXT_ORACLEPARAMHELP2);
  mvwaddstr(Window,3,2,TEXT_ORACLEPARAMHELP3);  
  mvwaddstr(Window,5,2,TEXT_ORACLEPARAMHELP4);  

  mvwaddstr(Window,0,2,TEXT_DIALOGTOP1);

  refresh();
  wrefresh(Window);

  c = getch();

  delwin(Window);
}

/*-------------------------------------------------------------------------------------*/
int AdabasParameterDialog(int InstallAdabas)
{
WINDOW* Window;
FIELD* Fields[3];
FORM* Form;
int Finish = 0;
char* Ptr;

  Window=newwin(8,60,6,10);
  wbkgdset(Window,COLOR_PAIR(4)); 
  werase(Window);

  Fields[0] = new_field(1,29,2,27,0,1);
  Fields[1] = new_field(1,29,4,27,0,1);
  Fields[2] = NULL;

  set_field_buffer(Fields[0],0,"/adabas");
  set_field_buffer(Fields[1],0,"adabas");

  field_opts_off(Fields[0],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[1],O_AUTOSKIP | O_STATIC);

  set_field_just(Fields[0],JUSTIFY_LEFT);
  set_field_just(Fields[1],JUSTIFY_LEFT);

  set_field_fore(Fields[0],0 | A_BOLD);
  set_field_back(Fields[0],6 | A_BOLD);
  set_field_fore(Fields[1],0 | A_BOLD);
  set_field_back(Fields[1],0 | A_BOLD);
  
  Form = new_form(&Fields[0]);

  set_form_win(Form,Window);
  post_form(Form);


  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2,TEXT_ADABASPARAMDIALOG1);
  mvwaddstr(Window,4,2,TEXT_ADABASPARAMDIALOG2);

  mvwaddstr(Window,7,30,TEXT_DIALOGBOTTOM2);

  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2,TEXT_ADABASPARAMDIALOG);

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
            Finish = -1;
            break;
          case KEY_F(10):
          case 13:
            form_driver(Form,REQ_FIRST_FIELD);
            Finish = 1;
            break;
          case KEY_F(1):
            HilfeAdabasParameterDialog();
            refresh();
            break;
          default :
            form_driver(Form,c);
            break;   
        }

    if (Finish == 1)
      {
        strcpy(DBROOT,field_buffer(Fields[0],0));

        if ((Ptr = strchr(DBROOT,' ')) != NULL)
          *Ptr = 0;

        strcpy(DBUSER,field_buffer(Fields[1],0));
        if ((Ptr = strchr(DBUSER,' ')) != NULL)
          *Ptr = 0;


        if (getpwnam(DBUSER) == NULL)
          {
            WarnungAdabasBenutzer();
	    if (FORCE != 1)
              Finish =0;
          }
       if ((InstallAdabas != 1) && (IsFileWriteable(DBROOT) == 0))
          {
            WarnungAdabasVerzeichnis();
            refresh();
	    if (FORCE != 1)
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

  return Finish;
}
/*---------- stefan ---------------------------------------------------------------------------*/
int OracleParameterDialog(int InstallAdabas)
{
WINDOW* Window;
FIELD* Fields[6];
FORM* Form;
int Finish = 0;
char* Ptr;

  Window=newwin(15,60,4,10);
  wbkgdset(Window,COLOR_PAIR(4)); 
  werase(Window);

  Fields[0] = new_field(1,29,2,27,0,1);
  Fields[1] = new_field(1,29,4,27,0,1);
  Fields[2] = new_field(1,29,6,27,0,1);
  Fields[3] = new_field(1,29,8,27,0,1);
  Fields[4] = new_field(1,29,10,27,0,1);
  Fields[5] = NULL;

  set_field_buffer(Fields[0],0,"/opt/oracle/yourDB");
  set_field_buffer(Fields[1],0,"ORCL");
  set_field_buffer(Fields[2],0,"change_on_install");
  set_field_buffer(Fields[3],0,"VSHOP");
  set_field_buffer(Fields[4],0,"VSHOP");

  field_opts_off(Fields[0],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[1],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[2],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[3],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[4],O_AUTOSKIP | O_STATIC);
  
  set_field_just(Fields[0],JUSTIFY_LEFT);
  set_field_just(Fields[1],JUSTIFY_LEFT);
  set_field_just(Fields[2],JUSTIFY_LEFT);
  set_field_just(Fields[3],JUSTIFY_LEFT);
  set_field_just(Fields[4],JUSTIFY_LEFT);

  set_field_fore(Fields[0],0 | A_BOLD);
  set_field_back(Fields[0],6 | A_BOLD);
  set_field_fore(Fields[1],0 | A_BOLD);
  set_field_back(Fields[1],0 | A_BOLD);
  set_field_fore(Fields[2],0 | A_BOLD);
  set_field_back(Fields[2],0 | A_BOLD);
  set_field_fore(Fields[3],0 | A_BOLD);
  set_field_back(Fields[3],0 | A_BOLD);
  set_field_fore(Fields[4],0 | A_BOLD);
  set_field_back(Fields[4],0 | A_BOLD);

  
  Form = new_form(&Fields[0]);

  set_form_win(Form,Window);
  post_form(Form);


  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2,TEXT_ORACLEPARAMDIALOG1);
  mvwaddstr(Window,4,2,TEXT_ORACLEPARAMDIALOG2);
  mvwaddstr(Window,6,2,TEXT_ORACLEPARAMDIALOG3);
  mvwaddstr(Window,8,2,TEXT_ORACLEPARAMDIALOG4);
  mvwaddstr(Window,10,2,TEXT_ORACLEPARAMDIALOG5);

  mvwaddstr(Window,13,30,TEXT_DIALOGBOTTOM2);

  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2,TEXT_ORACLEPARAMDIALOG);

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
            if (field_index(current_field(Form)) == 2)
              form_driver(Form,REQ_FIRST_FIELD);
            else
              form_driver(Form,REQ_NEXT_FIELD);
            break;
          case KEY_CTAB:
          case KEY_UP:
            if (field_index(current_field(Form)) == 0)
				form_driver(Form,REQ_FIRST_FIELD);
    		else
				form_driver(Form,REQ_PREV_FIELD);
	        break;
          case 27:
            Finish = -1;
            break;
          case KEY_F(10):
          case 13:
            form_driver(Form,REQ_FIRST_FIELD);
            Finish = 1;
            break;
          case KEY_F(1):
            HilfeOracleParameterDialog();
            refresh();
            break;
          default :
            form_driver(Form,c);
            break;   
        }

    if (Finish == 1)
      {
        strcpy(DBROOT,field_buffer(Fields[0],0));
        if ((Ptr = strchr(DBROOT,' ')) != NULL)
          *Ptr = 0;

        strcpy(ORACLESID,field_buffer(Fields[1],0));
        if ((Ptr = strchr(ORACLESID,' ')) != NULL)
          *Ptr = 0;
		  
        strcpy(ORACLEDBA,field_buffer(Fields[2],0));
        if ((Ptr = strchr(ORACLEDBA,' ')) != NULL)
          *Ptr = 0;
		  
        strcpy(DBUSER,field_buffer(Fields[3],0));
        if ((Ptr = strchr(DBUSER,' ')) != NULL)
          *Ptr = 0;

        strcpy(DBPASSWORD,field_buffer(Fields[4],0));
        if ((Ptr = strchr(DBPASSWORD,' ')) != NULL)
          *Ptr = 0;

		/*  
        if (getpwnam(DBUSER) == NULL)
          {
            WarnungAdabasBenutzer();
	    if (FORCE != 1)
              Finish =0;
          }
		  */
		  
       if ((InstallAdabas != 1) && (IsFileWriteable(DBROOT) == 0))
          {
			WarnungOracleVerzeichnis();
            refresh();
	    if (FORCE != 1)
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

  return Finish;
}
/*-------------------------------------------------------------------------------------*/
void WarnungHTTPDCONF()
{
WINDOW* Window;

  Window=newwin(7,40,8,20);
  wbkgdset(Window,COLOR_PAIR(7) | A_BOLD); 
  attron(A_BOLD);
  werase(Window);
  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2,TEXT_HTTPDCONFERROR1);
  mvwaddstr(Window,3,2,HTTPDCONF);
  mvwaddstr(Window,4,2,TEXT_HTTPDCONFERROR2);
  mvwaddstr(Window,0,2,TEXT_ERRORINSTALL);
  wrefresh(Window);
  getch();
  delwin(Window);
}
/*-------------------------------------------------------------------------------------*/
void HilfeWWWServer()
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
int WWWServer()
{
ITEM* MenuItems[7];
MENU* Menu;
FORM* Form;
WINDOW* Window;
WINDOW* MenuWindow;
WINDOW* FormWindow;
int Finish = 0;
FIELD* Fields[2];
char Directories[10][50];
char DirectoriesText[10][50];
char DirectoriesNumber[10][5];
int Counter = 0;
int i;
char Buffer1[5];
char Buffer2[80];

  if (access("/etc/httpd/httpd.conf",R_OK) == 0)
    {
      strcpy(&Directories[Counter][0],"/etc/httpd/httpd.conf");
      sprintf(&DirectoriesText[Counter][0],"%s %s",TEXT_WWWSERVERDIALOG2,&Directories[Counter][0]);
      sprintf(&DirectoriesNumber[Counter][0],"%i.",Counter+2);
      Counter++;
    }
  if (access("/etc/httpsd/httpd.conf",R_OK) == 0)
    {
      strcpy(&Directories[Counter][0],"/etc/httpsd/httpd.conf");
      sprintf(&DirectoriesText[Counter][0],"%s %s",TEXT_WWWSERVERDIALOG2,&Directories[Counter][0]);
      sprintf(&DirectoriesNumber[Counter][0],"%i.",Counter+2);
      Counter++;
    }
  if (access("/etc/httpd/conf/httpd.conf",R_OK) == 0)
    {
      strcpy(&Directories[Counter][0],"/etc/httpd/conf/httpd.conf");
      sprintf(&DirectoriesText[Counter][0],"%s %s",TEXT_WWWSERVERDIALOG2,&Directories[Counter][0]);
      sprintf(&DirectoriesNumber[Counter][0],"%i.",Counter+2);
      Counter++;
    }
  if (access("/etc/httpsd/conf/httpd.conf",R_OK) == 0)
    {
      strcpy(&Directories[Counter][0],"/etc/httpsd/conf/httpd.conf");
      sprintf(&DirectoriesText[Counter][0],"%s %s",TEXT_WWWSERVERDIALOG2,&Directories[Counter][0]);
      sprintf(&DirectoriesNumber[Counter][0],"%i.",Counter+2);
      Counter++;
    }

  Fields[0] = new_field(1,45,0,0,0,1);
  Fields[1] = NULL;
  field_opts_off(Fields[0],O_AUTOSKIP | O_STATIC);

  MenuItems[0] = new_item("1.",TEXT_WWWSERVERDIALOG1);

  for (i=0; i < Counter; i++)
    {
      MenuItems[i+1] = new_item(&DirectoriesNumber[i][0],&DirectoriesText[i][0]);
    }

  sprintf(Buffer1,"%i.",Counter+2);
  MenuItems[1+Counter] = new_item(Buffer1,TEXT_WWWSERVERDIALOG6);
  MenuItems[2+Counter] = NULL;

  Window = newwin(7+Counter,60,5,5);
  wbkgdset(Window,COLOR_PAIR(4));
  werase(Window);
  wborder(Window,0,0,0,0,0,0,0,0);

  MenuWindow = newwin(2+Counter,50,7,7);
  wbkgdset(MenuWindow,COLOR_PAIR(4));
  wattron(MenuWindow,A_BOLD);
  werase(MenuWindow);

  FormWindow = newwin(2,48,9+Counter,11);
  wbkgdset(FormWindow,COLOR_PAIR(4));
  werase(FormWindow);

  mvwaddstr(Window,6+Counter,30,TEXT_DIALOGBOTTOM2);

  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2,TEXT_WWWSERVERDIALOG);

  Menu = new_menu(&MenuItems[0]);
  set_menu_fore(Menu,COLOR_PAIR(1));
  set_menu_back(Menu,COLOR_PAIR(3));
  set_menu_win(Menu,MenuWindow);
  post_menu(Menu);

  Form = new_form(&Fields[0]);

  set_field_fore(Fields[0],0 | A_BOLD);

  set_form_win(Form,FormWindow);
  post_form(Form);

  refresh();
  wrefresh(Window);
  wrefresh(FormWindow);
  wrefresh(MenuWindow);

  do
    {
      int c = getch();
      switch(c)
        {
          case KEY_DOWN:
              menu_driver(Menu,REQ_DOWN_ITEM);
              if (item_index(current_item(Menu)) == Counter+1)
                {
                  form_driver(Form,REQ_FIRST_FIELD);
                }
            break;
          case KEY_UP:
            menu_driver(Menu,REQ_UP_ITEM);
            break;
          case KEY_RIGHT:
            if (item_index(current_item(Menu)) == Counter+1)
              form_driver(Form,REQ_NEXT_CHAR);
            break;
          case KEY_LEFT:
            if (item_index(current_item(Menu)) == Counter+1)
              form_driver(Form,REQ_PREV_CHAR);
            break;
          case KEY_DC:
            if (item_index(current_item(Menu)) == Counter+1)
              form_driver(Form,REQ_DEL_CHAR);
            break;
          case KEY_BACKSPACE:
            if (item_index(current_item(Menu)) == Counter+1)
              form_driver(Form,REQ_DEL_PREV);
            break;
          case KEY_HOME:
            if (item_index(current_item(Menu)) == Counter+1)
              form_driver(Form,REQ_BEG_LINE);
            break;
          case KEY_END:
            if (item_index(current_item(Menu)) == Counter+1)
              form_driver(Form,REQ_END_LINE);
            break;
          case 27:
            Finish = -1;
            break;
          case KEY_F(10):
          case 13:
            form_driver(Form,REQ_FIRST_FIELD);
            Finish = 1;
            break;
          case KEY_F(1):
            HilfeWWWServer();
            refresh();
            wrefresh(Window);
            break;
          default :
            if (item_index(current_item(Menu)) == Counter+1)
              {
                form_driver(Form,c);
              }
            else
              {
                menu_driver(Menu,c);
                menu_driver(Menu,REQ_CLEAR_PATTERN);
              }
            break;   
        }

      if (Finish == 1)
        {
          if (item_index(current_item(Menu)) == 0)
            strcpy(HTTPDCONF,"");
          else 
          if (item_index(current_item(Menu)) <= Counter)
            {
              strcpy(HTTPDCONF,&Directories[item_index(current_item(Menu))-1][0]);
            }
          else
            {
              strcpy(HTTPDCONF,field_buffer(Fields[0],0));

              if (strchr(HTTPDCONF,' ') != NULL)
                *strchr(HTTPDCONF,' ') = 0;
            }

          if (strcmp(HTTPDCONF,"") != 0)
            if (access(HTTPDCONF,R_OK) != 0)
              {
                WarnungHTTPDCONF();
                refresh();

                wrefresh(Window);
                wrefresh(FormWindow);
                menu_driver(Menu,REQ_FIRST_ITEM);
                wrefresh(MenuWindow);
                Finish =0;
             }
        }

      wrefresh(MenuWindow);
      if (item_index(current_item(Menu)) == Counter+1)
        wrefresh(FormWindow);

    }
  while(Finish == 0);

  
  unpost_menu(Menu);
  free_menu(Menu);
  free_item(MenuItems[0]);
  free_item(MenuItems[1]);

  unpost_form(Form);
  free_form(Form);
  free_field(Fields[0]);
  
  delwin(FormWindow);
  delwin(MenuWindow);
  delwin(Window);

  return Finish;
}
/*-------------------------------------------------------------------------------------*/
int GetSRM()
{
  return -1;
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
  mvwaddstr(Window,0,2,TEXT_ERRORINSTALL);
  wrefresh(Window);
  getch();
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
  mvwaddstr(Window,0,2,TEXT_ERRORINSTALL);
  wrefresh(Window);
  getch();
  delwin(Window);
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
int InstallAdabas()
{
WINDOW* Window;
char Buffer[200];
char Hostname[200];
struct passwd *Passwd;
int Return;

  Window=newwin(5,40,8,20);
  wbkgdset(Window,COLOR_PAIR(4)); 
  werase(Window);

  wborder(Window,0,0,0,0,0,0,0,0);

  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2,TEXT_INSTALL1);

  wbkgdset(Window,COLOR_PAIR(4)); 
  mvwaddstr(Window,2,1,TEXT_INSTALL2);
  wrefresh(Window);

  mkdir(DBROOT,775);
  
  if (Copy("adabas/install/adabas11.tgz",DBROOT) == 0)
    Exit(2); 

  #ifdef ENGLISH
    if (Copy("adabas/install/docu.tgz", DBROOT) == 0);
  #else
    if (Copy("adabas/install/doku.tgz", DBROOT) == 0);
  #endif

  mvwaddstr(Window,2,1,TEXT_INSTALL3);
  wrefresh(Window);

  chdir(DBROOT);
  if (Untar(DBROOT,"adabas11.tgz") == 0)
    Exit(2);

  #ifdef ENGLISH
    if (Untar(DBROOT,"docu.tgz") == 0);
    Delete(DBROOT,"docu.tgz");
  #else
    if (Untar(DBROOT,"doku.tgz") == 0);
    Delete(DBROOT,"doku.tgz");
  #endif

  gethostname(Hostname,200);

  Passwd = getpwnam(DBUSER);

  delwin(Window);

  def_prog_mode();

  bkgdset(COLOR_PAIR(0));
  erase();
  refresh();
  endwin();
  printf(TEXT_INSTALL4);

  sprintf(Buffer,"bin/x_install -r %s -o %i -g %i -n %s",DBROOT,Passwd->pw_uid,Passwd->pw_gid,Hostname);

  Return = system(Buffer);

  Delete(DBROOT,"adabas11.tgz");

  bkgdset(COLOR_PAIR(1));
  erase();
  border(0,0,0,0,0,0,0,0);
  bkgdset(COLOR_PAIR(2));
  attron(A_BOLD);
  #ifdef DOWNLOAD
    mvaddstr(0,2,TEXT_VERSIONDOWNLOAD);
  #else
    mvaddstr(0,2,TEXT_VERSION);
  #endif

  refresh();

  return Return;
}
/*-------------------------------------------------------------------------------------*/
void ParameterDialog()
{
WINDOW* Window;
FIELD* Fields[5];
FORM* Form;
int Finish = 0;

  Window=newwin(13,60,6,10);
  wbkgdset(Window,COLOR_PAIR(4)); 
  werase(Window);

  Fields[0] = new_field(1,29,2,27,0,1);
  Fields[1] = new_field(1,29,4,27,0,1);
  Fields[2] = new_field(1,29,6,27,0,1);
  Fields[3] = new_field(1,29,8,27,0,1);
  Fields[4] = new_field(1,29,10,27,0,1);
  Fields[5] = NULL;

  set_field_buffer(Fields[0],0,"/DatenbankPfad");
  set_field_buffer(Fields[1],0,"http://ihr.server.name/vshop");
  set_field_buffer(Fields[2],0,"http://ihr.server.name/cgi-bin");
  set_field_buffer(Fields[3],0,"/htdocs");
  set_field_buffer(Fields[4],0,"/usr/local/httpd/cgi-bin");

  field_opts_off(Fields[0],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[1],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[2],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[3],O_AUTOSKIP | O_STATIC);
  field_opts_off(Fields[4],O_AUTOSKIP | O_STATIC);
  
  set_field_fore(Fields[0],0 | A_BOLD);
  set_field_fore(Fields[1],0 | A_BOLD);
  set_field_fore(Fields[2],0 | A_BOLD);
  set_field_fore(Fields[3],0 | A_BOLD);
  set_field_fore(Fields[4],0 | A_BOLD);

  Form = new_form(&Fields[0]);

  set_form_win(Form,Window);
  post_form(Form);

  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,2," Adabas D Verzeichnis : ");
  mvwaddstr(Window,4,2," Shop URL (BASEREF)   : ");
  mvwaddstr(Window,6,2," CGI URL              : ");    
  mvwaddstr(Window,8,2," HTML Verzeichnis     : ");
  mvwaddstr(Window,10,2," CGI Verzeichnis      : ");


  mvwaddstr(Window,12,30,"[F1=Hilfe/F10=OK/ESC=Abbruch]");

  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2," Parameter Eingabe ");

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
            if (field_index(current_field(Form)) == 3)
              form_driver(Form,REQ_FIRST_FIELD);
            else
              form_driver(Form,REQ_NEXT_FIELD);
            break;
          case 13:
          case KEY_CTAB:
            form_driver(Form,REQ_PREV_FIELD);
            break;
          case 27:
            Finish = 2;
            break;
          case KEY_F(10):
            form_driver(Form,REQ_FIRST_FIELD);
            Finish = 1;
          default :
            form_driver(Form,c);
            break;   
        }
     wrefresh(Window);
    }
  while(Finish == 0);

  unpost_form(Form);
  free_form(Form);
  free_field(Fields[0]);
  
  delwin(Window);

  refresh();
}
/*-------------------------------------------------------------------------------------*/
void NeuerShopDialog()
{
WINDOW* Window;
int Finish = 0;

  Window=newwin(5,40,8,20);
  wbkgdset(Window,COLOR_PAIR(4)); 
  werase(Window);


  wborder(Window,0,0,0,0,0,0,0,0);
  mvwaddstr(Window,2,1,TEXT_NEWSHOPDIALOG1);
  mvwaddstr(Window,4,10,TEXT_DIALOGBOTTOM2);

  wbkgdset(Window,COLOR_PAIR(3)); 
  mvwaddstr(Window,0,2,TEXT_NEWSHOPDIALOG);

  wrefresh(Window);

  do
    {
      int c = getch();
      switch(c)
        {
          case 27:
            Finish = 2;
            break;
          case KEY_F(10):
            Finish = 1;
          default :
            break;   
        }
     wrefresh(Window);
    }
  while(Finish == 0);

  delwin(Window);
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
    mvaddstr(0,2,TEXT_VERSION);
  #endif
}
/*-------------------------------------------------------------------------------------*/
static void Exit(int sig)
{
  bkgdset(COLOR_PAIR(0));
  erase();
  refresh();
  endwin();

  if (sig == 30)
    printf(TEXT_ERRORINSTALL1);
  else
  if (sig == 20)
    printf(TEXT_ERRORINSTALL2);
  else
  if (sig <=1)
    #ifdef INSTALL
      printf(TEXT_ERRORINSTALL3);
    #else
      printf("\n");
    #endif
  else
    #ifdef INSTALL
      printf(TEXT_ERRORINSTALL4);
    #else
      printf(TEXT_ERRORINSTALL5);
    #endif

  exit(sig);
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
void MakeVSConfOracle(char* Filename)
{
FILE* File;

  
  if ((File = fopen(Filename,"w")) != NULL)
    {
      fprintf(File,"DBROOT %s\n", DBROOT);
	  fprintf(File,"VSHOP_DBNAME %s\n",ORACLESID);
      fprintf(File,"VSHOP_DBUSER VSHOP\n");
      fprintf(File,"VSHOP_DBPASSWORD VSHOP\n");
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
/*
void MakeVSConf(char* Filename)
{
FILE* File;

  
  if ((File = fopen(Filename,"w")) != NULL)
    {
      fprintf(File,"DBROOT %s\n", DBROOT);
      fprintf(File,"SHOP20_DBUSER SHOP20\n");
      fprintf(File,"SHOP20_DBPASSWORD agettbcjgthoi\n");
      fprintf(File,"SHOP20_DBERRORURL http://%s/shop20/dberror.htm\n", SERVERNAME);
      fprintf(File,"SHOP20_TEMPLATEROOT %sshop20\n", HTDOCS);
      fprintf(File,"SHOP20_BASEREF http://%s/shop20\n", SERVERNAME);
      fprintf(File,"SHOP20_CGIBIN http://%s%sshop20\n", SERVERNAME,CGIBIN);
      fprintf(File,"SHOP20_SHOP20 %sshop20\n", CGIBINPATH);
      fprintf(File,"SHOP20_CHDBPASSWD %sshop20/admin/bin/shop20passwd\n", HTDOCS);
      fprintf(File,"#SHOP20_SECUREBASEREF https://%s/shop20...\n", SERVERNAME);
      fprintf(File,"#SHOP20_SECURECGIBIN https://%s%shop20...\n", SERVERNAME,CGIBIN);
      fclose(File);
    }
}
*/
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
      fprintf(File,"\t;;\n");
      fprintf(File,"\tstop)\n");
 	  fprintf(File,"\t\t# echo \"Shutting down VShop ADABAS D remote SQL.\";\n");
	  fprintf(File,"\t\t# su %s -c \"%sbin/x_server stop\";\n",DBUSER,DBROOT);
	  fprintf(File,"\t\techo \"Shutting down VShop ADABAS D.\";\n");
	  fprintf(File,"\t\tsu %s -c \"%sbin/xutil -d vshop -u control,control shutdown\";\n",DBUSER,DBROOT);
	  fprintf(File,"\t\tsu %s -c \"%sbin/x_stop vshop\";\n",DBUSER,DBROOT);
      fprintf(File,"\t;;\n");
      fprintf(File,"\tclear)\n");
	  fprintf(File,"\t\t echo \"Clearing VShop ADABAS D.\";\n");
      fprintf(File,"\t\tsu %s -c \"%sbin/x_clear vshop\";\n",DBUSER,DBROOT);
      fprintf(File,"\t;;\n");
      fprintf(File,"\t*)\n");
      fprintf(File,"\techo \"Usage $0 {start|stop|clear}\";\n");
      fprintf(File,"esac\n");           
      fprintf(File,"\n");
      fclose(File);
   }
}
/*-------------------------------------------------------------------------------------*/
void main(int argc, char* argv[])
{
int Ret;
int INSTALLADABAS = 0;
/* stefan */
int CONNECTORACLE = 0;

int SERVERISAPACHE = 0;
char* Ptr;
WINDOW* KeyWindow;
char Buffer[200];
char SPVSHOP[200];
char SPVSLCOMPILE[200];
char SPVSCONFIG[200];
char SPMTLCOMPILE[200];

  if (argc >=2)
    if (strcmp(argv[1],"-f") == 0)
      FORCE = 1;
  
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

  signal(SIGINT, Exit);
  Init();

  if (InstallDialog() == -1)
    Exit(1);

  if (Libc5WarnungDialog() == -1)
    Exit(1);
	
  /* stefan */
  #ifndef DB_ORACLE
    #ifndef WITHOUT_ADABAS
      INSTALLADABAS = IstAdabasInstalliert();
      if (INSTALLADABAS == -1)
      {
        Exit(2);
      }
    #else
       INSTALLADABAS = 0;
    #endif
  #else
     INSTALLADABAS = 0;
	 CONNECTORACLE = 1;
  #endif

  refresh();
  #ifndef DB_ORACLE
     Ret = AdabasParameterDialog(INSTALLADABAS);
  #else
     Ret = OracleParameterDialog(INSTALLADABAS);
  #endif
  if (Ret == -1)
    Exit(3);

  refresh();
  SERVERISAPACHE = WWWServer();
  if (SERVERISAPACHE == -1)
    Exit(4);

  if (strcmp(HTTPDCONF,"") != 0)
    {
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
          strcpy(WWWUSER,DBUSER);
        }

      if((Ptr = GetFirstParam("DirectoryIndex")) != NULL)
        {
          strcpy(INDEXHTML,Ptr);
          if ((Ptr = strchr(INDEXHTML,' ')) != NULL)
            *Ptr = 0;


          if((Ptr = GetFirstParam("DocumentRoot ")) != NULL)
            {
              if (*Ptr == '"')
                strcpy(HTDOCS,Ptr+1);
              else
                strcpy(HTDOCS,Ptr);

              if (HTDOCS[strlen(HTDOCS)-1] == '"')
                HTDOCS[strlen(HTDOCS)-1] = '\0';
            }
          else
            strcpy(HTDOCS,"");

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
              strcpy(CGIBINPATH,"");
            }
        }
      else
        {
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
            }
          CloseIniFile();
          OpenIniFile(SRM);

          if((Ptr = GetFirstParam("DirectoryIndex")) != NULL)
            {
              strcpy(INDEXHTML,Ptr);
              if ((Ptr = strchr(INDEXHTML,' ')) != NULL)
                *Ptr = 0;
            }
          else
            strcpy(INDEXHTML,"index.html");

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
            strcpy(HTDOCS,"");

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
              strcpy(CGIBINPATH,"");
            }
        }  
      CloseIniFile();
    }  

  refresh();
  Ret = VShopParameterDialog();
  if (Ret == -1)
    Exit(5);

  if (INSTALLADABAS == 1)
    InstallAdabas();

  chdir(INSTALLPATH);

  KeyWindow=newwin(5,40,8,20);
  wbkgdset(KeyWindow,COLOR_PAIR(4)); 
  werase(KeyWindow);
  wborder(KeyWindow,0,0,0,0,0,0,0,0);

  mvwaddstr(KeyWindow,2,2,TEXT_INSTALL5);
  wrefresh(KeyWindow);

  if (CGIBINPATH[strlen(CGIBINPATH)-1] != '/')
    strcat(CGIBINPATH,"/");

 strcpy(SPVSHOP,"vshop/engine/vshop");
 strcpy(SPVSLCOMPILE,"vshop/engine/vsl-compile");
 strcpy(SPMTLCOMPILE,"vshop/engine/mtl-compile");


  /* vshop kopieren */
  if (Copy(SPVSHOP, CGIBINPATH) == 0)
    exit(2);
  sprintf(Buffer,"%svshop",CGIBINPATH);
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);
#ifndef ENGLISH
  /* pips kopieren */
  if (Copy("vshop/engine/pips", CGIBINPATH) == 0)
    exit(2);
  sprintf(Buffer,"%spips",CGIBINPATH);
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);

  if (Copy("vshop/engine/pips.log", CGIBINPATH) == 0)
    exit(2);
  sprintf(Buffer,"%spips.log",CGIBINPATH);
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);
#endif

  /* vsl-compile kopieren */
  if (Copy(SPVSLCOMPILE, CGIBINPATH) == 0)
    exit(2);
  sprintf(Buffer,"%svsl-compile",CGIBINPATH);
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);

  /* msl-compile kopieren */
  if (Copy(SPMTLCOMPILE, CGIBINPATH) == 0)
    exit(2);
  sprintf(Buffer,"%smtl-compile",CGIBINPATH);
  ChangeOwner(Buffer,WWWUSER);
  ChangeMod(Buffer);

  /* stefan */
  if (CONNECTORACLE == 0)
  {

	  /* adabas vshop kopieren */
	
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
	
	  mvwaddstr(KeyWindow,2,2,TEXT_INSTALL6);
	  wrefresh(KeyWindow);
	
	  chdir(DBROOT);
	  if (Untar(DBROOT,"vsdb.tgz") == 0)
	    exit(2);
	
	  sprintf(Buffer,"%swrk/",DBROOT);
	  ChangeOwner(Buffer,DBUSER);
	  ChangeMod(Buffer);
	
	  sprintf(Buffer,"%swrk/",DBROOT);
	  ChangeOwner(Buffer,DBUSER);
	  ChangeMod(Buffer);
	  Delete(DBROOT,"vsdb.tgz");
	
	  mvwaddstr(KeyWindow,2,2,TEXT_INSTALL7);
	  wrefresh(KeyWindow);
	
	  sprintf(Buffer,"DBROOT=%s %spgm/putparam vshop RUNDIRECTORY %swrk/vshop",DBROOT,DBROOT,DBROOT);
	  system(Buffer);
	
	  sprintf(Buffer,"DBROOT=%s %sbin/x_param -d vshop -u control,control BCHECK",DBROOT,DBROOT);
	  system(Buffer);
	
	  mvwaddstr(KeyWindow,2,2,TEXT_INSTALL8);
	  wrefresh(KeyWindow);     
	
	  chdir(INSTALLPATH);
	
	  chdir(HTDOCS);
	  if (Untar(HTDOCS,"vshtml.tgz") == 0)
	    exit(2);
	  ChangeModRW(HTDOCS);
	
	  MakeVSConf("/etc/vshop.conf");
	  ChangeOwner("/etc/vshop.conf",WWWUSER);
	  ChangeMod("/etc/vshop.conf");
	  MakeVSConf("/etc/vshop.conf.bak");
	  ChangeOwner("/etc/vshop.conf.bak",WWWUSER);
	  ChangeMod("/etc/vshop.conf.bak");
	  MakeIndexhtm();
	  MakeAdminIndexhtm();
	  MakeVShopDB();
	
	  sprintf(Buffer,"%svshopdb",DBROOT);
	  ChangeModX(Buffer);
	
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
	  
	  sprintf(Buffer,"%svshop/",HTDOCS);
	  ChangeOwner(Buffer,WWWUSER);
	  ChangeModRW(Buffer);
	  Delete(HTDOCS,"vshtml.tgz");
	
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
	
	        if (Copy(SPMTLCOMPILE, "/opt/vshop/engine") == 0)
	          exit(2);
	        ChangeOwner("/opt/vshop/engine/mtl-compile",WWWUSER);
	        ChangeMod("/opt/vshop/engine/mtl-compile");
	        ChangeModX("/opt/vshop/engine/mtl-compile");
	
	        if (Copy("vshop/engine/vshoppasswd", "/opt/vshop/engine") == 0)
	          exit(2);
	        ChangeOwner("/opt/vshop/engine/vshoppasswd",WWWUSER);
	        ChangeMod("/opt/vshop/engine/vshoppasswd");
	        ChangeModX("/opt/vshop/engine/vshoppasswd");
	
	        #ifndef ENGLISH
	          /* pips kopieren */
	          Copy("vshop/engine/pips", "/opt/vshop/engine");
	          ChangeOwner("/opt/vshop/engine/pips",WWWUSER); 
	          ChangeMod("/opt/vshop/engine/pips");
	          ChangeModX("/opt/vshop/engine/pips");
	 
	          Copy("vshop/engine/pips.log", "/opt/vshop/engine");
	          ChangeOwner("/opt/vshop/engine/pips.log",WWWUSER);
	          ChangeMod("/opt/vshop/engine/pips.log");
	        #endif
	
	        if (Copy("vshop/vsadmin", "/opt/vshop") == 0)
	          exit(2);
	        ChangeModX("/opt/vshop/vsadmin");
	
	        if (Copy("vshop/createtabs", "/opt/vshop") == 0)
	          exit(2);
	
	        if (Copy("vshop/vshtmlp.tgz", "/opt/vshop/vshtml.tgz") == 0)
	          exit(2);
	
	        if (! FileExists("/opt/vshop/vsadmin.conf"))
	          MakeVSAdminConf();
	      }
	  #endif
	
	  bkgdset(COLOR_PAIR(0));
	  erase();
	  refresh();
	  endwin();
	
	  sprintf(Buffer,"%svshopdb start",DBROOT);
	  system(Buffer);
	  printf(TEXT_ERRORINSTALL3);
	
	  sprintf(Buffer,TEXT_ENDE1,DBROOT);
	  printf(Buffer);
	  sprintf(Buffer,TEXT_ENDE2,HTDOCS);
	  printf(Buffer);
	  sprintf(Buffer,TEXT_ENDE3,SERVERNAME);
	  printf(Buffer);
	  sprintf(Buffer,TEXT_ENDE3b,SERVERNAME);
	  printf(Buffer);
	  printf(TEXT_ENDE4);
	  printf(TEXT_ENDE5);
	  printf(TEXT_ENDE6);
	  printf(TEXT_ENDE7);
	  printf("\n");
  }
  else
  {
	/* stefan */  
	  /* oracle vshop kopieren */
	
	  if (DBROOT[strlen(DBROOT)-1] != '/')
	    strcat(DBROOT,"/");
	
	  if (HTDOCS[strlen(HTDOCS)-1] != '/')
	    strcat(HTDOCS,"/");
	
	  if (Copy("vshop/vshtml.tgz", HTDOCS) == 0)
	    exit(2);
	
	  mvwaddstr(KeyWindow,2,2,TEXT_INSTALL8);
	  wrefresh(KeyWindow);     
	
	  chdir(INSTALLPATH);
	
	  chdir(HTDOCS);
	  if (Untar(HTDOCS,"vshtml.tgz") == 0)
	    exit(2);
	  ChangeModRW(HTDOCS);
	
	  MakeVSConfOracle("/etc/vshop.conf");
	  ChangeOwner("/etc/vshop.conf",WWWUSER);
	  ChangeMod("/etc/vshop.conf");
	  MakeVSConfOracle("/etc/vshop.conf.bak");
	  ChangeOwner("/etc/vshop.conf.bak",WWWUSER);
	  ChangeMod("/etc/vshop.conf.bak");
	  MakeIndexhtm();
	  MakeAdminIndexhtm();
	
	  sprintf(Buffer,"%svshop/",HTDOCS);
	  ChangeOwner(Buffer,WWWUSER);
	  ChangeModRW(Buffer);
	  Delete(HTDOCS,"vshtml.tgz");
	
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
	
	        if (Copy(SPMTLCOMPILE, "/opt/vshop/engine") == 0)
	          exit(2);
	        ChangeOwner("/opt/vshop/engine/mtl-compile",WWWUSER);
	        ChangeMod("/opt/vshop/engine/mtl-compile");
	        ChangeModX("/opt/vshop/engine/mtl-compile");
	
	        if (Copy("vshop/engine/vshoppasswd", "/opt/vshop/engine") == 0)
	          exit(2);
	        ChangeOwner("/opt/vshop/engine/vshoppasswd",WWWUSER);
	        ChangeMod("/opt/vshop/engine/vshoppasswd");
	        ChangeModX("/opt/vshop/engine/vshoppasswd");
	
	        #ifndef ENGLISH
	          /* pips kopieren */
	          Copy("vshop/engine/pips", "/opt/vshop/engine");
	          ChangeOwner("/opt/vshop/engine/pips",WWWUSER); 
	          ChangeMod("/opt/vshop/engine/pips");
	          ChangeModX("/opt/vshop/engine/pips");
	 
	          Copy("vshop/engine/pips.log", "/opt/vshop/engine");
	          ChangeOwner("/opt/vshop/engine/pips.log",WWWUSER);
	          ChangeMod("/opt/vshop/engine/pips.log");
	        #endif
	
	        if (Copy("vshop/vsadmin", "/opt/vshop") == 0)
	          exit(2);
	        ChangeModX("/opt/vshop/vsadmin");
	
	        if (Copy("vshop/vshtmlp.tgz", "/opt/vshop/vshtml.tgz") == 0)
	          exit(2);
	
	        if (! FileExists("/opt/vshop/vsadmin.conf"))
	          MakeVSAdminConf();
	      }
		  
		  mvwaddstr(KeyWindow,2,2,TEXT_INSTALL6);
		  wrefresh(KeyWindow);
		  
	      if (Copy("vshop/createtabs", "/opt/vshop") == 0)
	        exit(2);
	
	      if (Copy("vshop/createvshop", "/opt/vshop") == 0)
	        exit(2);
		
	  	  sprintf(Buffer,"%sbin/sqlplus sys/%s@%s < /opt/vshop/createvshop > /dev/null 2>&1",DBROOT,ORACLEDBA,ORACLESID);
		  system(Buffer);
		  
		  sprintf(Buffer,"%sbin/sqlplus vshop/vshop@%s < /opt/vshop/createtabs > /dev/null 2>&1",DBROOT,ORACLESID);
		  system(Buffer);
		  
	  #endif
	  
	  #ifndef VSHOP_PROVIDER
		  mvwaddstr(KeyWindow,2,2,TEXT_INSTALL6);
		  wrefresh(KeyWindow);
		  
	  	  sprintf(Buffer,"%sbin/sqlplus sys/%s@%s < vshop/createvshop > /dev/null 2>&1",DBROOT,ORACLEDBA,ORACLESID);
		  system(Buffer);
		  
		  sprintf(Buffer,"%sbin/sqlplus vshop/vshop@%s < vshop/createtabs > /dev/null 2>&1",DBROOT,ORACLESID);
		  system(Buffer);
	  #endif
	
	  bkgdset(COLOR_PAIR(0));
	  erase();
	  refresh();
	  endwin();

	  printf(TEXT_ERRORINSTALL3);
	
	  sprintf(Buffer,TEXT_ENDE1b);
	  printf(Buffer);
	  sprintf(Buffer,TEXT_ENDE2,HTDOCS);
	  printf(Buffer);
	  sprintf(Buffer,TEXT_ENDE3,SERVERNAME);
	  printf(Buffer);
	  sprintf(Buffer,TEXT_ENDE3b,SERVERNAME);
	  printf(Buffer);
	  printf(TEXT_ENDE4);
	  printf(TEXT_ENDE5);
	  printf(TEXT_ENDE6);
	  printf(TEXT_ENDE7);
	  printf("\n");
  
  }

}
