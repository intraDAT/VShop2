/*
 *
 * update program for 
 *
 * updates from pre vshop version 1.6.1
 *
 */

/* -------------------------------------------------- */

/*
 * 
 * defines
 *
 */

#define PROVIDER

#define VERSION "161"

#define CHMAP (S_IRWXU | S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG |  S_IRGRP | S_IWGRP | S_IXGRP | S_IRWXO | S_IROTH | S_IWOTH | S_IXOTH)

#define COMMENT 1

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define OLD 1
#define NEW 2

#ifndef S_IFMT
#define S_IFMT 00170000
#endif

#ifndef S_IFLNK
#define S_IFLNK 0120000
#endif

/* -------------------------------------------------- */

/* 
 *
 * includes 
 *
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
/*#include <statbuf.h>*/
#include <stdarg.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* #include <asm/errno.h> */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h> 

/* -------------------------------------------------- */

/*
 *
 * structures 
 *
 */

typedef struct tableStruct
{
  char DBROOT[256];
  char DBUSER[256];
  char DBPASSWORD[256];
  char TEMPLATEROOT[256];
  char BASEREF[256];
  char PROFILING[2];
  char TIMEOUTURL[512];
  char DBERRORURL[512];
  char MAILSERVER[100];
  char MAILFROM[256];
  char EURODM[24];
  char EUROUSD[24];
  char RTF[256];
  char ADMINREAD[256];
  struct stat *AdminRights;
  struct stat *DokRights;
} tstrT;

typedef struct vsStruct
{
  char *SHOPNAME;
  struct vsStruct *next;
} vsstrT;

typedef struct fList
{
  char *name;
  ino_t inode;
  struct fList *next;
} flistT;

typedef struct inoList
{
  ino_t inode;
  struct inoList *next;
} inostrT;

/* -------------------------------------------------- */

/*
 *
 * global variables
 *
 */

int count = 0;
int stage = 0;
int vshopfd = 0;

char tableNam[7];

vsstrT *gvsBufferPtr = NULL;

char *findPath[3] = {"/etc", "/sbin", "/var"};

/* -------------------------------------------------- */

/*
 *
 * prototypes
 *
 */

int AdabasRun (tstrT *);
int Addstr (char *, char *, int, char *, int);
int RecChmod (char *, struct stat *);
int RecRem (char *);
int Libc ();
int Variables (char *, int, char *, tstrT *);
int VariSearch (char *, int, char *, int, int, char *, char *);
int VsadFile (vsstrT *);

char *GetNextFile (flistT *);
char *VsadNext ();

struct stat *AdminRights (char *);
struct stat *DokRights (char *);

flistT *FindFile (char *, char *, flistT *, inostrT *);

int UpSystem (char *);
int ShSystem (char *);

void UpExit (int);

/* -------------------------------------------------- */

