#if CGICDEBUG
#define CGICDEBUGSTART { FILE *dout; dout = fopen("cgic.debug", "a"); 
	
#define CGICDEBUGEND fclose(dout); }
#else /* CGICDEBUG */
#define CGICDEBUGSTART
#define CGICDEBUGEND
#endif /* CGICDEBUG */


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* include stefan */
#include <strings.h>

#ifndef NO_UNISTD
#ifndef WIN32
  #include <unistd.h>
#else
  #include <io.h>
  #include <fcntl.h>
  #include "win32tools.h"
#endif
#endif /* NO_UNISTD */
#include "cgic.h"

#define cgiStrEq(a, b) (!strcasecmp((a), (b)))

char *cgiServerSoftware;
char *cgiServerName;
char *cgiGatewayInterface;
char *cgiServerProtocol;
char *cgiServerPort;
char *cgiRequestMethod;
char *cgiPathInfo;
char *cgiPathTranslated;
char *cgiScriptName;
char *cgiQueryString;
char *cgiRemoteHost;
char *cgiRemoteAddr;
char *cgiAuthType;
char *cgiRemoteUser;
char *cgiRemoteIdent;
char *cgiContentType;
int cgiContentLength;
char *cgiAccept;
char *cgiUserAgent;
char *cgiReferer;
char *cgiCookie;

char PROGRAMNAME[40];
char CGIBIN[150];
char SECURECGIBIN[150];
char BASEREF[150];
char SECUREBASEREF[200];
char TEMPLATEROOT[200];
char PROGRAMPATH[256];
char ENGINEPATH[256];
int VSHOP_PROFILING;

FILE *cgiIn;
FILE *cgiOut;



/* One form entry, consisting of an attribute-value pair. */


/* The first form entry. */
static cgiFormEntry *cgiFormEntryFirst;

/* True if CGI environment was restored from a file. */
static int cgiRestored = 0;

static void cgiGetenv(char **s, char *var);

typedef enum {
	cgiParseSuccess,
	cgiParseMemory,
	cgiParseIO
} cgiParseResultType;

static cgiParseResultType cgiParseCommandLineInput(char* Arg);
static cgiParseResultType cgiParseGetFormInput();
static cgiParseResultType cgiParsePostFormInput();
static cgiParseResultType cgiParseFormInput(char *data, int length);
static void cgiSetupConstants();
static void cgiFreeResources();
static int cgiStrEqNc(char *s1, char *s2);

