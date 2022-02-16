%{
#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "m_code.h"
extern int skip_newline;
extern int string_parse;
extern int html_parse;
extern int yylex();
extern void yyerror();
extern void ErrorMessage(char* Text);

%}
%union
{
  char *string;
  struct _if_hints if_hints; 
  struct _while_hints while_hints; 
  struct _for_hints for_hints; 
  struct _do_hints do_hints; 
  struct _func_hints func_hints; 
};

%token <string> NOT OR AND EQCOMP ARITHCOMP 
%token <string> IDENT 
%token <string> NUMBER FLOAT STRING
%token <string> HTML PRINTSTACK
%token <string> ESC IF ELSE ENDIF FOR NEXT WHILE WEND DO LOOP FOR_VALUE REPEAT UNTIL 
%token <string> PRODUCTLIST PRODUCTEND PRODUCTVALUE PRODUCT PRODUCTNEXT PRODUCTPREV PRODUCTLINK
%token <string> CATALOGLIST CATALOGEND CATALOGVALUE CATALOG CATALOGNEXT CATALOGPREV CATALOGLINK
%token <string> BASKETLIST BASKETEND BASKETVALUE BASKET BASKETNEXT BASKETPREV
%token <string> PROMOTIONLIST PROMOTIONEND PROMOTIONVALUE PROMOTION
%token <string> RELEVANTLIST RELEVANTEND RELEVANTVALUE RELEVANT
%token <string> PAYMENTLIST PAYMENTEND PAYMENT
%token <string> SHIPPINGCOSTLIST SHIPPINGCOSTEND SHIPPINGCOST
%token <string> SETCOOKIE REDIRECT MIMETYPE SENDFILE INCLUDEVSL
%type <string> error
%type <string> ident
%type <string> full file cmd html builtin table_func
%type <if_hints> if_cmd if_else_cmd
%type <for_hints> for_cmd for_help product_cmd product_help catalog_cmd catalog_help basket_cmd basket_help promotion_cmd promotion_help relevant_cmd relevant_help
%type <do_hints> do_cmd 
%type <while_hints> while_cmd while_expr repeat_cmd payment_cmd shippingcost_cmd
%type <string> BEG_STR str_expr bool_expr expr simple_expr HTMLliteral
%type <func_hints> opt_arg beginn_args arg_list

%right '='
%left '<' '>'
%left OR
%left AND
%left EQCOMP
%left ARITHCOMP
%left '+' '-'
%left '*' '/' '%'
%left NOT
%%

full: file { p_code_finish(); };

file: /* empty */  {}  
      | cmd file  {} 
;
/*-------------------------------------------------------------------------------*/
HTMLliteral : '<'  { $$ = "<"; html_parse = 1;}
	     	| '>'  { $$ = ">"; html_parse = 1;}
		    | '='  { $$ = "="; html_parse = 1;}
    		| '?'  { $$ = "?"; html_parse = 1;}
	    	| '&'  { $$ = "&"; html_parse = 1;}
      		| '/'  { $$ = "/"; html_parse = 1;}
    		| '\\' { $$ = "\\"; html_parse = 1;}
     		| '"'  { $$ = "\""; html_parse = 1;}
     		| '\''  { $$ = "\'"; html_parse = 1;}
    		| '!'  { $$ = "!"; html_parse = 1;}
    		| '('  { $$ = "("; html_parse = 1;}
     		| ')'  { $$ = ")"; html_parse = 1;}
			| '{'  { $$ = "{"; html_parse = 1;}
			| '}'  { $$ = "}"; html_parse = 1;}
			| 'º'  { $$ = "º"; html_parse = 1;}
			| '$'  { $$ = "$"; html_parse = 1;}
      		| '+'  { $$ = "+"; html_parse = 1;}
    		| '*'  { $$ = "*"; html_parse = 1;}
	    	| '#'  { $$ = "#"; html_parse = 1;}
	    	| '-'  { $$ = "-"; html_parse = 1;}
		    | ':'  { $$ = ":"; html_parse = 1;}
    		| ';'  { $$ = ";"; html_parse = 1;}
	    	| '|'  { $$ = "|"; html_parse = 1;}
    		| '^'  { $$ = "^"; html_parse = 1;}
	    	| '@'  { $$ = "@"; html_parse = 1;}
    		| '%'  { $$ = "%"; html_parse = 1;}
	    	| ident { $$=" "; html_parse = 1;}