int
main (int argc, char **argv)
{
  extern char **environ;

  int adabasState;
  int bufLen;
  int forkRes;
  int i;
  int oldNewDoc;
  int readres;
  int unreadlast;
  int unread;

  char actwd[256];
  char cgiPath[256];
  char instDir[256];
  char startwd[256];
  char systemStr[1024];
  char table[64];

  char testBuf[512];

  char *buffer;
  char *testPtr;
  char *s1Ptr;
  char *vsPtr;

  off_t foffset;
  uid_t iam;

  struct stat fstat;
  struct stat fstat0;

  struct dirent *testdirent;
  struct passwd *pwstr;

  tstrT tableVar;

  vsstrT *vsBuffer;

  if (argc > 1)
    Help ();

  adabasState = FALSE;
  stage = 0;

  memset (&tableNam[0], '\0', sizeof (tableNam));
  sprintf (&tableNam[0], "vshop");

  /* ***** test if we are superuser ***** */

  printf ("\n\n\t Es wird ueberprueft ob das Programm ueber \"root\" Rechte verfuegt ...\n");

  if ((pwstr = (struct passwd *) getpwnam ("root")) == NULL)
    {
      printf ("\n\n(001) error: can't get passwd file entry for \"root\",\n exiting update program !\n");
      UpExit (-1);
    }

  iam = getuid ();

  if (pwstr->pw_uid != iam)
    {
      printf ("\n\n(002) error: become \"root\" before you start the update program,\n exiting update program !\n");
      UpExit (-1);
    }

      /* ***** get the directory where update is installed ***** */
  
      memset (&testBuf[0], '\0', sizeof (testBuf));

      if (argv[0][0] != '/')
	{
	  if (getcwd(&testBuf[0], sizeof (testBuf)) == NULL)
	    {
	      printf ("\n\n(003) error: internal error testBuf to small to hold path for actual working directory,\n exiting update program !\n");
	      UpExit (-1);
	    }
      
	  strcat (&testBuf[0], "/");
	}
  
      strcat (&testBuf[0], argv[0]);

      s1Ptr = (char *) strrchr (&testBuf[0], '/');
      *(s1Ptr + 1) = '\0';

      if (chdir ((const char *) &testBuf[0]) != 0)
	{
	  printf ("\n\n(004) error: internal error path %s not found,\n exiting update program !\n", &testBuf[0]);
	  UpExit (-1);
	}
  
      memset (&instDir[0], '\0', sizeof (instDir));

      if (getcwd (&instDir[0], sizeof (instDir)) != &instDir[0])
	{
	  printf ("\n\n(005) error: could not get actuell working directory,\n exiting update program !\n");
	  UpExit (-1);
	}

  /* ***** open the file "/etc/vshop.conf" and read out variose usefull variables ***** */

  printf ("\n\n\t Es werden nun verschiedene Variablen aus der Datei \n \"/etc/vshop.conf\" ausgelesen ...\n");

  if ((vshopfd = open("/etc/vshop.conf", O_RDWR | O_APPEND | O_SYNC)) == -1)
    {
      printf ("\n\n(006) error: can't open \"/etc/vshop.conf\" file does not exist or no permision,\n exiting update program !\n");
      UpExit (-1);
    }

  if ((foffset = lseek (vshopfd, 0, SEEK_END)) <= 0)
    {
      printf ("\n\n(007) error: can't read from \"/etc/vshop.conf\" or file corrupted,\n exiting update program !\n");
      UpExit (-1);
    }

  bufLen = foffset + 1;
  if ((buffer = (char *) calloc (1, bufLen)) == NULL)
    {
      printf ("\n\n(008) error: can't allocat buffer memory,\n exiting update program !\n");
      UpExit (-1);
    }

  if (lseek (vshopfd, 0, SEEK_SET) == -1)
    {
      printf ("\n\n(009) error: can't read from \"/etc/vshop.conf\" or file corrupted,\n exiting update program !\n");
      UpExit (-1);
    }

  unreadlast = 0;
  unread = foffset;
  readres = 0;
  
  while (readres < foffset)
    {
      unread = (int ) read (vshopfd, (void *) buffer, (size_t ) unread);
      if (unreadlast > unread)
	{
	  printf ("\n\n(010) error: in reading \"/etc/vshop.conf\" file,\n exiting update program !\n");
	  UpExit (-1);
	}
      readres += unread;
      unreadlast = unread;
    }

#ifndef PROVIDER

  memset (&tableNam[0], '\0', sizeof (tableNam));
  sprintf (&tableNam[0], "vshop");

#endif 

#ifdef PROVIDER

  if (VsadFile (vsBuffer) != 0)
    {
      printf ("\n\n(011) error: in reading \"/opt/vshop/vsadmin.conf\" file,\n exiting update program !\n");
      UpExit (-1);
    }

  while ((vsPtr = VsadNext ()) != NULL)
    {
      memset (&tableNam[0], '\0', sizeof (tableNam));
      strcpy (&tableNam[0], vsPtr);

#endif

      memset (&tableVar, '\0', sizeof (tableVar));  
      
      if (Variables (buffer, bufLen, &tableNam[0], &tableVar) == -1)
	{
	  printf ("\n\n(012) error: in reading \"/etc/vshop.conf\" file,\n exiting update program !\n");
	  UpExit (-1);
	}

      /* ***** test if the adabas database is running ***** */

      if (adabasState == FALSE)
	{
	  printf ("\n\n\t Fuer das Update ist es notwendig, dass die Datenbank \n vshop gestartet ist,sollte dies noch nicht geschehen sein \n wird versucht das Start Script \"vshopdb\" auszufuehren ...\n");
	  adabasState = TRUE;
	}

      if (AdabasRun(&tableVar) != 1)
	{
	  printf ("\n\n(013) error: it is nececarry that database \"vshop\" is running,\n \
please start the database, \n \
the programm was not able to atomaticly detect the startup script.\n exiting update program !\n");
	  UpExit (-1);
	}

      /* ***** first get the actual working directory "startwd" ***** */

      if ((testPtr = getcwd (&startwd[0], 255)) != &startwd[0])
	{
	  printf ("\n\n(014) error: can't get actual working directory,\n exiting update program !\n");
	  UpExit (-1);
	}

#ifdef PROVIDER

      /* ***** test if directory '/opt' is present ***** */

      memset (&fstat, '\0', sizeof (struct stat));

      if (stat ("/opt", &fstat) != 0)
	{
	  printf ("\n\n(015) error: directory \"/opt\" doase not exist, don't know where to find the admin region\n ,\n exiting update program !\n");
	  UpExit (-1);
	}

      memset (&fstat, '\0', sizeof (struct stat));

      if (stat ("/opt/vshop", &fstat) != 0)
	{
	  printf ("\n\n(016) error: directory \"/opt\" doase not exist, don't know where to find the admin region\n ,\n exiting update program !\n");
	  UpExit (-1);
	}

#endif

      /* ***** determin the rights, set on files in the admin zone ***** */

      if ((tableVar.AdminRights = AdminRights(&tableVar.TEMPLATEROOT[0])) == 0)
	{
	  printf ("\n\n(017) error: directory \"%s/admin\" does not exist or access denied,\n exiting update program !\n", &tableVar.TEMPLATEROOT[0]);
	  UpExit (-1);
	}

      if ((tableVar.DokRights = DokRights(&tableVar.TEMPLATEROOT[0])) == 0)
	{
	  printf ("\n\n(018) error: directory \"%s/dok\" does not exist or access denied,\n exiting update program !\n", &tableVar.TEMPLATEROOT[0]);
	  UpExit (-1);
	}

      /* ***** create a backup of admin region ***** */

      /* ***** create backup statement ***** */

      printf ("\n\n\tErstellen eines Backups des alten Administrationsbereichs \n unter %s ...\n", &tableVar.TEMPLATEROOT[0]);

      if (chdir (&tableVar.TEMPLATEROOT[0]) != 0)
	{
	  printf ("\n\n(019) error: could not change working directory to \"%s\" ,\n exiting update program !\n", &tableVar.TEMPLATEROOT[0]);
	  UpExit (-1);
	}

      oldNewDoc = OLD;
      
      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "./update-backup.%s.tar.gz", VERSION);
      
      if (stat (&systemStr[0], &fstat0) == -1)
	{
      
	  memset (&systemStr[0], '\0', sizeof (systemStr));  
	  sprintf (&systemStr[0], "tar zcf ./update-backup.%s.tar.gz ./admin ./dok", VERSION);

	  if (UpSystem (&systemStr[0]) != 0)
	    {
	      printf ("\n\n(020) error: could not backup your old admin and dok directorys\n under %s ,\n try to tar vshop_documentation directory!\n", &tableVar.TEMPLATEROOT[0]);
	  
	      memset (&systemStr[0], '\0', sizeof (systemStr));  
	      sprintf (&systemStr[0], "tar zcf ./update-backup.%s.tar.gz ./admin ./vshop_documentation", VERSION);

	      if (UpSystem (&systemStr[0]) != 0)
		{
		  printf ("\n\n(021) error: could not backup your vshop_documentation directory,\n exiting update program !\n");
		  UpExit (-1);
		}
	  
	      else
		oldNewDoc = NEW;
	    }
	}

      /* ***** wipe out the old admin region ***** */

      /* ***** remove old admin region statement ***** */

      printf ("\n\n\tLoeschen des alten Administrations- und Dokumentationsbereiches...\n");

      memset (&systemStr[0], '\0', sizeof (systemStr));
      
      if (oldNewDoc = OLD)
	sprintf (&systemStr[0], "rm -fr %s/admin %s/dok", &tableVar.TEMPLATEROOT[0], &tableVar.TEMPLATEROOT[0]);

      else
	sprintf (&systemStr[0], "rm -fr %s/admin %s/vshop_documentation", &tableVar.TEMPLATEROOT[0], &tableVar.TEMPLATEROOT[0]);

      if (ShSystem (&systemStr[0]) != 0)
	{
	  printf ("\n\n(022) error: could not remove your old admin and dok directorys under %s ,\n exiting update program !\n", &tableVar.TEMPLATEROOT[0]);
	  UpExit (-1);
	}


      /* ***** for recovery reasons we reached stage 1  ***** */

      stage = 1;

      if (chdir ((const char *) &tableVar.TEMPLATEROOT[0]) != 0)
	{
	  printf ("\n\n(023) error: could not change working directory to %s ,\n exiting update program !\n", &tableVar.TEMPLATEROOT[0]);
	  UpExit (-1);
	}
  
      /* ***** extract the new admin region ***** */

      /* ***** extrtact statement ***** */

      printf ("\n\n\t Entpacken des neuen Administrations Bereiches ...\n");

      memset (&systemStr[0], '\0', sizeof (systemStr));  
      sprintf (&systemStr[0], "tar zxvf %s/vshtml%supdate.tgz", &instDir[0], VERSION);

      if (ShSystem (&systemStr[0]) != 0)
	{
	  printf ("\n\n(024) error: error in tar command,\n exiting update program !\n");
	  UpExit (-1);
	}

      if (setenv ("DBROOT", &tableVar.DBROOT[0], 1) != 0)
	{
	  printf ("\n\n(025) error: could not set environment variable \"DBROOT\",\n exiting update program !\n");
	  UpExit (-1);
	}

      if (chdir ((const char *) &tableVar.DBROOT[0]) != 0)
	{
	  printf ("\n\n(026) error: could not change directory to %s,\n exiting update program !\n", &tableVar.DBROOT[0]);
	  UpExit (-1);
	}

      /* ***** set access rights for admin region ***** */

      memset (&systemStr[0], '\0', sizeof (systemStr));  
      sprintf (&systemStr[0], "%s/admin", &tableVar.TEMPLATEROOT[0]);
      
      if (RecChmod (&systemStr[0], tableVar.AdminRights) != 0)
	{
	  printf ("\n\n(027) error: could not set access rights under %s/admin,\n exiting update program !\n", &tableVar.TEMPLATEROOT[0]);
	  UpExit (-1);
	}

      memset (&systemStr[0], '\0', sizeof (systemStr));  
      sprintf (&systemStr[0], "%s/vshop_documentation", &tableVar.TEMPLATEROOT[0]);
      
      if (RecChmod (&systemStr[0], tableVar.DokRights) != 0)
	{
	  printf ("\n\n(028) error: could not set access rights under %s/vshop_documentation,\n exiting update program !\n", &tableVar.TEMPLATEROOT[0]);
	  UpExit (-1);
	}
      
      /* ***** get the database working ***** */

      /* ***** database startup statement ***** */

      printf ("\n\n\t Anpassen der Datenbank ...\n");

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "%s/bin/xload -d vshop -u %s,%s -S ADABAS -b %s/vsdb%supdate", &tableVar.DBROOT[0], \
	       &tableVar.DBUSER[0], &tableVar.DBPASSWORD[0], &instDir[0], VERSION);

      system (&systemStr[0]);

      /*
	if (system (&systemStr[0]) != 0)
	{
	printf ("\n\n(029) error: error in execution of xload,\n exiting update program !\n");
	UpExit (-1);
	}
	*/

      /* ***** update the database ***** */

      printf ("\n\n________________________________________\n\n \
\t UPDATE des vshops \"%s\"\n\n", &tableNam[0]);

      while (TRUE)
	{      
	  printf ("\t Bitte geben sie den Pfad des CGI-Verzeichnisses\n \
\t fuer den shop \"%s\" an:\n\n\t>", &tableNam[0], &tableNam[0]);
      
	  memset (&cgiPath[0], '\0', sizeof (cgiPath));
	  fgets (&cgiPath[0], sizeof (cgiPath), (FILE *) stdin);
	  
      printf ("\n\n________________________________________\n\n");

	  while ((cgiPath[strlen (&cgiPath[0]) - 1] == '\n') ||
		 (cgiPath[strlen (&cgiPath[0]) - 1] == '\t') ||
		 (cgiPath[strlen (&cgiPath[0]) - 1] == ' '))
	    cgiPath[strlen (&cgiPath[0]) - 1] = '\0';
      
	  memset (&fstat, '\0', sizeof (struct stat));
	  memset (&fstat0, '\0', sizeof (struct stat));

	  memset (&systemStr[0], '\0', sizeof (systemStr));
	  sprintf (&systemStr[0], "%s/%s", &cgiPath[0], &tableNam[0]);

	  if ((stat ((const char *) &cgiPath[0], &fstat) != 0) ||
	      (stat ((const char *) &systemStr[0], &fstat0) != 0))
	    printf ("\n\n\t Das Verzeichniss \"%s\" existiert nicht,\n\t \
oder ist nicht das korrekte CGI-Verzeichniss \n\t \
fuer \"%s\"\n\n", &cgiPath[0], &tableNam[0]);

	  else
	    break;
	}

      /* ***** copy the right vshop engine in place ***** */

      /* ***** vshop 'engine' statement ***** */

      printf ("\n\n\t Kopieren der Vshop \"engine\"\n");

      memset (&testBuf[0], '\0', sizeof (testBuf));
      sprintf (&testBuf[0], "libc.so.6");
  
      if (Libc (&testBuf[0]) == 0)
	{
	  if (chdir (&tableVar.TEMPLATEROOT[0]) != 0)
	    {
	      printf ("\n\n(030) error: error could not change working directory to \"%s\",\n exiting update program !\n", &tableVar.TEMPLATEROOT[0]);
	      UpExit (-1);
	    }
	  
	  memset (&systemStr[0], '\0', sizeof (systemStr));
	  sprintf (&systemStr[0], "cp -f %s/engine/libc6/vshop %s/%s", &instDir[0], &cgiPath[0], &tableNam[0]);
      
	  if (UpSystem (&systemStr[0]) != 0)
	    {
	      printf ("\n\n(031) error: error in execution of cp command,\n exiting update program !\n");
	      UpExit (-1);
	    }

	  memset (&systemStr[0], '\0', sizeof (systemStr));
	  sprintf (&systemStr[0], "%s/%s", &cgiPath[0], &tableNam[0]);
	  
	  setreuid (getuid(), getuid ());
	  setregid (getgid(), getgid ());

	  chown (&systemStr[0], (tableVar.AdminRights)->st_uid, (tableVar.AdminRights)->st_gid);
	  
	  setreuid (getuid(), (tableVar.AdminRights)->st_uid);
	  setregid (getgid(), (tableVar.AdminRights)->st_gid);
	  
	  chmod (&systemStr[0], (tableVar.AdminRights)->st_mode & CHMAP);

	  memset (&systemStr[0], '\0', sizeof (systemStr));
	  sprintf (&systemStr[0], "cp -f %s/engine/libc6/vsl-compile %s/vsl-compile", &instDir[0], &cgiPath[0]);

	  if (ShSystem (&systemStr[0]) != 0)
	    {
	      printf ("\n\n(032) error: error in execution of cp command,\n exiting update program !\n");
	      UpExit (-1);
	    }

	  memset (&systemStr[0], '\0', sizeof (systemStr));
	  sprintf (&systemStr[0], "%s/vsl-compile", &cgiPath[0]);
		  
	  setreuid (getuid(), getuid ());
	  setregid (getgid(), getgid ());
		
	  chown (&systemStr[0], (tableVar.AdminRights)->st_uid, (tableVar.AdminRights)->st_gid);

	  setreuid (getuid(), (tableVar.AdminRights)->st_uid);
	  setregid (getgid(), (tableVar.AdminRights)->st_gid);

	  chmod (&systemStr[0], (tableVar.AdminRights)->st_mode & CHMAP);
	}
 
      else
	{
	  memset (&testBuf[0], '\0', sizeof (testBuf));
	  sprintf (&testBuf[0], "libc.so.5");
      
	  if (Libc (&testBuf[0]) == 0)
	    {
	      if (chdir (&tableVar.TEMPLATEROOT[0]) != 0)
		{
		  printf ("\n\n(033) error: error could not change working directory to \"%s\",\n exiting update program !\n", &tableVar.TEMPLATEROOT[0]);
		  UpExit (-1);
		} 

	      memset (&systemStr[0], '\0', sizeof (systemStr));
	      sprintf (&systemStr[0], "cp -f %s/engine/libc5/vshop %s/%s", &instDir[0], &cgiPath[0], &tableNam[0]);

	      if (UpSystem (&systemStr[0]) != 0)
		{
		  printf ("\n\n(034) error: error in execution of command cp,\n exiting update program !\n");
		  UpExit (-1);
		} 

	      memset (&systemStr[0], '\0', sizeof (systemStr));
	      sprintf (&systemStr[0], "%s/%s", &cgiPath[0], &tableNam[0]);

	      setreuid (getuid(), (tableVar.AdminRights)->st_uid);
	      setregid (getgid(), (tableVar.AdminRights)->st_gid);

	      chmod (&systemStr[0], (tableVar.AdminRights)->st_mode & CHMAP);
		      
	      setreuid (getuid(), getuid ());
	      setregid (getgid(), getgid ());
	      
	      chown (&systemStr[0], (tableVar.AdminRights)->st_uid, (tableVar.AdminRights)->st_gid);

	      memset (&systemStr[0], '\0', sizeof (systemStr));
	      sprintf (&systemStr[0], "cp -f %s/engine/libc5/vsl-compile %s/vsl-compile", &instDir[0],  &cgiPath[0]);

	      if (UpSystem (&systemStr[0]) != 0)
		{
		  printf ("\n\n(035) error: error in execution of command cp,\n exiting update program !\n");
		  UpExit (-1);
		} 

	      memset (&systemStr[0], '\0', sizeof (systemStr));
	      sprintf (&systemStr[0], "%s/vsl-compile", &cgiPath[0]);
	      
	      setreuid (getuid(), (tableVar.AdminRights)->st_uid);
	      setregid (getgid(), (tableVar.AdminRights)->st_gid);
	      
	      chmod (&systemStr[0], (tableVar.AdminRights)->st_mode & CHMAP);
	      
	      setreuid (getuid(), getuid ());
	      setregid (getgid(), getgid ());
	      
	      chown (&systemStr[0], (tableVar.AdminRights)->st_uid, (tableVar.AdminRights)->st_gid);
	    }
      
	  else
	    {
	      printf ("\n\n(036) error: can't determin your libc version,\n exiting update program !\n");
	      UpExit (-1);
	    }
	}

      /* ***** removing all user '.p' files ***** */

      /* ***** remove statement ***** */

      printf ("\n\n\tLoeschen aller \"user\" .p Dateien unter %s ...\n", &tableVar.TEMPLATEROOT[0]);

      if (RecRem (&tableVar.TEMPLATEROOT[0]) != 0)
	{
	  printf ("\n\n(037) error: can't remove all user \".p\" files,\n exiting update program !\n");
	  UpExit (-1);
	}

      /* ***** for recovery reasons we reached stage 2  ***** */

      stage = 2;

      /* ***** search for 'productlink' in TEMPLATEROOT ***** */

      /* ***** search statement ***** */

      printf ("\n\n\tSuchen nach \"productlink\" in allen Textdateien\n unter %s ...\n", &tableVar.TEMPLATEROOT[0]);

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "grep --line-number -2 productlink `find %s/ -regex .*\\.[htm,html,HTM,HTML]$` > %s/productlink.log 2> /dev/null", &tableVar.TEMPLATEROOT[0], &tableVar.TEMPLATEROOT[0]);
  
      ShSystem (&systemStr[0]);

      /* --- O D
	 if (system (&systemStr[0]) != 0)
	 {
	 printf ("\n\n(038) error: error in execution of command string,\n exiting update program !\n");
	 UpExit (-1);
	 }
	 O D --- */
  