int main(int argc, char *argv[]) 
{
int result;
char *cgiContentLengthString;



  #ifdef WIN32
    _setmode( _fileno( stdin ), _O_BINARY );
  #endif

	cgiSetupConstants();
	cgiGetenv(&cgiServerSoftware, "SERVER_SOFTWARE");
	cgiGetenv(&cgiServerName, "SERVER_NAME");
	cgiGetenv(&cgiGatewayInterface, "GATEWAY_INTERFACE");
	cgiGetenv(&cgiServerProtocol, "SERVER_PROTOCOL");
	cgiGetenv(&cgiServerPort, "SERVER_PORT");
	cgiGetenv(&cgiRequestMethod, "REQUEST_METHOD");
	cgiGetenv(&cgiPathInfo, "PATH_INFO");
	cgiGetenv(&cgiPathTranslated, "PATH_TRANSLATED");
	cgiGetenv(&cgiScriptName, "SCRIPT_NAME");
	cgiGetenv(&cgiQueryString, "QUERY_STRING");
	cgiGetenv(&cgiRemoteHost, "REMOTE_HOST");
	cgiGetenv(&cgiRemoteAddr, "REMOTE_ADDR");
	cgiGetenv(&cgiAuthType, "AUTH_TYPE");
	cgiGetenv(&cgiRemoteUser, "REMOTE_USER");
	cgiGetenv(&cgiRemoteIdent, "REMOTE_IDENT");
	cgiGetenv(&cgiContentType, "CONTENT_TYPE");
	cgiGetenv(&cgiContentLengthString, "CONTENT_LENGTH");
	cgiContentLength = atoi(cgiContentLengthString);	
	cgiGetenv(&cgiAccept, "HTTP_ACCEPT");
	cgiGetenv(&cgiUserAgent, "HTTP_USER_AGENT");
  cgiGetenv(&cgiReferer, "HTTP_REFERER");
  cgiGetenv(&cgiRemoteAddr, "REMOTE_ADDR");
  cgiGetenv(&cgiRemoteHost, "REMOTE_HOST");
  cgiGetenv(&cgiCookie, "HTTP_COOKIE");

  char* Ptr;
  int Counter = 0;




  #ifndef WIN32
    if ((Ptr = strrchr(argv[0],'/')) != NULL)
      strcpy(PROGRAMNAME,Ptr+1);
    else
      strcpy(PROGRAMNAME,argv[0]);

    strcpy(PROGRAMPATH,argv[0]);
    if ((Ptr = strrchr(PROGRAMPATH,'/')) != NULL)
      *(Ptr+1) = '\0';

  #else
    if ((Ptr = strrchr(argv[0],'\\')) != NULL)
      strcpy(PROGRAMNAME,Ptr+1);
    else
    if ((Ptr = strrchr(argv[0],'/')) != NULL)
      strcpy(PROGRAMNAME,Ptr+1);
    else
      strcpy(PROGRAMNAME,argv[0]);

    strcpy(PROGRAMPATH,argv[0]);
    if ((Ptr = strrchr(PROGRAMPATH,'\\')) != NULL)
      *(Ptr+1) = '\0';
    else
    if ((Ptr = strrchr(PROGRAMPATH,'/')) != NULL)
      *(Ptr+1) = '\0';
  #endif
  chdir(PROGRAMPATH);
  
    getcwd(PROGRAMPATH,255);


  /* Die Frage ist was wird da Oben gemacht ? */
  getcwd(ENGINEPATH,255);

  for (Counter = 0;Counter < (int) strlen(PROGRAMNAME);Counter++)
    if (((int) PROGRAMNAME[Counter] > 96) && ((int) PROGRAMNAME[Counter] < 123))
              PROGRAMNAME[Counter] -= 32;

#ifdef CGICDEBUG
	CGICDEBUGSTART
  fprintf(dout, "---------------------------\n");
	fprintf(dout, "ContentLength : %d\n", cgiContentLength);
	fprintf(dout, "RequestMethod : %s\n", cgiRequestMethod);
	fprintf(dout, "ContentType   : %s\n", cgiContentType);
	CGICDEBUGEND	
#endif /* CGICDEBUG */
	cgiFormEntryFirst = 0;
	cgiIn = stdin;
	cgiOut = stdout;
	cgiRestored = 0;

  
  
  if (argc > 1) 
    {
      cgiContentLength = strlen(argv[1]);
		  if (cgiParseCommandLineInput(argv[1]) != cgiParseSuccess) 
        {
				  cgiFreeResources();
				  return -1;
			  }	
     	result = cgiMain(1);
    	cgiFreeResources();
      return result;

  	}	
  else
	if (cgiStrEqNc(cgiRequestMethod, "post")) 
    {
      if (cgiStrEqNc(cgiContentType, "application/x-www-form-urlencoded")) 
        {	
			    if (cgiParsePostFormInput() != cgiParseSuccess) 
            {
				      cgiFreeResources();
				      return -1;
			      }	
		    }
      else
        {
          GetUpload();
          return cgiMain(0);
        }
	  } 
  else 
    if (cgiStrEqNc(cgiRequestMethod, "get")) 
      {	
		    /* The spec says this should be taken care of by
			     the server, but... it isn't */
		    cgiContentLength = strlen(cgiQueryString);
		    if (cgiParseGetFormInput() != cgiParseSuccess) 
          {
			      cgiFreeResources();
			      return -1;
		      }
        else 
          {	
            #ifdef CGICDEBUG
			        CGICDEBUGSTART
			        fprintf(dout, "GetFormInput succeeded\n");
			        CGICDEBUGEND	
            #endif /* CGICDEBUG */
		      }
	    }
	result = cgiMain(0);
	cgiFreeResources();

	/* The following two lines allegedly produce better behavior
		when run with the CERN server on some platforms. 
		Remove them and experiment if you wish. */
/*
	fflush(stdout);
	sleep(1);
*/
	return result;
}

static void cgiGetenv(char **s, char *var){
	*s = getenv(var);
	if (!(*s)) {
		*s = "";
	}
}

/*--------------------------------------------------------------------------------*/
static cgiParseResultType cgiParseCommandLineInput(char* Arg) 
{
char *input;
cgiParseResultType result;

	if (!cgiContentLength) 
    {
		  return cgiParseSuccess;
	  }
	
  input = (char *) malloc(cgiContentLength);
	if (!input) 
    {
		  return cgiParseMemory;	
	  }
	
  strcpy(input,Arg);
	
  result = cgiParseFormInput(input, cgiContentLength);
	free(input);
	return result;
}

/*--------------------------------------------------------------------------------*/

static cgiParseResultType cgiParsePostFormInput() {
	char *input;
	cgiParseResultType result;
	if (!cgiContentLength) {
		return cgiParseSuccess;
	}
	input = (char *) malloc(cgiContentLength);
	if (!input) {
		return cgiParseMemory;	
	}
	if (fread(input, 1, cgiContentLength, cgiIn) != (unsigned int) cgiContentLength) {
		return cgiParseIO;
	}	
	result = cgiParseFormInput(input, cgiContentLength);
	free(input);
	return result;
}

static cgiParseResultType cgiParseGetFormInput() {
	return cgiParseFormInput(cgiQueryString, cgiContentLength);
}

typedef enum {
	cgiEscapeRest,
	cgiEscapeFirst,
	cgiEscapeSecond
} cgiEscapeState;

typedef enum {
	cgiUnescapeSuccess,
	cgiUnescapeMemory
} cgiUnescapeResultType;


