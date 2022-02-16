#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <malloc.h>
#include "compiler.h"
#include "vslanguage.h"
#include "y.tab.h"

#ifdef WIN32
  #include "win32tools.h"
  #include <io.h>
  #include <fcntl.h>
  #define strncasecmp _strnicmp
#endif

extern int yydebug;
extern void yyparse();

char *out_file_name;
int yyinit = 1;

int html_parse = 1;
int string_parse = 0;
int skip_newline = 0;
int LineNumber = 1;

#ifdef MTLCOMPILE
  #define REM "!!REM"
  #define REMEND "!!REMEND"
  #define ECHO "!!ECHO"
  #define ECHOEND "!!ECHOEND"
  #define PREFIX "!!"
  #define PREFIXCHAR '!'
#else
  #define REM "%%REM"
  #define REMEND "%%REMEND"
  #define ECHO "%%ECHO"
  #define ECHOEND "%%ECHOEND"
  #define PREFIX "%%"
  #define PREFIXCHAR '%'
#endif

/*----------------------------------------------------------------------------------*/
void ErrorMessage(char *Text)
{
/*
  if (DebugOutput == 1)    double
    printf("<font color=\"#FF0000\">%s</font>",Text);
*/
}
/*----------------------------------------------------------------------------------*/
void yyerror()
{
  if (DebugOutput == 1)
    printf("<font color=\"#FF0000\"><blink><b>ERROR</b></blink></font>");
}
/*----------------------------------------------------------------------------------*/

#define TOK_MATCH(s1,token) if(!strncasecmp(buf+current,s1,strlen(s1))) { current+=strlen(s1); if (DebugOutput == 1) printf("<font color=\"#000099\">%s</font>", s1); begin=current; return token; }

#ifndef WIN32
  #define end_of_file (current > buflength)
#else
  #define end_of_file (current >= buflength)
#endif