#ifdef PROVIDER
  
      /* ***** add additional configuration statments to '/etc/vshop.conf' ***** */

      /* ***** configuration statement ***** */

      printf ("\n\n\tErgaenzen von /etc/vshop.conf ...\n");

      i = 0;
      
      while (i < strlen (tableNam))
	{
	  table[i] = toupper (*(tableNam + i));
	  i++;
	}

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "%s_PROFILING", &table[0]);

      if (strstr (buffer, &systemStr[0]) == NULL)
	if (Addstr (&tableNam[0], "1", vshopfd, "_PROFILING ", 0) != 0)
	  {
	    printf ("\n\n(039) error: could not add PROFILING string to \"/etc/vshop.conf\",\n exiting update program !\n");
	    UpExit (-1);
	  }

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "%s_TIMEOUTURL", &table[0]);

      if (strstr (buffer, &systemStr[0]) == NULL)
	if (Addstr (&tableNam[0], "http://your.server.name/...", vshopfd, "_TIMEOUTURL ", COMMENT) != 0)
	  {
	    printf ("\n\n(040) error: could not add TIMEOUTURL string to \"/etc/vshop.conf\",\n exiting update program !\n");
	    UpExit (-1);
	  }

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "%s_DBERRORURL", &table[0]);

      if (strstr (buffer, &systemStr[0]) == NULL)
	if (Addstr (&tableNam[0], "http://your.server.name/...", vshopfd, "_DBERRORURL ", COMMENT) != 0)
	  {
	    printf ("\n\n(041) error: could not add DBERRORURL string to \"/etc/vshop.conf\",\n exiting update program !\n");
	    UpExit (-1);
	  }

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0],"%s_MAILSERVER",  &table[0]);

      if (strstr (buffer, &systemStr[0]) == NULL)
	if (Addstr (&tableNam[0], "your.mail.server", vshopfd, "_MAILSERVER ", COMMENT) != 0)
	  {
	    printf ("\n\n(042) error: could not add MAILSERVER string to \"/etc/vshop.conf\",\n exiting update program !\n");
	    UpExit (-1);
	  }

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "%s_MAILFROM", &table[0]);

      if (strstr (buffer, &systemStr[0]) == NULL)
	if (Addstr (&tableNam[0], "shop@your.server.name", vshopfd, "_MAILFROM ", COMMENT) != 0)
	  {
	    printf ("\n\n(043) error: could not add MAILFROM string to \"/etc/vshop.conf\",\n exiting update program !\n");
	    UpExit (-1);
	  }

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "%s_MAILTO", &table[0]);

      if (strstr (buffer, &systemStr[0]) == NULL)
	if (Addstr (&tableNam[0], "orders@your.server.name", vshopfd, "_MAILTO ", COMMENT) != 0)
	  {
	    printf ("\n\n(044) error: could not add MAILTO string to \"/etc/vshop.conf\",\n exiting update program !\n");
	    UpExit (-1);
	  }

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "%s_EURODM", &table[0]);

      if (strstr (buffer, &systemStr[0]) == NULL)
	if (Addstr (&tableNam[0], "0.5112918811962", vshopfd, "_EURODM ", 0) != 0)
	  {
	    printf ("\n\n(045) error: could not add EURODM string to \"/etc/vshop.conf\",\n exiting update program !\n");
	    UpExit (-1);
	  }

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "%s_EUROUSD", &table[0]);

      if (strstr (buffer, &systemStr[0]) == NULL)
	if (Addstr (&tableNam[0], "0.8637816360024", vshopfd, "_EUROUSD ", 0) != 0)
	  {
	    printf ("\n\n(046) error: could not add EUROUSD string to \"/etc/vshop.conf\",\n exiting update program !\n");
	    UpExit (-1);
	  }

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "%s_RTF", &table[0]);

      if (strstr (buffer, &systemStr[0]) == NULL)
	if (Addstr (&tableNam[0], "application/rtf", vshopfd, "_RTF ", 0) != 0)
	  {
	    printf ("\n\n(047) error: could not add RTF string to \"/etc/vshop.conf\",\n exiting update program !\n");
	    UpExit (-1);
	  }

      memset (&systemStr[0], '\0', sizeof (systemStr));
      sprintf (&systemStr[0], "%s_ADMINIP", &table[0]);

      if (strstr (buffer, &systemStr[0]) == NULL)
	if (Addstr (&tableNam[0], "*", vshopfd, "_ADMINIP ", 0) != 0)
	  {
	    printf ("\n\n(047) error: could not add ADMINIP string to \"/etc/vshop.conf\",\n exiting update program !\n");
	    UpExit (-1);
	  }


      /* ***** for recovery reasons we reached stage 3  ***** */

      stage = 3;

    }