static cgiUnescapeResultType cgiUnescapeChars(char **sp, char *cp, int len);

static cgiParseResultType cgiParseFormInput(char *data, int length) {
	/* Scan for pairs, unescaping and storing them as they are found. */

#ifdef CGICDEBUG
			CGICDEBUGSTART
			fprintf(dout, "Data : %s\n",data);
			CGICDEBUGEND	
#endif /* CGICDEBUG */

	int pos = 0;
	cgiFormEntry *n;
	cgiFormEntry *l = 0;
	while (pos != length) {
		int foundEq = 0;
		int foundAmp = 0;
		int start = pos;
		int len = 0;
		char *attr;
		char *value;
		while (pos != length) {
			if (data[pos] == '=') {
				foundEq = 1;
				pos++;
				break;
			}
			pos++;
			len++;
		}
		if (!foundEq) {
			break;
		}
		if (cgiUnescapeChars(&attr, data+start, len)
			!= cgiUnescapeSuccess) {
			return cgiParseMemory;
		}	
		start = pos;
		len = 0;
		while (pos != length) {
			if (data[pos] == '&') {
				foundAmp = 1;
				pos++;
				break;
			}
                       else
                        if (data[pos] == '?') {
                                foundAmp = 1;
                                pos++;
                                break;
                        }
 
			pos++;
			len++;
		}
		/* The last pair probably won't be followed by a &, but
			that's fine, so check for that after accepting it */
		if (cgiUnescapeChars(&value, data+start, len)
			!= cgiUnescapeSuccess) {
			return cgiParseMemory;
		}	
		/* OK, we have a new pair, add it to the list. */
		n = (cgiFormEntry *) malloc(sizeof(cgiFormEntry));	
		if (!n) {
			return cgiParseMemory;
		}
		n->attr = attr;
		n->value = value;
		n->next = 0;
		if (!l) {
			cgiFormEntryFirst = n;
		} else {
			l->next = n;
		}
		l = n;
		if (!foundAmp) {
			break;
		}			
	}
	return cgiParseSuccess;
}

static int cgiHexValue[256];

cgiUnescapeResultType cgiUnescapeChars(char **sp, char *cp, int len) {
	char *s;
	cgiEscapeState escapeState = cgiEscapeRest;
	int escapedValue = 0;
	int srcPos = 0;
	int dstPos = 0;
	s = (char *) malloc(len + 1);
	if (!s) {
		return cgiUnescapeMemory;
	}
	while (srcPos < len) {
		int ch = cp[srcPos];
		switch (escapeState) {
			case cgiEscapeRest:
			if (ch == '%') {
				escapeState = cgiEscapeFirst;
			} else if (ch == '+') {
				s[dstPos++] = ' ';
			} else {
				s[dstPos++] = ch;	
			}
			break;
			case cgiEscapeFirst:
			escapedValue = cgiHexValue[ch] << 4;	
			escapeState = cgiEscapeSecond;
			break;
			case cgiEscapeSecond:
			escapedValue += cgiHexValue[ch];
			s[dstPos++] = escapedValue;
			escapeState = cgiEscapeRest;
			break;
		}
		srcPos++;
	}
	s[dstPos] = '\0';
	*sp = s;
	return cgiUnescapeSuccess;
}		
	
static void cgiSetupConstants() {
	int i;
	for (i=0; (i < 256); i++) {
		cgiHexValue[i] = 0;
	}
	cgiHexValue['0'] = 0;	
	cgiHexValue['1'] = 1;	
	cgiHexValue['2'] = 2;	
	cgiHexValue['3'] = 3;	
	cgiHexValue['4'] = 4;	
	cgiHexValue['5'] = 5;	
	cgiHexValue['6'] = 6;	
	cgiHexValue['7'] = 7;	
	cgiHexValue['8'] = 8;	
	cgiHexValue['9'] = 9;
	cgiHexValue['A'] = 10;
	cgiHexValue['B'] = 11;
	cgiHexValue['C'] = 12;
	cgiHexValue['D'] = 13;
	cgiHexValue['E'] = 14;
	cgiHexValue['F'] = 15;
	cgiHexValue['a'] = 10;
	cgiHexValue['b'] = 11;
	cgiHexValue['c'] = 12;
	cgiHexValue['d'] = 13;
	cgiHexValue['e'] = 14;
	cgiHexValue['f'] = 15;
}

static void cgiFreeResources() {
	cgiFormEntry *c = cgiFormEntryFirst;
	cgiFormEntry *n;
	while (c) {
		n = c->next;
		free(c->attr);
		free(c->value);
		free(c);
		c = n;
	}
	/* If the cgi environment was restored from a saved environment,
		then these are in allocated space and must also be freed */
	if (cgiRestored) {
		free(cgiServerSoftware);
		free(cgiServerName);
		free(cgiGatewayInterface);
		free(cgiServerProtocol);
		free(cgiServerPort);
		free(cgiRequestMethod);
		free(cgiPathInfo);
		free(cgiPathTranslated);
		free(cgiScriptName);
		free(cgiQueryString);
		free(cgiRemoteHost);
		free(cgiRemoteAddr);
		free(cgiAuthType);
		free(cgiRemoteUser);
		free(cgiRemoteIdent);
		free(cgiContentType);
		free(cgiAccept);
		free(cgiUserAgent);
	}
}

