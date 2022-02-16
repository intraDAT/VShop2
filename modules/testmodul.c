
#include "vshop_modul.h"

#ifdef __cplusplus 
extern "C" {
#endif


/*--------------------------------------------------------------------------------*/  
/* Beispiel fuer eine einfache Funktion mit Parameter */
/* Ohne Rueckgabe */
/*--------------------------------------------------------------------------------*/  
t_Value base_testfunc1(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Not_Set;
  
  if (arg_list[0].typ == String_Value)
    printf("Ihr Parameter war : %s",arg_list[0].sValue);
  else
    printf("<H2> Falscher (keine Zeichenkette) oder kein Parameter </H2>");

  return ret_val;
}
/*--------------------------------------------------------------------------------*/  
/* Beispiel fuer eine Funktion mit Reuckgabe */
/* 1. Parameter ist der Tabellenname */
/* 2. Parameter ist der Spaltenname */
/* Beispielaufruf : %%testfunc3["sessions","index"]
/*--------------------------------------------------------------------------------*/  
t_Value base_testfunc2(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Int_Value;
  ret_val.nValue = 2;

  return ret_val;
}
/*--------------------------------------------------------------------------------*/  
/* Beispiel fuer eine Funktion mit Datenbankabfrage */
/* Ohne Parameter */
/* Ohne Rueckgabe */
/*--------------------------------------------------------------------------------*/  
t_Value base_testfunc3(t_Value *arg_list)
{
t_Value ret_val;

  ret_val.typ = Not_Set;

  DBSelect(1,"select * from sessions");
  while(!(DBIsEOF(1)).nValue)
    {
      ret_val = DBGetColumn(1, "INDEX");
      if (ret_val.typ == String_Value)
        {
          printf("%s<BR>",ret_val.sValue);
          free(ret_val.sValue);
        }
      DBGetNext(1);
    }

  return ret_val;
}
/*--------------------------------------------------------------------------------*/  
/* Beispiel fuer eine Funktion mit Datenbankabfrage */
/* 1. Parameter ist der Tabellenname */
/* 2. Parameter ist der Spaltenname */
/* Ohne Rueckgabe */
/* Beispielaufruf : %%testfunc3["sessions","index"]
/*--------------------------------------------------------------------------------*/  
t_Value base_testfunc4(t_Value *arg_list)
{
t_Value ret_val;
char SQLBuffer[101];

  ret_val.typ = Not_Set;


  if ((arg_list[0].typ == String_Value) && (arg_list[1].typ == String_Value))
    {
      sprintf(SQLBuffer,"select * from %s",arg_list[0].sValue);
      if (DBSelect(1,SQLBuffer).nValue)
        {
          do
            {
              ret_val = DBGetColumn(1, arg_list[1].sValue);
              if (ret_val.typ == String_Value)
                {
                  printf("%s<BR>\n",ret_val.sValue);
                  free(ret_val.sValue);
                }
              DBGetNext(1);
            }
          while(!(DBIsEOF(1)).nValue);
        }
    }
  else
    printf("<H2> Falsche(r) (keine Zeichenkette) oder keine 2 Parameter </H2>");


      return ret_val;
}
/*--------------------------------------------------------------------------------*/  
t_Value base_createview(t_Value *arg_list)
{
t_Value ret_val;
char SQLBuffer[255];
char Field[19];

  ret_val.typ = Bool_Value;


  if ((arg_list[0].typ == String_Value) && 
      (arg_list[1].typ == String_Value) &&
      (arg_list[1].typ == String_Value))
    {

      sprintf(SQLBuffer,"select * from %s",arg_list[0].sValue);



      sprintf(SQLBuffer,
              "create or replace view %s as select %s from %s union select %s from %s",
              arg_list[0].nValue,
              Field,
              arg_list[1].nValue,
              Field,
              arg_list[2].nValue);

      ret_val.nValue = DBSSQL(SQLBuffer);
    }
  else
    {
      printf("<b> createview benoetigt 3 Parameter (STRING) </b>");
    }


  
  
  
  && (arg_list[0].typ == String_Value))
    {
      sprintf(SQLBuffer,"select * from %s",arg_list[0].sValue);
      if (DBSelect(1,SQLBuffer).nValue)
        {
          do
            {
              ret_val = DBGetColumn(1, arg_list[1].sValue);
              if (ret_val.typ == String_Value)
                {
                  printf("%s<BR>\n",ret_val.sValue);
                  free(ret_val.sValue);
                }
              DBGetNext(1);
            }
          while(!(DBIsEOF(1)).nValue);
        }
    }
  else
    printf("<H2> Falsche(r) (keine Zeichenkette) oder keine 2 Parameter </H2>");


      return ret_val;
}