#endif

  /* ***** new vsadmin region will be copied to '/opt/vshop/..' ***** */

  memset (&systemStr[0], '\0', sizeof (systemStr));  
  
  sprintf (&systemStr[0], "cp -f %s/vshtmlp%s.tgz /opt/vshop/vshtml.tgz", &instDir[0], VERSION);

  if (ShSystem (&systemStr[0]) != 0)
    {
      printf ("\n\n(048) error: error in cp command,\n exiting update program !\n");
      UpExit (-1);
    }

  /* ***** for recovery reasons we reached stage 4  ***** */

  stage = 4;

  /* ***** 'vsadmin'statement ***** */

  printf ("\n\n\t Kopieren von \"vsadmin\"\n");

  memset (&systemStr[0], '\0', sizeof (systemStr));  

  sprintf (&systemStr[0], "cp -f %s/engine/vsadmin /opt/vshop/vsadmin", &instDir[0]);

  if (ShSystem (&systemStr[0]) != 0)
    {
      printf ("\n\n(049) error: error in cp command,\n exiting update program !\n");
      UpExit (-1);
    }

  /* ***** for recovery reasons we reached stage 5  ***** */

  stage = 5;

  memset (&systemStr[0], '\0', sizeof (systemStr));  
  
  sprintf (&systemStr[0], "cp -f %s/createtabs%s /opt/vshop/createtabs", &instDir[0], VERSION);

  if (ShSystem (&systemStr[0]) != 0)
    {
      printf ("\n\n(050) error: error in cp command,\n exiting update program !\n");
      UpExit (-1);
    }

  /* ***** for recovery reasons we reached stage 6  ***** */
  
  stage = 6;


  /* ***** end statment of update program ***** */

  printf ("\n\n\tDas Update auf VShop %s wurde durchgefuehrt.\n \
\t Bitte ueberpruefen Sie die Zugriffsrechte der neu\n \
\t installierten Dateien, da es sonst zu Problemen\n \
\t kommen kann.\n\n",VERSION);

}

/* -------------------------------------------------- */

/*
 *
 * Variables function
 *
 */