static cgiFormResultType cgiFormEntryString(
	cgiFormEntry *e, char *result, int max, int newlines);

static cgiFormEntry *cgiFormEntryFindFirst(char *name);
static cgiFormEntry *cgiFormEntryFindNext();


/*--------------------------------------------------------------------------------*/
static void GetUpload()
{
char ContentLength[20];
char QueryString[200];
char* Buffer;
char Header[4000];
char FileName[250];
char* Ptr;
FILE *infile;
long Read;
int BoundaryLength;
char ID[200];
char Seite[200];
char Path[300];



#ifdef CGICDEBUG
	CGICDEBUGSTART
  fprintf(dout, "START ---------------------------\n");
	CGICDEBUGEND	
#endif /* CGICDEBUG */


  if (getenv("CONTENT_LENGTH") != NULL)
    strcpy(ContentLength,getenv("CONTENT_LENGTH"));
  else
    {
      return ;
    }

  if (getenv("QUERY_STRING") != NULL)
    {
      strcpy(QueryString,getenv("QUERY_STRING"));

      strcpy(ID,&QueryString[3]);
      Ptr = strchr(ID,'&');
      strcpy(Path,Ptr +6);
      *Ptr = '\0';

      Ptr = strchr(Path,'&');
      strcpy(Seite,Ptr +6);
      *Ptr = '\0';


      #ifdef CGICDEBUG
        CGICDEBUGSTART
	      fprintf(dout, "QueryString : %s\n",QueryString);
        fprintf(dout, "ID : %s\n",ID);
        fprintf(dout, "Path : %s\n",Path);
        fprintf(dout, "Seite : %s\n",Seite);
	      CGICDEBUGEND
      #endif 

      InsertEntry("ID", ID);
      InsertEntry("HTML", Seite);
    }


  Buffer =(char*) malloc(atoi(ContentLength));
  memset(Buffer, '\0', atoi(ContentLength));

  if ((Read = fread(Buffer, 1, atoi(ContentLength), cgiIn)) != atoi(ContentLength))
    {
      free(Buffer);
      return ;
    }

      #ifdef CGICDEBUG
        CGICDEBUGSTART
        fprintf(dout, "ContentLength : %s\n",ContentLength);
        fprintf(dout, "Read : #%ld#\n",Read);
	      fprintf(dout, "Data : %s\n",Buffer);
	      CGICDEBUGEND
      #endif 


  if ((Ptr = (char*) memchr(Buffer,'\n', Read)) == NULL)
    return;

  BoundaryLength = (Ptr-Buffer)+3;
  if ((Ptr = (char*) memchr(Ptr+1,'\n', Read)) == NULL)
    return;
  if ((Ptr = (char*) memchr(Ptr+1,'\n', Read)) == NULL)
    return;
  if ((Ptr = (char*) memchr(Ptr+1,'\n', Read)) == NULL)
    return;

  *Ptr = '\0';

      #ifdef CGICDEBUG
        CGICDEBUGSTART
        fprintf(dout, "BoundaryLength : %ld\n",BoundaryLength);
	      CGICDEBUGEND
      #endif 

  strcpy(Header,Buffer);

  Read = Read - (Ptr+1-Buffer);
  memmove(Buffer, Ptr+1, Read);

  #ifdef CGICDEBUG
    CGICDEBUGSTART
    fprintf(dout, "Header : %s\n",Header);
    CGICDEBUGEND
  #endif 

    if ((Ptr = strstr(Header,"\r\n")) == NULL)
    {
      #ifdef CGICDEBUG
        CGICDEBUGSTART
        fprintf(dout, "1. Header : %s\n",Header);
        CGICDEBUGEND
      #endif 
    }
  strcpy(Header,Ptr);


  if ((Ptr = strchr(Header,'=')) == NULL)
    {
      #ifdef CGICDEBUG
        CGICDEBUGSTART
        fprintf(dout, "2. Header : %s\n",Header);
        CGICDEBUGEND
      #endif 
    }
  strcpy(Header,Ptr+1);

  if ((Ptr = strchr(Header,'=')) == NULL)
    {
      #ifdef CGICDEBUG
        CGICDEBUGSTART
        fprintf(dout, "3. Header : %s\n",Header);
        CGICDEBUGEND
      #endif 
    }
  strcpy(Header,Ptr+2);

  if ((Ptr = strchr(Header,'"')) == NULL)
    {
      #ifdef CGICDEBUG
        CGICDEBUGSTART
        fprintf(dout, "4. Header : %s\n",Header);
        CGICDEBUGEND
      #endif 
    }
  strncpy(FileName,Header, (Ptr- &Header[0])+1);

  #ifdef CGICDEBUG
     CGICDEBUGSTART
     fprintf(dout, "RohFilename : %s\n",FileName);
     CGICDEBUGEND
  #endif 


  Ptr = strrchr(FileName,'\\');
  if (Ptr ==NULL)
    {
       Ptr = strrchr(FileName,'/');
       if (Ptr != NULL)
        {
          #ifndef WIN32
            strcpy(FileName,Ptr+1);
            FileName[strlen(FileName)-1]='\0';
          #endif
        }
      else
        {
          #ifndef WIN32
            FileName[strlen(FileName)-1]='\0';
          #endif
        }
    }
  else
    {
          #ifndef WIN32
            strcpy(FileName,Ptr+1);
            FileName[strlen(FileName)-1]='\0';
          #endif
    }

  if (Path[strlen(Path)-1] != '/')
    strcat(Path,"/");

  strcat(Path,FileName);

  if (Path[strlen(Path)-1] == '"')
    Path[strlen(Path)-1] = '\0';
  else
    {
      if ((Ptr = strrchr(Path,'"')) != NULL)
        *Ptr = '\0';
    }


  InsertEntry("FILENAME", Path);

  #ifdef CGICDEBUG
    CGICDEBUGSTART
    fprintf(dout, "Filename : %s\n",Path);
    fprintf(dout, "length %ld",Read-BoundaryLength);
    CGICDEBUGEND
  #endif 

#ifdef WIN32
  if ((infile = fopen(Path, "w+b")) == NULL)
#else
  if ((infile = fopen(Path, "w+")) == NULL)
#endif
    {
      #ifdef CGICDEBUG
        CGICDEBUGSTART
        fprintf(dout,"Konnte Datei nicht oeffnen\n");
        CGICDEBUGEND
      #endif 
      free(Buffer);
      return;
    }

  fwrite(Buffer, 1, Read-BoundaryLength-2, infile);
  fclose(infile);

  free(Buffer);

} 
/*--------------------------------------------------------------------------------*/
void InsertEntry(char* Name, char* Value)
{
cgiFormEntry *n;

		n = (cgiFormEntry *) malloc(sizeof(cgiFormEntry));	
  
		n->attr = (char*) malloc(strlen(Name)+1);
    strcpy(n->attr,Name);
		n->value = (char*) malloc(strlen(Value)+1);
    strcpy(n->value,Value);

		n->next = cgiFormEntryFirst;
    cgiFormEntryFirst = n;

}
/*--------------------------------------------------------------------------------*/
char* cgiString(char *name) 
{
cgiFormEntry *e;

	e = cgiFormEntryFindFirst(name);

	if (e != NULL) 
    {
       return e->value;
	  }
	return NULL;
}
/*--------------------------------------------------------------------------------*/
char* String2HREF(char* String)
{
char Buffer1[500];
char Buffer2[500];
char Buffer3[10];
/*char* Ret;*/
unsigned int Counter;



  strcpy(Buffer1,String);      
  strcpy(Buffer2,"");

  for (Counter = 0; Counter < strlen(Buffer1) ; Counter++)
    {
      if (Buffer1[Counter] < 48)
        sprintf(Buffer3,"%%%X",(unsigned char) Buffer1[Counter]);
      else
        sprintf(Buffer3,"%c",(unsigned char) Buffer1[Counter]);

      strcat(Buffer2,Buffer3);

    }                               

  strcpy(String,Buffer2);

  return String;
}
/*--------------------------------------------------------------------------------*/
cgiFormResultType cgiFormString(char *name, char *result, int max) 
{
cgiFormEntry *e;

	e = cgiFormEntryFindFirst(name);
	if (!e) {
		strcpy(result, "");
		return cgiFormNotFound;
	}
	return cgiFormEntryString(e, result, max, 1);
}
/*--------------------------------------------------------------------------------*/
cgiFormResultType cgiFormStringNoNewlines(
        char *name, char *result, int max) {
	cgiFormEntry *e;
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		strcpy(result, "");
		return cgiFormNotFound;
	}
	return cgiFormEntryString(e, result, max, 0);
}

