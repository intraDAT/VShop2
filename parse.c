#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>

static char fieldSeparator = '^';

int GetField(FILE *in, char *field, int space)
{
	int ch;
	int begun = 0;
	int len = 0;

	while(1) 
    {
		  ch = getc(in);
		  if ((ch == fieldSeparator) || (ch == '\r') || (ch == '\n') || (ch == EOF)) 
        {
			    if (len < space) 
            field[len] = '\0';
          else 
            field[space-1] = '\0';

			    if ((ch == '\n') || (ch == '\r') || (ch == EOF))
            {
				      ungetc(ch, in);
				      //if (!begun) 
                return 0;
			      }	
			    return 1;
		    }

		  if (!begun) 
        {
  			  if (!isspace(ch)) 
    				begun = 1;
  	  	}	
	
    	if (begun) 
        {
  	  		if (len < space) 
            field[len] = ch;
  			  len++;
  	  	}			
	  }
}

int NextRecord(FILE *in) 
{
	int ch;
	while(1) 
    {
		  ch = getc(in);
		  if (ch == EOF) 
        {
			    return 0;
		    }
		  if (ch == '\n') 
        {
			    while(1) 
            {
				      ch = getc(in);
				      if (ch == EOF) 
                {
					        return 0;
				        }
				      if (!isspace(ch)) 
                {
					        ungetc(ch, in);
					        return 1;
				        }
			      }
		    }
	  }
}

void SetFieldSeparator(char sep) {
	fieldSeparator = sep;
}