int
Variables (char *buffer, int bufLen, char *tableNam, tstrT *tableVar)
{
  int i;

  char searchStr[255];
  char table[64];

  char *s1Ptr; 
  char *s2Ptr;

  struct stat fstat;

  i = 0;

  memset (&table[0], '\0', sizeof (table));

  while (i <= strlen (tableNam))
    {
      table[i] = toupper(*(tableNam + i));
      i++;
    }
    
  if ((s1Ptr = (char *) strstr ((const char *) buffer, "DBROOT")) == NULL)
    return (-1);
  
  s2Ptr = s1Ptr + strlen ("DBROOT");

  while (((*s2Ptr == ' ') ||
	  (*s2Ptr == '\t')) &&
	 (s2Ptr <= buffer + bufLen))
    s2Ptr++;

  if ((*s2Ptr == '\n') || 
      (*s2Ptr == EOF))
    return (-1);
  
  if (*s2Ptr == '"')
    {
      i = 0;

      while ((*s2Ptr != '"') && 
	     (*(s2Ptr - 1) != '\\') && 
	     (s2Ptr <= buffer + bufLen) &&
	     (i <= 255))
	{
	  if (*s2Ptr == '\n')
	    return (-1);
	  tableVar->DBROOT[i] = *s2Ptr;
	  i++;
	  s2Ptr++;
	}
      *s2Ptr = '"';
    }

  else
    {
      i = 0;

      while ((*s2Ptr != '\n') &&
	     (*s2Ptr != ' ') &&
	     (*s2Ptr != '\t') &&
	     (s2Ptr <= buffer + bufLen) &&
	     (i <= 255))
	{
	  tableVar->DBROOT[i] = *s2Ptr;
	  i++;
	  s2Ptr++;
	}
    }

  memset (&fstat, '\0', sizeof (struct stat));
  
  if (stat ((const char *) &(tableVar->DBROOT[0]), &fstat) == -1)
    return (-1);
  
  if (IsDir(fstat.st_mode) == FALSE)
    return (-1);

  if (VariSearch (buffer, bufLen, &table[0], FALSE, 255, "_DBUSER", &(tableVar->DBUSER[0])) != 0)
    return (-1);

  if (VariSearch (buffer, bufLen, &table[0], FALSE, 255, "_DBPASSWORD", &(tableVar->DBPASSWORD[0])) != 0)
    return (-1);

  if (VariSearch (buffer, bufLen, &table[0], TRUE, 255, "_TEMPLATEROOT", &(tableVar->TEMPLATEROOT[0])) != 0)
    return (-1);

  if (VariSearch (buffer, bufLen, &table[0], FALSE, 255, "_BASEREF", &(tableVar->BASEREF[0])) != 0)
    return (-1);

  /* ***** recommended but optional variables ***** */

  VariSearch (buffer, bufLen, &table[0], FALSE, 1, "_PROFILING", &(tableVar->PROFILING[0]));

  VariSearch (buffer, bufLen, &table[0], FALSE, 511, "_TIMEOUTURL", &(tableVar->TIMEOUTURL[0]));

  VariSearch (buffer, bufLen, &table[0], FALSE, 511, "_DBERORURL", &(tableVar->DBERRORURL[0]));

  VariSearch (buffer, bufLen, &table[0], FALSE, 99, "_MAILSERVER", &(tableVar->MAILSERVER[0]));

  VariSearch (buffer, bufLen, &table[0], FALSE, 255, "_MAILFROM", &(tableVar->MAILFROM[0]));

  VariSearch (buffer, bufLen, &table[0], FALSE, 23, "_EURODM", &(tableVar->EURODM[0]));

  VariSearch (buffer, bufLen, &table[0], FALSE, 23, "_EUROUSD", &(tableVar->EUROUSD[0]));

  VariSearch (buffer, bufLen, &table[0], FALSE, 255, "_RTF", &(tableVar->RTF[0]));

  return (0);
}

/* -------------------------------------------------- */

/*
 *
 * VariSearch function
 *
 */

int
VariSearch (char *buffer, int bufLen, char *tableNam, int dofstat, int tabLen, char *extension, char *taboffset)
{
  int i;

  char searchStr[255];

  char *s1Ptr; 
  char *s2Ptr;

  struct stat fstat;

  s1Ptr = NULL;
  s2Ptr = NULL;

  memset (&searchStr[0], '\0', sizeof (searchStr));
  sprintf (&searchStr[0], "%s%s", tableNam, extension);
  if ((s1Ptr = (char *) strstr (buffer, &searchStr[0])) == NULL)
    return (-1);

  s2Ptr = s1Ptr + strlen (searchStr);

  while (((*s2Ptr == ' ') ||
	  (*s2Ptr == '\t')) &&
	 (s2Ptr <= buffer + bufLen))
    s2Ptr++;

  if ((*s2Ptr == '\n') || 
      (*s2Ptr == EOF))
    return (-1);
  
  if (*s2Ptr == '"')
    {
      i = 0;

      while ((*s2Ptr != '"') && 
	     (*(s2Ptr - 1) != '\\') && 
	     (s2Ptr <= buffer + bufLen) &&
	     (i <= 255))
	{
	  if (*s2Ptr == '\n')
	    return (-1);
	  *(taboffset + i) = *s2Ptr;
	  i++;
	  s2Ptr++;
	}
      *s2Ptr = '"';
    }

  else
    {
      i = 0;

      while ((*s2Ptr != '\n') &&
	     (*s2Ptr != ' ') &&
	     (*s2Ptr != '\t') &&
	     (s2Ptr <= buffer + bufLen) &&
	     (i <= 255))
	{
	  *(taboffset + i) = *s2Ptr;
	  i++;
	  s2Ptr++;
	}
    }

  if (dofstat)
    {
      memset (&fstat, '\0', sizeof (struct stat));

      if (stat ((const char *) (taboffset), &fstat) == -1)
	return (-1);
      
      if (IsDir(fstat.st_mode) == FALSE)
	return (-1);
    }

  return (0);
}

/* -------------------------------------------------- */

/*
 *
 * AdabasRun function
 *
 */

int
AdabasRun (tstrT *tableVar)
{
  FILE *fd;

  int i;
  int numOfEn;
  int readRes;
  int retVal;
  int dbtest;
  
  char buffer[256];
  char systemStr[1024];

  flistT  *foundFileList;
  char *foundPath;
  
  struct stat fstat;

  retVal = 0;
  foundFileList = NULL;

  if ((fd = popen("/bin/ps ax", "r")) == NULL)
    return (-1);

  memset (&buffer[0], '\0', sizeof (buffer)); 

  while (!feof(fd))
    {
      if (fgetc (fd) == 'v')
	{
	  if (fgetc (fd) == 's')
	    {
	      if (fgetc (fd) == 'h')
		{
		  if (fgetc (fd) == 'o')
		    {
		      if (fgetc (fd) == 'p')
			retVal = 1;
		    }
		}
	    }
	}
    }

  fclose (fd);

  if (retVal == 0)
    {
      /* ***** try to find the location of 'vshopdb' start/stop script ***** */

      i = 0;

      while (i < 3)
	{
	  if ((foundFileList = FindFile (findPath[i], "vshopdb", NULL, NULL)) != NULL)
	    while ((foundPath = (char *) GetNextFile(foundFileList)) != NULL)
	      {
		memset (&fstat, '\0', sizeof (struct stat));
		memset (&systemStr[0], '\0', sizeof (systemStr));  
		dbtest = 1;

		if (stat (tableVar->DBROOT, &fstat) == -1)
		  {
		    retVal = 0;
		    break;
		  }
		
		if (setenv ("DBROOT", tableVar->DBROOT, 1) != 0)
		  {
		    retVal = 0;
		    break;
		  }

		sprintf (&systemStr[0], tableVar->DBROOT);
		strcat (&systemStr[0], "/bin/x_clear vshop");
		
		if (UpSystem (&systemStr[0]) != 0)
		  {
		    retVal = 0;
		    break;
		  }
		
		memset (&fstat, '\0', sizeof (struct stat));

		if (stat (foundPath, &fstat) == -1)
		  retVal = 0;      
		    
		if (IsReg(fstat.st_mode) == TRUE)
		  {
		    memset (&systemStr[0], '\0', sizeof (systemStr));  
		    
		    sprintf (&systemStr[0], foundPath);
		    strcat (&systemStr[0], " start");

		    if (UpSystem (&systemStr[0]) != 0)
		      {
			retVal = 0;
			break;
		      }
		    
		    else
		      retVal = 1;
		  }
	      }
	  
	  retVal = 0;
	  
	  if ((fd = popen("/bin/ps ax", "r")) == NULL)
	    return (-1);
	  
	  memset (&buffer[0], '\0', sizeof (buffer)); 
	  
	  while (!feof(fd))
	    {
	      if (fgetc (fd) == 'v')
		{
		  if (fgetc (fd) == 's')
		    {
		      if (fgetc (fd) == 'h')
			{
			  if (fgetc (fd) == 'o')
			    {
			      if (fgetc (fd) == 'p')
				retVal = 1;
			    }
			}
		    }
		}
	    }
	  
	  fclose (fd);
	  
	  if (retVal == 1)
	    break;
	}
    }
  return (retVal);
}