cgiFormResultType cgiFormStringMultiple(
        char *name, char ***result) {
	char **stringArray;
	cgiFormEntry *e;
	int i;
	int total = 0;
	/* Make two passes. One would be more efficient, but this
		function is not commonly used. The select menu and
		radio box functions are faster. */
	e = cgiFormEntryFindFirst(name);
	if (e != 0) {
		do {
			total++;
		} while ((e = cgiFormEntryFindNext()) != 0); 
	}
	stringArray = (char **) malloc(sizeof(char *) * (total + 1));
	if (!stringArray) {
		*result = 0;
		return cgiFormMemory;
	}
	/* initialize all entries to null; the last will stay that way */
	for (i=0; (i <= total); i++) {
		stringArray[i] = 0;
	}
	/* Now go get the entries */
	e = cgiFormEntryFindFirst(name);
#ifdef CGICDEBUG
	CGICDEBUGSTART
	fprintf(dout, "StringMultiple Beginning\n");
	CGICDEBUGEND
#endif /* CGICDEBUG */
	if (e) {
		i = 0;
		do {
			int max = strlen(e->value) + 1;
			stringArray[i] = (char *) malloc(max);
			if (stringArray[i] == 0) {
				/* Memory problems */
				cgiStringArrayFree(stringArray);
				*result = 0;
				return cgiFormMemory;
			}	
			strcpy(stringArray[i], e->value);
			cgiFormEntryString(e, stringArray[i], max, 1);
			i++;
		} while ((e = cgiFormEntryFindNext()) != 0); 
		*result = stringArray;
#ifdef CGICDEBUG
		CGICDEBUGSTART
		fprintf(dout, "StringMultiple Succeeding\n");
		CGICDEBUGEND
#endif /* CGICDEBUG */
		return cgiFormSuccess;
	} else {
		*result = stringArray;
#ifdef CGICDEBUG
		CGICDEBUGSTART
		fprintf(dout, "StringMultiple found nothing\n");
		CGICDEBUGEND
#endif /* CGICDEBUG */
		return cgiFormNotFound;
	}	
}