;			   
/*-------------------------------------------------------------------------------*/
cmd:   builtin {html_parse=1;}
     | html  {$$ = NULL;} 
     | simple_expr { op_code(PRINT, 0); html_parse=1;} 
	 | INCLUDEVSL '[' BEG_STR STRING ']'
      {
        char Filename[256];
	    char tmpbuf2[64000];
	    long length;
	    FILE *inp;

	    #ifdef WIN32
	      sprintf(Filename,"%s\\%s",Path,$4);
	    #else
	      sprintf(Filename,"%s/%s",Path,$4);
	    #endif

	    memcpy(tmpbuf2,buf,current);

	    if ((inp = fopen(Filename, "r")) != NULL)
	      {
	        length = fread(tmpbuf2+current, 1, 32000, inp);
		    fclose(inp);
	  	    memcpy(tmpbuf2+current+length,buf+current,buflength-current);
		    memcpy(buf,tmpbuf2,64000);
		    buflength += length;
		  }
		 else
		   {
			 sprintf(Filename,"Konnte include-Datei '%s/%s' nicht oeffnen",Path,$4);
			 p_code_pushs(new_text_entry(Filename));
		   }
		html_parse=1;
	   }
;
/*-------------------------------------------------------------------------------*/
html: HTML { p_code_pushs(new_text_entry($1)); op_code(PRINT, 0);}
;
/*-------------------------------------------------------------------------------*/
builtin: if_cmd file endif 
           { 
             p_code_label($1.else_label); 
           }
         | if_else_cmd file endif 
           { 
             p_code_label($1.endif_label); 
           } 
         | for_cmd file next 
           {
             op_code(SPECIAL, FOR_LOOP);
             p_code_jmp($1.for_label, JMP_UNC);
             p_code_label($1.next_label);
             op_code(POP, 0);
             op_code(POP, 0);
             op_code(POP, 0);
           }

         | product_cmd file productend
           {
             op_code(SPECIAL, PRODUCT_LOOP);
             p_code_jmp($1.for_label, JMP_UNC);
             p_code_label($1.next_label);
             op_code(POP, 0);
             op_code(POP, 0);
             op_code(POP, 0);
           }

         | catalog_cmd file catalogend 
           {
             op_code(SPECIAL, CATALOG_LOOP);
             p_code_jmp($1.for_label, JMP_UNC);
             p_code_label($1.next_label);
             op_code(POP, 0);
             op_code(POP, 0);
             op_code(POP, 0);
           }

         | basket_cmd file basketend 
           {
             op_code(SPECIAL, BASKET_LOOP);
             p_code_jmp($1.for_label, JMP_UNC);
             p_code_label($1.next_label);
             op_code(POP, 0);
             op_code(POP, 0);
             op_code(POP, 0);
           }

         | promotion_cmd file promotionend 
           {
             op_code(SPECIAL, PROMOTION_LOOP);
             p_code_jmp($1.for_label, JMP_UNC);
             p_code_label($1.next_label);
             op_code(POP, 0);
             op_code(POP, 0);
             op_code(POP, 0);
           }

         | relevant_cmd file relevantend 
           {
/* printf( "** got here 1 **\n" ); */
             op_code(SPECIAL, RELEVANT_LOOP);
             p_code_jmp($1.for_label, JMP_UNC);
             p_code_label($1.next_label);
             op_code(POP, 0);
             op_code(POP, 0);
             op_code(POP, 0);
           }

         | payment_cmd file paymentend
		 
           {
             p_code_jmp($1.while_label, JMP_NZ);
           }

         | shippingcost_cmd file shippingcostend
		 
           {
             p_code_jmp($1.while_label, JMP_NZ);
           }

         | do_cmd file loop 
           { 
             p_code_jmp($1.do_label, JMP_UNC); 
             p_code_label($1.loop_label); 
           }
         | repeat_cmd file until bracket_open bool_expr bracket_close
           {
             p_code_jmp($1.while_label, JMP_Z);
           }
         | while_cmd file wend 
           { 
             p_code_jmp($1.while_label, JMP_UNC); 
             p_code_label($1.wend_label); 
           }
         | ESC ident HTMLliteral
           {
             long func_name;
             char buf[256];
             sprintf(buf, "base_%s", $2);
             func_name= new_text_entry(buf);
             p_code_pushi(0);
             p_code_pushs(func_name);
             p_code_call();
			 op_code(PRINT, 0);
             p_code_pushs(new_text_entry($3)); 
			 op_code(PRINT, 0);
		     html_parse = 1;
           }
         | ESC table_func HTMLliteral 
           { 
             long func_name;
             char buf[256];
             p_code_pushi(1);
             sprintf(buf, "table_%s", $2);
             func_name = new_text_entry(buf);
             p_code_pushs(func_name);
             p_code_call();
			 op_code(PRINT, 0);
             p_code_pushs(new_text_entry($3)); 
			 op_code(PRINT, 0);
		     html_parse = 1;
           }