/* -------------------------------------------------- */

/*
 *
 * AdabasRights function
 *
 */

struct stat *
AdminRights (char *path)
{
  char searchPath[256];

  struct stat *fstat;

  if ((fstat = (struct stat *) calloc (1, sizeof (struct stat))) == NULL)
     return (NULL);

  memset (&searchPath[0], '\0', sizeof (searchPath));  

  if (strlen (path) < (255 - strlen ("/admin")))
    sprintf (&searchPath[0], "%s/admin", path);
  
  else
    return (NULL);

  if (stat ((const char *) &searchPath[0], fstat) != 0)
    return (NULL);

  return (fstat);  
}

/* -------------------------------------------------- */

/*
 *
 * DokRights function
 *
 */

struct stat *
DokRights (char *path)
{
  char searchPath[256];
  
  struct stat *fstat;
  
  memset (&searchPath[0], '\0', sizeof (searchPath));
  
  if ((fstat = (struct stat *) calloc (1, sizeof (struct stat))) == NULL)
     return (NULL);

  if (strlen (path) < (255 - strlen ("dok")))
    sprintf (&searchPath[0], "%s/dok", path);
  
  else
    return (NULL);
  
  if (stat ((const char *) &searchPath[0], fstat) != 0)
    {
      memset (&searchPath[0], '\0', sizeof (searchPath));
  
      if (strlen (path) < (255 - strlen ("vshop_documentation")))
	sprintf (&searchPath[0], "%s/vshop_documentation", path);
  
      else
	return (NULL);
  
      if (stat ((const char *) &searchPath[0], fstat) != 0)
	return (NULL);
    }
  
  return (fstat);
}

/* -------------------------------------------------- */

/*
 *
 * Libc
 *
 */

int 
Libc (char *libNam)
{

  FILE* ConfFile;

  int Finish = 0;
  int c = 0;

  char Buffer[80];
  char Line[401];
  char testBuf[256];

  char* Ptr;


  if ((ConfFile = fopen("/etc/ld.so.conf","r")) != NULL)
    {
      while (fgets(Line,400,ConfFile) != NULL)
	{
	  if ((Ptr = (char *) strrchr(Line,'=')) != NULL)
	    *Ptr = '\0';

          else
	    Line[strlen(Line)-1] = '\0';

	  if (Line[strlen(Line)-1] != '/')
	    strcat(Line,"/");

	  strcat(Line,libNam);

	  if (access(Line,F_OK) == 0)
	    {
	      fclose(ConfFile);
	      return 0;
	    }
	}

      fclose(ConfFile);

      memset (&testBuf[0], '\0', sizeof (testBuf));
      sprintf (&testBuf[0], "/lib/%s", libNam);

      if (access(&testBuf[0], F_OK) == 0)
	return 0;

      memset (&testBuf[0], '\0', sizeof (testBuf));
      sprintf (&testBuf[0], "/usr/lib/%s", libNam);
      
      if (access(&testBuf[0], F_OK) == 0)
	return 0;
    }
    
  else
    return (-1);
}

/* -------------------------------------------------- */

/*
 *
 * VsadFile function
 *
 */

int
VsadFile (vsstrT *vsBuffer)
{
  int bufLen;
  int foffset;
  int readres;
  int unread;
  int unreadlast;
  int vsadfd;
  
  char testBuf[64];
  char *buffer;
  char *bufferPtr;

  vsstrT *vsBufferPtr;  

  /* ***** open the file "/opt/vshop/vsadmin.conf" and read out variose usefull variables ***** */

  if ((vsadfd = open("/opt/vshop/vsadmin.conf", O_SYNC)) == -1)
    return (-1);

  if ((foffset = lseek (vsadfd, 0, SEEK_END)) <= 0)
    {
      close (vsadfd);
      return (-1);
    }

  bufLen = foffset + 1;
  if ((buffer = (char *) calloc (1, bufLen)) == NULL)
    {
      close (vsadfd);
      return (-1);
    }

  if (lseek (vsadfd, 0, SEEK_SET) == -1)
    {
      close (vsadfd);
      return (-1);
    }

  unreadlast = 0;
  unread = foffset;
  readres = 0;

  while (readres < foffset)
    {
      unread = (int ) read (vsadfd, (void *) buffer, (size_t ) unread);
      if (unreadlast > unread)
	{
	  close (vsadfd);
	  return (-1);
	}

      readres += unread;
      unreadlast = unread;
    }

  vsBuffer = NULL;

  memset (&testBuf[0], '\0', sizeof (testBuf));

  if (VariSearch (buffer, bufLen, "SHOPNAME", FALSE, 63, "", &testBuf[0]) == 0)
    {
      if ((vsBufferPtr = (vsstrT *) calloc (1, sizeof (vsstrT))) == NULL)
	{
	  close (vsadfd);
	  vsBuffer = NULL;
	  return (-1);
	}
      
      if ((vsBufferPtr->SHOPNAME = (char *) calloc (1, strlen (&testBuf[0]) + 1)) == NULL)
	{
	  close (vsadfd);
	  vsBuffer = NULL;
	  return (-1);
	}

      strcpy (vsBufferPtr->SHOPNAME, &testBuf[0]);

      vsBufferPtr->next = NULL;
      vsBuffer = vsBufferPtr;
    }

  bufferPtr = (char *) strstr (buffer, &testBuf[0]);
  bufLen = strlen (bufferPtr);

  memset (&testBuf[0], '\0', sizeof (testBuf));

  while (VariSearch (bufferPtr, bufLen, "SHOPNAME", FALSE, 63, "", &testBuf[0]) == 0)
    {
      if ((vsBufferPtr->next = (vsstrT *) calloc (1, sizeof (vsstrT))) == NULL)
	{
	  close (vsadfd);
	  vsBuffer = NULL;
	  return (-1);
	}

      if ((vsBufferPtr->next->SHOPNAME = (char *) calloc (1, strlen (&testBuf[0]) + 1)) == NULL)
	{
	  close (vsadfd);
	  vsBuffer = NULL;
	  return (-1);
	}

      strcpy (vsBufferPtr->next->SHOPNAME, &testBuf[0]);

      vsBufferPtr->next->next = NULL;
      vsBufferPtr = vsBufferPtr->next;
      bufferPtr = (char *) strstr (buffer, &testBuf[0]);
      bufLen = strlen (bufferPtr);
      memset (&testBuf[0], '\0', sizeof (testBuf));
    }

  gvsBufferPtr = vsBuffer;
  close (vsadfd);
  return (0);
}

/* -------------------------------------------------- */

/*
 *
 * VsadNext function
 *
 */