cgiFormResultType cgiFormStringSpaceNeeded(
        char *name, int *result) {
	cgiFormEntry *e;
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		*result = 1;
		return cgiFormNotFound; 
	}
	*result = strlen(e->value) + 1;
	return cgiFormSuccess;
}

static cgiFormResultType cgiFormEntryString(
	cgiFormEntry *e, char *result, int max, int newlines) {
	char *dp, *sp;
	int truncated = 0;
	int len = 0;
	int avail = max-1;
	int crCount = 0;
	int lfCount = 0;	
	dp = result;
	sp = e->value;	
	while (1) {
		int ch;
		ch = *sp;
		if (len == avail) {
			truncated = 1;
			break;
		}	 
		/* Fix the CR/LF, LF, CR nightmare: watch for
			consecutive bursts of CRs and LFs in whatever
			pattern, then actually output the larger number 
			of LFs. Consistently sane, yet it still allows
			consecutive blank lines when the user
			actually intends them. */
		if ((ch == 13) || (ch == 10)) {
			if (ch == 13) {
				crCount++;
			} else {
				lfCount++;
			}	
		} else {
			if (crCount || lfCount) {
				int lfsAdd = crCount;
				if (lfCount > crCount) {
					lfsAdd = lfCount;
				}
				/* Stomp all newlines if desired */
				if (!newlines) {
					lfsAdd = 0;
				}
				while (lfsAdd) {
					if (len == avail) {
						truncated = 1;
						break;
					}
					*dp = 10;
					dp++;
					lfsAdd--;
					len++;		
				}
				crCount = 0;
				lfCount = 0;
			}
			if (ch == '\0') {
				/* The end of the source string */
				break;				
			}	
			*dp = ch;
			dp++;
			len++;
		}
		sp++;	
	}	
	*dp = '\0';
	if (truncated) {
		return cgiFormTruncated;
	} else if (!len) {
		return cgiFormEmpty;
	} else {
		return cgiFormSuccess;
	}
}

static int cgiFirstNonspaceChar(char *s);

cgiFormResultType cgiFormInteger(
        char *name, int *result, int defaultV) {
	cgiFormEntry *e;
	int ch;
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		*result = defaultV;
		return cgiFormNotFound; 
	}	
	if (!strlen(e->value)) {
		*result = defaultV;
		return cgiFormEmpty;
	}
	ch = cgiFirstNonspaceChar(e->value);
	if (!(isdigit(ch)) && (ch != '-') && (ch != '+')) {
		*result = defaultV;
		return cgiFormBadType;
	} else {
		*result = atoi(e->value);
		return cgiFormSuccess;
	}
}

cgiFormResultType cgiFormIntegerBounded(
        char *name, int *result, int min, int max, int defaultV) {
	cgiFormResultType error = cgiFormInteger(name, result, defaultV);
	if (error != cgiFormSuccess) {
		return error;
	}
	if (*result < min) {
		*result = min;
		return cgiFormConstrained;
	} 
	if (*result > max) {
		*result = max;
		return cgiFormConstrained;
	} 
	return cgiFormSuccess;
}

cgiFormResultType cgiFormDouble(
        char *name, double *result, double defaultV) {
	cgiFormEntry *e;
	int ch;
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		*result = defaultV;
		return cgiFormNotFound; 
	}	
	if (!strlen(e->value)) {
		*result = defaultV;
		return cgiFormEmpty;
	} 
	ch = cgiFirstNonspaceChar(e->value);
	if (!(isdigit(ch)) && (ch != '.') && (ch != '-') && (ch != '+')) {
		*result = defaultV;
		return cgiFormBadType;
	} else {
		*result = atof(e->value);
		return cgiFormSuccess;
	}
}

cgiFormResultType cgiFormDoubleBounded(
        char *name, double *result, double min, double max, double defaultV) {
	cgiFormResultType error = cgiFormDouble(name, result, defaultV);
	if (error != cgiFormSuccess) {
		return error;
	}
	if (*result < min) {
		*result = min;
		return cgiFormConstrained;
	} 
	if (*result > max) {
		*result = max;
		return cgiFormConstrained;
	} 
	return cgiFormSuccess;
}