;
/*-------------------------------------------------------------------------------*/
bracket_open: '[' {}
			| error {ErrorMessage("missing '['");}
;

bracket_close: ']' {}
			| error {ErrorMessage("missing ']'");}
;

dot: '.' {}
			| error {ErrorMessage("missing '.'");}
;

ident: IDENT {$$=$1;}
;
/*-------------------------------------------------------------------------------*/
endif: ENDIF {}
       | error {ErrorMessage("missing %%%%endif");}
;

if_cmd: IF bracket_open bool_expr bracket_close 
          {
            $$.else_label = p_code_new_label();
            $$.endif_label = p_code_new_label();
            p_code_jmp($$.else_label, JMP_Z);
            html_parse=1;
          }
;

if_else_cmd: if_cmd file ELSE 
               { 
                 $$=$1;
                 p_code_jmp($$.endif_label, JMP_UNC);
                 p_code_label($$.else_label);
                 html_parse=1;
               }
;
/*-------------------------------------------------------------------------------*/
next: NEXT {}
      | NEXT HTMLliteral
	    {
		  p_code_pushs(new_text_entry($2)); 
		  html_parse = 1;
		} 
      | error {ErrorMessage("missing %%next");}
;

for_cmd: for_help  expr ',' expr ',' expr ']' 
           { 
             $$=$1;
             op_code(SPECIAL, FOR_INIT);
             p_code_label($$.for_label);
             op_code(SPECIAL, FOR_TEST);
             p_code_jmp($$.next_label, JMP_Z);
             html_parse=1;
           }
         | for_help  expr ',' expr ']'
           {
             $$=$1;
             p_code_pushi(1);
             op_code(SPECIAL, FOR_INIT);
             p_code_label($$.for_label);
             op_code(SPECIAL, FOR_TEST);
             p_code_jmp($$.next_label, JMP_Z);
             html_parse=1;
           }
;

for_help: FOR 
           {
             $$.for_label = p_code_new_label();
             $$.next_label = p_code_new_label();
           }
;
/*-------------------------------------------------------------------------------*/
productend: PRODUCTEND {}
       | error {ErrorMessage("missing %%endproduct");}
;