char *
VsadNext ()
{
  char *cPtr;

  if (gvsBufferPtr == NULL)
    return (NULL);

  cPtr = gvsBufferPtr->SHOPNAME;
  gvsBufferPtr = gvsBufferPtr->next;

  return (cPtr);
}

/* -------------------------------------------------- */

/*
 *
 * Addstr function
 *
 */

int
Addstr (char *tableNam, char *tablePtr, int vshopfd, char *extension, int comment)
{
  int  i;
  int writeRes;

  char table[64];

  char systemStr[1024];

  memset (&table[0], '\0', sizeof (table));

  i = 0;

  while (i < strlen (tableNam))
    {
      table[i] = toupper (*(tableNam + i));
      i++;
    }
    
  if (strlen (tablePtr) != 0)
    { 
      lseek (vshopfd, 0, SEEK_END);
      memset (&systemStr[0], '\0', sizeof (systemStr));

      if (comment == COMMENT)
	  sprintf (&systemStr[0], "#%s", &table[0]);
      
      else
	sprintf (&systemStr[0], "%s", &table[0]);

      strcat (&systemStr[0], extension);
      strcat (&systemStr[0], tablePtr);
      strcat (&systemStr[0], "\n");
      
      writeRes = 0;

      while ((writeRes < (strlen (&systemStr[0]))) &&
	     (writeRes >= 0))
	{
	  writeRes = write (vshopfd, &systemStr[writeRes], strlen (&systemStr[0]) - writeRes);
	  /* --- O D --- 
	  printf ("\nwriting to vshop.conf:>%s<\n", &systemStr[0]);
	  --- O D --- */
	}
      
      while (writeRes = 0)
	  writeRes = write (vshopfd, "\r", 1);
      
      return (0);
    }
  
  return (-1);
}


/* -------------------------------------------------- */

/*
 *
 * FindFile function
 *
 */

flistT *
FindFile (char *startDir, char *fileNam, flistT *flistPtr, inostrT *inoList)
{
  int i;
  int num;
  int retVal;
  
  char systemStr[1024];

  struct dirent **nameList;
  struct stat fstat;

  inostrT *inoListPtr;
  flistT *list;
  flistT *listPtr;

  if (flistPtr == NULL)
    {
      list = NULL;
      listPtr = list;
    }
  
  else
    listPtr = flistPtr;

  i = 0;

  if ((num = scandir (startDir, &nameList, 0, 0)) > 0)
    {
      while (i < num)
	{
	  memset (&systemStr[0], '\0', sizeof (systemStr));  
	  sprintf (&systemStr[0], startDir);
	  if (*(startDir + strlen (startDir) - 1) != '/')
	    strcat (&systemStr[0], "/");
	  strcat (&systemStr[0],nameList[i]->d_name);
	  memset (&fstat, '\0', sizeof (struct stat));

	  if (stat ((const char *) &systemStr[0], &fstat) == 0)
	    {
	      if (strlen (nameList[i]->d_name) == strlen (fileNam))
		{
		  if (strcmp (nameList[i]->d_name, fileNam) == 0)
		    {
		      if (IsReg (fstat.st_mode) == TRUE)
			{
			  retVal = TRUE;
			  listPtr = list;
			      
			  while (listPtr != NULL)
			    {
			      if (listPtr->inode == fstat.st_ino)
				retVal = FALSE;
			      listPtr = listPtr->next;
			    }
			      
			  if (retVal == TRUE)
			    {

			      if (inoList == NULL)
				{
				  if ((inoList = (inostrT *) calloc (1, sizeof (inostrT))) == NULL)
				    return (NULL);
				  inoListPtr = inoList;
				}

			      else 
				{
				  inoListPtr = inoList;
				  
				  while (inoListPtr->next != NULL)
				    inoListPtr = inoListPtr->next;

				  if ((inoListPtr->next = (inostrT *) calloc (1, sizeof (inostrT))) == NULL)
				    return (NULL);

				  else
				    inoListPtr = inoListPtr->next;
				}
	      
			      inoListPtr->inode = fstat.st_ino;

			      if (list == NULL)
				{
				  if ((list = (flistT *) calloc (1, sizeof (flistT))) == NULL)
				    return (NULL);
				  
				  else
				    listPtr = list;
				}

			      listPtr = list;
				  
			      while (listPtr->next != NULL)
				listPtr = listPtr->next;

			      if ((listPtr->inode != 0) &&
				  ((listPtr->next = (flistT *) calloc (1, sizeof (flistT))) != NULL))
				listPtr = listPtr->next;

			      if (listPtr != NULL)
				{
				  if ((listPtr->name = (char *) calloc (1, strlen (&systemStr[0]) + 1)) != NULL)
				    {
				      strcpy (listPtr->name, &systemStr[0]);
				      listPtr->inode = fstat.st_ino;
				    }

				  else 
				    return (NULL);
				}
			      else
				return (NULL);
			    }
			}
		    }
		}
	      
	      if ((IsDir (fstat.st_mode) == TRUE) &&
		  (strcmp (nameList[i]->d_name, ".") != 0) &&
		  (strcmp (nameList[i]->d_name, "..") != 0))
		{
		  retVal = TRUE;
		  inoListPtr = inoList;
	      
		  while (inoListPtr != NULL)
		    {
		      if (inoListPtr->inode == fstat.st_ino)
			retVal = FALSE;
		      inoListPtr = inoListPtr->next;
		    }

		  if (retVal == TRUE)
		    {

		      if (inoList == NULL)
			{
			  if ((inoList = (inostrT *) calloc (1, sizeof (inostrT))) == NULL)
			    return (NULL);
			  inoListPtr = inoList;
			}

		      else 
			{
			  inoListPtr = inoList;
			  
			  while (inoListPtr->next != NULL)
			    inoListPtr = inoListPtr->next;
			  
			  if ((inoListPtr->next = (inostrT *) calloc (1, sizeof (inostrT))) == NULL)
			    return (NULL);
			  
			  else
			    inoListPtr = inoListPtr->next;
			}
		      
		      inoListPtr->inode = fstat.st_ino;

		      if (listPtr == NULL)
			{
			  list = FindFile (&systemStr[0], fileNam, NULL, inoList);
			  listPtr = list;
			  
			  if (listPtr != NULL)
			    while (listPtr->next != NULL)
			      listPtr = listPtr->next;
			}
		      
		      else
			{
			  listPtr->next = FindFile (&systemStr[0], fileNam, NULL, inoList);
			  
			  if (listPtr != NULL)
			    while (listPtr->next != NULL)
			      listPtr = listPtr->next;
			}
		    
		    }
		}
	    }

	  if (i < num)
	    i++;

	  else
	    break;

	}
    }
  
  return (list);
}

/* -------------------------------------------------- */

/*
 *
 * GetNextFile function
 *
 */

char *
GetNextFile (flistT *foundFileList)
{
  int i;

  char *retVal;

  flistT *listPtr;

  i = 0;
  listPtr = foundFileList;

  while (i <= count)
    {
      retVal = listPtr->name;
      
      if (listPtr->next != NULL)
	{
	  listPtr = listPtr->next;
	  i++;
	}

      else
	break;
    }
  
  if (i < count)
    retVal = NULL;

  else
    {
      retVal = listPtr->name;
      count++;
    }
  return (retVal);
}

/* -------------------------------------------------- */

/*
 *
 * IsDir function
 *
 */

int
IsDir (mode_t statMode)
{
  if ((statMode & S_IFMT & S_IFDIR) == S_IFDIR)
    return (TRUE);
  
  else
    return (FALSE);
}

/* -------------------------------------------------- */

/*
 *
 * IsLink function
 *
 */

int
IsLink (mode_t statMode)
{
  if ((statMode & S_IFMT & S_IFLNK) == S_IFLNK)
    return (TRUE);
  
  else
    return (FALSE);
}

/* -------------------------------------------------- */

/*
 *
 * IsReg function
 *
 */