/*--------------------------------------------------------

Funktion zur Umrechnung von DM-Betraegen in EURO.

Parameter : DM-Betrag als Integer, Float oder String
Rueckgabe : EURO-Betrag als Float

---------------------------------------------------------*/  
t_Value base_dm2euro(t_Value *arg_list)
{
double DM;

/* Variable zum Speichern der Rueckgabe vom Typ */
/* t_Value (siehe Definition der Variablenstruktur */
t_Value ret_val;


  /* Ueberpruefen von welchem Typ der 1. Parameter ist */

  /* Ist Parameter1 vom Typ String_Value (Zeichenkette) ? */
  if (arg_list[0].typ == String_Value)
    {
      /* Ja, dann in Kommazahl umwandeln und merken */
      DM = atof(arg_list[0].sValue);
    }
  else
  /* Ist Parameter1 vom Typ Int_Value (Ganzzahl) ? */
  if (arg_list[0].typ == Int_Value)
    {
      /* Ja, dann in Kommazahl umwandeln und merken */
      DM = arg_list[0].nValue;
    }
  else
  /* Ist Parameter1 vom Typ Float_Value (Kommazahl) ? */
  if (arg_list[0].typ == Float_Value)
    {
      /* Ja, dann merken */
      DM = arg_list[0].dValue;
    }
  /* Dann ist der 1. Parameter fuer diese Funktion unzulaessig */
  else
    {
      /* Fehlermeldung ausgeben */
      printf("<b> dm2euro : Falscher Parameter </b> <br>");

      /* Rueckgabetyp auf Not_Set (Nicht gesetzt) setzen */
      ret_val.typ = Not_Set;

      /* und raus */
      return  ret_val;
    }

  /* Rueckgabetyp auf Float_Value (Kommazahl) setzen */
  ret_val.typ = Float_Value;

  /* EURO berechnen und in die Rueckgabestruktur schreiben */
  ret_val.dValue = DM * 0.511292;

  return  ret_val;
}
/*--------------------------------------------------------------------------------*/  

/* Definition der Moeglichen Variablen-Typen */
typedef enum 
{ 
  Bool_Value,         /* Boolscher Wert     */ 
  Int_Value,          /* Ganzzahl           */
  Float_Value,        /* Kommazahl          */
  String_Value,       /* Zeichenkette       */
  Time_Value,         /* Zeit               */
  Date_Value,         /* Datum              */
  Timestamp_Value,    /* Zeitmarke          */  
  Not_Set,            /* Keine Rueckgabe    */
  Unknown             /* Unbekannt (Intern) */ 
} Id_type;

/* Definition der Variablen-Struktur */
typedef struct t_Value 
{
  Id_type typ;        /* Variablen-Typ           */
  long cValue;        /* Intern                  */
  char *sValue;       /* Zeiger auf Zeichenkette */
  long nValue;        /* Ganzzahl                */
  double dValue;      /* Kommazahl               */
  time_t tValue;      /* Alle Zeit/Datumswerte   */
} t_Value;







#ifdef __cplusplus 
}
#endif