product_cmd: product_help '[' str_expr ',' expr ']'
               {
                 $$=$1;
                 p_code_pushi(1);
                 op_code(SPECIAL, PRODUCT_INIT);
                 p_code_label($$.for_label);
                 op_code(SPECIAL, FOR_TEST);
                 p_code_jmp($$.next_label, JMP_Z);
                 html_parse=1;
               }
		    | product_help '[' expr ']'
		       {
				 $$=$1;
                 p_code_pushi(1);
				 p_code_pushi(1);
                 op_code(SPECIAL, PRODUCT_INIT2);
                 p_code_label($$.for_label);
                 op_code(SPECIAL, FOR_TEST);
                 p_code_jmp($$.next_label, JMP_Z);
                 html_parse=1;
		       }
;

product_help: PRODUCTLIST 
           {
             $$.for_label = p_code_new_label();
             $$.next_label = p_code_new_label();
           }
;
/*-------------------------------------------------------------------------------*/
catalogend: CATALOGEND {}
       | error {ErrorMessage("missing %%catalogend");}
;

catalog_cmd: catalog_help '[' expr ',' expr ']'
           {
             $$=$1;
             p_code_pushi(0);
             op_code(SPECIAL, CATALOG_INIT);
             p_code_label($$.for_label);
             op_code(SPECIAL, FOR_TEST);
             p_code_jmp($$.next_label, JMP_Z);
             html_parse=1;
           }
		 | catalog_help '[' expr ']'
		   {
             $$=$1;
             p_code_pushi(0);
			 p_code_pushi(0);
             op_code(SPECIAL, CATALOG_INIT);
             p_code_label($$.for_label);
             op_code(SPECIAL, FOR_TEST);
             p_code_jmp($$.next_label, JMP_Z);
             html_parse=1;
           }
 
         | catalog_help '[' ']'
           {
             $$=$1;
             p_code_pushi(0);
			 p_code_pushi(0);
			 p_code_pushi(0);
             op_code(SPECIAL, CATALOG_INIT);
             p_code_label($$.for_label);
             op_code(SPECIAL, FOR_TEST);
             p_code_jmp($$.next_label, JMP_Z);
             html_parse=1;
           }
;

catalog_help: CATALOGLIST 
           {
             $$.for_label = p_code_new_label();
             $$.next_label = p_code_new_label();
           }
;
/*-------------------------------------------------------------------------------*/
basketend: BASKETEND {}
       | error {ErrorMessage("missing %%basketend");}
;

basket_cmd: basket_help '[' expr ']'
           {
             $$=$1;
			 p_code_pushi(1);
			 p_code_pushi(1);
             op_code(SPECIAL, BASKET_INIT);
             p_code_label($$.for_label);
             op_code(SPECIAL, FOR_TEST);
             p_code_jmp($$.next_label, JMP_Z);
             html_parse=1;
           }
;

basket_help: BASKETLIST 
           {
             $$.for_label = p_code_new_label();
             $$.next_label = p_code_new_label();
           }
;
/*-------------------------------------------------------------------------------*/
promotionend: PROMOTIONEND {}
       | error {ErrorMessage("missing %%promotionend");}
;

promotion_cmd: promotion_help '[' expr ']'
			     {
				   $$=$1;
				   p_code_pushi(1);
			       p_code_pushi(1);
                   op_code(SPECIAL, PROMOTION_INIT);
                   p_code_label($$.for_label);
                   op_code(SPECIAL, FOR_TEST);
                   p_code_jmp($$.next_label, JMP_Z);
                   html_parse=1;
                 }
			   | promotion_help '[' expr ',' str_expr ']'
			     {
				   $$=$1;
			       p_code_pushi(1);
                   op_code(SPECIAL, PROMOTION_INIT);
                   p_code_label($$.for_label);
                   op_code(SPECIAL, FOR_TEST);
                   p_code_jmp($$.next_label, JMP_Z);
                   html_parse=1;
                 }

			   | promotion_help '[' expr ',' str_expr ',' str_expr ']'
			     {
				   $$=$1;
                   op_code(SPECIAL, PROMOTION_INIT);
                   p_code_label($$.for_label);
                   op_code(SPECIAL, FOR_TEST);
                   p_code_jmp($$.next_label, JMP_Z);
                   html_parse=1;
                 }