int
IsReg (mode_t statMode)
{
  if ((statMode & S_IFMT & S_IFREG) == S_IFREG)
    return (TRUE);
  
  else
    return (FALSE);
}

/* -------------------------------------------------- */

/*
 *
 * RecChmod function
 *
 */

int
RecChmod (char *startDir, struct stat *status)
{

  int i;
  int num;
  int retVal;
  
  char systemStr[1024];

  struct dirent **nameList;
  struct stat fstat;

  inostrT *inoList ;
  inostrT *inoListPtr;

  i = 0;

  inoList = NULL;
  inoListPtr = NULL;
  
  if ((num = scandir (startDir, &nameList, 0, 0)) > 0)
    {
      while (i < num)
	{
	  memset (&systemStr[0], '\0', sizeof (systemStr));  
	  sprintf (&systemStr[0], startDir);
	  if (*(startDir + strlen (startDir) - 1) != '/')
	    strcat (&systemStr[0], "/");
	  strcat (&systemStr[0],nameList[i]->d_name);
	  memset (&fstat, '\0', sizeof (struct stat));

	  if (stat ((const char *) &systemStr[0], &fstat) == 0)
	    {
	      if (IsReg (fstat.st_mode) == TRUE)
		{
		  if (inoList == NULL)
		    {
		      if ((inoList = (inostrT *) calloc (1, sizeof (inostrT))) == NULL)
			return (-1);
		      inoListPtr = inoList;
		    }

		  else 
		    {
		      inoListPtr = inoList;
				  
		      while (inoListPtr->next != NULL)
			inoListPtr = inoListPtr->next;

		      if ((inoListPtr->next = (inostrT *) calloc (1, sizeof (inostrT))) == NULL)
			return (-1);

		      else
			inoListPtr = inoListPtr->next;
		    }
	      
		  inoListPtr->inode = fstat.st_ino;

		  setreuid (getuid(), getuid ());
		  setregid (getgid(), getgid ());

		  chown (&systemStr[0], status->st_uid, status->st_gid);

		  setreuid (getuid(), status->st_uid);
		  setregid (getgid(), status->st_gid);
		  
		  chmod (&systemStr[0], status->st_mode & CHMAP);

		}

	      
	      if ((IsDir (fstat.st_mode) == TRUE) &&
		  (strcmp (nameList[i]->d_name, ".") != 0) &&
		  (strcmp (nameList[i]->d_name, "..") != 0))
		{
		  retVal = TRUE;
		  inoListPtr = inoList;
	      
		  while (inoListPtr != NULL)
		    {
		      if (inoListPtr->inode == fstat.st_ino)
			retVal = FALSE;
		      inoListPtr = inoListPtr->next;
		    }

		  if (retVal == TRUE)
		    {

		      if (inoList == NULL)
			{
			  if ((inoList = (inostrT *) calloc (1, sizeof (inostrT))) == NULL)
			    return (-1);
			  inoListPtr = inoList;
			}

		      else 
			{
			  inoListPtr = inoList;
			  
			  while (inoListPtr->next != NULL)
			    inoListPtr = inoListPtr->next;
			  
			  if ((inoListPtr->next = (inostrT *) calloc (1, sizeof (inostrT))) == NULL)
			    return (-1);
			  
			  else
			    inoListPtr = inoListPtr->next;
			}
		      
		      inoListPtr->inode = fstat.st_ino;

		      setreuid (getuid(), getuid ());
		      setregid (getgid(), getgid ());

		      chown (&systemStr[0], status->st_uid, status->st_gid);

		      setreuid (getuid(), status->st_uid);
		      setregid (getgid(), status->st_gid);

		      chmod (&systemStr[0], status->st_mode & CHMAP);
		      		    
		      if (RecChmod (&systemStr[0], status) == -1)
			return (-1);
		    }
		}
	    }

	  if (i < num)
	    i++;

	  else
	    break;

	}
    }

  memset (&systemStr[0], '\0', sizeof (systemStr));  
  sprintf (&systemStr[0], startDir);
  if (*(startDir + strlen (startDir) - 1) != '/')
    strcat (&systemStr[0], "/");
  strcat (&systemStr[0], ".");
  
  setreuid (getuid(), getuid ());
  setregid (getgid(), getgid ());

  chown (systemStr, status->st_uid, status->st_gid);
  
  return (0);
}


/* -------------------------------------------------- */

/*
 *
 * RecRem function
 *
 */

int
RecRem (char *startDir)
{
  int i;
  int num;

  char systemStr[1024];

  memset (&systemStr[0], '\0', sizeof (systemStr));  

  sprintf (&systemStr[0], "rm -fr %s/*.p", startDir, startDir, startDir);

  if (UpSystem (&systemStr[0]) != 0)
      return (-1);

  return (0);
}

/* -------------------------------------------------- */

/*
 *
 * Help function
 *
 */

int
Help ()
{
  printf ("usage: update\n\n\tDas update Programm benoetigt keine Optionen,\n jedoch sollten die Programme bash, rm, cp, tar im aktuellen Pfad:\n\n\t%s\n\n erreichbar sein.\n", getenv ("PATH"));
  exit (0);
}

/* -------------------------------------------------- */

/*
 *
 * UpSystem function
 *
 */

int
UpSystem (char *commandStr)
{
  int i;
  int length;
  int pid;
  int status;
  
  extern char **environ;

  char *argv[24];
  char *s1Ptr;
  char *s2Ptr;

  length = strlen (commandStr);

  if (strlen (commandStr) == 0)
    return (-1);

  s1Ptr = commandStr;
  
  i = 0;

  if ((s2Ptr = strchr (commandStr, ' ')) == NULL)
    {
      argv[0] = commandStr;
      argv[1] = NULL;
    }
  
  else
    {
      while ((commandStr + length) >= s1Ptr)
	{
	  if ((s2Ptr = strchr (s1Ptr, ' ')) != NULL)		  
	    *s2Ptr = '\0';
	  
	  else
	    s2Ptr = s1Ptr + strlen (s1Ptr) + 1;
	  
	  if ((argv[i] = (char *) calloc (1, strlen (s1Ptr))) == NULL)
	    return (-1);
	  
	  if (strcpy (argv[i], s1Ptr) == NULL)
	    return (-1);
	 
	  i++;
	  s1Ptr = s2Ptr + 1;
	}
    }

      argv[i] = NULL;

      pid = fork ();

      if (pid == -1)
	return (-1);

      if (pid == 0)
	{
	  /*
	  fclose (stdin);
	  fclose (stdout);
	  fclose (stderr);  
	  */
	  execvp (argv[0], argv);
	  exit (127);
	}

      do 
	{
	  if (waitpid(pid, &status, 0) == -1) 
	    {
	      if (errno != EINTR)
		return -1;
	    }	      

	  else if (WIFEXITED (status) != 0)
	    return (WEXITSTATUS (status));
	  
	} while(1);
}


/* -------------------------------------------------- */

/*
 *
 * ShSystem function
 *
 */

int
ShSystem (char *commandStr)
{
  int i;
  int length;
  int pid;
  int status;
  
  extern char **environ;

  char *argv[4];

  argv[0] = "bash";
  argv[1] = "-c";
  argv[2] = commandStr;
  argv[3] = NULL;
      pid = fork ();

      if (pid == -1)
	return (-1);

      if (pid == 0)
	{
	  fclose (stdin);
	  fclose (stdout);
	  fclose (stderr);  
	  execvp (argv[0], argv);
	  exit (127);
	}

      do 
	{
	  if (waitpid(pid, &status, 0) == -1) 
	    {
	      if (errno != EINTR)
		return -1;
	    }	      

	  else if (WIFEXITED (status) != 0)
	    return (WEXITSTATUS (status));

	} while(1);
}

/* -------------------------------------------------- */

/*
 *
 * UpExit function
 *
 */

void
UpExit (int state)
{
  close (vshopfd);
  exit (stage);
}