cgiFormResultType cgiFormSelectSingle(
	char *name, char **choicesText, int choicesTotal, 
	int *result, int defaultV) 
{
	cgiFormEntry *e;
	int i;
	e = cgiFormEntryFindFirst(name);
#ifdef CGICDEBUG
	CGICDEBUGSTART
	fprintf(dout, "%d\n", (int) e);
	CGICDEBUGEND
#endif /* CGICDEBUG */
	if (!e) {
		*result = defaultV;
		return cgiFormNotFound;
	}
	for (i=0; (i < choicesTotal); i++) {
#ifdef CGICDEBUG
		CGICDEBUGSTART
		fprintf(dout, "%s %s\n", choicesText[i], e->value);
		CGICDEBUGEND
#endif /* CGICDEBUG */
		if (cgiStrEq(choicesText[i], e->value)) {
#ifdef CGICDEBUG
			CGICDEBUGSTART
			fprintf(dout, "MATCH\n");
			CGICDEBUGEND
#endif /* CGICDEBUG */
			*result = i;
			return cgiFormSuccess;
		}
	}
	*result = defaultV;
	return cgiFormNoSuchChoice;
}

cgiFormResultType cgiFormSelectMultiple(
	char *name, char **choicesText, int choicesTotal, 
	int *result, int *invalid) 
{
	cgiFormEntry *e;
	int i;
	int hits = 0;
	int invalidE = 0;
	for (i=0; (i < choicesTotal); i++) {
		result[i] = 0;
	}
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		*invalid = invalidE;
		return cgiFormNotFound;
	}
	do {
		int hit = 0;
		for (i=0; (i < choicesTotal); i++) {
			if (cgiStrEq(choicesText[i], e->value)) {
				result[i] = 1;
				hits++;
				hit = 1;
				break;
			}
		}
		if (!(hit)) {
			invalidE++;
		}
	} while ((e = cgiFormEntryFindNext()) != 0);

	*invalid = invalidE;

	if (hits) {
		return cgiFormSuccess;
	} else {
		return cgiFormNotFound;
	}
}

cgiFormResultType cgiFormCheckboxSingle(
	char *name)
{
	cgiFormEntry *e;
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		return cgiFormNotFound;
	}
	return cgiFormSuccess;
}

extern cgiFormResultType cgiFormCheckboxMultiple(
	char *name, char **valuesText, int valuesTotal, 
	int *result, int *invalid)
{
	/* Implementation is identical to cgiFormSelectMultiple. */
	return cgiFormSelectMultiple(name, valuesText, 
		valuesTotal, result, invalid);
}

cgiFormResultType cgiFormRadio(
	char *name, 
	char **valuesText, int valuesTotal, int *result, int defaultV)
{
	/* Implementation is identical to cgiFormSelectSingle. */
	return cgiFormSelectSingle(name, valuesText, valuesTotal, 
		result, defaultV);
}

void cgiHeaderLocation(char *redirectUrl) {
	fprintf(cgiOut, "Location: %s%c%c", redirectUrl, 10, 10); 
}

void cgiHeaderStatus(int status, char *statusMessage) {
	fprintf(cgiOut, "Status: %d %s%c%c", status, statusMessage, 
	10, 10);
}

void cgiHeaderContentType(char *mimeType) {
	fprintf(cgiOut, "Content-type: %s%c%c", mimeType, 10, 10);
}

#ifndef NO_SYSTEM

int cgiSaferSystem(char *command) {
	char *s;
	char *sp;
	int i;
	int len = (strlen(command) * 2) + 1;
	s = (char *) malloc(len);
	if (!s) {
		return -1;
	}
	sp = s;
	for (i=0; (i < len); i++) {
		if (command[i] == ';') {
			*sp = '\\';
			sp++;
		} else if (command[i] == '|') {
			*sp = '\\';
			sp++;
		}
		*sp = command[i];
		sp++;
	}
	*sp = '\0';
	return system(s);
}		

#endif /* NO_SYSTEM */

static int cgiWriteString(FILE *out, char *s);

static int cgiWriteInt(FILE *out, int i);

cgiEnvironmentResultType cgiWriteEnvironment(char *filename) {
	FILE *out;
	cgiFormEntry *e;
	/* Be sure to open in binary mode */
	out = fopen(filename, "wb");
	if (!out) {
		/* Can't create file */
		return cgiEnvironmentIO;
	}
	if (!cgiWriteString(out, cgiServerSoftware)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiServerName)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiGatewayInterface)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiServerProtocol)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiServerPort)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiRequestMethod)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiPathInfo)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiPathTranslated)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiScriptName)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiQueryString)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiRemoteHost)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiRemoteAddr)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiAuthType)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiRemoteUser)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiRemoteIdent)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiContentType)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiAccept)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiUserAgent)) {
		goto error;
	}
	if (!cgiWriteInt(out, cgiContentLength)) {
		goto error;
	}
	e = cgiFormEntryFirst;
	while (e) {
		if (!cgiWriteString(out, e->attr)) {
			goto error;
		}
		if (!cgiWriteString(out, e->value)) {
			goto error;
		}
		e = e->next;
	}
	fclose(out);
	return cgiEnvironmentSuccess;