;

promotion_help: PROMOTIONLIST 
           {
             $$.for_label = p_code_new_label();
             $$.next_label = p_code_new_label();
           }
;
/*-------------------------------------------------------------------------------*/
relevantend: RELEVANTEND {}
       | error {ErrorMessage("missing %%relevantend");}
;

relevant_cmd: relevant_help '[' expr ']'
           {
/* printf( "** got here 2 %d **\n", $1 );*/
             $$=$1;
			 p_code_pushi(1);
			 p_code_pushi(2);
             op_code(SPECIAL, RELEVANT_INIT);
             p_code_label($$.for_label);
             op_code(SPECIAL, FOR_TEST);
             p_code_jmp($$.next_label, JMP_Z);
             html_parse=1;
           }
;

relevant_help: RELEVANTLIST 
           {
/* printf( "** got here 3 $$ **\n" );*/
             $$.for_label = p_code_new_label();
             $$.next_label = p_code_new_label();
/* printf( "** got here 3.5 **\n" );*/
           }
;
/*-------------------------------------------------------------------------------*/
payment_cmd: PAYMENTLIST 
  { 
    p_code_pushi(0);
    p_code_pushs(new_text_entry("base_paymentlist"));
    p_code_call();
    $$.while_label = p_code_new_label();
    p_code_label($$.while_label);
	html_parse=1;
  }
;

paymentend: PAYMENTEND 
             {
               p_code_pushi(0);
               p_code_pushs(new_text_entry("base_paymentend"));
               p_code_call();
			 }
            | error {ErrorMessage("missing %%paymentend");}
;
/*-------------------------------------------------------------------------------*/
shippingcost_cmd: SHIPPINGCOSTLIST 
  { 
    p_code_pushi(0);
    p_code_pushs(new_text_entry("base_shippingcostlist"));
    p_code_call();
    $$.while_label = p_code_new_label();
    p_code_label($$.while_label);
	html_parse=1;
  }
;

shippingcostend: SHIPPINGCOSTEND 
             {
               p_code_pushi(0);
               p_code_pushs(new_text_entry("base_shippingcostend"));
               p_code_call();
			 }
            | error {ErrorMessage("missing %%shippingcostend");}
;
/*-------------------------------------------------------------------------------*/
loop: LOOP {}
       | error {ErrorMessage("missing %%loop");}
;

do_cmd: DO 
          { 
            $$.do_label = p_code_new_label();
            $$.loop_label = p_code_new_label();
            p_code_label($$.do_label);
            html_parse=1;
          }
;
/*-------------------------------------------------------------------------------*/
repeat_cmd: REPEAT 
  { 
    $$.while_label = p_code_new_label();
    p_code_label($$.while_label);
	html_parse=1;
  }
;

until: UNTIL {}
       | error {ErrorMessage("missing %%until");}
;
/*-------------------------------------------------------------------------------*/
wend: WEND {}
      | error {ErrorMessage("missing %%wend");}
;

while_cmd: while_expr bracket_open bool_expr bracket_close 
             {
               $$=$1;
               p_code_jmp($1.wend_label, JMP_Z);
               html_parse=1;
             }
;

while_expr: WHILE 
              { 
                $$.while_label = p_code_new_label();
                $$.wend_label = p_code_new_label();
                p_code_label($$.while_label);
              }
;
/*-------------------------------------------------------------------------------*/
BEG_STR : '"' { string_parse = 1;}


str_expr: expr {}
           | BEG_STR STRING { p_code_pushs(new_text_entry($2)); }
;

