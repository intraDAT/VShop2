#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifndef DB_ORACLE
  #include "adabas.h"
#else
  #include "oracle.h"
#endif
#include "inifile.h"
#include "hfct.h"
#include "cgic.h"
#include "skey.h"
/* stefan include*/
#include <strings.h>

/* stefan */
CEngineDB* EngineDB;




#ifdef HPUX
  #include <ctype.h>
#endif

FILE *fh;
/*---------------------------------------------------------------------------------*/
char* DBGetConfig(char* Setting)
{
char SQL[121];
CDBField* DBField;

  sprintf(SQL,"select VALUE from SYSTEMPREFERENCES where SETTING='%s'",Setting);
  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 1)
    return NULL;

  if ((DBField = EngineDB->GetField(MAX_SQLSELECT-1,"VALUE")) == NULL)
    return NULL;

  return DBField->String;
}
/*--------------------------------------------------------------------------------*/
float OrdersSum(char* OrderID)
{
char SQL[201];
char Buffer[41];

CDBField* CUSTOMDISC;
CDBField* CUSTOMVAT;
CDBField* QUANTITY;
CDBField* PRICE;

float Sum = 0;
float Price;
	

      sprintf(SQL,"SELECT * from ORDERITEMS where ORDERID='%s'",OrderID);
      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
        {
          do
            {
              PRICE =  EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");
              CUSTOMDISC =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMDISC");
              CUSTOMVAT =  EngineDB->GetField(MAX_SQLSELECT-1,"CUSTOMVAT");
              QUANTITY =  EngineDB->GetField(MAX_SQLSELECT-1,"QUANTITY");

              PRICE = EngineDB->GetField(MAX_SQLSELECT-1,"PRICE");

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
int SavePaymentData(char* PaymentID,char* AID, int Flag)
{
char Buffer[5000];


  if (EngineDB->SQLInsert("PAYMENTDATA",PaymentID) == 0)
    {
      return 0;
    }

  switch(Flag)
    {
      case 0:
        sprintf(Buffer,"UPDATE PAYMENTDATA SET PAYMENTNAME='VISA',DATA_STRING1='%s'",AID);
        break;
      case 1:
        sprintf(Buffer,"UPDATE PAYMENTDATA SET PAYMENTNAME='Mastercard',DATA_STRING1='%s'",AID);
        break;
      case 2:
        sprintf(Buffer,"UPDATE PAYMENTDATA SET PAYMENTNAME='Amex',DATA_STRING1='%s'",AID);
        break;
      case 3:
        sprintf(Buffer,"UPDATE PAYMENTDATA SET PAYMENTNAME='Diners',DATA_STRING1='%s'",AID);
        break;
      case 4:
        sprintf(Buffer,"UPDATE PAYMENTDATA SET PAYMENTNAME='Geldkarte',DATA_STRING1='%s'",AID);
        break;
      case 5:
        sprintf(Buffer,"UPDATE PAYMENTDATA SET PAYMENTNAME='ELastschrift',DATA_STRING1='%s'",AID);
        break;
    }

  strcat(Buffer," WHERE VSINDEX='");
  strcat(Buffer,PaymentID);
  strcat(Buffer,"'");

  return EngineDB->SQLUpdate(Buffer);
}
/*----------------------------------------------------------------------*/
int RCIn()
{
char MID[100];
char RENR[100];
char DATUM[100];
char AID[100];
char VORGART[100];
char UEKZ[100];
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
CDBField* TOTAL_REVENUE;
CDBField* CUSTOMERLOGIN;
CDBField* CRITERIA1;
CDBField* CRITERIA2;
CDBField* BASKETOPTIONINDEX;
CDBField* ORDERITEMSINDEX;
CDBField* ORDERITEMSORDERID;

char Customer[51];
char PaymentData[21];
char ID[21];
char* Value;
float TotalRevenue = 0;
char Buffer[200];
int Count;
char* Ptr;
char DBUser[50];
char DBPassword[50];
char DBNode[50];
char DBName[50];
int Flag;
time_t TimeT;
tm* Today;

char CARDNR[50];
char RETCODE[50];
char SID[50];
double WholePrice = 0;

  time(&TimeT);
  Today = localtime(&TimeT);
  strftime(Buffer,50,"%d.%m.%Y %H:%M",Today);
  
  cgiFormString("MID",MID,50);
  cgiFormString("RENR",RENR,50);
  cgiFormString("DATUM",DATUM,50);
  cgiFormString("AID",AID,50);
  cgiFormString("VORGART",VORGART,50);
  cgiFormString("UEKZ",UEKZ,50);
  cgiFormString("RETCODE",RETCODE,50);

  fprintf(fh,"Aufruf von RCIn\n");
  fprintf(fh,"DATUM   :  %s \n",DATUM);
  fprintf(fh,"RENR    :  %s \n",RENR);
  fprintf(fh,"AID     :  %s \n",AID);
  fprintf(fh,"VORGART :  %s \n",VORGART);
  fprintf(fh,"UEKZ    :  %s \n",UEKZ);
  fprintf(fh,"RETCODE :  %s \n",RETCODE);

  if (strcmp(UEKZ,"1") == 0)
    {
   strcpy(PROGRAMNAME,&RENR[1]);
  for(Count = 0; Count < (int) strlen(PROGRAMNAME); Count++)
    {
      if (PROGRAMNAME[Count] == '#')
        {
          strcpy(ID,&PROGRAMNAME[Count+1]);
          PROGRAMNAME[Count] = '\0';
          if ((Ptr = strchr(ID,' ')) != NULL)
            *Ptr = 0;
          break;
        }
    }

      if ((Ptr = strchr(AID,' ')) != NULL)
        *Ptr = 0;


      RENR[1] = '\0';
      Flag = atoi(RENR);

      if (!OpenIniFile("/etc/vshop.conf"))
        {
          fprintf(fh,"Fehler beim ôffnen der Datei '/etc/vshop.conf'\n");
          return 0;
        }

      if((Ptr = GetFirstParam("DBROOT")) != NULL)
        {
            sprintf(Buffer,"DBROOT=%s",Ptr);
            putenv(Buffer);
	/* stefan 
          #ifdef HPUX
            sprintf(Buffer,"DBROOT=%s",Ptr);
            putenv(Buffer);
          #else
            setenv("DBROOT",Ptr,1);
          #endif
	*/
        }
      else
       {
         CloseIniFile();
         fprintf(fh,"Fehler beim Lesen von DBROOT in '/etc/vshop.conf'\n");
         return 0;
       }

      sprintf(Buffer,"%s_DBUSER",PROGRAMNAME);
      if((Ptr = GetFirstParam(Buffer)) != NULL)
        strcpy(DBUser,Ptr);
      else
       {
         CloseIniFile();
         fprintf(fh,"Fehler beim Lesen von %s in '/etc/vshop.conf'\n",Buffer);
         return 0;
       }

      sprintf(Buffer,"%s_DBPASSWORD",PROGRAMNAME);
      if((Ptr = GetFirstParam(Buffer)) != NULL)
        strcpy(DBPassword,Ptr);
      else
       {
         CloseIniFile();
         fprintf(fh,"Fehler beim Lesen von %s in '/etc/vshop.conf'\n",Buffer);
         return 0;
       }

      sprintf(Buffer,"%s_DBNODE",PROGRAMNAME);
      if((Ptr = GetFirstParam(Buffer)) != NULL)
        strcpy(DBNode,Ptr);
      else
        strcpy(DBNode,"");

      sprintf(Buffer,"%s_DBNAME",PROGRAMNAME);
      if((Ptr = GetFirstParam(Buffer)) != NULL)
        strcpy(DBName,Ptr);
      else
        strcpy(DBName,"vshop");
	
      CloseIniFile();

      EngineDB = new CEngineDB();

      #ifndef PIPS16
        decryptpassword (&Ptr, DBPassword, "VSHOPDBPASSWORD");
        strcpy(DBPassword,Ptr);
      #endif


      if( EngineDB->Connect(DBName,DBUser, DBPassword,DBNode) == 0)
        fprintf(fh,"Connect to %s with %s,%s failed\n",DBName,DBUser,DBPassword);	

      sprintf(SQL,"select PRODUCTCATEGORIES.NAMEOFCATEGORY,BASKETITEMS.* from PRODUCTCATEGORIES,BASKETITEMS where PRODUCTCATEGORIES.VSINDEX=BASKETITEMS.CATEGORYID AND BASKETID='%s'",ID);

      #ifdef DEBUG
        fprintf(fh,"%s\n\n",SQL);
      #endif

      if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) == 0)
        {
          fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
          delete EngineDB;
          return 0;
        }

      if (EngineDB->SQLInsert("ORDERS",NewOrderID) == 0)
        {
          fprintf(fh,"Fehler beim EinfÅgen in ORDERS\n");
          delete EngineDB;
          return 0;
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

          sprintf(SQL,"select PRICE,NAME,SHORTDESCR from %s where VSINDEX='%s'",CATEGORYNAME->String,PRODUCTID->String);  

          #ifdef DEBUG
            fprintf(fh,"%s\n\n",SQL);
          #endif

          if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
            {
              PRICE = EngineDB->GetField(MAX_SQLSELECT-2,"PRICE");
              NAME = EngineDB->GetField(MAX_SQLSELECT-2,"NAME");
              SHORTDESCR = EngineDB->GetField(MAX_SQLSELECT-2,"SHORTDESCR");
            }
          else
            {
              fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
              delete EngineDB;
              return 0;
            }

          if (EngineDB->SQLInsert("ORDERITEMS",NewOrderItemsID) == 0)
            {
              fprintf(fh,"Fehler beim EinfÅgen in ORDERSITEMS\n");
              delete EngineDB;
              return 0;
            }
	    
          sprintf(SQL,"update ORDERITEMS set ORDERID='%s',CATEGORYID='%s',PRODUCTID='%s',QUANTITY=%ld,NAMEOFCATEGORY='%s',PRODUCTNAME='%s',SHORTDESCR='%s',PRICE=%f,CUSTOMDISC=%f,CUSTOMVAT=%f, CRITERIA1='%s',CRITERIA2 ='%s' where VSINDEX='%s'",
                  NewOrderID,CATEGORYID->String,PRODUCTID->String,QUANTITY->Long,CATEGORYNAME->String,
                  NAME->String,SHORTDESCR->String,PRICE->Double,CUSTOMDISC->Double,CUSTOMVAT->Double,
		  CRITERIA1->String,CRITERIA2->String,NewOrderItemsID);

          #ifdef DEBUG
            fprintf(fh,"%s\n\n",SQL);
          #endif

          if (EngineDB->SQLUpdate(SQL) == 0)
            {
              fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
              delete EngineDB;
              return 0;
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
                      fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
                      delete EngineDB;
                      return 0;
                    }

                  sprintf(SQL,"insert into ORDEROPTION select * from BASKETOPTION where VSINDEX='%s'",BASKETOPTIONINDEX->String);
		  
                  if (EngineDB->SQLUpdate(SQL) == 0)
                    {
                      fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
                      delete EngineDB;
                      return 0;
                    }
		}
              EngineDB->GetNext(MAX_SQLSELECT-1);  		
	    }
	}  

      if (SavePaymentData(PaymentData,AID,Flag) == 0)
        {
          fprintf(fh,"Fehler beim speichern in PAYMENTS\n");
          delete EngineDB;
          return 0;
        }
	
      if(cgiFormString("CARDNR",CARDNR,50) != cgiFormNotFound)
        {
	  cgiFormString("RETCODE",RETCODE,50);
          cgiFormString("SID",SID,50);
	  sprintf(SQL,"update PAYMENTDATA set DATA_STRING3='%s',DATA_STRING4='%s/%s' WHERE VSINDEX='%s'",
 	          CARDNR,RETCODE,SID,PaymentData);
	
	  EngineDB->SQLUpdate(SQL);
	}

      sprintf(SQL,"SELECT VALUE FROM SESSIONVAR WHERE NAME='LOGIN' AND VSINDEX='%s'",ID);
      #ifdef DEBUG
        fprintf(fh,"%s\n\n",SQL);
      #endif

      if (EngineDB->SQLSelect(MAX_SQLSELECT-1, SQL) != 0)
        {
          CUSTOMERLOGIN = EngineDB->GetField(MAX_SQLSELECT-1,"VALUE");
          strcpy(Customer,CUSTOMERLOGIN->String);
        }
      else
        {
          fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
          delete EngineDB;
          return 0;
        }

      sprintf(SQL,"SELECT VSINDEX,TOTAL_REVENUE FROM CUSTOMERS WHERE LOGIN='%s'",Customer);

      #ifdef DEBUG
        fprintf(fh,"%s\n\n",SQL);
      #endif

      if (EngineDB->SQLSelect(MAX_SQLSELECT-2, SQL) != 0)
        {
          CUSTOMER = EngineDB->GetField(MAX_SQLSELECT-2,"VSINDEX");
          strcpy(Customer,CUSTOMER->String);
          TOTAL_REVENUE = EngineDB->GetField(MAX_SQLSELECT-2,"TOTAL_REVENUE");
          TotalRevenue = TOTAL_REVENUE->Double;
        }
      else
        strcpy(Customer,"UNKNOWN");
	
      WholePrice = OrdersSum(NewOrderID);
      TotalRevenue += WholePrice;
      

      sprintf(SQL,"update PAYMENTDATA set ORDERID='%s',CUSTOMERID='%s',DATA_STRING2='%s' WHERE VSINDEX='%s'",
              NewOrderID,Customer,ID,PaymentData);

      #ifdef DEBUG
        fprintf(fh,"%s\n\n",SQL);
      #endif

      if (EngineDB->SQLUpdate(SQL) == 0)
        {
          fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
          delete EngineDB;
          return 0;
        }
      
      sprintf(SQL,"update ORDERS set CUSTOMER='%s',PAYMENTDATA='%s', PAID = %f where VSINDEX='%s'",
              Customer,PaymentData,WholePrice,NewOrderID);

      #ifdef DEBUG
        fprintf(fh,"%s\n\n",SQL);
      #endif

      if (EngineDB->SQLUpdate(SQL) == 0)
        {
          fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
          delete EngineDB;
          return 0;
        }

      sprintf(SQL,"update CUSTOMERS set TOTAL_REVENUE=%.2f where VSINDEX='%s'",
              TotalRevenue,Customer);

      #ifdef DEBUG
        fprintf(fh,"%s\n\n",SQL);
      #endif

      if (EngineDB->SQLUpdate(SQL) == 0)
        {
          fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
          delete EngineDB;
          return 0;
        }

      sprintf(SQL,"delete from BASKETITEMS where BASKETID='%s'",ID);
      #ifdef DEBUG
        fprintf(fh,"%s\n\n",SQL);
      #endif
      if (EngineDB->SQLUpdate(SQL) == 0)
        {
          fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
          delete EngineDB;
          return 0;
        }

      sprintf(SQL,"delete from BASKETOPTION where BASKETID='%s'",ID);
      #ifdef DEBUG
        fprintf(fh,"%s\n\n",SQL);
      #endif
      if (EngineDB->SQLUpdate(SQL) == 0)
        {
        }
	
	
      fprintf(fh,"Bestellung verbucht\n");
    }

  delete EngineDB;
  return 1;
}
/*----------------------------------------------------------------------*/
void RCOut(int Error)
{
  printf("Content-type: application/x-www-form-urlencoded\n");
  if (Error == 0)
    {
      fprintf(fh,"RÅckgabe an das IPS : Fehler \n\n");
      printf("Content-length: 15\n\n");
      printf("Quittung=Fehler");
    }
  else
    {
      fprintf(fh,"RÅckgabe an das IPS : Korrekt \n\n");
      printf("Content-length: 16\n\n");
      printf("Quittung=Korrekt");
    }
}
/*----------------------------------------------------------------------*/
void OC()
{
char Buffer[200];
char PostBuffer[10000];
char MID[50];
char RENR[50];
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
CDBField* HKEY_TEMP;

CDBField* PRODUCTID2;
CDBField* CUSTOMDISC2;
CDBField* CUSTOMVAT2;
CDBField* QUANTITY2;
CDBField* NAMEOFCATEGORY2;
CDBField* CATEGORYID2;


char HKEY[50];
CDBField* HID_TEMP;
char HID[50];
CDBField* FURL_TEMP;
char FURL[200];
char ID[21];
int Count;
float Price;
float Sum = 0;
char Filled1[200];
char Filled2[200];
char PROGRAMNAME[50];
char* Ptr;
char DBUser[50];
char DBPassword[50];
char DBNode[50];
char DBName[50];
time_t TimeT;
tm* Today;
char CURRENCY[10];

  time(&TimeT);
  Today = localtime(&TimeT);
  strftime(Buffer,50,"%d.%m.%Y %H:%M",Today);

  cgiFormString("MID",MID,50);
  cgiFormString("RENR",RENR,50);

  fprintf(fh,"Aufruf von OC\n");
  fprintf(fh,"MID  :  %s\n",MID);
  fprintf(fh,"RENR :  %s\n",RENR);

  strcpy(PROGRAMNAME,&RENR[1]);
  for(Count = 0; Count < (int) strlen(PROGRAMNAME); Count++)
    {
      if (PROGRAMNAME[Count] == '#')
        {
          strcpy(ID,&PROGRAMNAME[Count+1]);
          PROGRAMNAME[Count] = '\0';
          if ((Ptr = strchr(ID,' ')) != NULL)
            *Ptr = 0;
          break;
        }
    }

  if (!OpenIniFile("/etc/vshop.conf"))
    {
      fprintf(fh,"Fehler beim ôffnen der Datei '/etc/vshop.conf'\n");
      return;
    }

  if((Ptr = GetFirstParam("DBROOT")) != NULL)
    {
       sprintf(Buffer,"DBROOT=%s",Ptr);
       putenv(Buffer);

    /* stefan 
      #ifdef HPUX
        sprintf(Buffer,"DBROOT=%s",Ptr);
        putenv(Buffer);
      #else
        //setenv("DBROOT",Ptr,1);
      #endif
    */
    }
  else
   {
     CloseIniFile();
     fprintf(fh,"Fehler beim Lesen von %s in '/etc/vshop.conf'\n",Buffer);
     return ;
   }

  sprintf(Buffer,"%s_DBUSER",PROGRAMNAME);
      
  if((Ptr = GetFirstParam(Buffer)) != NULL)
    strcpy(DBUser,Ptr);
  else
   {
     CloseIniFile();
     fprintf(fh,"Fehler beim Lesen von %s in '/etc/vshop.conf'\n",Buffer);
     return ;
   }
  sprintf(Buffer,"%s_DBPASSWORD",PROGRAMNAME);
  if((Ptr = GetFirstParam(Buffer)) != NULL)
    strcpy(DBPassword,Ptr);
  else
   {
     CloseIniFile();
     fprintf(fh,"Fehler beim Lesen von %s in '/etc/vshop.conf'\n",Buffer);
     return ;
   }

      sprintf(Buffer,"%s_DBNODE",PROGRAMNAME);
      if((Ptr = GetFirstParam(Buffer)) != NULL)
        strcpy(DBNode,Ptr);
      else
        strcpy(DBNode,"");

      sprintf(Buffer,"%s_DBNAME",PROGRAMNAME);
      if((Ptr = GetFirstParam(Buffer)) != NULL)
        strcpy(DBName,Ptr);
      else
        strcpy(DBName,"vshop");

  CloseIniFile();

  EngineDB = new CEngineDB();
  
      #ifndef PIPS16
        decryptpassword (&Ptr, DBPassword, "VSHOPDBPASSWORD");
        strcpy(DBPassword,Ptr);
      #endif
	if( EngineDB->Connect(DBName,DBUser, DBPassword,DBNode) == 0) 
           fprintf(fh,"Connect to %s with %s,%s failed\n",DBName,DBUser,DBPassword);


  sprintf(SQL,"select MID,HID,HKEY,FURL from IVPROPERTIES");

  if (EngineDB->SQLSelect(MAX_SQLSELECT-1,SQL) != 0)
    {
      MID_TEMP =  EngineDB->GetField(MAX_SQLSELECT-1,"MID");
      if (strcmp(MID,MID_TEMP->String) != 0)
        {
          fprintf(fh,"MID nicht gleich !\n");
          fprintf(fh,"Empfangen : '%s'\n",MID);
          fprintf(fh,"Erwartet  : '%s'\n",MID_TEMP->String);
          return;
        }

      HKEY_TEMP =  EngineDB->GetField(MAX_SQLSELECT-1,"HKEY");
      strcpy(HKEY,HKEY_TEMP->String);
      HID_TEMP =  EngineDB->GetField(MAX_SQLSELECT-1,"HID");
      strcpy(HID,HID_TEMP->String);
      FURL_TEMP =  EngineDB->GetField(MAX_SQLSELECT-1,"FURL");
      strcpy(FURL,FURL_TEMP->String);
    }
  else
    {
      fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
      return;
    }

  if ((Ptr = DBGetConfig("PRIMARY_CURRENCY")) != NULL)
    strcpy(CURRENCY,Ptr);
    
  if (strcmp(CURRENCY,"DM") == 0)
    strcpy(CURRENCY,"DEM");

  Count = 0;
  sprintf(SQL,"SELECT PRODUCTCATEGORIES.NAMEOFCATEGORY, BASKETITEMS.* from PRODUCTCATEGORIES,BASKETITEMS where PRODUCTCATEGORIES.VSINDEX=BASKETITEMS.CATEGORYID AND BASKETID='%s'",ID);


  #ifdef DEBUG
    fprintf(fh,"%s\n\n",SQL);
  #endif

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

          sprintf(SQL,"SELECT PRICE FROM %s where VSINDEX='%s'",NAMEOFCATEGORY->String,PRODUCTID->String);

          #ifdef DEBUG
            fprintf(fh,"%s\n\n",SQL);
          #endif

          if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
            {
              PRICE = EngineDB->GetField(MAX_SQLSELECT-2,"PRICE");
              Price = PRICE->Double;
              Price = Price - ((Price / 100) * CUSTOMDISC->Double);
              Price = Price + ((Price / 100) * CUSTOMVAT->Double);
              sprintf(Buffer,"%.2f",Price);
              Price= atof(Buffer);

              Sum += Price * QUANTITY->Long;
              Count++;

            }
          else
            {
              fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
              return;
            }
        }
      while (EngineDB->GetNext(MAX_SQLSELECT-1));
    }
  else
    {
      fprintf(fh,"Fehler beim Senden von :  %s \n",SQL);
      return;
    }
      
  FillZeroFloat(Filled1,Sum,12);
  FillZeroInt(Filled2,Count,2);

  printf("Content-type: application/x-www-form-urlencoded\n");

  RENR[strlen(RENR)-1] = '\0';

  sprintf(Buffer,"KUNID=&MID=%s&HKEY=%s&RENR=%s&REBET=%s&WKZ=%s&FURL=%s&ANZPOS=%s",MID,HKEY,RENR,Filled1,CURRENCY,FURL,Filled2);
  strcpy(PostBuffer,Buffer);
  
  Count = 0;
  sprintf(SQL,"SELECT PRODUCTCATEGORIES.NAMEOFCATEGORY, BASKETITEMS.* from PRODUCTCATEGORIES,BASKETITEMS where PRODUCTCATEGORIES.VSINDEX=BASKETITEMS.CATEGORYID AND BASKETID='%s'",ID);

  #ifdef DEBUG
    fprintf(fh,"%s\n\n",SQL);
  #endif

  if (EngineDB->SQLSelect(MAX_SQLSELECT-3,SQL) != 0)
    {
      do
        {
          Count++;
          PRODUCTID2 =  EngineDB->GetField(MAX_SQLSELECT-3,"PRODUCTID");
          CUSTOMDISC2 =  EngineDB->GetField(MAX_SQLSELECT-3,"CUSTOMDISC");
          CUSTOMVAT2 =  EngineDB->GetField(MAX_SQLSELECT-3,"CUSTOMVAT");
          QUANTITY2 =  EngineDB->GetField(MAX_SQLSELECT-3,"QUANTITY");
          NAMEOFCATEGORY2 = EngineDB->GetField(MAX_SQLSELECT-3,"NAMEOFCATEGORY");
          CATEGORYID2 = EngineDB->GetField(MAX_SQLSELECT-3,"CATEGORYID");

          sprintf(SQL,"SELECT NAME,PRICE FROM %s where VSINDEX='%s'",NAMEOFCATEGORY2->String,PRODUCTID2->String);

          #ifdef DEBUG
            fprintf(fh,"%s\n\n",SQL);
          #endif


          if (EngineDB->SQLSelect(MAX_SQLSELECT-2,SQL) != 0)
            {
              PRICE = EngineDB->GetField(MAX_SQLSELECT-2,"PRICE");
              NAME = EngineDB->GetField(MAX_SQLSELECT-2,"NAME");

              Price = PRICE->Double;
              Price = Price - ((Price / 100) * CUSTOMDISC2->Double);
              Price = Price + ((Price / 100) * CUSTOMVAT2->Double);
              sprintf(Buffer,"%.2f",Price);
              Price= atof(Buffer);
              #ifdef DEBUG
                fprintf(fh,"Price : %f\n\n",Price);
              #endif

              Sum += Price * QUANTITY2->Long;
              Replace(NAME->String,";"," ");
              sprintf(Buffer,"&HID%i=%s&ARTNR%i=%s/%s&ARTBEZ%i=%s&MENGE%i=%s&EPREIS%i=%s",
                      Count,HID,Count,CATEGORYID2->String,PRODUCTID2->String,
                      Count,NAME->String,Count,FillZeroInt(Filled1,QUANTITY2->Long,5),
                      Count,FillZeroFloat(Filled2,Price,12));
            }
          strcat(PostBuffer,Buffer);
         }
      while (EngineDB->GetNext(MAX_SQLSELECT-3));
    }

  printf("Content-length: %i\n\n",strlen(PostBuffer));
  printf(PostBuffer);

  fprintf(fh,"RÅckgabe an das IPS :  %s\n\n",PostBuffer);

  delete EngineDB;

}
/******************************************************************************************/
// Main
/******************************************************************************************/
int cgiMain(int Flag)
{
time_t TimeT;
tm* Today;
char Buffer[100];

  time(&TimeT);
  Today = localtime(&TimeT);
  strftime(Buffer,50,"%d.%m.%Y %H:%M:%S",Today);

  fh=fopen("pips.log","a");
  if (fh)
    {
      fprintf(fh,"--------------------------------------------------------------\n");
      #ifdef DEBUG
        fprintf(fh,"PIPS 2.1.1 (DEBUG) %s\n",Buffer);
      #else
        fprintf(fh,"PIPS 2.1.1 %s\n",Buffer);
      #endif
      fprintf(fh,"--------------------------------------------------------------\n");

      if (strlen(cgiPathInfo) > 0)
        {
          if (strncasecmp(cgiPathInfo,"/RC",strlen(cgiPathInfo)) == 0)
            RCOut(RCIn());
          else
          if (strncasecmp(cgiPathInfo,"/OC",strlen(cgiPathInfo)) == 0)
            OC();
          else
            {
              fprintf(fh,"Falscher Aufruf :  %s\n\n",cgiPathInfo);
            }
        }
      else
        {
          fprintf(fh,"Aufruf ohne cgiPathInfo\n\n");
        }
  fprintf(fh,"\n");
  fclose(fh);
 }

  return 0;
}