error:
	fclose(out);
	/* If this function is not defined in your system,
		you must substitute the appropriate 
		file-deletion function. */
	unlink(filename);
	return cgiEnvironmentIO;
}

static int cgiWriteString(FILE *out, char *s) {
	unsigned int len = strlen(s);
	cgiWriteInt(out, len);
	if (fwrite(s, 1, len, out) != len) {
		return 0;
	}
	return 1;
}

static int cgiWriteInt(FILE *out, int i) {
	if (!fwrite(&i, sizeof(int), 1, out)) {
		return 0;
	}
	return 1;
}

static int cgiReadString(FILE *out, char **s);

static int cgiReadInt(FILE *out, int *i);

cgiEnvironmentResultType cgiReadEnvironment(char *filename) {
	FILE *in;
	cgiFormEntry *e, *p;
	/* Free any existing data first */
	cgiFreeResources();
	/* Be sure to open in binary mode */
	in = fopen(filename, "rb");
	if (!in) {
		/* Can't access file */
		return cgiEnvironmentIO;
	}
	if (!cgiReadString(in, &cgiServerSoftware)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiServerName)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiGatewayInterface)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiServerProtocol)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiServerPort)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiRequestMethod)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiPathInfo)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiPathTranslated)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiScriptName)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiQueryString)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiRemoteHost)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiRemoteAddr)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiAuthType)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiRemoteUser)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiRemoteIdent)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiContentType)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiAccept)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiUserAgent)) {
		goto error;
	}
	if (!cgiReadInt(in, &cgiContentLength)) {
		goto error;
	}
	p = 0;
	while (1) {
		e = (cgiFormEntry *) malloc(sizeof(cgiFormEntry));
		if (!e) {
			cgiFreeResources();
			fclose(in);
			return cgiEnvironmentMemory;
		}
		if (!cgiReadString(in, &e->attr)) {
			/* This means we've reached the end of the list. */
			free(e);
			break;
		}
		if (!cgiReadString(in, &e->value)) {
			free(e);
			goto error;
		}
		e->next = 0;
		if (p) {
			p->next = e;
		} else {
			cgiFormEntryFirst = e;
		}	
		p = e;
	}
	fclose(in);
	cgiRestored = 1;
	return cgiEnvironmentSuccess;
error:
	cgiFreeResources();
	fclose(in);
	return cgiEnvironmentIO;
}

static int cgiReadString(FILE *in, char **s) {
	int len;
	cgiReadInt(in, &len);
	*s = (char *) malloc(len + 1);
	if (!(*s)) {
		return 0;
	}	
	if (fread(*s, 1, len, in) != (unsigned int) len) {
		return 0;
	}
	(*s)[len] = '\0';
	return 1;
}

static int cgiReadInt(FILE *out, int *i) {
	if (!fread(i, sizeof(int), 1, out)) {
		return 0;
	}
	return 1;
}

static int cgiStrEqNc(char *s1, char *s2) {
	while(1) {
		if (!(*s1)) {
			if (!(*s2)) {
				return 1;
			} else {
				return 0;
			}
		} else if (!(*s2)) {
			return 0;
		}
		if (isalpha(*s1)) {
			if (tolower(*s1) != tolower(*s2)) {
				return 0;
			}
		} else if ((*s1) != (*s2)) {
			return 0;
		}
		s1++;
		s2++;
	}
}

static char *cgiFindTarget = 0;
static cgiFormEntry *cgiFindPos = 0;


cgiFormEntry *cgiFormEntryGetFirst() 
{
	cgiFindPos = cgiFormEntryFirst;
	return cgiFormEntryGetNext();
}

cgiFormEntry *cgiFormEntryGetNext() 
{
  if (cgiFindPos) 
    {
		  cgiFormEntry *c = cgiFindPos;
		  cgiFindPos = c->next;
	    return c;
	  }

	return NULL;
}



static cgiFormEntry *cgiFormEntryFindFirst(char *name) {
	cgiFindTarget = name;
	cgiFindPos = cgiFormEntryFirst;
	return cgiFormEntryFindNext();
}

static cgiFormEntry *cgiFormEntryFindNext() {
	while (cgiFindPos) {
		cgiFormEntry *c = cgiFindPos;
		cgiFindPos = c->next;
		if (!strcasecmp(c -> attr, cgiFindTarget)) {
			return c;
		}
	}
	return 0;
}

static int cgiFirstNonspaceChar(char *s) {
	int len = strspn(s, " \n\r\t");
	return s[len];
}

void cgiStringArrayFree(char **stringArray) {
	char *p;
	p = *stringArray;
	while (p) {
		free(p);
		stringArray++;
		p = *stringArray;
	}
}	