/*-------------------------------------------------------------------------------*/
bool_expr:     str_expr '=' str_expr   {op_code(FUNC, OP_EQ); }
			 |  '(' str_expr '=' str_expr ')'  {op_code(FUNC, OP_EQ); }	
             | str_expr '<' '>' str_expr  {op_code(FUNC, OP_NE); }
			 | '(' str_expr '<' '>' str_expr ')' {op_code(FUNC, OP_NE); }
             | str_expr '<' '=' str_expr  {op_code(FUNC, OP_LE); }
			 | '(' str_expr '<' '=' str_expr ')' {op_code(FUNC, OP_LE); }
             | str_expr '<' str_expr   {op_code(FUNC, OP_LT); }
			 | '(' str_expr '<' str_expr ')'  {op_code(FUNC, OP_LT); }
             | str_expr '>' '=' str_expr  {op_code(FUNC, OP_GE); }
			 | '(' str_expr '>' '=' str_expr ')' {op_code(FUNC, OP_GE); }
             | str_expr '>' str_expr   {op_code(FUNC, OP_GT); }
			 | '(' str_expr '>' str_expr ')'  {op_code(FUNC, OP_GT); }
             | bool_expr AND bool_expr {op_code(FUNC, OP_AND); }
			 | '(' bool_expr AND bool_expr ')' {op_code(FUNC, OP_AND); }
             | bool_expr OR bool_expr {op_code(FUNC, OP_OR); }
			 | '(' bool_expr OR bool_expr ')' {op_code(FUNC, OP_OR); }
             | NOT bool_expr {op_code(FUNC, OP_NOT); }
			 | '(' NOT bool_expr ')' {op_code(FUNC, OP_NOT); }
             | str_expr 
;
/*-------------------------------------------------------------------------------*/
expr:   simple_expr {}
      | '(' str_expr ')' {}
      | '-' str_expr {op_code(FUNC, OP_NEG);}
      | str_expr '+' str_expr {op_code(FUNC, OP_PLUS);}
      | str_expr '-' str_expr {op_code(FUNC, OP_MINUS);}
      | str_expr '*' str_expr {op_code(FUNC, OP_TIMES);}
      | str_expr '/' str_expr {op_code(FUNC, OP_DIV);}
      | str_expr '%' str_expr {op_code(FUNC, OP_MOD);}