int yylex()
{

  while (1) 
    {
	    if (end_of_file)
	      return -1;

	    if (html_parse) 
        {
	        while (1) 
            {
              if (buf[current] == '\n')
                {
	  	            LineNumber++;
                  if (DebugOutput == 1) printf("<BR>");
                }

		          if (buf[current] == PREFIXCHAR || end_of_file)
		            if (buf[current + 1] == PREFIXCHAR || end_of_file) 
                      {
                          if ((buf[begin] == '\n') || (buf[begin] == '\r'))
                            begin++;
                          if ((buf[begin] == '\n') || (buf[begin] == '\r'))
                            begin++;

			            html_parse = 0;
			            yylval.string = malloc(current - begin + 1);
			            strncpy(yylval.string, buf + begin, current - begin);
			            yylval.string[current - begin] = 0;
                        begin = current;

                        return HTML;
		              }
              if (DebugOutput == 1) 
                {
                  if (buf[current] == '<') 
                    printf("<font color=#555555><i>&lt");
                  else
                  if (buf[current] == '>') 
                    printf("&gt</i></font>");
                  else
                    printf("%c",buf[current]);
                }

              
		          current++;
	          }
        } 
      else 
        if (string_parse == 1) 
          {
	          begin = current;

	          while ((buf[current] != '"') ||  ((buf[current] == '"') && (buf[current-1] == '\\')))
                {
                  if (buf[current] == '"')
                    {
                      int StartPoint = current;
                        while(buf[StartPoint] != '\0')
                          {
                            buf[StartPoint-1] = buf[StartPoint];
                            StartPoint++;
                          }
                    }
		          current++;
                }

	          string_parse = 0;
	          yylval.string = malloc(current - begin + 1);
	          strncpy(yylval.string, buf + begin, current - begin);
	          yylval.string[current - begin] = 0;

            if (DebugOutput == 1) printf("%s",yylval.string);
	          current++;
	          return STRING;
	        } 
        else 
          {
	          begin = current;
	          if (isdigit(buf[current])) 
              {
		            int isfloat = 0;
		            while (isdigit(buf[current]))
		              current++;

		            if (buf[current] == '.') 
                  {
		                isfloat = 1;
		                current++;

		                while (isdigit(buf[current]))
			                current++;
		              }

		            yylval.string = malloc(current - begin + 1);
		            strncpy(yylval.string, buf + begin, current - begin);
		            yylval.string[current - begin] = 0;

        				if (DebugOutput == 1) printf("%s",yylval.string);

		            if (isfloat) 
           		    return FLOAT;
		            else 
            	    return NUMBER;
          	  } 
            else 
              {
                if(strncasecmp(buf+current,REM,strlen(REM))==0) 
                  {
                  
                    while(strncasecmp(buf+current,REMEND,strlen(REMEND)) != 0
                    && (! end_of_file))
                      current++;

              	    if (end_of_file)
	                    return -1;

                    current +=8;
                    begin=current;
                  }

                if(strncasecmp(buf+current,"ECHO",strlen("ECHO"))==0) 
                  {
                    begin=current + 6;
                    while(strncasecmp(buf+current,"ECHOEND",strlen("ECHOEND")) != 0
                    && (! end_of_file))
                      current++;

     		            yylval.string = malloc(current - begin + 1);
    		            strncpy(yylval.string, buf + begin, current - begin);
		                yylval.string[current - begin] = 0;

                    current +=9;
                    begin=current;
                    return HTML;

                  }


		            TOK_MATCH("NOT ", NOT);
		            TOK_MATCH("OR ", OR);
		            TOK_MATCH("AND ", AND);

                if(strncasecmp(buf+current,PREFIX,strlen(PREFIX)) == 0)
                  {
                     current+=2;
                     begin=current;
                     TOK_MATCH("PRINTSTACK",PRINTSTACK);
                     TOK_MATCH("VAL_FOR", FOR_VALUE);
                     TOK_MATCH("FORVALUE", FOR_VALUE);

	                 TOK_MATCH("IF", IF);
	                 TOK_MATCH("ELSE", ELSE);
	                 TOK_MATCH("ENDIF", ENDIF);
	                 TOK_MATCH("FOR[", FOR);
                     TOK_MATCH("FOR [", FOR);
	                 TOK_MATCH("NEXT", NEXT);
	                 TOK_MATCH("WHILE", WHILE);
	                 TOK_MATCH("WEND", WEND);
	                 TOK_MATCH("REPEAT", REPEAT);
	                 TOK_MATCH("UNTIL", UNTIL);
	                 TOK_MATCH("CATALOGLIST", CATALOGLIST);
	                 TOK_MATCH("CATALOGEND", CATALOGEND);
                     TOK_MATCH("CATALOG.[", CATALOG);
                     TOK_MATCH("CATALOGVALUE", CATALOGVALUE);
                     TOK_MATCH("CATALOGLINK", CATALOGLINK);
	                 TOK_MATCH("PRODUCTLIST", PRODUCTLIST);
	                 TOK_MATCH("PRODUCTEND", PRODUCTEND);
                     TOK_MATCH("PRODUCT.[", PRODUCT);
	                 TOK_MATCH("PRODUCTVALUE", PRODUCTVALUE);
                     TOK_MATCH("PRODUCTLINK", PRODUCTLINK);
	                 TOK_MATCH("BASKETLIST", BASKETLIST);
	                 TOK_MATCH("BASKETEND", BASKETEND);
                     TOK_MATCH("BASKET.[", BASKET);
	                 TOK_MATCH("BASKETVALUE", BASKETVALUE);
	                 TOK_MATCH("PAYMENTLIST", PAYMENTLIST);
	                 TOK_MATCH("PAYMENTEND", PAYMENTEND);
                     TOK_MATCH("PAYMENT.[", PAYMENT);
	                 TOK_MATCH("PROMOTIONLIST", PROMOTIONLIST);
	                 TOK_MATCH("PROMOTIONEND", PROMOTIONEND);
                     TOK_MATCH("PROMOTION.[", PROMOTION);
	                 TOK_MATCH("PROMOTIONVALUE", PROMOTIONVALUE);
	                 TOK_MATCH("RELEVANTLIST", RELEVANTLIST);
	                 TOK_MATCH("RELEVANTEND", RELEVANTEND);
                     TOK_MATCH("RELEVANT.[", RELEVANT);
	                 TOK_MATCH("RELEVANTVALUE", PROMOTIONVALUE);
                     TOK_MATCH("SHIPPINGCOST.[", SHIPPINGCOST);
                     TOK_MATCH("SETCOOKIE", SETCOOKIE);
                     TOK_MATCH("REDIRECT", REDIRECT);
                     TOK_MATCH("MIMETYPE", MIMETYPE);
                     TOK_MATCH("SENDFILE", SENDFILE);
                     TOK_MATCH("INCLUDEVSL", INCLUDEVSL);
                     TOK_MATCH("MODULE", INCLUDEVSL);
		                 TOK_MATCH("", ESC);
                  }
  	            if (isalnum(buf[current]) || buf[current] == '_') 
                  {
		                while (isalnum(buf[current]) || buf[current] == '_')
			                current++;

		                yylval.string = malloc(current - begin + 1);
		                strncpy(yylval.string, buf + begin, current - begin);
		                yylval.string[current - begin] = 0;

                    if (DebugOutput == 1)
                      printf("<font color=\"#000000\">%s</font>", yylval.string);

                    return IDENT;

		              } 
                else 
                  if (isspace(buf[current])) 
                    {
                      #ifdef WIN32
		                    if ((buf[current] == '\r') || (buf[current] == '\n')) 
                          {
                            begin++;
                      #else
                        if (buf[current] == '\n') 
                          {
                            begin++;
                      #endif
                     				if (DebugOutput == 1) printf("<BR>\n");
			                      LineNumber++;
			                      if (!skip_newline)
	   		                      html_parse = 1;
		                      }
                        else
                          if (DebugOutput == 1) printf(" ");
		                } 
                  else 
                    {
                      begin++;
               				if (DebugOutput == 1) 
                        {
                          printf("<font color=#000099><B>%c</B></font>",buf[current]);
                        }  
		                  return buf[current++];
		                }
	            }
	        }
	    current++;                                        
    }
}
/*----------------------------------------------------------------------------------*/
void main(int argc, char **argv)
{
#ifndef WIN32
  FILE *inp;
#else
  int fd;
#endif

char* Ptr;

  if (argc < 2)
    {
      printf("%s\n",TEXT_VERSION);
      exit(0);
    }

  if (argc == 3)
    DebugOutput = 1;
  else
    DebugOutput = 0;

  Magic = 0;

   #ifndef WIN32
    if (yyinit) 
      {
	      if ((inp = fopen(argv[1], "r")) == NULL)
          exit(0);

	      buflength = fread(buf, 1, 64000, inp);
	      fclose(inp);
      }
    #else
      if ((fd = _open(argv[1], O_RDONLY | _O_TEXT )) == NULL)
        exit(0);

      buflength = _read(fd,buf,64000);
      _close(fd);
    #endif


    yyinit = 0;            

    strcpy(Path,argv[1]);
    #ifdef WIN32
      if ((Ptr = strrchr(Path,'\\')) != NULL)
        {
          *Ptr = '\0';
        }
    #else
      if ((Ptr = strrchr(Path,'/')) != NULL)
        {
          *Ptr = '\0';
        }
    #endif

    out_file_name = malloc(strlen(argv[1]) + 3);
    strcpy(out_file_name, argv[1]);
    strcat(out_file_name, ".p");
    yydebug = 1;

    yyparse();
    free(out_file_name);

    exit(1);
}
