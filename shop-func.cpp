#ifndef VSL_ENGINE

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
//#include "adabas.h"
#include "db.h"
#include "vsl-exec.h"
#include "vslanguage.h"
#include "shop-func.h"
#include "inifile.h"
#include "skey.h"

//#include "carp.h"
#include "priceutil.h"

#ifdef __cplusplus 
extern "C" {
#endif

t_Value base_price(t_Value *arg_list)
{
t_Value ret_val;
char SQLBuffer[200];
char* Ptr;
char Buffer1[200];
char Buffer2[200];
char Buffer3[200];
char Category[200];
CDBField* PRODUCTCATEGORIES_INDEX;

#if ! defined(VSHOP_BASIC) && ! defined(VSHOP_BASIC)
  CDBField* CUSTOMERS_TAXPROFILE;
  char Taxprofile[20];

  CDBField* CUSTOMERS_DISCOUNTPROFILE;
  char Discountprofile[20];
#endif

CDBField* DISCOUNT;
double Discount = 0;

CDBField* TAX;
double Tax = 0;

char Index[20];
char ID[20];

char* Value;
double Price;
int Start;
unsigned int Counter;

  if (arg_list[0].typ == Float_Value)
    {
      Price = arg_list[0].dValue;
      if (cgiFormString("CATEGORY",Category,200) == cgiFormNotFound)
        {
          RuntimeError1("%%%%price[PARAMETER] - CATEGORY not set");
          ret_val.typ = Not_Set;
        }

      if (cgiFormString("LOGIN",Buffer2,200) == cgiFormNotFound)
        {
          if (cgiFormString("ID",ID,20) != cgiFormNotFound)
            {
              if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
                {
                  strcpy(Buffer2,Value);
                }
              else
                strcpy(Buffer2,"ANONYM");
            }
          else
            strcpy(Buffer2,"ANONYM");
        }

          sprintf(SQLBuffer,"SELECT VSINDEX FROM PRODUCTCATEGORIES WHERE NAMEOFCATEGORY='%s'",Category);
          if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
            {
              if ((PRODUCTCATEGORIES_INDEX = EngineDB->GetField(MAX_SQLSELECT-2,"VSINDEX")) != NULL)
                {
                  strcpy(Index,PRODUCTCATEGORIES_INDEX->String);
  
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                    sprintf(SQLBuffer,"SELECT TAXPROFILE,DISCOUNTPROFILE FROM CUSTOMERS WHERE LOGIN='%s'",Buffer2);
                    if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
                      {
                        CUSTOMERS_TAXPROFILE = EngineDB->GetField(MAX_SQLSELECT-2,"TAXPROFILE");
                        if (CUSTOMERS_TAXPROFILE != NULL)
                          strcpy(Taxprofile,CUSTOMERS_TAXPROFILE->String);

                        CUSTOMERS_DISCOUNTPROFILE = EngineDB->GetField(MAX_SQLSELECT-2,"DISCOUNTPROFILE");
                        if (CUSTOMERS_DISCOUNTPROFILE != NULL)
                          strcpy(Discountprofile,CUSTOMERS_DISCOUNTPROFILE->String);

                        sprintf(SQLBuffer,"SELECT DISCOUNT FROM DISCOUNTS WHERE PROFILEID='%s' AND CATEGORYID='%s'",Discountprofile,Index);
                  #else
                        sprintf(SQLBuffer,"SELECT DISCOUNT FROM DISCOUNTS WHERE PROFILEID='0' AND CATEGORYID='%s'",Index);
                  #endif

                      if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)                      
                        {
                          DISCOUNT = EngineDB->GetField(MAX_SQLSELECT-2,"DISCOUNT");
                          Discount = DISCOUNT->Double;
                        }

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                      sprintf(SQLBuffer,"SELECT TAX FROM TAXES WHERE PROFILEID='%s' AND CATEGORYID='%s'",Taxprofile,Index);
                  #else
                      sprintf(SQLBuffer,"SELECT TAX FROM TAXES WHERE PROFILEID='0' AND CATEGORYID='%s'",Index);
                  #endif

                      if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)                      
                        {
                          TAX = EngineDB->GetField(MAX_SQLSELECT-2,"TAX");
                          Tax = TAX->Double;
                        }
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                    }
                  #endif
                }
            }

        if (Discount > 0)
          {
            Price = Price - ((Price / 100) * Discount);
          }

        if (Tax > 0)

          //
          // check price display
          //
          if ( priceDisplay() == PRICEDISP_GROSS )
            Price = Price + ( Price * ( Tax / 100 ) );

      sprintf(Buffer1,"%.2f",Price);

      if (DECIMALSYMBOL != '.')
        while((Ptr = strchr(Buffer1,'.')) != NULL)
          *Ptr=DECIMALSYMBOL;


  if ((Ptr = strchr(Buffer1,DECIMALSYMBOL)) != NULL)
    {
      strcpy(Buffer3,"");


      Start = strlen(Buffer1) -3;

      if (Start > 2)
        {
          Start = (Start % 3);

          Mid(Buffer2, &Buffer1[0], Start);
          strcat(Buffer3,Buffer2);

          if ((Start != 0) && strlen(Buffer1) > 6)
            strcat(Buffer3,DIGITSYMBOL_STRING);


          for(Counter = 3;Counter < (strlen(Buffer1)-5); Counter+=3)
            {
              Mid(Buffer2,&Buffer1[Start],3);
              Start += 3;
              strcat(Buffer3,Buffer2);

              strcat(Buffer3,DIGITSYMBOL_STRING);
            }
          strcat(Buffer3,&Buffer1[Start]);
          strcpy(Buffer1,Buffer3);

        }
      }
      ret_val.sValue = (char*) malloc(strlen(Buffer1)+1);
      strcpy(ret_val.sValue,Buffer1);
      ret_val.typ = String_Value;
    }
  else
    {
      RuntimeError1("%%%%price[PARAMETER] - PARAMETER must be FLOAT");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_evalprice(t_Value *arg_list)
{
t_Value ret_val;
char SQLBuffer[200];
char Buffer2[200];
char Category[200];
CDBField* PRODUCTCATEGORIES_INDEX;

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  CDBField* CUSTOMERS_TAXPROFILE;
  char Taxprofile[20];

  CDBField* CUSTOMERS_DISCOUNTPROFILE;
  char Discountprofile[20];
#endif

CDBField* DISCOUNT;
double Discount = 0;

CDBField* TAX;
double Tax = 0;

char Index[20];
char ID[20];

char* Value;
double Price;

  if (arg_list[0].typ == Float_Value)
    {
      Price = arg_list[0].dValue;
      if (cgiFormString("CATEGORY",Category,200) == cgiFormNotFound)
        {
          RuntimeError1("%%%%price[PARAMETER] - CATEGORY not set");
          ret_val.typ = Not_Set;
        }

      if (cgiFormString("LOGIN",Buffer2,200) == cgiFormNotFound)
        {
          if (cgiFormString("ID",ID,20) != cgiFormNotFound)
            {
              if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
                {
                  strcpy(Buffer2,Value);
                }
              else
                strcpy(Buffer2,"ANONYM");
            }
          else
            strcpy(Buffer2,"ANONYM");
        }

          sprintf(SQLBuffer,"SELECT VSINDEX FROM PRODUCTCATEGORIES WHERE NAMEOFCATEGORY='%s'",Category);
          if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
            {
              if ((PRODUCTCATEGORIES_INDEX = EngineDB->GetField(MAX_SQLSELECT-2,"VSINDEX")) != NULL)
                {
                  strcpy(Index,PRODUCTCATEGORIES_INDEX->String);
  
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                    sprintf(SQLBuffer,"SELECT TAXPROFILE,DISCOUNTPROFILE FROM CUSTOMERS WHERE LOGIN='%s'",Buffer2);
                    if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
                      {
                        CUSTOMERS_TAXPROFILE = EngineDB->GetField(MAX_SQLSELECT-2,"TAXPROFILE");
                        if (CUSTOMERS_TAXPROFILE != NULL)
                          strcpy(Taxprofile,CUSTOMERS_TAXPROFILE->String);

                        CUSTOMERS_DISCOUNTPROFILE = EngineDB->GetField(MAX_SQLSELECT-2,"DISCOUNTPROFILE");
                        if (CUSTOMERS_DISCOUNTPROFILE != NULL)
                          strcpy(Discountprofile,CUSTOMERS_DISCOUNTPROFILE->String);

                        sprintf(SQLBuffer,"SELECT DISCOUNT FROM DISCOUNTS WHERE PROFILEID='%s' AND CATEGORYID='%s'",Discountprofile,Index);
                  #else
                        sprintf(SQLBuffer,"SELECT DISCOUNT FROM DISCOUNTS WHERE PROFILEID='0' AND CATEGORYID='%s'",Index);
                  #endif

                      if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)                      
                        {
                          DISCOUNT = EngineDB->GetField(MAX_SQLSELECT-2,"DISCOUNT");
                          Discount = DISCOUNT->Double;
                        }

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                      sprintf(SQLBuffer,"SELECT TAX FROM TAXES WHERE PROFILEID='%s' AND CATEGORYID='%s'",Taxprofile,Index);
                  #else
                      sprintf(SQLBuffer,"SELECT TAX FROM TAXES WHERE PROFILEID='0' AND CATEGORYID='%s'",Index);
                  #endif

                      if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)                      
                        {
                          TAX = EngineDB->GetField(MAX_SQLSELECT-2,"TAX");
                          Tax = TAX->Double;
                        }
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                    }
                  #endif
                }
            }

        if (Discount > 0)
          {
            Price = Price - ((Price / 100) * Discount);
          }

        if (Tax > 0)

          //
          // check price display
          //
          if ( priceDisplay() == PRICEDISP_GROSS )
            Price = Price + ((Price / 100) * Tax);

      ret_val.dValue = Price;
      ret_val.typ = Float_Value;
    }
  else
    {
      RuntimeError1("%%%%evalprice[PARAMETER] - PARAMETER must be FLOAT");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_vat(t_Value *arg_list)
{
t_Value ret_val;
char SQLBuffer[200];
char PINDEX[20];
char Buffer2[200];
char ID[20];
CDBField* PRODUCTCATEGORIES_INDEX;

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  CDBField* CUSTOMERS_TAXPROFILE;
#endif

CDBField* TAX = NULL;
char* Value;

  if (arg_list[0].typ == String_Value )
    {
      if (cgiFormString("LOGIN",Buffer2,200) == cgiFormNotFound)
        {
          if (cgiFormString("ID",ID,20) != cgiFormNotFound)
            {
              if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
                {
                  strcpy(Buffer2,Value);
                }
              else
                strcpy(Buffer2,"ANONYM");
            }
          else
            strcpy(Buffer2,"ANONYM");
        }

          sprintf(SQLBuffer,"SELECT VSINDEX FROM PRODUCTCATEGORIES WHERE NAMEOFCATEGORY='%s'",arg_list[0].sValue);
          if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
            {
              if ((PRODUCTCATEGORIES_INDEX = EngineDB->GetField(MAX_SQLSELECT-2,"VSINDEX")) != NULL)
                {
                  strcpy(PINDEX,PRODUCTCATEGORIES_INDEX->String);
                  
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                    sprintf(SQLBuffer,"SELECT TAXPROFILE FROM CUSTOMERS WHERE LOGIN='%s'",Buffer2);
                    if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
                      {
                        CUSTOMERS_TAXPROFILE = EngineDB->GetField(MAX_SQLSELECT-2,"TAXPROFILE");
                        sprintf(SQLBuffer,"SELECT TAX FROM TAXES WHERE PROFILEID='%s' AND CATEGORYID='%s'",CUSTOMERS_TAXPROFILE->String,PINDEX);
                  #else
                        sprintf(SQLBuffer,"SELECT TAX FROM TAXES WHERE PROFILEID='0' AND CATEGORYID='%s'",PINDEX);
                  #endif    

                      if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)                      
                        TAX = EngineDB->GetField(MAX_SQLSELECT-2,"TAX");
                      else
                        TAX = NULL;
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                    }
                  #endif
                }
            }

      ret_val.typ = Float_Value;

      if (TAX != NULL)
        {
          ret_val.dValue = TAX->Double;
        }
      else
        {
          ret_val.dValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%vat[PARAMETER] - PARAMETER must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_discount(t_Value *arg_list)
{
t_Value ret_val;
char SQLBuffer[200];
char Buffer2[200];
char ID[20];
char PINDEX[20];
CDBField* PRODUCTCATEGORIES_INDEX;

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  CDBField* CUSTOMERS_DISCOUNTPROFILE;
#endif
CDBField* DISCOUNT = NULL;
char* Value;

  if (arg_list[0].typ == String_Value )
    {

      if (cgiFormString("LOGIN",Buffer2,200) == cgiFormNotFound)
        {
          if (cgiFormString("ID",ID,20) != cgiFormNotFound)
            {
              if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
                {
                  strcpy(Buffer2,Value);
                }
              else
                strcpy(Buffer2,"ANONYM");
            }
          else
            strcpy(Buffer2,"ANONYM");
        }

          sprintf(SQLBuffer,"SELECT VSINDEX FROM PRODUCTCATEGORIES WHERE NAMEOFCATEGORY='%s'",arg_list[0].sValue);

          if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
            {
              if ((PRODUCTCATEGORIES_INDEX = EngineDB->GetField(MAX_SQLSELECT-2,"VSINDEX")) != NULL)
                {
                  strcpy(PINDEX,PRODUCTCATEGORIES_INDEX->String);

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                    sprintf(SQLBuffer,"SELECT DISCOUNTPROFILE FROM CUSTOMERS WHERE LOGIN='%s'",Buffer2);

                    if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
                      {
                        CUSTOMERS_DISCOUNTPROFILE = EngineDB->GetField(MAX_SQLSELECT-2,"DISCOUNTPROFILE");
                        sprintf(SQLBuffer,"SELECT DISCOUNT FROM DISCOUNTS WHERE PROFILEID='%s' AND CATEGORYID='%s'",CUSTOMERS_DISCOUNTPROFILE->String,PINDEX);
                  #else
                        sprintf(SQLBuffer,"SELECT DISCOUNT FROM DISCOUNTS WHERE PROFILEID='0' AND CATEGORYID='%s'",PINDEX);
                  #endif

                        if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)                      
                          DISCOUNT = EngineDB->GetField(MAX_SQLSELECT-2,"DISCOUNT");
                        else
                          DISCOUNT = NULL;

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                    }
                  #endif

                }
            }

      ret_val.typ = Float_Value;

      if (DISCOUNT != NULL)
        {
          ret_val.dValue = DISCOUNT->Double;
        }
      else
        {
          ret_val.dValue = 9;
        }

    }
  else
    {
      RuntimeError1("%%%%discount[PARAMETER] - PARAMETER must be STRING");
      ret_val.typ = Not_Set;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_currency(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[20];

  if (arg_list[0].typ != Not_Set)
    strcpy(Buffer,CURRENCYNAME);
  else
    strcpy(Buffer,CURRENCYSYMBOL);

  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
  strcpy(ret_val.sValue,Buffer);
  ret_val.typ = String_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_language(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[10];

  strcpy(Buffer,"D");

  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
  strcpy(ret_val.sValue,Buffer);
  ret_val.typ = String_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_productnext(t_Value *arg_list)
{
t_Value ret_val;
char VSPI[20];
char VSSL[20];
char ID[20];
char HTML[150];
char Category[150];
char SQLBuffer[200];
char Buffer[200];
CDBField* TEMPLATE;


  ret_val.typ = Bool_Value;


  if (EngineDB->IsEOF(MAX_SQLSELECT-1) == 0)
    {
      cgiFormString("VSPI",VSPI,20);
      cgiFormString("VSSL",VSSL,20);
      cgiFormString("ID",ID,20);
      cgiFormString("CATEGORY",Category,150);

      if (strcmp(VSPI,"") == 0)
        {
          ret_val.nValue = 0;
          return ret_val;
        }

      ret_val.nValue = 1;
/*
      strcpy(Buffer,"");
      sprintf(SQLBuffer,"SELECT TEMPLATE FROM %s WHERE VSSI=%s",Category,VSPI);

      if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
        {
          TEMPLATE = EngineDB->GetField(MAX_SQLSELECT-2,"TEMPLATE");
          if (TEMPLATE != NULL)
            strcpy(Buffer,TEMPLATE->String);

          sprintf(SQLBuffer,"SELECT PATH FROM HTML_TEMPLATES WHERE VSINDEX='%s'",Buffer);
          if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
            {
              TEMPLATE = EngineDB->GetField(MAX_SQLSELECT-2,"PATH");
              if (TEMPLATE != NULL)
                strcpy(Buffer,TEMPLATE->String);
            }
         }
          if (strlen(Buffer) == 0)
            cgiFormString("HTML",HTML,100);
          else	
            strcpy(HTML,Buffer);
*/      
      cgiFormString("HTML",HTML,100);

      if (arg_list[0].typ == String_Value)
        fprintf(cgiOut,"<A HREF=\"%s?HTML=%s&VSSI=%i&ID=%s&CATEGORY=%s\" >%s</A>",CGIBIN,HTML,atoi(VSPI),ID,Category,arg_list[0].sValue);
      else
        fprintf(cgiOut,"%s?HTML=%s&VSSI=%i&ID=%s&CATEGORY=%s",CGIBIN,HTML,atoi(VSPI),ID,Category);
    }
  else
    ret_val.nValue = 0;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_productprev(t_Value *arg_list)
{
t_Value ret_val;
char VSSI[20];
char ID[20];
char VSSL[20];
char HTML[150];
char Category[150];
char Buffer1[20];
char SQLBuffer[200];
char Buffer[200];
CDBField* TEMPLATE;


  ret_val.typ = Bool_Value;

  cgiFormString("VSSI",VSSI,20);
  cgiFormString("ID",ID,20);
  cgiFormString("CATEGORY",Category,150);

  if (strcmp(VSSI,"") == 0)
    {
      ret_val.nValue = 0;
      return ret_val;
    }

  if (cgiFormString("VSSL",VSSL,20) == cgiFormNotFound)
    strcpy(VSSL,"1");

  if ((atoi(VSSI)-atoi(VSSL)) > 0)
    {
      ret_val.nValue = 1;
/*
      sprintf(SQLBuffer,"SELECT TEMPLATE FROM %s WHERE VSSI=%i",Category,atoi(VSSI)-atoi(VSSL));
      strcpy(Buffer,"");
      if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
        {
          TEMPLATE = EngineDB->GetField(MAX_SQLSELECT-2,"TEMPLATE");
          if (TEMPLATE != NULL)
            strcpy(Buffer,TEMPLATE->String);

          sprintf(SQLBuffer,"SELECT PATH FROM HTML_TEMPLATES WHERE VSINDEX='%s'",Buffer);
          if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
            {
              TEMPLATE = EngineDB->GetField(MAX_SQLSELECT-2,"PATH");
              if (TEMPLATE != NULL)
                strcpy(Buffer,TEMPLATE->String);
            }
	}    

      if (strlen(Buffer) == 0)
        cgiFormString("HTML",HTML,100);
      else	
        strcpy(HTML,Buffer);
*/

      cgiFormString("HTML",HTML,100);
      
      if (arg_list[0].typ == String_Value)
        fprintf(cgiOut,"<A HREF=\"%s?HTML=%s&VSSI=%i&ID=%s&CATEGORY=%s\">%s</A>",CGIBIN,HTML,atoi(VSSI)-atoi(VSSL),ID,Category,arg_list[0].sValue);
      else
        fprintf(cgiOut,"%s?HTML=%s&VSSI=%i&ID=%s&CATEGORY=%s",CGIBIN,HTML,atoi(VSSI)-atoi(VSSL),ID,Category);
    }
  else
    ret_val.nValue = 0;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_productlink(t_Value *arg_list)
{
t_Value ret_val;
char SQLBuffer[200];
char Buffer[200];
char Buffer1[20];
char Buffer2[200];
char ID[20];
char VSPI[20];
char HTML[101];
char VSPINDEX[20];
CDBField* TEMPLATE;

  cgiFormString("VSPINDEX",VSPINDEX,20);

      if (cgiFormString("CATEGORY",Buffer1,20) != cgiFormNotFound)
        {
          sprintf(SQLBuffer,"SELECT TEMPLATE FROM %s WHERE VSINDEX='%s'",Buffer1,VSPINDEX);

          if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
            {
              TEMPLATE = EngineDB->GetField(MAX_SQLSELECT-2,"TEMPLATE");
              if (TEMPLATE != NULL)
                strcpy(Buffer,TEMPLATE->String);
              else
                strcpy(Buffer,"");

              sprintf(SQLBuffer,"SELECT PATH FROM HTML_TEMPLATES WHERE VSINDEX='%s'",Buffer);
              if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
                {
                  TEMPLATE = EngineDB->GetField(MAX_SQLSELECT-2,"PATH");

                  if (TEMPLATE != NULL)
                    strcpy(Buffer,TEMPLATE->String);
                  else
                   {
                      strcpy(Buffer,"");
                    }
                }
              else
                strcpy(Buffer,"");

          if (strlen(Buffer) == 0)
            {
              cgiFormString("HTML",HTML,100);
              strcpy(Buffer,HTML);
            }

          cgiFormString("ID",ID,20);
          cgiFormString("VSPI",VSPI,20);


          sprintf(Buffer2,"%s?HTML=%s&ID=%s&CATEGORY=%s&VSINDEX=%s&VSSI=%s",CGIBIN,Buffer,ID,Buffer1,VSPINDEX,VSPI);

       }

      ret_val.typ = String_Value;
      ret_val.sValue = (char*) malloc(strlen(Buffer2)+1);
      strcpy(ret_val.sValue,Buffer2);
      }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_catalognext(t_Value *arg_list)
{
t_Value ret_val;
char VSCI[20];
char VSCL[20];
char ID[20];
char HTML[150];
char VSCP[21];

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      if (EngineDB->IsEOF(MAX_SQLSELECT-1) == 0)
        {
          ret_val.nValue = 1;

          cgiFormString("HTML",HTML,150);
          cgiFormString("VSCI",VSCI,20);
          cgiFormString("VSCL",VSCL,20);


          cgiFormString("ID",ID,20);
          cgiFormString("VSCP",VSCP,150);


          fprintf(cgiOut,"<A HREF=\"%s?HTML=%s&VSCI=%i&ID=%s&VSCP=%s\">%s</A>",CGIBIN,HTML,atoi(VSCI)+atoi(VSCL),ID,VSCP,arg_list[0].sValue);
        }
      else
        {
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%listnext[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_catalogprev(t_Value *arg_list)
{
t_Value ret_val;
char VSCI[20];
char ID[20];
char VSCL[20];
char HTML[150];
char VSCP[150];

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      cgiFormString("VSCL",VSCL,20);
      cgiFormString("VSCI",VSCI,20);

      if ((atoi(VSCI)-atoi(VSCL)) >= 0)
        {
          cgiFormString("VSCI",VSCI,20);
          cgiFormString("HTML",HTML,150);

          cgiFormString("ID",ID,20);
          cgiFormString("VSCP",VSCP,150);

          fprintf(cgiOut,"<A HREF=\"%s?HTML=%s&VSCI=%i&ID=%s&VSCP=%s\">%s</A>",CGIBIN,HTML,atoi(VSCI)-atoi(VSCL),ID,VSCP,arg_list[0].sValue);
          ret_val.nValue = 1;
        }
      else
        {
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%catalogprev[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_cataloglink(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Not_Set;

char SQLBuffer[200];
char Buffer[200];
char Buffer1[50];
char Buffer2[200];
char ID[20];
char HTML[101];
char VSCINDEX[20];
CDBField* TEMPLATE;
CDBField* CATEGORY;


   strcpy(Buffer2,"");

   cgiFormString("VSCINDEX",VSCINDEX,20);


          sprintf(SQLBuffer,"SELECT TEMPLATE,NAMEOFCATEGORY FROM PRODUCTCATEGORIES WHERE VSINDEX='%s'",VSCINDEX);
          
          
          if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
            {
              TEMPLATE = EngineDB->GetField(MAX_SQLSELECT-2,"TEMPLATE");
              if (TEMPLATE != NULL)
                strcpy(Buffer,TEMPLATE->String);
              else
                strcpy(Buffer,"");

              CATEGORY = EngineDB->GetField(MAX_SQLSELECT-2,"NAMEOFCATEGORY");
              if (CATEGORY != NULL)
                strcpy(Buffer1,CATEGORY->String);
              else
                strcpy(Buffer1,"");

              sprintf(SQLBuffer,"SELECT PATH FROM HTML_TEMPLATES WHERE VSINDEX='%s'",Buffer);
              if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
                {
                  TEMPLATE = EngineDB->GetField(MAX_SQLSELECT-2,"PATH");

                  if (TEMPLATE != NULL)
                    strcpy(Buffer,TEMPLATE->String);
                  else
                    {
                      strcpy(Buffer,"");
                    }

                }
              else
                strcpy(Buffer,"");

              if (strlen(Buffer) == 0)
                {
                  cgiFormString("HTML",HTML,100);
                  strcpy(Buffer,HTML);
                }


              cgiFormString("ID",ID,20);

              sprintf(Buffer2,"%s?HTML=%s&ID=%s&CATEGORY=%s&VSCP=%s",CGIBIN,String2HREF(Buffer),ID,String2HREF(Buffer1),VSCINDEX);
            }

      ret_val.typ = String_Value;
      ret_val.sValue = (char*) malloc(strlen(Buffer2)+1);
      strcpy(ret_val.sValue,Buffer2);
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_basketnext(t_Value *arg_list)
{
t_Value ret_val;
char VSBI[20];
char VSBL[20];
char ID[20];
char HTML[150];

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      if (EngineDB->IsEOF(MAX_SQLSELECT-1) == 0)
        {
          ret_val.nValue = 1;

          cgiFormString("HTML",HTML,150);
          cgiFormString("VSBI",VSBI,20);
          cgiFormString("VSBL",VSBL,20);

          cgiFormString("ID",ID,20);

          if (arg_list[0].typ == String_Value)
            fprintf(cgiOut,"<A HREF=\"%s?HTML=%s&VSBI=%i&ID=%s\">%s</A>",CGIBIN,HTML,atoi(VSBI)+atoi(VSBL),ID,arg_list[0].sValue);
          else
            fprintf(cgiOut,"%s?HTML=%s&VSBI=%i&ID=%s",CGIBIN,HTML,atoi(VSBI)+atoi(VSBL),ID);
        }
      else
        {
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%basketnext[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_basketprev(t_Value *arg_list)
{
t_Value ret_val;
char VSBI[20];
char ID[20];
char VSBL[20];
char HTML[150];

  ret_val.typ = Bool_Value;

  if (arg_list[0].typ == String_Value)
    {
      cgiFormString("VSBL",VSBL,20);
      cgiFormString("VSBI",VSBI,20);

      if ((atoi(VSBI)-atoi(VSBL)) >= 0)
        {
          cgiFormString("VSBI",VSBI,20);
          cgiFormString("HTML",HTML,150);


          cgiFormString("ID",ID,20);

          if (arg_list[0].typ == String_Value)
            fprintf(cgiOut,"<A HREF=\"%s?HTML=%s&VSBI=%i&ID=%s\">%s</A>",CGIBIN,HTML,atoi(VSBI)-atoi(VSBL),ID,arg_list[0].sValue);
          else
            fprintf(cgiOut,"%s?HTML=%s&VSBI=%i&ID=%s",CGIBIN,HTML,atoi(VSBI)-atoi(VSBL),ID);

          ret_val.nValue = 1;
        }
      else
        {
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%listprev[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_tobasket(t_Value *arg_list)
{
t_Value ret_val;
char SQLBuffer[200];
char ID[20];
char VSPINDEX[20];
char VSCINDEX[20];
char Link[100];
char Category[20];

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  CDBField* BLOCKONMINIMUM;
  CDBField* MINIMUM;
  CDBField* ONSTOCK;
#endif

CDBField* INDEX;

  ret_val.typ = Bool_Value;

  cgiFormString("CATEGORY",Category,20);

  if (arg_list[1].typ != String_Value)
    {
      strcpy(Link,arg_list[0].sValue);
      cgiFormString("VSINDEX",VSPINDEX,20);
    }
  else
    {
      strcpy(Link,arg_list[0].sValue);
      strcpy(VSPINDEX,arg_list[1].sValue);
    }

  sprintf(SQLBuffer,"SELECT VSINDEX FROM PRODUCTCATEGORIES WHERE NAMEOFCATEGORY='%s'",Category);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) == 0)
    {
      ret_val.nValue = 0;
      return ret_val;
    }

  INDEX = EngineDB->GetField(MAX_SQLSELECT-2,"VSINDEX");
  if (INDEX == NULL)
    {
      ret_val.nValue = 0;
      return ret_val;
    }

  strcpy(VSCINDEX,INDEX->String);


#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  
  sprintf(SQLBuffer,"SELECT BLOCKONMINIMUM,MINIMUM,ONSTOCK FROM INVENTORYCONTROL WHERE CATEGORY='%s' AND VSINDEX='%s'",VSCINDEX,VSPINDEX);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
    {
      BLOCKONMINIMUM = EngineDB->GetField(MAX_SQLSELECT-2,"BLOCKONMINIMUM");
      if (BLOCKONMINIMUM != NULL) 
        {
          if (BLOCKONMINIMUM->Long == 1)
            {
              MINIMUM = EngineDB->GetField(MAX_SQLSELECT-2,"MINIMUM");
              ONSTOCK = EngineDB->GetField(MAX_SQLSELECT-2,"ONSTOCK");

              if (ONSTOCK->Long <= MINIMUM->Long)
                {
                  ret_val.nValue = 0;
                  return ret_val;
                }
            }
        }
    }
#endif


  cgiFormString("ID",ID,20);

  fprintf(cgiOut,"<A HREF=\"%s?HTML=basket/2basket.htm&ID=%s&CATEGORY=%s&INDEX=%s\">%s</A>",CGIBIN,ID,Category,VSPINDEX,Link);

  ret_val.nValue = 1;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_tobasketwindow(t_Value *arg_list)
{
t_Value ret_val;
char SQLBuffer[200];
char ID[20];
char VSPINDEX[20];
char VSCINDEX[20];
char Target[100];
char Link[100];
char Category[20];
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  CDBField* BLOCKONMINIMUM;
  CDBField* MINIMUM;
  CDBField* ONSTOCK;
#endif
CDBField* INDEX;

  ret_val.typ = Bool_Value;

  cgiFormString("CATEGORY",Category,20);

  if (arg_list[0].typ != String_Value)
    {
      RuntimeError1("%%%%tobasketwindow[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.nValue = 0;
      return ret_val;
    }
  else
    strcpy(Target,arg_list[0].sValue);

  if (arg_list[2].typ != String_Value)
    {
      strcpy(Link,arg_list[1].sValue);
      cgiFormString("VSINDEX",VSPINDEX,20);
    }
  else
    {
      strcpy(Link,arg_list[1].sValue);
      strcpy(VSPINDEX,arg_list[2].sValue);
    }

  sprintf(SQLBuffer,"SELECT VSINDEX FROM PRODUCTCATEGORIES WHERE NAMEOFCATEGORY='%s'",Category);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) == 0)
    {
      ret_val.nValue = 0;
      return ret_val;
    }

  INDEX = EngineDB->GetField(MAX_SQLSELECT-2,"VSINDEX");
  if (INDEX == NULL)
    {
      ret_val.nValue = 0;
      return ret_val;
    }

  strcpy(VSCINDEX,INDEX->String);

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  sprintf(SQLBuffer,"SELECT BLOCKONMINIMUM,MINIMUM,ONSTOCK FROM INVENTORYCONTROL WHERE CATEGORY='%s' AND VSINDEX='%s'",VSCINDEX,VSPINDEX);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQLBuffer) != 0)
    {
      BLOCKONMINIMUM = EngineDB->GetField(MAX_SQLSELECT-2,"BLOCKONMINIMUM");
      if (BLOCKONMINIMUM != NULL) 
        {
          if (BLOCKONMINIMUM->Long == 1)
            {
              MINIMUM = EngineDB->GetField(MAX_SQLSELECT-2,"MINIMUM");
              ONSTOCK = EngineDB->GetField(MAX_SQLSELECT-2,"ONSTOCK");

              if (ONSTOCK->Long <= MINIMUM->Long)
                {
                  ret_val.nValue = 0;
                  return ret_val;
                }
            }
        }
    }
#endif

  cgiFormString("ID",ID,20);

  fprintf(cgiOut,"<A HREF=\"%s?HTML=basket/2basket.htm&ID=%s&CATEGORY=%s&INDEX=%s\" TARGET=%s>%s</A>",CGIBIN,ID,Category,VSPINDEX,Target,Link);

  ret_val.nValue = 1;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
double GetCustomDisc(char* CategoryID,char* Login)
{
char SQLBuffer[300];
CDBField* DISCOUNT;

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  CDBField* CUSTOMERS_DISCOUNTPROFILE;
#endif

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)


    sprintf(SQLBuffer,"SELECT DISCOUNTPROFILE FROM CUSTOMERS WHERE LOGIN='%s'",Login);
    if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQLBuffer) != 0)
      {
        CUSTOMERS_DISCOUNTPROFILE = EngineDB->GetField(MAX_SQLSELECT-1,"DISCOUNTPROFILE");
        if (CUSTOMERS_DISCOUNTPROFILE != NULL)
          {
            sprintf(SQLBuffer,"SELECT DISCOUNT FROM DISCOUNTS WHERE PROFILEID='%s' AND CATEGORYID='%s'",CUSTOMERS_DISCOUNTPROFILE->String,CategoryID);
  #else
            sprintf(SQLBuffer,"SELECT DISCOUNT FROM DISCOUNTS WHERE PROFILEID='0' AND CATEGORYID='%s'",CategoryID);
  #endif

          if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQLBuffer) != 0)                      
            {
              if ((DISCOUNT = EngineDB->GetField(MAX_SQLSELECT-1,"DISCOUNT")) != NULL)
                return DISCOUNT->Double;
            }
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
          }
      }
   #endif


  return 0;
}
/*--------------------------------------------------------------------------------*/
double GetCustomVat(char* CategoryID, char* Login)
{
char SQLBuffer[300];
CDBField* TAX;

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  CDBField* CUSTOMERS_TAXPROFILE;
#endif


#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)

    sprintf(SQLBuffer,"SELECT TAXPROFILE FROM CUSTOMERS WHERE LOGIN='%s'",Login);


    if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQLBuffer) != 0)
      {
        CUSTOMERS_TAXPROFILE = EngineDB->GetField(MAX_SQLSELECT-1,"TAXPROFILE");
        if (CUSTOMERS_TAXPROFILE != NULL)
          {
            sprintf(SQLBuffer,"SELECT TAX FROM TAXES WHERE PROFILEID='%s' AND CATEGORYID='%s'",CUSTOMERS_TAXPROFILE->String,CategoryID);
  #else
            sprintf(SQLBuffer,"SELECT TAX FROM TAXES WHERE PROFILEID='0' AND CATEGORYID='%s'",CategoryID);
  #endif

            if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQLBuffer) != 0)                      
              {
               TAX = EngineDB->GetField(MAX_SQLSELECT-1,"TAX");

                if (TAX != NULL)
                  return TAX->Double;
              }
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
          }
      }
  #endif

  return 0;
}
/*--------------------------------------------------------------------------------*/
t_Value base_basketinsert(t_Value *arg_list)
{
t_Value ret_val;
char SQL[401];
char ID[21];
CDBField* CATEGORYID;
CDBField* PRICE;
char CategoryID[21];
char Category[21];
double CustomVat =0;
double Price =0;
char NewID[21];
char Criteria1[21];
char Criteria2[101];
  CDBField* ONSTOCK;
  CDBField* QUANTITY;
  long NewOnStock;
  CDBField* BLOCKONMINIMUM;
  CDBField* MINIMUM;
char* Value = NULL;
char Buffer[51];
double CustomDisc =0;
char BasketCurrency[20];
int Ret;
char* Ptr;

long OnStock;
long BlockOnMinimum;
long Minimum;

  ret_val.typ = Bool_Value;
  Ret = 0;

  if (arg_list[0].typ == String_Value)
    {
      if (GetVar("BASKETCURRENCY", BasketCurrency) == 0)
        strcpy(BasketCurrency,"PRICE");

      if ((cgiFormString("ID",ID,20) != cgiFormNotFound) && (cgiFormString("CATEGORY",Category,20) != cgiFormNotFound))
        {
          sprintf(SQL,"DELETE FROM BASKETITEMS WHERE CATEGORYID<'100' AND BASKETID='%s'",ID);
          EngineDB->SQLUpdate(SQL);

          sprintf(SQL,"SELECT VSINDEX from PRODUCTCATEGORIES where NAMEOFCATEGORY='%s'",Category);
          if ((Ret = EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL)) != 0)
            {
              CATEGORYID = EngineDB->GetField(MAX_SQLSELECT-2,"VSINDEX");
              strcpy(CategoryID,CATEGORYID->String);

              sprintf(SQL,"SELECT %s FROM %s WHERE VSINDEX='%s'",BasketCurrency,Category,arg_list[0].sValue);

              if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) == 0)
                {
                  ret_val.nValue = 0;
                  return ret_val;
                }
              
              PRICE = EngineDB->GetField(MAX_SQLSELECT-2,BasketCurrency);
              Price = PRICE->Double;

              cgiFormString("CRITERIA1",Criteria1,20);
              cgiFormString("CRITERIA2",Criteria2,100);


              OnStock = 0;
              BlockOnMinimum = 0;
              Minimum = 0;
#if ! defined(VSHOP_BASIC) && ! defined(VSHOP_BASIC)
                  sprintf(SQL,"select ONSTOCK,MINIMUM,BLOCKONMINIMUM from INVENTORYCONTROL where CATEGORY='%s' AND PRODUCTINDEX='%s'",
                          CategoryID,arg_list[0].sValue);

                  if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
                    {

                      ONSTOCK = EngineDB->GetField(MAX_SQLSELECT-2,"ONSTOCK");
                      OnStock = ONSTOCK->Long;
                      BLOCKONMINIMUM  = EngineDB->GetField(MAX_SQLSELECT-2,"BLOCKONMINIMUM");
                      BlockOnMinimum = BLOCKONMINIMUM->Long;

                      MINIMUM   = EngineDB->GetField(MAX_SQLSELECT-2,"MINIMUM");
                      Minimum = MINIMUM->Long;
                    }
              #endif

             sprintf(SQL,"SELECT QUANTITY from BASKETITEMS where BASKETID='%s' AND CATEGORYID='%s' AND PRODUCTID='%s' AND CRITERIA1='%s' AND CRITERIA2='%s'",ID,CategoryID,arg_list[0].sValue,Criteria1,Criteria2);
             if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
               {
	       
                 QUANTITY  = EngineDB->GetField(MAX_SQLSELECT-2,"QUANTITY");
                 if ((Ptr = DBGetConfig("CUMULATIVE")) != NULL)
                    {
                      if (strcmp(Ptr,"TRUE") == 0)
                        {
                          if ((BlockOnMinimum == 1) && ((OnStock - (QUANTITY->Long+1)) <= (Minimum)))
                            {
                              ret_val.nValue = 0;
                              return ret_val;
                            }

                          sprintf(SQL,"UPDATE BASKETITEMS SET QUANTITY=QUANTITY+1 where BASKETID='%s' AND CATEGORYID='%s' AND PRODUCTID='%s' AND CRITERIA1='%s' AND CRITERIA2='%s'",ID,CategoryID,arg_list[0].sValue,Criteria1,Criteria2);
                          EngineDB->SQLUpdate(SQL);

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                            NewOnStock = OnStock - 1;
                            sprintf(SQL,"update INVENTORYCONTROL set ONSTOCK=%ld where CATEGORY='%s' AND PRODUCTINDEX='%s'",NewOnStock,CategoryID,arg_list[0].sValue);
                            EngineDB->SQLUpdate(SQL);
                          #endif

                          ret_val.nValue = 1;
                          return ret_val;
                        }
                    }
                  ret_val.nValue = 0;
                  return ret_val;
                }
		
              if ((BlockOnMinimum == 1) && ((OnStock - (1)) < (Minimum)))

                {
                  ret_val.nValue = 0;
                  return ret_val;
                }

              if (cgiFormString("LOGIN",Buffer,200) == cgiFormNotFound)
                {
                  if (cgiFormString("ID",ID,20) != cgiFormNotFound)
                    {
                      if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
                        {
                          strcpy(Buffer,Value);
                        }
                      else
                        strcpy(Buffer,"ANONYM");
                    }
                  else
                    strcpy(Buffer,"ANONYM");
                }


              CustomVat = GetCustomVat(CategoryID,Value);
              CustomDisc = GetCustomDisc(CategoryID,Value);


              if ((Ret = EngineDB->SQLInsert("BASKETITEMS",NewID)) != 0)
                {
                  InsertEntry("BASKETITEMINDEX",NewID);
                    sprintf(SQL,"update BASKETITEMS set BASKETID='%s',CATEGORYID='%s',PRODUCTID='%s',CUSTOMDISC=%f,CUSTOMVAT=%f,QUANTITY=1, CRITERIA1='%s',CRITERIA2='%s',PRICE=%f WHERE VSINDEX='%s'",
                            ID,CategoryID,arg_list[0].sValue,CustomDisc,CustomVat,Criteria1,Criteria2,Price,NewID);

                  if ((Ret = EngineDB->SQLUpdate(SQL)) != 0)
                    {
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                        sprintf(SQL,"select ONSTOCK from INVENTORYCONTROL where CATEGORY='%s' AND PRODUCTINDEX='%s'",
                                CategoryID,arg_list[0].sValue);

                        if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
                          {
                            sprintf(SQL,"select ONSTOCK from INVENTORYCONTROL where CATEGORY='%s' AND PRODUCTINDEX='%s'",CategoryID,arg_list[0].sValue);
                 
                            ONSTOCK = EngineDB->GetField(MAX_SQLSELECT-2,"ONSTOCK");

                            NewOnStock = ONSTOCK->Long - 1;

                            sprintf(SQL,"update INVENTORYCONTROL set ONSTOCK=%ld where CATEGORY='%s' AND PRODUCTINDEX='%s'",NewOnStock,CategoryID,arg_list[0].sValue);
                            EngineDB->SQLUpdate(SQL);
                          }
                        #endif
                      }
                }
            }
        }
    }
  else
    {
      RuntimeError1("%%%%basketinsert[PARAMETER] - PARAMETER must be STRING");

      ret_val.nValue = Ret;
   }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
t_Value base_available(t_Value *arg_list)
{
t_Value ret_val;
char SQL[401];
CDBField* ONSTOCK;
CDBField* MINIMUM;

char NameOfCategory[41];
char ProductID[41];

  ret_val.nValue = 0;
  ret_val.typ = Int_Value;


  if (arg_list[0].typ == String_Value)
    {
      if (strcmp(arg_list[0].sValue,"") == 0)
        cgiFormString("CATEGORY",NameOfCategory,40);
      else
        strcpy(NameOfCategory,arg_list[0].sValue);
    }
  else
    cgiFormString("CATEGORY",NameOfCategory,40);


  if (arg_list[1].typ == String_Value)
    {
      if (strcmp(arg_list[1].sValue,"") == 0)
        cgiFormString("VSPINDEX",ProductID,40);
      else
        strcpy(ProductID,arg_list[1].sValue);
    }
  else
    cgiFormString("VSPINDEX",ProductID,40);

   sprintf(SQL,"select INVENTORYCONTROL.ONSTOCK,INVENTORYCONTROL.MINIMUM,PRODUCTCATEGORIES.VSINDEX,PRODUCTCATEGORIES.NAMEOFCATEGORY from INVENTORYCONTROL,PRODUCTCATEGORIES where PRODUCTCATEGORIES.VSINDEX=INVENTORYCONTROL.CATEGORY AND PRODUCTCATEGORIES.NAMEOFCATEGORY='%s' AND INVENTORYCONTROL.PRODUCTINDEX='%s'",
            NameOfCategory,ProductID);

  if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
    {
      ONSTOCK = EngineDB->GetField(MAX_SQLSELECT-2,"ONSTOCK");
      MINIMUM   = EngineDB->GetField(MAX_SQLSELECT-2,"MINIMUM");

      ret_val.nValue = ONSTOCK->Long - MINIMUM->Long;
    }
  else
    {
      ret_val.nValue = -1;
    }

  return ret_val;
}
#endif
/*--------------------------------------------------------------------------------*/
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
t_Value base_ininventory(t_Value *arg_list)
{
t_Value ret_val;
char SQL[401];
char NameOfCategory[41];
char ProductID[41];

  ret_val.nValue = 0;
  ret_val.typ = Bool_Value;


  if (arg_list[0].typ == String_Value)
    {
      if (strcmp(arg_list[0].sValue,"") == 0)
        cgiFormString("CATEGORY",NameOfCategory,40);
    }
  else
    cgiFormString("CATEGORY",NameOfCategory,40);


  if (arg_list[1].typ == String_Value)
    {
      if (strcmp(arg_list[1].sValue,"") == 0)
        cgiFormString("VSPINDEX",ProductID,40);
    }
  else
    cgiFormString("VSPINDEX",ProductID,40);

   sprintf(SQL,"select INVENTORYCONTROL.ONSTOCK,PRODUCTCATEGORIES.VSINDEX,PRODUCTCATEGORIES.NAMEOFCATEGORY from INVENTORYCONTROL,PRODUCTCATEGORIES where PRODUCTCATEGORIES.VSINDEX=INVENTORYCONTROL.CATEGORY AND PRODUCTCATEGORIES.NAMEOFCATEGORY='%s' AND INVENTORYCONTROL.PRODUCTINDEX='%s'",
            NameOfCategory,ProductID);
             
  if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
    ret_val.nValue = 1;
  else
    ret_val.nValue = 0;

  return ret_val;
}
#endif
/*--------------------------------------------------------------------------------*/
t_Value base_basketupdate(t_Value *arg_list)
{
t_Value ret_val;
char SQL[201];
char ID[21];
char CategoryID[21];
char ProductID[21];
CDBField* CATEGORYID;
CDBField* PRODUCTID;
CDBField* QUANTITY;
long OldQuantity;
long NewQuantity;
int Ret;

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  CDBField* ONSTOCK;
  CDBField* BLOCKONMINIMUM;
  CDBField* MINIMUM;
  long NewOnStock;
#endif


  ret_val.typ = Bool_Value;
  Ret = 0;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          NewQuantity = atol(arg_list[1].sValue);
        }
      else 
      if (arg_list[1].typ == Int_Value)
        {
          NewQuantity = arg_list[1].nValue;
        }
      else
        NewQuantity = 0;

      if (NewQuantity < 0)
        {
          ret_val.nValue = 0;
          return ret_val;
        }


      if (cgiFormString("ID",ID,20) != cgiFormNotFound)
        {
          sprintf(SQL,"SELECT * from BASKETITEMS where VSINDEX='%s'",arg_list[0].sValue);

          if ((Ret = EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL)) != 0)
            {
              CATEGORYID = EngineDB->GetField(MAX_SQLSELECT-2,"CATEGORYID");
              strcpy(CategoryID,CATEGORYID->String);
              PRODUCTID = EngineDB->GetField(MAX_SQLSELECT-2,"PRODUCTID");
              strcpy(ProductID,PRODUCTID->String);
              QUANTITY = EngineDB->GetField(MAX_SQLSELECT-2,"QUANTITY");

              if (QUANTITY->Long == NewQuantity)
                {
                  ret_val.nValue = 1;
                  return ret_val;
                }

              OldQuantity = QUANTITY->Long;

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
              sprintf(SQL,"select ONSTOCK,MINIMUM,BLOCKONMINIMUM from INVENTORYCONTROL where CATEGORY='%s' AND PRODUCTINDEX='%s'",
                      CategoryID,ProductID);
          
              if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
                {

                  ONSTOCK = EngineDB->GetField(MAX_SQLSELECT-2,"ONSTOCK");
                  BLOCKONMINIMUM  = EngineDB->GetField(MAX_SQLSELECT-2,"BLOCKONMINIMUM");
                  MINIMUM   = EngineDB->GetField(MAX_SQLSELECT-2,"MINIMUM");


//                  FIXED
//                  if ((BLOCKONMINIMUM->Long == 1) && 
//                     ((ONSTOCK->Long + (OldQuantity - NewQuantity)) <= MINIMUM->Long))

                  if ((BLOCKONMINIMUM->Long == 1) && 
                      ((ONSTOCK->Long + (OldQuantity - NewQuantity)) < MINIMUM->Long))
                    {
                      ret_val.nValue = 0;
                      return ret_val;
                    }

                  if (NewQuantity == 0)
                    sprintf(SQL,"delete from BASKETITEMS where VSINDEX='%s'",arg_list[0].sValue);
                  else
                    sprintf(SQL,"update BASKETITEMS set QUANTITY=%ld where VSINDEX='%s'",NewQuantity,arg_list[0].sValue);

                  if (EngineDB->SQLUpdate(SQL) != 0)
                    {
                      NewOnStock = ONSTOCK->Long + (OldQuantity - NewQuantity);

                      sprintf(SQL,"update INVENTORYCONTROL set ONSTOCK=%ld where CATEGORY='%s' AND PRODUCTINDEX='%s'",NewOnStock,CategoryID,ProductID);

                      EngineDB->SQLUpdate(SQL);
                    }
                }
              else
#endif
                {
                  if (NewQuantity == 0)
                    sprintf(SQL,"delete from BASKETITEMS where VSINDEX='%s'",arg_list[0].sValue);
                  else
                    sprintf(SQL,"update BASKETITEMS set QUANTITY=%ld where VSINDEX='%s'",NewQuantity,arg_list[0].sValue);

                 EngineDB->SQLUpdate(SQL);
                }
            }
        }
    }
  else
    {
      RuntimeError1("%%%%basketupdate[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }

  ret_val.nValue = 1;
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
double OrdersSum(char* OrderID)
{
char SQL[201];
char Buffer[41];

CDBField* CUSTOMDISC;
CDBField* CUSTOMVAT;
CDBField* QUANTITY;
CDBField* PRICE;
double Sum = 0;
double Price;

      sprintf(SQL,"SELECT * from ORDERITEMS where ORDERID='%s'",OrderID);
      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
        {
          do
            {
              PRICE =  EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");
              
              CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMDISC");
              CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMVAT");
              QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");

              Price = PRICE->Double;
              Price = Price - ((Price / 100) * CUSTOMDISC->Double);
              Price = Price + ((Price / 100) * CUSTOMVAT->Double);
              sprintf(Buffer,"%.2f",Price);
              Price= atof(Buffer) * QUANTITY->Long;

              Sum += Price;
            }
          while (EngineDB->GetNext(MAX_SQLSELECT-1));
        }

  return Sum;
}
/*--------------------------------------------------------------------------------*/
t_Value base_ordersum(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.dValue = 0;
  ret_val.typ = Float_Value;

  if (arg_list[0].typ == String_Value)
    {
      ret_val.dValue = OrdersSum(arg_list[0].sValue);
    }
  else
    {
      RuntimeError1("%%%%ordersum[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_ordervat(t_Value *arg_list)
{
t_Value ret_val;
char SQL[201];
char Buffer[41];

CDBField* CUSTOMDISC;
CDBField* CUSTOMVAT;
CDBField* QUANTITY;
CDBField* PRICE;
double Sum = 0;
double Price = 0;
double Vat;

  if (arg_list[0].typ == String_Value)
    {
      sprintf(SQL,"SELECT * from ORDERITEMS where ORDERID='%s'",arg_list[0].sValue);
      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
        {
          do
            {
              PRICE =  EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");
              
              CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMDISC");
              CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMVAT");
              QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");

              Price = PRICE->Double;
              Price = Price - ((Price / 100) * CUSTOMDISC->Double);
              Price += ((Price / 100) * CUSTOMVAT->Double);
              sprintf(Buffer,"%.2f",Price);
              Price= atof(Buffer) * QUANTITY->Long;
              Vat = Price / (100+CUSTOMVAT->Double) * CUSTOMVAT->Double;
              sprintf(Buffer,"%.2f",(Price / (100+CUSTOMVAT->Double))* CUSTOMVAT->Double);
              Vat = atof(Buffer);
              Sum += Vat;
            }
          while (EngineDB->GetNext(MAX_SQLSELECT-1));
        }
    }
  else
    {
      RuntimeError1("%%%%ordersum[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }

  ret_val.dValue = Sum;
  ret_val.typ = Float_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_orderrowvat(t_Value *arg_list)
{
t_Value ret_val;
char SQL[201];
char Buffer[41];

CDBField* CUSTOMDISC;
CDBField* CUSTOMVAT;
CDBField* QUANTITY;
CDBField* PRICE;
double Price = 0;
double Vat = 0;

  if (arg_list[0].typ == String_Value)
    {
      sprintf(SQL,"SELECT * from ORDERITEMS where VSINDEX='%s'",arg_list[0].sValue);
      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
        {
            PRICE =  EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");
              
            CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMDISC");
            CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMVAT");
            QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");

            Price = PRICE->Double;
            Price = Price - ((Price / 100) * CUSTOMDISC->Double);
            Price += ((Price / 100) * CUSTOMVAT->Double);
            sprintf(Buffer,"%.2f",Price);
            Price= atof(Buffer) * QUANTITY->Long;
            sprintf(Buffer,"%.2f",(Price / (100+CUSTOMVAT->Double) * CUSTOMVAT->Double));
            Vat = atof(Buffer);
        }
    }
  else
    {
      RuntimeError1("%%%%ordersum[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }

  ret_val.dValue = Vat;
  ret_val.typ = Float_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_orderrowsum(t_Value *arg_list)
{
t_Value ret_val;
char SQL[201];
char Buffer[41];

CDBField* CUSTOMDISC;
CDBField* CUSTOMVAT;
CDBField* QUANTITY;
CDBField* PRICE;
double Price = 0;

  if (arg_list[0].typ == String_Value)
    {
      sprintf(SQL,"SELECT * from ORDERITEMS where VSINDEX='%s'",arg_list[0].sValue);
      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
        {

            PRICE =  EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");
              
            CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMDISC");
            CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMVAT");
            QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");

            Price = PRICE->Double;
            Price = Price - ((Price / 100) * CUSTOMDISC->Double);
            Price = Price + ((Price / 100) * CUSTOMVAT->Double);
            sprintf(Buffer,"%.2f",Price);
            Price= atof(Buffer) * QUANTITY->Long;
        }
    }
  else
    {
      RuntimeError1("%%%%ordersum[PARAMETER] - PARAMETER must be STRING");
      ret_val.nValue = 0;
    }

  ret_val.dValue = Price;
  ret_val.typ = Float_Value;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_basketsum(t_Value *arg_list)
{
t_Value ret_val;
char SQL[201];
char ID[21];
char Buffer[41];

CDBField* PRODUCTID;
CDBField* CUSTOMDISC;
CDBField* CUSTOMVAT;
CDBField* QUANTITY;
CDBField* NAMEOFCATEGORY;
CDBField* PRICE;

double Sum = 0;
double Price;

  ret_val.typ = Float_Value;


  if (cgiFormString("ID",ID,20) != cgiFormNotFound)
    {
      sprintf(SQL,"SELECT PRODUCTCATEGORIES.NAMEOFCATEGORY, BASKETITEMS.* from PRODUCTCATEGORIES,BASKETITEMS where PRODUCTCATEGORIES.VSINDEX=BASKETITEMS.CATEGORYID AND BASKETID='%s'",ID);
      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
        {
          do
            {
              PRODUCTID =  EngineDB->GetField(MAX_SQLSELECT-1,"PRODUCTID");
              CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMDISC");
              CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMVAT");
              QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");
              NAMEOFCATEGORY = EngineDB->GetField(MAX_SQLSELECT-1,"NAMEOFCATEGORY");
              PRICE = EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");

              Price = PRICE->Double;
              Price = Price - ((Price / 100) * CUSTOMDISC->Double);

             // if ( priceDisplay() == PRICEDISP_GROSS )
                Price = Price + ((Price / 100) * CUSTOMVAT->Double);
      
              sprintf(Buffer,"%.2f",Price);
              Price= atof(Buffer);
              Sum += Price * QUANTITY->Long;
            }
          while (EngineDB->GetNext(MAX_SQLSELECT-1));
        }
    }

  ret_val.dValue = Sum;
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_basketrowsum(t_Value *arg_list) {
  t_Value ret_val;
  char SQL[201];
  char VSBINDEX[21];
  char Buffer[41];

  double CustomDisc;
  double CustomVat;
  double Price = 0;
  long Quantity;
  CDBField* CUSTOMDISC;
  CDBField* CUSTOMVAT;
  CDBField* QUANTITY;
  CDBField* PRICE;

  ret_val.typ = Float_Value;

  if (cgiFormString("VSBINDEX",VSBINDEX,20) != cgiFormNotFound) {

    sprintf(SQL,
            "SELECT productcategories.nameofcategory, basketitems.* "
            "  FROM productcategories,basketitems "
            " WHERE productcategories.vsindex=basketitems.categoryid "
            "   AND basketitems.vsindex='%s'",
            VSBINDEX);

    if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0) {

      CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-2,"CUSTOMDISC");
      CustomDisc = CUSTOMDISC->Double;

      CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-2,"CUSTOMVAT");
      CustomVat = CUSTOMVAT->Double;

      QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-2,"QUANTITY");
      Quantity = QUANTITY->Long;

      PRICE = EngineDB->GetField(MAX_SQLSELECT-2,"PRICE");
      Price = PRICE->Double;

      //
      // calculate row price
      //
      if ( taxCalcMethod() == TAXCALCMETH_RETAIL ) {
        if ( priceDisplay() == PRICEDISP_GROSS )
          Price = rowGrossRetail( Price, CustomDisc, Quantity,
                                  CustomVat );
      }
      else {
        Price = rowGross( Price, CustomDisc, Quantity, CustomVat );
      }

    }
  }

  ret_val.dValue = Price;
  return ret_val;
}

/*--------------------------------------------------------------------------------*/

int priceDisplay() {

  char *priceDispStr = 0;

  int priceDisp = PRICEDISP_NET;

  if ( priceDispStr = getSetting( MAX_SQLSELECT - 2, // careful with the handle
                                  "FRONTEND",
                                  "PRICEDISPLAY" ) ) {
    if ( !strcmp( priceDispStr, PRICEDISPSTR_NET ) )
      priceDisp = PRICEDISP_NET;
    else
      priceDisp = PRICEDISP_GROSS;
  }

  return priceDisp;
}

int taxCalcMethod() {
  char *taxCalcMethStr = 0;

  int taxCalcMeth = TAXCALCMETH_RETAIL;

  if ( taxCalcMethStr = getSetting( MAX_SQLSELECT - 2, // careful with handle
                                    "FRONTEND",
                                    "TAXCALCMETHOD" ) ) {
    if ( !strcmp( taxCalcMethStr, TAXCALCMETHSTR_RETAIL ) )
      taxCalcMeth = TAXCALCMETH_RETAIL;
    else
      taxCalcMeth = TAXCALCMETH_WHOLESALE;
  }

  return taxCalcMeth;
}

t_Value base_basketrowvat(t_Value *arg_list) {
  t_Value ret_val;
  char SQL[201];
  char VSBINDEX[21];

  double CustomDisc;
  double CustomVat;
  long Quantity;

  CDBField* PRODUCTID;
  CDBField* CUSTOMVAT;
  CDBField* CUSTOMDISC;
  CDBField* QUANTITY;
  CDBField* NAMEOFCATEGORY;
  CDBField* PRICE;
  char Buffer[41];
  double Vat = 0;
  double Price;

  //
  // specify query handle
  //
  int qh = MAX_SQLSELECT - 2;

  ret_val.typ = Float_Value;

  if (cgiFormString("VSBINDEX",VSBINDEX,20) != cgiFormNotFound) {

    //
    // get price display
    //
    int priceDisp = priceDisplay();
  
    sprintf(SQL,"SELECT PRODUCTCATEGORIES.NAMEOFCATEGORY, BASKETITEMS.* "
                "from PRODUCTCATEGORIES,BASKETITEMS where "
                "PRODUCTCATEGORIES.VSINDEX=BASKETITEMS.CATEGORYID AND "
                "BASKETITEMS.VSINDEX='%s'",VSBINDEX);

    if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0) {

      CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-2,"CUSTOMVAT");
      CustomVat = CUSTOMVAT->Double;

      CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-2,"CUSTOMDISC");
      CustomDisc = CUSTOMDISC->Double;

      QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-2,"QUANTITY");
      Quantity = QUANTITY->Long;

      PRICE =  EngineDB->GetField(MAX_SQLSELECT-2,"PRICE");
      Price = PRICE->Double ;

      if ( taxCalcMethod() == TAXCALCMETH_RETAIL )
        Vat = rowTaxRetail( Price, CustomDisc, Quantity, CustomVat );
      else
        Vat = rowTaxWholesale( Price, CustomDisc, Quantity, CustomVat );
    }
  }

  ret_val.dValue = Vat;
  return ret_val;
}

//
// Rounds a number to two decimal places.
//
double round( double d ) {
  char buf[ 41 ];
  sprintf( buf, "%.2f", d );
  return atof( buf );
}

/*--------------------------------------------------------------------------------*/
t_Value base_basketvat(t_Value *arg_list) {
  t_Value ret_val;
  char SQL[201];
  char ID[21];

  CDBField* PRODUCTID;
  CDBField* CUSTOMVAT;
  CDBField* CUSTOMDISC;
  CDBField* QUANTITY;
  CDBField* NAMEOFCATEGORY;
  CDBField* PRICE;
  char Buffer[41];
  double Vat = 0;
  double Price;

  ret_val.typ = Float_Value;

  if (cgiFormString("ID",ID,20) != cgiFormNotFound)
    {
      sprintf(SQL,"SELECT PRODUCTCATEGORIES.NAMEOFCATEGORY, BASKETITEMS.* from PRODUCTCATEGORIES,BASKETITEMS where PRODUCTCATEGORIES.VSINDEX=BASKETITEMS.CATEGORYID AND BASKETID='%s'",ID);
      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
        {
          do
            {
              PRODUCTID =  EngineDB->GetField(MAX_SQLSELECT-1,"PRODUCTID");
              CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMVAT");
              CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMDISC");
              QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");
              NAMEOFCATEGORY = EngineDB->GetField(MAX_SQLSELECT-1,"NAMEOFCATEGORY");
              PRICE =  EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");

              if ( taxCalcMethod() == TAXCALCMETH_RETAIL )
                Vat += rowTaxRetail( PRICE->Double, CUSTOMDISC->Double,
                                     QUANTITY->Long, CUSTOMVAT->Double );
              else
                Vat += rowTaxWholesale( PRICE->Double, CUSTOMDISC->Double,
                                        QUANTITY->Long, CUSTOMVAT->Double );
            }
          while (EngineDB->GetNext(MAX_SQLSELECT-1));
        }
    }

  ret_val.dValue = Vat;
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_basketdelete(t_Value *arg_list)
{
t_Value ret_val;
char SQL1[201];
char SQL2[201];
char ID[21];
CDBField* CATEGORYID;
CDBField* PRODUCTID;
CDBField* QUANTITY;
int Ret;

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  CDBField* ONSTOCK;
  long OnStock;
  char SQL[201];
#endif

  ret_val.typ = Bool_Value;
  Ret = 0;

  cgiFormString("ID",ID,20);

  if (arg_list[0].typ == String_Value)
    {
      sprintf(SQL1,"SELECT CATEGORYID,QUANTITY,PRODUCTID FROM BASKETITEMS WHERE VSINDEX='%s'",arg_list[0].sValue);
      sprintf(SQL2,"DELETE from BASKETITEMS where VSINDEX='%s'",arg_list[0].sValue);
    }
  else
  if (arg_list[0].typ == Not_Set)
    {
      sprintf(SQL1,"SELECT CATEGORYID,PRODUCTID,QUANTITY FROM BASKETITEMS WHERE BASKETID='%s'",ID);
      sprintf(SQL2,"DELETE from BASKETITEMS where BASKETID='%s'",ID);
    }

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL1) != 0)
    {
      do
        {
          CATEGORYID =  EngineDB->GetField(MAX_SQLSELECT-1,"CATEGORYID");
          PRODUCTID =  EngineDB->GetField(MAX_SQLSELECT-1,"PRODUCTID");
          QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
          sprintf(SQL,"SELECT ONSTOCK FROM INVENTORYCONTROL where CATEGORY='%s' AND PRODUCTINDEX='%s'",
                  CATEGORYID->String,PRODUCTID->String);

            if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
              {
                ONSTOCK =  EngineDB->GetField(MAX_SQLSELECT-2,"ONSTOCK");
                OnStock = ONSTOCK->Long + QUANTITY->Long;
                sprintf(SQL,"update INVENTORYCONTROL set ONSTOCK=%ld where CATEGORY='%s' AND PRODUCTINDEX='%s'",
                        OnStock,CATEGORYID->String,PRODUCTID->String);
                EngineDB->SQLUpdate(SQL);
              }
#endif
        }
      while (EngineDB->GetNext(MAX_SQLSELECT-1));
    }

  ret_val.nValue = EngineDB->SQLUpdate(SQL2);

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
int SavePaymentData(char* PaymentID)
{
char Buffer[5000];
char Buffer2[500];
CDBField* DBField;
int Counter;


  if (EngineDB->SQLInsert("PAYMENTDATA",PaymentID) == 0)
    {
      return 0;
    }
  
  sprintf(Buffer,"select * from PAYMENTDATA where VSINDEX=''");
  EngineDB->SQLSelect(MAX_SQLSELECT-1,Buffer);

  sprintf(Buffer,"UPDATE PAYMENTDATA SET ");
  for (Counter=1;Counter <= EngineDB->GetNumCols(MAX_SQLSELECT-1);Counter++)
    {
      DBField = EngineDB->GetField(MAX_SQLSELECT-1,Counter);

      if ((cgiFormString(DBField->Name,Buffer2,500) != cgiFormNotFound) && (strcmp(DBField->Name,"VSINDEX") !=0))
        {
          strcat(Buffer,DBField->Name);
          strcat(Buffer2,"");

          switch(DBField->Type)
            {
              case FT_STRING:
              case FT_DATE:
              case FT_TIME:
              case FT_TIMESTAMP:
                strcat(Buffer,"='");
                strcat(Buffer,Buffer2);
                strcat(Buffer,"',");
                break;
              case FT_BOOLEAN:
                strcat(Buffer,"=");
                if (strcmp(Buffer2,"FALSE") == 0)
                  strcat(Buffer,"FALSE");
                else
                  strcat(Buffer,"TRUE");
                strcat(Buffer,",");
                break;
              case FT_LONG:
              case FT_DOUBLE:
                strcat(Buffer,"=");
                strcat(Buffer,Buffer2);
                strcat(Buffer,",");
                break;
              default:
                RuntimeError2("SavePayment - unsupported typ of column ",DBField->Name);
                break;

            }
        }
    }

  Buffer[strlen(Buffer)-1] = '\0';
  strcat(Buffer," WHERE VSINDEX='");
  cgiFormString("VSINDEX",Buffer2,500);
  strcat(Buffer,PaymentID);
  strcat(Buffer,"'");

  return EngineDB->SQLUpdate(Buffer);
}
/*--------------------------------------------------------------------------------*/
t_Value base_basketorder(t_Value *arg_list)
{
t_Value ret_val;
char NewOrderItemsID[21];
char NewOrderID[21];
char SQL[801];
CDBField* CATEGORYNAME;
CDBField* CATEGORYID;
CDBField* PRODUCTID;
CDBField* CUSTOMDISC;
CDBField* CUSTOMVAT;
CDBField* QUANTITY;
CDBField* NAME;
CDBField* PRICE;
CDBField* SHORTDESCR;
CDBField* CUSTOMER;
CDBField* CRITERIA1;
CDBField* CRITERIA2;
CDBField* BASKETOPTIONINDEX;
CDBField* ORDERITEMSINDEX;
CDBField* ORDERITEMSORDERID;

char Customer[51];
char PaymentData[21];
char ID[21];
char* Value;

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  CDBField* TOTAL_REVENUE;
  double TotalRevenue = 0;
#else
  CDBField* GROUP;
#endif


  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  cgiFormString("ID",ID,20);

  sprintf(SQL,"select PRODUCTCATEGORIES.NAMEOFCATEGORY,BASKETITEMS.* from PRODUCTCATEGORIES,BASKETITEMS where PRODUCTCATEGORIES.VSINDEX=BASKETITEMS.CATEGORYID AND BASKETID='%s'",ID);

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
    {
      if (EngineDB->SQLInsert("ORDERS",NewOrderID) == 0)
        {
          return ret_val;
        }

      do
        {
          CATEGORYNAME = EngineDB->GetField(MAX_SQLSELECT-1,"NAMEOFCATEGORY");
          CATEGORYID = EngineDB->GetField(MAX_SQLSELECT-1,"CATEGORYID");
          PRODUCTID = EngineDB->GetField(MAX_SQLSELECT-1,"PRODUCTID");
          CUSTOMDISC = EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMDISC");
          CUSTOMVAT = EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMVAT");
          QUANTITY = EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");
          CRITERIA1 = EngineDB->GetField(MAX_SQLSELECT-1,"CRITERIA1");
          CRITERIA2 = EngineDB->GetField(MAX_SQLSELECT-1,"CRITERIA2");
          PRICE =  EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");

          sprintf(SQL,"select * from %s where VSINDEX='%s'",CATEGORYNAME->String,PRODUCTID->String);  
          if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
            {
              NAME = EngineDB->GetField(MAX_SQLSELECT-2,"NAME");
              SHORTDESCR = EngineDB->GetField(MAX_SQLSELECT-2,"SHORTDESCR");
            }
          else
            {
              fprintf(cgiOut,SQL);
              return ret_val;
            }

          if (EngineDB->SQLInsert("ORDERITEMS",NewOrderItemsID) == 0)
            {
              return ret_val;
            }

          sprintf(SQL,"update ORDERITEMS set ORDERID='%s',CATEGORYID='%s',PRODUCTID='%s',QUANTITY=%ld,NAMEOFCATEGORY='%s',PRODUCTNAME='%s',SHORTDESCR='%s',PRICE=%f,CUSTOMDISC=%f,CUSTOMVAT=%f,CRITERIA1='%s',CRITERIA2='%s' where VSINDEX='%s'",
                  NewOrderID,CATEGORYID->String,PRODUCTID->String,QUANTITY->Long,CATEGORYNAME->String,
                  NAME->String,SHORTDESCR->String,PRICE->Double,CUSTOMDISC->Double,CUSTOMVAT->Double,CRITERIA1->String,CRITERIA2->String,NewOrderItemsID);
          if (EngineDB->SQLUpdate(SQL) == 0)
            {
              fprintf(cgiOut,SQL);
              return ret_val;
            }

        }
      while(EngineDB->GetNext(MAX_SQLSELECT-1));

      sprintf(SQL,"select VSINDEX from BASKETOPTION where BASKETID='%s'",ID);

      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL))
        {
	  while(! EngineDB->IsEOF(MAX_SQLSELECT-1))
	    {
	      BASKETOPTIONINDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
	      
	      sprintf(SQL,"select VSINDEX,ORDERID from ORDERITEMS where CRITERIA1='%s' OR CRITERIA2='%s'",
	      BASKETOPTIONINDEX->String,BASKETOPTIONINDEX->String);

              if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL))
                {
		  ORDERITEMSINDEX = EngineDB->GetField(MAX_SQLSELECT-2,"VSINDEX");
   		  ORDERITEMSORDERID = EngineDB->GetField(MAX_SQLSELECT-2,"ORDERID");
		
		  sprintf(SQL,"update BASKETOPTION set BASKETINDEX='%s',BASKETID='%s' where VSINDEX='%s'",
                  ORDERITEMSINDEX->String,ORDERITEMSORDERID->String,BASKETOPTIONINDEX->String);
                  if (EngineDB->SQLUpdate(SQL) == 0)
                    {
                      fprintf(cgiOut,SQL);
                      return ret_val;
                    }

                  sprintf(SQL,"insert into ORDEROPTION select * from BASKETOPTION where VSINDEX='%s'",BASKETOPTIONINDEX->String);
                  if (EngineDB->SQLUpdate(SQL) == 0)
                    {
                      fprintf(cgiOut,SQL);
                      return ret_val;
                    }
		}
              EngineDB->GetNext(MAX_SQLSELECT-1);  		
	    }
	}  
      
      if (SavePaymentData(PaymentData) == 0)
        {
          return ret_val;
        }
     
  if (cgiFormString("LOGIN",Customer,200) == cgiFormNotFound)
    {
      if (cgiFormString("ID",ID,20) != cgiFormNotFound)
        {
          if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
            {
              strcpy(Customer,Value);
            }
          else
            strcpy(Customer,"ANONYM");
        }
      else
        strcpy(Customer,"ANONYM");
    }

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
    sprintf(SQL,"SELECT VSINDEX,TOTAL_REVENUE FROM CUSTOMERS WHERE LOGIN='%s'",Customer);
  #else
    sprintf(SQL,"SELECT VSINDEX,VSGROUP FROM CUSTOMERS WHERE LOGIN='%s'",Customer);
  #endif

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)
    {
      CUSTOMER = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
      strcpy(Customer,CUSTOMER->String);

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
        TOTAL_REVENUE = EngineDB->GetField(MAX_SQLSELECT-1,"TOTAL_REVENUE");
        TotalRevenue = TOTAL_REVENUE->Double;
        TotalRevenue += OrdersSum(NewOrderID);
      #else
        GROUP = EngineDB->GetField(MAX_SQLSELECT-1,"VSGROUP");
        if (atol(GROUP->String) > 1)
           {
             RuntimeError1(WRONGGROUP);
             ret_val.nValue = 0;
             return ret_val;
           }
      #endif

    }
  else
    strcpy(Customer,"UNKNOWN");

  sprintf(SQL,"update PAYMENTDATA set ORDERID='%s',CUSTOMERID='%s' WHERE VSINDEX='%s'",
          NewOrderID,Customer,PaymentData);
  
  if (EngineDB->SQLUpdate(SQL) == 0)
    {
      fprintf(cgiOut,SQL);
      return ret_val;
    }
#ifdef DB_ORACLE      
  sprintf(SQL,"update ORDERS set CUSTOMER='%s',PAYMENTDATA='%s', PAID=0, VSDATE=sysdate where VSINDEX='%s'",
          Customer,PaymentData,NewOrderID);
#else
  sprintf(SQL,"update ORDERS set CUSTOMER='%s',PAYMENTDATA='%s', PAID=0 where VSINDEX='%s'",
          Customer,PaymentData,NewOrderID);
#endif
  
      if (EngineDB->SQLUpdate(SQL) == 0)
        {
          fprintf(cgiOut,SQL);
          return ret_val;
        }

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
    sprintf(SQL,"update CUSTOMERS set TOTAL_REVENUE=%.2f where VSINDEX='%s'",
            TotalRevenue,Customer);
      
    if (EngineDB->SQLUpdate(SQL) == 0)
      {
        fprintf(cgiOut,SQL);
        return ret_val;
      }
  #endif

      sprintf(SQL,"delete from BASKETITEMS where BASKETID='%s'",ID);
      if (EngineDB->SQLUpdate(SQL) == 0)
        {
          fprintf(cgiOut,SQL);
          return ret_val;
        }
	
      sprintf(SQL,"delete from BASKETOPTION where BASKETID='%s'",ID);
      if (EngineDB->SQLUpdate(SQL) == 0)
        {
        }
    }

  ret_val.nValue = 1;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_payorder(t_Value *arg_list)
{
t_Value ret_val;
char SQL[801];
CDBField* NAMEOFCATEGORY;
CDBField* CATEGORYID;
CDBField* PRODUCTID;
CDBField* CUSTOMDISC;
CDBField* CUSTOMVAT;
CDBField* QUANTITY;
CDBField* PRICE;
CDBField* NAME;
CDBField* MID_TEMP;
CDBField* HID_TEMP;
char HID[50];
char MID[50];
char ID[21];
int Count;
double Price;
double Sum;
char Buffer[100];
char Filled1[200];
char Filled2[200];
char IPSADRESSE[200];
char IPSADRESSE_CGI[200];
char CURRENCY[10];
char* Ptr;


  ret_val.typ = Not_Set;

  cgiFormString("ID",ID,20);

  if ((Ptr = DBGetConfig("IPSURL")) != NULL)
    strcpy(IPSADRESSE,Ptr);
  else
    strcpy(IPSADRESSE,"");

  if ((Ptr = DBGetConfig("IPSCGIURL")) != NULL)
    strcpy(IPSADRESSE_CGI,Ptr);
  else
    strcpy(IPSADRESSE_CGI,"");

  if ((Ptr = DBGetConfig("PRIMARY_CURRENCY")) != NULL)
    strcpy(CURRENCY,Ptr);
    
  if (strcmp(CURRENCY,"DM") == 0)
    strcpy(CURRENCY,"DEM");
    
     

  if ((arg_list[0].typ == Int_Value) && (arg_list[1].typ == String_Value) && (arg_list[2].typ == String_Value))
    {
      fprintf(cgiOut,"<APPLET\n");

      switch (arg_list[0].nValue)
        {
          case 0:
          case 1:
          case 2:
          case 3:
            fprintf(cgiOut,"  NAME=Kreditkarte\n");
            fprintf(cgiOut,"  CODE=Kreditkarte.class\n");
            break;
          case 4:
            fprintf(cgiOut,"  NAME=Geldkarte\n");
            fprintf(cgiOut,"  CODE=Geldkarte.class\n");
            break;
          case 5:
            fprintf(cgiOut,"  NAME=ELastschrift\n");
            fprintf(cgiOut,"  CODE=ELastschrift.class\n");
            break;
        }
      fprintf(cgiOut,"  CODEBASE=https://%s/\n",IPSADRESSE);
      fprintf(cgiOut,"  WIDTH=400\n");
      fprintf(cgiOut,"  HEIGHT=300\n");
      fprintf(cgiOut,"  ALIGN=\"Top\"\n");
      fprintf(cgiOut,"  ALT=\"Ihr Browser ist nicht JAVA faehig !\"\n");
      fprintf(cgiOut,">\n");
      fprintf(cgiOut,"<PARAM NAME=DOWNLOADPAGE VALUE=\"https://%s/GKS200.zip\">\n",IPSADRESSE);
      fprintf(cgiOut,"<PARAM NAME=CGIURL VALUE=\"https://%s/cgi-bin/cgihdl\">\n",IPSADRESSE_CGI);
      fprintf(cgiOut,"<PARAM NAME=CANVASURL VALUE=\"https://%s/Bitmap/\">\n",IPSADRESSE);
      fprintf(cgiOut,"<PARAM NAME=CANVASBASENAME VALUE=\"pis_\">\n");
      fprintf(cgiOut,"<PARAM NAME=CANVASFIRST VALUE=\"0\">\n");
      fprintf(cgiOut,"<PARAM NAME=CANVASLAST VALUE=\"16\">\n");
      switch (arg_list[0].nValue)
        {
          case 0:
            fprintf(cgiOut,"<PARAM NAME=LOGOURL VALUE=\"https://%s/wallet/images/visa.gif\">\n",IPSADRESSE);
            break;
          case 1:
            fprintf(cgiOut,"<PARAM NAME=LOGOURL VALUE=\"https://%s/wallet/images/mastercard.gif\">\n",IPSADRESSE);
            break;
          case 2:
            fprintf(cgiOut,"<PARAM NAME=LOGOURL VALUE=\"https://%s/wallet/images/amex.gif\">\n",IPSADRESSE);
            break;
          case 3:
            fprintf(cgiOut,"<PARAM NAME=LOGOURL VALUE=\"https://%s/wallet/images/diners.gif\">\n",IPSADRESSE);
            break;
          case 4:
            fprintf(cgiOut,"<PARAM NAME=LOGOURL VALUE=\"https://%s/wallet/images/geldkarte.gif\">\n",IPSADRESSE);
            break;
          case 5:
            fprintf(cgiOut,"<PARAM NAME=LOGOURL VALUE=\"https://%s/wallet/images/elv.gif\">\n",IPSADRESSE);
            break;
        }
      fprintf(cgiOut,"<PARAM NAME=URLERFOLG VALUE=\"%s\">\n",arg_list[1].sValue);
      fprintf(cgiOut,"<PARAM NAME=URLFEHLER VALUE=\"%s\">\n",arg_list[2].sValue);
      fprintf(cgiOut,"<PARAM NAME=TEXTERFOLG VALUE=\"Weiter\">\n");
      fprintf(cgiOut,"<PARAM NAME=WAEHRUNG VALUE=\"%s\">\n",CURRENCY);
      fprintf(cgiOut,"<PARAM NAME=SID VALUE=\"00000000\">\n");

      switch (arg_list[0].nValue)
        {
          case 0:
          case 1:
          case 2:
          case 3:
            fprintf(cgiOut,"<PARAM NAME=CABBASE VALUE=\"https://%s/Kreditkarte.cab\">\n",IPSADRESSE);
            fprintf(cgiOut,"<PARAM NAME=ARCHIVE VALUE=\"https://%s/Kreditkarte.jar\">\n",IPSADRESSE);
            fprintf(cgiOut,"<PARAM NAME=KARTENART VALUE=\"0%li\">\n",arg_list[0].nValue);
            fprintf(cgiOut,"<PARAM NAME=KARTENNUMMER VALUE=\"\">\n");
            fprintf(cgiOut,"<PARAM NAME=GUELTIGBISMONAT VALUE=\"\">\n");
            fprintf(cgiOut,"<PARAM NAME=GUELTIGBISJAHR VALUE=\"\">\n");
            break;
          case 4:
            fprintf(cgiOut,"<PARAM NAME=CABBASE VALUE=https://%s/Geldkarte.cab>\n",IPSADRESSE);
            fprintf(cgiOut,"<PARAM NAME=ARCHIVE VALUE=https://%s/Geldkarte.jar>\n",IPSADRESSE);
            fprintf(cgiOut,"<PARAM NAME=RECOVERYPAGE VALUE=\"https://%s/Recovery.html\">\n",IPSADRESSE);
            break;
          case 5:
            fprintf(cgiOut,"<PARAM NAME=CABBASE VALUE=\"https://%s/ELastschrift.cab\">\n",IPSADRESSE);
            fprintf(cgiOut,"<PARAM NAME=ARCHIVE VALUE=\"https://%s/ELastschrift.jar\">\n",IPSADRESSE);
            fprintf(cgiOut,"<PARAM NAME=KONTONUMMER VALUE=\"\">\n");
            fprintf(cgiOut,"<PARAM NAME=BLZ VALUE=\"\">\n");
            break;
        }
      Count = 0;
      Sum = 0;
      Price = 0;

      strcpy(SQL,"select MID,HID from IVPROPERTIES");
      EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL);
      MID_TEMP =  EngineDB->GetField(MAX_SQLSELECT-1,"MID");
      HID_TEMP =  EngineDB->GetField(MAX_SQLSELECT-1,"HID");
      if ((MID_TEMP == NULL) || (HID_TEMP == NULL))
        {
          fprintf(cgiOut,"</APPLET>\n");
          RuntimeError1("%%%%payorder : Fehler beim Lesen der Tabelle IVPROPERTIES");
          return ret_val;
        }
      else
        {
          strcpy(HID,HID_TEMP->String);
          strcpy(MID,MID_TEMP->String);
        }
      sprintf(SQL,"SELECT PRODUCTCATEGORIES.NAMEOFCATEGORY, BASKETITEMS.* from PRODUCTCATEGORIES,BASKETITEMS where PRODUCTCATEGORIES.VSINDEX=BASKETITEMS.CATEGORYID AND BASKETID='%s'",ID);

      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
        {
          do
            {
              PRODUCTID =  EngineDB->GetField(MAX_SQLSELECT-1,"PRODUCTID");
              CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMDISC");
              CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMVAT");
              QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");
              NAMEOFCATEGORY = EngineDB->GetField(MAX_SQLSELECT-1,"NAMEOFCATEGORY");
              CATEGORYID = EngineDB->GetField(MAX_SQLSELECT-1,"CATEGORYID");
              PRICE =  EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");

              Price = PRICE->Double;
              Price = Price - ((Price / 100) * CUSTOMDISC->Double);
              Price = Price + ((Price / 100) * CUSTOMVAT->Double);
              sprintf(Buffer,"%.2f",Price);
              Price= atof(Buffer);

              Sum += Price * QUANTITY->Long;
              Count++;
            }
          while (EngineDB->GetNext(MAX_SQLSELECT-1));
        }
    
      FillZeroFloat(Filled1,Sum,12);
      fprintf(cgiOut,"<PARAM NAME=RECHNUNGSBETRAG VALUE=\"%s\">\n",Filled1);

      FillZeroFloat(Filled1,Sum,12);
      FillZeroInt(Filled2,Count,2);
      fprintf(cgiOut,"<PARAM NAME=R_INFO VALUE=\";;1;%s;%li%s#%s;%s;%s;%s",MID,arg_list[0].nValue,PROGRAMNAME,ID,Filled1,CURRENCY,Filled2);

      sprintf(SQL,"SELECT PRODUCTCATEGORIES.NAMEOFCATEGORY, BASKETITEMS.* from PRODUCTCATEGORIES,BASKETITEMS where PRODUCTCATEGORIES.VSINDEX=BASKETITEMS.CATEGORYID AND BASKETID='%s'",ID);
      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
        {
          do
            {
              PRODUCTID =  EngineDB->GetField(MAX_SQLSELECT-1,"PRODUCTID");
              CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMDISC");
              CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMVAT");
              QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");
              NAMEOFCATEGORY = EngineDB->GetField(MAX_SQLSELECT-1,"NAMEOFCATEGORY");
              CATEGORYID = EngineDB->GetField(MAX_SQLSELECT-1,"CATEGORYID");
              PRICE =  EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");

              sprintf(SQL,"SELECT * FROM %s where VSINDEX='%s'",NAMEOFCATEGORY->String,PRODUCTID->String);

              if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
                {
                  NAME = EngineDB->GetField(MAX_SQLSELECT-2,"NAME");


                  Price = PRICE->Double;
                  Price = Price - ((Price / 100) * CUSTOMDISC->Double);
                  Price = Price + ((Price / 100) * CUSTOMVAT->Double);
                  sprintf(Buffer,"%.2f",Price);
                  Price= atof(Buffer);

                  Sum += Price * QUANTITY->Long;
                  Replace(NAME->String,";"," ");
                  fprintf(cgiOut,";%s;%s/%s;%s;%s;%s",HID,
                     CATEGORYID->String,PRODUCTID->String,NAME->String,
                     FillZeroInt(Filled1,QUANTITY->Long,5),FillZeroFloat(Filled2,Price,12));

                }

            }
          while (EngineDB->GetNext(MAX_SQLSELECT-1));
        }

      fprintf(cgiOut,"\">\n");
      fprintf(cgiOut,"</APPLET>\n");
    }
  else
    {
      RuntimeError1("%%%%payorder[PARAMETER1,PARAMETER2,PARAMETER3] - PARAMETER must be INTEGER");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_login(t_Value *arg_list)
{
t_Value ret_val;
char ID[21];
char SQL[201];

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
  char DiscountProfile[21];
  CDBField* DISCOUNTPROFILE;

  char TaxProfile[21];
  CDBField* TAXPROFILE;
#else
  CDBField* GROUP;
#endif

CDBField* DISCOUNT;
double Discount = 0;

CDBField* INDEX;
CDBField* CATEGORYID;
CDBField* TAX;
CDBField* CUSTOMERID;
char CustomerID[13];
double Tax = 0;
char Password[50];

  ret_val.typ = Bool_Value;

  cgiFormString("ID",ID,20);

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          if (strcmp(arg_list[1].sValue,"") != 0)
            strcpy(Password,crypt(arg_list[1].sValue,"VS"));
          else
            strcpy(Password,"");

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
           sprintf(SQL,"select TAXPROFILE,DISCOUNTPROFILE,VSINDEX from CUSTOMERS where LOGIN='%s' AND PASSWORD='%s'",arg_list[0].sValue,Password);
         #else
           sprintf(SQL,"select VSGROUP,VSINDEX from CUSTOMERS where LOGIN='%s' AND PASSWORD='%s'",arg_list[0].sValue,Password);
         #endif

         if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) == 0)
           {
             ret_val.nValue = 0;
             return ret_val;
           }

         CUSTOMERID = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
         strcpy(CustomerID,CUSTOMERID->String);

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
           TAXPROFILE = EngineDB->GetField(MAX_SQLSELECT-1,"TAXPROFILE");
           strcpy(TaxProfile,TAXPROFILE->String);

           DISCOUNTPROFILE = EngineDB->GetField(MAX_SQLSELECT-1,"DISCOUNTPROFILE");
           strcpy(DiscountProfile,DISCOUNTPROFILE->String);
         #else
           GROUP = EngineDB->GetField(MAX_SQLSELECT-1,"VSGROUP");
           if (atol(GROUP->String) > 1)
             {
               RuntimeError1(WRONGGROUP);
               ret_val.nValue = 0;
               return ret_val;
             }
         #endif

         sprintf(SQL,"select VSINDEX,CATEGORYID from BASKETITEMS WHERE BASKETID='%s'",ID);
         if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
           {
             do
               {
                  INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");
                  CATEGORYID = EngineDB->GetField(MAX_SQLSELECT-1,"CATEGORYID");

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                    sprintf(SQL,"SELECT DISCOUNT FROM DISCOUNTS WHERE PROFILEID='%s' AND CATEGORYID='%s'",DiscountProfile,CATEGORYID->String);
                  #else
                    sprintf(SQL,"SELECT DISCOUNT FROM DISCOUNTS WHERE PROFILEID='0' AND CATEGORYID='%s'",CATEGORYID->String);
                  #endif

                  if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQL) != 0)                      
                    {
                      DISCOUNT = EngineDB->GetField(MAX_SQLSELECT-2,"DISCOUNT");
                      Discount = DISCOUNT->Double;
                    }

#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                    sprintf(SQL,"SELECT TAX FROM TAXES WHERE PROFILEID='%s' AND CATEGORYID='%s'",TaxProfile,CATEGORYID->String);
                  #else
                    sprintf(SQL,"SELECT TAX FROM TAXES WHERE PROFILEID='0' AND CATEGORYID='%s'",CATEGORYID->String);
                  #endif

                  if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQL) != 0)                      
                    {
                      TAX = EngineDB->GetField(MAX_SQLSELECT-2,"TAX");
                      Tax = TAX->Double;
                    }

                  sprintf(SQL,"update BASKETITEMS set CUSTOMVAT=%f, CUSTOMDISC=%f where VSINDEX='%s' ",Tax,Discount,INDEX->String);

                  EngineDB->SQLUpdate(SQL);
               }
            while(EngineDB->GetNext(MAX_SQLSELECT-1));

           }

         InsertEntry("LOGIN",arg_list[0].sValue);
         if (!EngineDB->UpdateVar(ID,"LOGIN",arg_list[0].sValue))
           EngineDB->InsertVar(ID,"LOGIN",arg_list[0].sValue);

         sprintf(SQL,"UPDATE SESSIONS set CUSTOMERID='%s' where VSINDEX='%s'",CustomerID,ID);
         EngineDB->SQLUpdate(SQL);


         if (VSHOP_PROFILING > 0)
           {
             sprintf(SQL,"UPDATE CATEGORYPROFILE set CUSTOMERID='%s' where SESSIONID='%s'",CustomerID,ID);
             EngineDB->SQLUpdate(SQL);
           }

         ret_val.nValue = 1;
        }
      else
        {
          RuntimeError1("%%%%login[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING");
          ret_val.nValue = 0;
        }
    }
  else
    {
      RuntimeError1("%%%%login[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING");
      ret_val.nValue = 0;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_paymentlist(t_Value *arg_list)
{
t_Value ret_val;

 ret_val.typ = Bool_Value;
 ret_val.nValue = DBPaymentSelect();
 return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_paymentend(t_Value *arg_list)
{
t_Value ret_val;

 ret_val.typ = Bool_Value;
 ret_val.nValue = DBPaymentNext();
 return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_shippingcostlist(t_Value *arg_list)
{
t_Value ret_val;

 ret_val.typ = Bool_Value;
 ret_val.nValue = DBShippingcostSelect();
 return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_hasshippingcosts(t_Value *arg_list)
{
t_Value ret_val;

 ret_val.typ = Bool_Value;
 ret_val.nValue = DBShippingcostSelect();
 return ret_val;
}


/*--------------------------------------------------------------------------------*/
t_Value base_shippingcostnext(t_Value *arg_list)
{
t_Value ret_val;

 ret_val.typ = Bool_Value;
 ret_val.nValue = DBShippingcostNext();
 return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_shippingcostend(t_Value *arg_list)
{
t_Value ret_val;

 ret_val.typ = Bool_Value;
 ret_val.nValue = DBShippingcostNext();
 return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_maintenance(t_Value *arg_list)
{
t_Value ret_val;
time_t TimeT;
tm* Today;
char Date[15];
char SQL[501];

CDBField* INDEX;
CDBField* CATEGORYID;
CDBField* PRODUCTID;
CDBField* QUANTITY;

  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;


  time(&TimeT);
  TimeT -= (60* 60*24*30);
  Today = localtime(&TimeT);
  sprintf(Date,"%i%.2i%.2i%.2i%.2i",Today->tm_year+1900,Today->tm_mon+1,
          Today->tm_mday,Today->tm_hour,Today->tm_min);

  fprintf(cgiOut,TEXT_MAINTENANCE1);
  sprintf(SQL,"delete from SESSIONS where LASTUSAGE < '%s' AND OLD=1",Date);
  EngineDB->SQLUpdate(SQL);

  time(&TimeT);
  TimeT -= (60 * 10);
  Today = localtime(&TimeT);

  sprintf(Date,"%i%.2i%.2i%.2i%.2i",Today->tm_year+1900,Today->tm_mon+1,
          Today->tm_mday,Today->tm_hour,Today->tm_min);

  fprintf(cgiOut,TEXT_MAINTENANCE2,Today->tm_mday,Today->tm_mon,
  Today->tm_year+1900,Today->tm_hour,Today->tm_min);

  sprintf(SQL,"select VSINDEX from SESSIONS where LASTUSAGE < '%s' AND OLD=0",Date);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)                      
    {
      do
        {
          INDEX = EngineDB->GetField(MAX_SQLSELECT-1,"VSINDEX");

          fprintf(cgiOut,TEXT_MAINTENANCE3,INDEX->String);
          sprintf(SQL,"delete from SESSIONVAR where VSINDEX='%s'",INDEX->String);
          EngineDB->SQLUpdate(SQL);

          fprintf(cgiOut,TEXT_MAINTENANCE4,INDEX->String);

          sprintf(SQL,"select CATEGORYID,PRODUCTID,QUANTITY from BASKETITEMS where BASKETID='%s'",INDEX->String);
          if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQL) != 0)                      
            {
              do
                {
                  fprintf(cgiOut,".");
                  CATEGORYID = EngineDB->GetField(MAX_SQLSELECT-2,"CATEGORYID");
                  PRODUCTID = EngineDB->GetField(MAX_SQLSELECT-2,"PRODUCTID");
                  QUANTITY = EngineDB->GetField(MAX_SQLSELECT-2,"QUANTITY");
#if !defined(VSHOP_BASIC) && !defined(VSHOP_BASIC)
                  sprintf(SQL,"update INVENTORYCONTROL set ONSTOCK=ONSTOCK+%ld where CATEGORY='%s' AND PRODUCTINDEX='%s'",
                          QUANTITY->Long,CATEGORYID->String,PRODUCTID->String);
                  EngineDB->SQLUpdate(SQL);
#endif
                }
               while(EngineDB->GetNext(MAX_SQLSELECT-2));

               sprintf(SQL,"delete from BASKETITEMS where BASKETID='%s'",INDEX->String);
               EngineDB->SQLUpdate(SQL);
            }
          fprintf(cgiOut,"<BR>\n");
  
          sprintf(SQL,"update SESSIONS set OLD=1 where VSINDEX='%s'",INDEX->String);
          EngineDB->SQLUpdate(SQL);
        }
  
      while(EngineDB->GetNext(MAX_SQLSELECT-1));
      fprintf(cgiOut,TEXT_MAINTENANCE5);
    }
  else
    {
      fprintf(cgiOut,TEXT_MAINTENANCE6);
    }

  ret_val.nValue = 1;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_categoryname(t_Value *arg_list)
{
char SQL[200];
t_Value ret_val;
char Category[41];
CDBField* NAME;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      sprintf(SQL,"select NAME from %s",arg_list[0].sValue);
    }
  else
    {
      if (cgiFormString("CATEGORY",Category,40) != cgiFormNotFound)
        {
          sprintf(SQL,"select NAME from PRODUCTCATEGORIES where NAMEOFCATEGORY='%s'",Category);
        }
      else
        {
          RuntimeError1("%%%%categoryname[PARAMETER1] - PARAMETER1 must be STRING or CATEGORY not set");
          return ret_val;
        }
    }

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) == 0)
    return ret_val;

  if ((NAME = EngineDB->GetField(MAX_SQLSELECT-1,"NAME")) != NULL)
    {
      ret_val.sValue = (char*) malloc(strlen(NAME->String)+1);
      strcpy(ret_val.sValue,NAME->String);
      ret_val.typ = String_Value;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_categorypicture(t_Value *arg_list)
{
char SQL[200];
t_Value ret_val;
char Category[41];
CDBField* PICTURE;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      sprintf(SQL,"select PICTURE from PRODUCTCATEGORIES where NAMEOFCATEGORY='%s'",arg_list[0].sValue);
    }
  else
    {
      if (cgiFormString("CATEGORY",Category,40) != cgiFormNotFound)
        {
          sprintf(SQL,"select PICTURE from PRODUCTCATEGORIES where NAMEOFCATEGORY='%s'",Category);
        }
      else
        {
          RuntimeError1("%%%%categorypicture[PARAMETER1] - PARAMETER1 must be STRING or CATEGORY not set");
          return ret_val;
        }
    }

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) == 0)
    return ret_val;

  if ((PICTURE = EngineDB->GetField(MAX_SQLSELECT-1,"PICTURE")) != NULL)
    {
      ret_val.sValue = (char*) malloc(strlen(PICTURE->String)+1);
      strcpy(ret_val.sValue,PICTURE->String);
      ret_val.typ = String_Value;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_productcount(t_Value *arg_list)
{
char SQL[200];
t_Value ret_val;
char Category[41];
CDBField* COUNT;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      sprintf(SQL,"select COUNT(*) ANZAHL from %s where ACTIVE=1",arg_list[0].sValue);
    }
  else
    {
      if (cgiFormString("CATEGORY",Category,40) != cgiFormNotFound)
        {
          sprintf(SQL,"select COUNT(*) ANZAHL from %s where ACTIVE=1",Category);
        }
      else
        {
          RuntimeError1("%%%%productcount[PARAMETER1] - PARAMETER1 must be STRING or CATEGORY not set");
          return ret_val;
        }
    }

  if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) == 0)
    return ret_val;

  if ((COUNT = EngineDB->GetField(MAX_SQLSELECT-2,"ANZAHL")) != NULL)
    {
      ret_val.nValue = COUNT->Long;
      ret_val.typ = Int_Value;
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_productshow(t_Value *arg_list)
{
char SQL[300];
char Category[41];
char INDEX[41];
t_Value ret_val;


  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  if (arg_list[0].typ == String_Value)
    strcpy(Category,arg_list[0].sValue);
  else
    {
      if (cgiFormString("CATEGORY",Category,40) == cgiFormNotFound)
        {
          RuntimeError1("%%%%productshow[PARAMETER1,PARAMETER2] - PARAMETER1 must be STRING or CATEGORY not set");
          return ret_val;
        }
    }

  if (arg_list[1].typ == String_Value)
    strcpy(INDEX,arg_list[1].sValue);
  else
    {
      if (cgiFormString("VSINDEX",INDEX,40) == cgiFormNotFound)
        {
          RuntimeError1("%%%%productshow[PARAMETER1,PARAMETER2] - PARAMETER2 must be STRING or VSINDEX not set");
          return ret_val;
        }
    }

  InsertEntry("VSPINDEX",INDEX);
  InsertEntry("VSSI","");
  InsertEntry("VSSL","");
  
  sprintf(SQL,"SELECT * from %s where VSINDEX='%s'",Category,INDEX);

  ret_val.nValue = EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL);
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_user(t_Value *arg_list)
{
CDBField* CUSTOMERFIELD;
char SQL[300];
char Buffer[101];
char ID[20];
t_Value ret_val;
char* Value;

  ret_val.typ = Not_Set;
  if (arg_list[0].typ == String_Value)
    {
      if (cgiFormString("LOGIN",Buffer,200) == cgiFormNotFound)
        {
          if (cgiFormString("ID",ID,20) != cgiFormNotFound)
            {
              if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
                {
                  strcpy(Buffer,Value);
                }
              else
                return ret_val;
            }
          else
            return ret_val;
        }

      sprintf(SQL,"select * from CUSTOMERS where LOGIN='%s'",Buffer);

      if (EngineDB->SQLSelect(MAX_SQLSELECT-3,SQL) == 0)
        return ret_val;

      if ((CUSTOMERFIELD = EngineDB->GetField(MAX_SQLSELECT-3,arg_list[0].sValue)) != NULL)
        {
          return DBToVSL(CUSTOMERFIELD);
        }
    }
  else
    {
      RuntimeError1("%%%%user[PARAMETER1] - PARAMETER1 must be STRING");
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

t_Value product_lookup(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      ret_val = DBListGetColumn(arg_list[0].sValue);
    }
  else
    {
      ret_val.typ = Not_Set;
      RuntimeError1("%%%%product[FIELDNAME] - FIELDNAME must be NAME");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value catalog_lookup(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      ret_val = DBCatalogGetColumn(arg_list[0].sValue);
    }
  else
    {
      ret_val.typ = Not_Set;
      RuntimeError1("%%%%catalog.[FIELDNAME] - FIELDNAME must be NAME");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value basket_lookup(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      ret_val = DBBasketGetColumn(arg_list[0].sValue);
    }
  else
    {
      ret_val.typ = Not_Set;
      RuntimeError1("%%%%basket.[FIELDNAME] - FIELDNAME must be NAME");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value payment_lookup(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      ret_val = DBPaymentGetColumn(arg_list[0].sValue);
    }
  else
    {
      ret_val.typ = Not_Set;
      RuntimeError1("%%%%payment.[FIELDNAME] - FIELDNAME must be NAME");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value promotion_lookup(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      ret_val = DBPromotionGetColumn(arg_list[0].sValue);
    }
  else
    {
      ret_val.typ = Not_Set;
      RuntimeError1("%%%%promotion.[FIELDNAME] - FIELDNAME must be NAME");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_haspromotions(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Bool_Value;
  if (arg_list[0].typ == String_Value)
    {
      ret_val.nValue = DBPromotionSelect(arg_list[0]);
    }
  else
    ret_val.nValue = 0;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value relevant_lookup(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      ret_val = DBRelevantGetColumn(arg_list[0].sValue);
    }
  else
    {
      ret_val.typ = Not_Set;
      RuntimeError1("%%%%relevant.[FIELDNAME] - FIELDNAME must be NAME");
    }
  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value shippingcost_lookup(t_Value *arg_list)
{
t_Value ret_val;

  if (arg_list[0].typ == String_Value)
    {
      ret_val = DBShippingcostGetColumn(arg_list[0].sValue);
    }
  else
    {
      ret_val.typ = Not_Set;
      RuntimeError1("%%%%shippingcost.[FIELDNAME] - FIELDNAME must be NAME");
    }
  return ret_val;
}

/*--------------------------------------------------------------------------------*/
t_Value base_allow(t_Value *arg_list)
{
t_Value ret_val;
char ID[20];
char* Value;
char Buffer[200];
char Arg[200];
char SQL[200];
CDBField* GROUP;
char Token[200];
char* Ptr;

  ret_val.typ = Not_Set;

  cgiFormString("ID",ID,20);

  if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
    {
      strcpy(Buffer,Value);
      if ((arg_list[0].typ == String_Value) && (arg_list[1].typ == String_Value))
        {
          if (strlen(arg_list[0].sValue) > 0)
            {
              strcpy(Arg,arg_list[0].sValue);
              
              while ((Ptr = strchr(Arg,',')) != NULL)
                {
                  strcpy(Token,Arg);
                  Token[Ptr-Arg] = '\0';
                  strcpy(Arg,Ptr+1);
                  if (strcmp(Buffer,Token) == 0)
                    return ret_val;
                }

              if (strcmp(Buffer,Arg) == 0)
                return ret_val;
            }

          if (strlen(arg_list[1].sValue) > 0)
            {
              sprintf(SQL,"select CUSTOMERGROUPS.NAME from CUSTOMERGROUPS,CUSTOMERS where CUSTOMERGROUPS.VSINDEX=CUSTOMERS.VSGROUP AND CUSTOMERS.LOGIN='%s'",Buffer);
              if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)
                {
                  if ((GROUP = EngineDB->GetField(MAX_SQLSELECT-1,"NAME")) != NULL)
                    {
                      strcpy(Arg,arg_list[1].sValue);
              
                      while ((Ptr = strchr(Arg,',')) != NULL)
                        {
                          strcpy(Token,Arg);
                          Token[Ptr-Arg] = '\0';
                          strcpy(Arg,Ptr+1);
                          if (strcmp(GROUP->String,Token) == 0)
                            return ret_val;
                        }
                      if (strcmp(GROUP->String,Arg) == 0)
                        return ret_val;

                    }

                }
            }
        }
    }

  printf("<HTML>\n<BODY>\n");
  RuntimeError1("Access denied");
  printf("</BODY>\n</HTML>\n");
  delete EngineDB;
  exit(1);

  return ret_val; 
}
/*--------------------------------------------------------------------------------*/
t_Value base_writeevent(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  if (arg_list[0].typ == Int_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          if (arg_list[2].typ == String_Value)
            {
              ret_val.nValue = WriteEvent(arg_list[0].nValue,arg_list[1].sValue,arg_list[2].sValue);
            }
          else
            RuntimeError1("%%%%writeevent.[Param1,Param2,Param3] - Parameter3 must be STRING");
        }
      else
        RuntimeError1("%%%%writeevent.[Param1,Param2,Param3] - Parameter2 must be STRING");
    }
  else
    RuntimeError1("%%%%writeevent.[Param1,Param2,Param3] - Parameter1 must be INTEGER");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_writelog(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];
FILE* File;

  ret_val.typ = Bool_Value;
  ret_val.nValue = 0;

  if (TEMPLATEROOT[strlen(TEMPLATEROOT)-1] != '/')
    sprintf(Buffer,"%s/admin/log",TEMPLATEROOT);
  else
    sprintf(Buffer,"%sadmin/log",TEMPLATEROOT);

  if (arg_list[0].typ == String_Value)
    {
      if ((File = fopen(Buffer,"a")) == NULL)
        return ret_val;

      fprintf(File,arg_list[0].sValue);
      fprintf(File,"\n");
      fclose(File);
      ret_val.nValue = 1;
    }
  else
    RuntimeError1("%%%%writelog.[Param] - Parameter must be STRING");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_help(t_Value *arg_list)
{
t_Value ret_val;
char HTML[101];
char ID[21];

  ret_val.typ = Not_Set;

  cgiFormString("HTML",HTML,100);
  cgiFormString("ID",ID,20);

  if (arg_list[0].typ == String_Value)
    fprintf(cgiOut,"<A HREF=\"%s?HTML=admin/hilfe/hilfe.html&ID=%s&PAGE=%s\">%s</A>",CGIBIN,ID,HTML,arg_list[0].sValue);
  else
    fprintf(cgiOut,"<A HREF=\"%s?HTML=admin/hilfe/hilfe.html&ID=%s&PAGE=%s\">Hilfe</A>",CGIBIN,ID,HTML);

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_maxpages(t_Value *arg_list)
{
t_Value ret_val;
double Buffer;
double Buffer1;
double Buffer2;

  ret_val.typ = Int_Value;
  ret_val.nValue = 0;

  if (arg_list[0].typ == Int_Value)
    {
      if (arg_list[1].typ == Int_Value)
        {
          Buffer1 = arg_list[0].nValue;
          Buffer2 = arg_list[1].nValue;
          if ((Buffer1 == 0) || (Buffer2 == 0))
            ret_val.nValue = 0;
          else
            {
              Buffer = Buffer2 / Buffer1 + 0.9;
              ret_val.nValue = (int) Buffer;
            }
        }
      else
        RuntimeError1("%%%%maxpages[PARAM1,PARAM2] - PARAMETER2 must be INTEGER");
    }
  else
    RuntimeError1("%%%%maxpages[PARAM1,PARAM2] - PARAMETER1 must be INTEGER");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_aktpage(t_Value *arg_list)
{
t_Value ret_val;
char VSSI[20];
double Buffer;
double Buffer1;
double Buffer2;

  ret_val.typ = Int_Value;
  ret_val.nValue = 0;

  if (cgiFormString("VSSI",VSSI,20) != cgiFormNotFound)
    {
      if (strcmp(VSSI,"") == 0)
        strcpy(VSSI,"1");
    }
  else
    strcpy(VSSI,"1");

  if (arg_list[0].typ == Int_Value)
    {
      Buffer1 = arg_list[0].nValue;
      Buffer2 = atoi(VSSI);
      if ((Buffer1 == 0) || (Buffer2 == 0))
        ret_val.nValue = 0;
      else
        {
          Buffer = (Buffer2 / Buffer1)+0.9;
          ret_val.nValue = (int) Buffer;
        }
    }
  else
    RuntimeError1("%%%%aktpage[PARAM] - PARAMETER must be INTEGER");


  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_pagestart(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Int_Value;

  if (arg_list[0].typ == Int_Value)
    {
      if (arg_list[1].typ == Int_Value)
        {
          ret_val.nValue = arg_list[0].nValue - ((arg_list[0].nValue - 1) % arg_list[1].nValue);
        }
      else
        RuntimeError1("%%%%pagestart[PARAM1,PARAM2] - PARAMETER2 must be INTEGER");
    }
  else
    RuntimeError1("%%%%pagestart[PARAM1,PARAM2] - PARAMETER1 must be INTEGER");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_pageend(t_Value *arg_list)
{
t_Value ret_val;
int Value;
double Maxpages;

  ret_val.typ = Int_Value;

  if (arg_list[0].typ == Int_Value)
    {
      if (arg_list[1].typ == Int_Value)
        {
          if (arg_list[2].typ == Int_Value)
            {
              if (arg_list[3].typ == Int_Value)
                {
                  Value = (arg_list[0].nValue - ((arg_list[0].nValue - 1) % arg_list[1].nValue) + arg_list[1].nValue) - 1;
                  Maxpages = ((int) ((double)arg_list[3].nValue / (double) arg_list[2].nValue + 0.9));

                  if (Value > Maxpages)
                    ret_val.nValue = (int) Maxpages;
                 else
                   ret_val.nValue = Value;
                }
              else
                RuntimeError1("%%%%pageend[PARAM1,PARAM2,PARAM3,PARAM4] - PARAMETER4 must be INTEGER");
            }
          else
            RuntimeError1("%%%%pageend[PARAM1,PARAM2,PARAM3,PARAM4] - PARAMETER3 must be INTEGER");
        }
      else
        RuntimeError1("%%%%pageend[PARAM1,PARAM2,PARAM3,PARAM4] - PARAMETER2 must be INTEGER");
    }
  else
    RuntimeError1("%%%%pageend[PARAM1,PARAM2,PARAM3,PARAM4] - PARAMETER1 must be INTEGER");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_pagelink(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[200];
char Buffer1[20];
char Buffer2[200];
char ID[20];
char HTML[101];


  ret_val.typ = Not_Set;

  if (arg_list[0].typ == Int_Value)
    {
      if (arg_list[1].typ == Int_Value)
        {
          if (cgiFormString("CATEGORY",Buffer1,20) != cgiFormNotFound)
            {
              cgiFormString("HTML",HTML,100);
              strcpy(Buffer,HTML);

              cgiFormString("ID",ID,20);
              sprintf(Buffer2,"%s?HTML=%s&ID=%s&CATEGORY=%s&VSSI=%ld",CGIBIN,Buffer,ID,Buffer1,((arg_list[0].nValue * arg_list[1].nValue)+1)-arg_list[0].nValue);
            }

          ret_val.typ = String_Value;
          ret_val.sValue = (char*) malloc(strlen(Buffer2)+1);
          strcpy(ret_val.sValue,Buffer2);
        }
      else
        RuntimeError1("%%%%pagelink[PARAM1,PARAM2,PARAM3] - PARAMETER2 must be INTEGER");
    }
  else
    RuntimeError1("%%%%pagelink[PARAM1,PARAM2,PARAM3] - PARAMETER1 must be INTEGER");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_pageprev(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[200];
char Buffer1[20];
char Buffer2[200];
char ID[20];
char HTML[101];

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == Int_Value)
    {
      if (arg_list[1].typ == Int_Value)
        {
          if (arg_list[2].typ == Int_Value)
            {
              if (arg_list[0].nValue - arg_list[1].nValue < 0)
                {
                  strcpy(Buffer2,"");
                }
              else
              if (cgiFormString("CATEGORY",Buffer1,20) != cgiFormNotFound)
                {
                  cgiFormString("HTML",HTML,100);
                  strcpy(Buffer,HTML);

                  cgiFormString("ID",ID,20);
                  sprintf(Buffer2,"%s?HTML=%s&ID=%s&CATEGORY=%s&VSSI=%ld",CGIBIN,Buffer,ID,Buffer1,(((arg_list[0].nValue - arg_list[1].nValue) * arg_list[2].nValue)+1)-arg_list[2].nValue);
                }

              ret_val.typ = String_Value;
              ret_val.sValue = (char*) malloc(strlen(Buffer2)+1);
              strcpy(ret_val.sValue,Buffer2);
            }
          else
            RuntimeError1("%%%%pageprev[PARAM1,PARAM2,PARAM3] - PARAMETER3 must be INTEGER");
        }
      else
        RuntimeError1("%%%%pageprev[PARAM1,PARAM2,PARAM3] - PARAMETER2 must be INTEGER");
    }
  else
    RuntimeError1("%%%%pageprev[PARAM1,PARAM2,PARAM3] - PARAMETER1 must be INTEGER");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_pagenext(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[200];
char Buffer1[20];
char Buffer2[200];
char ID[20];
char HTML[101];


  ret_val.typ = Not_Set;

  if (arg_list[0].typ == Int_Value)
    {
      if (arg_list[1].typ == Int_Value)
        {
          if (arg_list[2].typ == Int_Value)
            {
              if ((((arg_list[0].nValue+1) * arg_list[1].nValue) - (arg_list[1].nValue - 1)) > arg_list[2].nValue )
                {
                  strcpy(Buffer2,"");
                }
              else
              if (cgiFormString("CATEGORY",Buffer1,20) != cgiFormNotFound)
                {
                  cgiFormString("HTML",HTML,100);
                  strcpy(Buffer,HTML);

                  cgiFormString("ID",ID,20);
                  sprintf(Buffer2,"%s?HTML=%s&ID=%s&CATEGORY=%s&VSSI=%ld",CGIBIN,Buffer,ID,Buffer1,(((arg_list[0].nValue +1) * arg_list[1].nValue) +1) - arg_list[1].nValue);
                }

              ret_val.typ = String_Value;
              ret_val.sValue = (char*) malloc(strlen(Buffer2)+1);
              strcpy(ret_val.sValue,Buffer2);
            }
          else
            RuntimeError1("%%%%pagenext[PARAM1,PARAM2,PARAM3] - PARAMETER3 must be INTEGER");
        }
      else
        RuntimeError1("%%%%pagenext[PARAM1,PARAM2,PARAM3] - PARAMETER2 must be INTEGER");
    }
  else
    RuntimeError1("%%%%pagenext[PARAM1,PARAM2,PARAM3] - PARAMETER1 must be INTEGER");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_getconfig(t_Value *arg_list)
{
t_Value ret_val;
char* Ptr;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      Ptr = DBGetConfig(arg_list[0].sValue);

      if (Ptr != NULL)
        {
          ret_val.sValue = (char*) malloc(strlen(Ptr)+1);
          strcpy(ret_val.sValue,Ptr);
          ret_val.typ = String_Value;
        }
      else
        {
          ret_val.sValue = (char*) malloc(1);
          ret_val.sValue[0] = '\0';
          ret_val.typ = String_Value;
        }
    }
  else
    RuntimeError1("%%%%getconfig[PARAM,(PARAM)] - PARAM1 must be STRING");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_setconfig(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Not_Set;

  if (arg_list[0].typ == String_Value)
    {
      if (arg_list[1].typ == String_Value)
        {
          if (arg_list[2].typ == String_Value)
            {
              ret_val.nValue = DBSetConfig(arg_list[0].sValue,arg_list[1].sValue,arg_list[2].sValue);
            }
          else
            RuntimeError1("%%%%setconfig[PARAM1,PARAM2,PARAM3] - PARAM3 must be STRING");
        }
      else
        RuntimeError1("%%%%setconfig[PARAM1,PARAM2,PARAM3] - PARAM2 must be STRING");
    }
  else
    RuntimeError1("%%%%setconfig[PARAM1,PARAM2,PARAM3] - PARAM1 must be STRING");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_shopname(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[51];

  ret_val.typ = String_Value;

  strcpy(Buffer,strrchr(CGIBIN,'/')+1);

  ret_val.sValue = (char*) malloc(strlen(Buffer)+1);
  strcpy(ret_val.sValue,Buffer);
  ret_val.typ = String_Value;

  return ret_val; 
}
/*--------------------------------------------------------------------------------*/
t_Value base_testadminip(t_Value *arg_list)
{
t_Value ret_val;

char Buffer[250];
char Buffer2[250];
char* Ptr;
char* Ptr2;
int IPOK = 0;
int Pos;


      strcpy(Buffer2,arg_list[0].sValue);
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
      while ((char*)strcmp(Buffer2,"") != NULL);

    ret_val.typ = Bool_Value;
    ret_val.nValue = IPOK;

    return ret_val;
}
/*--------------------------------------------------------------------------------*/
t_Value base_paymentdescription(t_Value *arg_list)
{
t_Value ret_val;
char SQL[201];
char PaymentID[21];
CDBField* DESCRIPTION;
CDBField* PAYMENTGROUP;
char ID[21];
char* Value;
char Customer[51];

  if (cgiFormString("PAYMENTID",PaymentID,20) == cgiFormNotFound)
    if (arg_list[0].typ == String_Value) 
      strcpy(PaymentID,arg_list[0].sValue);
    else
      {

        RuntimeError1("%%%%paymentdescription[PARAM] - PARAMETER must be STRING");
        ret_val.typ=Not_Set;
        return ret_val; 
      }

  if (cgiFormString("LOGIN",Customer,200) == cgiFormNotFound)
    {
      if (cgiFormString("ID",ID,20) != cgiFormNotFound)
        {
          if ((Value = EngineDB->GetVar(ID,"LOGIN")) != NULL)
            {
              strcpy(Customer,Value);
            }
          else
            strcpy(Customer,"ANONYM");
        }
      else
        strcpy(Customer,"ANONYM");
    }

  sprintf(SQL,"SELECT CUSTOMERGROUPS.PAYMENTGROUP,CUSTOMERGROUPS.VSINDEX,CUSTOMERS.VSGROUP,CUSTOMERS.LOGIN from CUSTOMERGROUPS,CUSTOMERS where CUSTOMERS.VSGROUP=CUSTOMERGROUPS.VSINDEX AND CUSTOMERS.LOGIN='%s'",Customer);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
    {
      PAYMENTGROUP = EngineDB->GetField(MAX_SQLSELECT-1,"PAYMENTGROUP");

      sprintf(SQL,"SELECT DESCRIPTION FROM PAYMENTS WHERE TEMPLATE='%s' AND PAYMENTGROUP='%s'",PaymentID,PAYMENTGROUP->String);
      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
        {
          DESCRIPTION=EngineDB->GetField(MAX_SQLSELECT-1,"DESCRIPTION");
          ret_val.sValue = (char*) malloc(strlen(DESCRIPTION->String)+1);
          strcpy(ret_val.sValue,DESCRIPTION->String);
          ret_val.typ = String_Value;
        }
    }

  return ret_val; 
}
/*--------------------------------------------------------------------------------*/
t_Value base_paymenttemplate(t_Value *arg_list)
{
char SQL[201];
CDBField* PATH;
char PaymentID[21];
t_Value ret_val;

  ret_val.typ=Not_Set;

  if (cgiFormString("PAYMENTID",PaymentID,20) == cgiFormNotFound)
    if (arg_list[0].typ == String_Value) 
      strcpy(PaymentID,arg_list[0].sValue);
    else
      {

        RuntimeError1("%%%%paymenttemplate[PARAM] - PARAMETER must be STRING");
        return ret_val; 
      }

  sprintf(SQL,"SELECT HTML_TEMPLATES.PATH from HTML_TEMPLATES WHERE VSINDEX='%s'",PaymentID);
  EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL);

  if ((PATH=EngineDB->GetField(MAX_SQLSELECT-1,"PATH")) != NULL)
    {
      ret_val.sValue = (char*) malloc(strlen(PATH->String)+1);
      strcpy(ret_val.sValue,PATH->String);
      ret_val.typ = String_Value;
    }

  return ret_val;
}

/*--------------------------------------------------------------------------------*/
t_Value base_install(t_Value *arg_list)
{
t_Value ret_val;
char Buffer[500];

  ret_val.typ = Int_Value;

  if (arg_list[0].typ == String_Value)
    {
      sprintf(Buffer,"%s/admin/modules/",TEMPLATEROOT);

      #ifndef WIN32
       chdir(Buffer);
      #else
       _chdir(Buffer);
      #endif

      sprintf(Buffer,"tar xzf %s",arg_list[0].sValue);

      EngineDB->Commit();
      ret_val.nValue = system(Buffer);


      #ifndef WIN32
       chdir(PROGRAMPATH);
      #else
       _chdir(PROGRAMPATH);
      #endif
    }
  else
    RuntimeError1("%%%%install[PARAM1] - PARAM1 must be STRING");

  return ret_val;
}

/*--------------------------------------------------------------------------------*/


#ifdef   __cplusplus 
}
#endif


#endif