;
/*-------------------------------------------------------------------------------*/
simple_expr:   PRINTSTACK { op_code(SPECIAL, PRINT_STACK); }
             | FOR_VALUE { op_code(SPECIAL, FOR_VAL); }
             | PRODUCTVALUE { op_code(SPECIAL, FOR_VAL); }
             | CATALOGVALUE { op_code(SPECIAL, FOR_VAL); }
			 | BASKETVALUE { op_code(SPECIAL, FOR_VAL); }
			 | PROMOTIONVALUE { op_code(SPECIAL, FOR_VAL); }
			 | SETCOOKIE opt_arg
			   {
                 long func_name;
                 p_code_pushi($2.args);
                 func_name= new_text_entry("base_setcookie");
                 p_code_pushs(func_name);
                 p_code_call();
				 Magic = 1;
			   }
			 | REDIRECT opt_arg
			   {
                 long func_name;
                 p_code_pushi($2.args);
                 func_name= new_text_entry("base_redirect");
                 p_code_pushs(func_name);
                 p_code_call();
				 Magic = 1;
			   }
			 | MIMETYPE opt_arg
			   {
                 long func_name;
                 p_code_pushi($2.args);
                 func_name= new_text_entry("base_mimetype");
                 p_code_pushs(func_name);
                 p_code_call();
				 Magic = 1;
			   }

			 | SENDFILE opt_arg
			   {
                 long func_name;
                 p_code_pushi($2.args);
                 func_name= new_text_entry("base_sendfile");
                 p_code_pushs(func_name);
                 p_code_call();
				 Magic = 1;
			   }

             | ESC ident opt_arg
               {
                 long func_name;
                 char buf[256];
                 p_code_pushi($3.args);
                 sprintf(buf, "base_%s", $2);
                 func_name= new_text_entry(buf);
                 p_code_pushs(func_name);
                 p_code_call();
               }
             | ESC table_func opt_arg 
               { 
                 long func_name;
                 char buf[256];
                 p_code_pushi($3.args+1);
                 sprintf(buf, "table_%s", $2);
                 func_name = new_text_entry(buf);
                 p_code_pushs(func_name);
                 p_code_call();
               }
             | ESC bracket_open ident bracket_close dot bracket_open ident bracket_close 
			   { 
                 p_code_pushs(new_text_entry($3));
                 p_code_pushs(new_text_entry($7));
                 p_code_pushi(2);
                 p_code_pushs(new_text_entry("table_lookup"));
                 p_code_call();
               }
             | PRODUCT ident bracket_close 
			   { 
                 p_code_pushs(new_text_entry($2));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("product_lookup"));
                 p_code_call();
               }

             | PRODUCTNEXT bracket_open str_expr bracket_close
			   { 
                 p_code_pushs(new_text_entry($3));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("base_productnext"));
                 p_code_call();
               }

             | PRODUCTPREV bracket_open str_expr bracket_close
			   { 
                 p_code_pushs(new_text_entry($3));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("base_productprev"));
                 p_code_call();
               }

             | PRODUCTLINK
			   { 
				 p_code_pushi(0);
                 p_code_pushs(new_text_entry("base_productlink"));
                 p_code_call();
               }

             | CATALOG ident bracket_close 
			   { 
                 p_code_pushs(new_text_entry($2));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("catalog_lookup"));
                 p_code_call();
               }

             | CATALOGNEXT bracket_open str_expr bracket_close
			   { 
                 p_code_pushs(new_text_entry($3));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("base_catalognext"));
                 p_code_call();
               }

             | CATALOGPREV bracket_open str_expr bracket_close
			   { 
                 p_code_pushs(new_text_entry($3));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("base_catalogprev"));
                 p_code_call();
               }

             | CATALOGLINK
			   { 
				 p_code_pushi(0);
                 p_code_pushs(new_text_entry("base_cataloglink"));
                 p_code_call();
               }

             | BASKET ident bracket_close 
			   { 
                 p_code_pushs(new_text_entry($2));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("basket_lookup"));
                 p_code_call();
               }

             | BASKETNEXT bracket_open str_expr bracket_close 
			   { 
                 p_code_pushs(new_text_entry($3));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("base_catalognext"));
                 p_code_call();
               }

             | BASKETPREV bracket_open str_expr bracket_close
			   { 
                 p_code_pushs(new_text_entry($3));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("base_catalogprev"));
                 p_code_call();
               }

             | PROMOTION ident bracket_close 
			   { 
                 p_code_pushs(new_text_entry($2));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("promotion_lookup"));
                 p_code_call();
               }

             | RELEVANT ident bracket_close 
			   { 
                 p_code_pushs(new_text_entry($2));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("relevant_lookup"));
                 p_code_call();
               }

             | PAYMENT ident bracket_close 
			   { 
                 p_code_pushs(new_text_entry($2));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("payment_lookup"));
                 p_code_call();
               }

             | SHIPPINGCOST ident bracket_close 
			   { 
                 p_code_pushs(new_text_entry($2));
                 p_code_pushi(1);
                 p_code_pushs(new_text_entry("shippingcost_lookup"));
                 p_code_call();
               }

             | NUMBER { p_code_pushi(atoi($1)); }
             | FLOAT  { p_code_pushf(atof($1)); }
;
/*-------------------------------------------------------------------------------*/
table_func: bracket_open ident bracket_close dot ident 
              { 
                p_code_pushs(new_text_entry($2)); 
                $$=$5; 
              }
/*-------------------------------------------------------------------------------*/
opt_arg: beginn_args str_expr arg_list bracket_close {$$.args = $3.args+1;  skip_newline=0; }
/*
	     | literal 
		   { 
		     p_code_pushs(new_text_entry($1)); 
           }
*/
		 | /* empty */ {$$.args = 0;} 

;

beginn_args: '[' { skip_newline=1; }
;

arg_list: /* empty */ {$$.args = 0;}
          | ',' str_expr arg_list {$$.args=$3.args+1; }
;

%%

	







