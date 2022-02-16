
#include "adabasobj.h"


/*********************************************************************************/
// CDBField
/*********************************************************************************/
CDBField::CDBField()
{
  strcpy(Name,"");
  StringLength = 0;

  Long = 0;
  Double = 0;

  strcpy(Date,"");
  strcpy(Time,"");
  strcpy(TimeStamp,"");

  String = NULL;
}
/*--------------------------------------------------------------------------------*/
CDBField::CDBField(char* FieldName)
{
  strcpy(Name, FieldName);
  Name[strcspn(Name," ")] = '\0';
  StringLength = 0;

  Long = 0;
  Double = 0;

  strcpy(Date,"");
  strcpy(Time,"");
  strcpy(TimeStamp,"");

  String = NULL;
}
/*--------------------------------------------------------------------------------*/
CDBField::CDBField(char* FieldName, int NewStringLength)
{
  strcpy(Name, FieldName);
  Name[strcspn(Name," ")] = '\0';
  StringLength = 0;

  Long = 0;
  Double = 0;

  strcpy(Date,"");
  strcpy(Time,"");
  strcpy(TimeStamp,"");

  String = (char*) malloc(NewStringLength+1);
  StringLength = NewStringLength;
  strcpy(String,"");

}
/*--------------------------------------------------------------------------------*/
CDBField::~CDBField()
{
  if ((Type == FT_STRING) || (Type == FT_MEMO))
    free(String);
}


/*********************************************************************************/
// CDBFieldList
/*********************************************************************************/
CDBFieldList::CDBFieldList()
{
  Items = new _Item;
  Items->Element = NULL;
  Items->Next = NULL;
  p = Items; 
}
/*--------------------------------------------------------------------------------*/
CDBFieldList::~CDBFieldList()
{
  Clear();

  delete Items; 
}
/*--------------------------------------------------------------------------------*/
void CDBFieldList::Add(CDBField* Item) 
{
  while (p->Next != NULL) 
    p = p->Next; 

  p->Next = new _Item; 
  p = p->Next; 
  p->Element = Item;
  p->Next = NULL; 
}
/*--------------------------------------------------------------------------------*/
void CDBFieldList::Clear()
{
_Item *tmp;
  
  p = Items;

  while (Items->Next!=NULL)
	{
      if (p->Next != NULL)   
        {         
	      tmp = p->Next; 
          p->Next = (p->Next)->Next;         
		  delete tmp->Element;
	      delete tmp;  
		}
	}
}
/*--------------------------------------------------------------------------------*/  
CDBField* CDBFieldList::Seek(char* Name)
{  
  p = Items;

  while(p->Next != NULL)
	{
    #ifdef WIN32
	    if (_stricmp(p->Next->Element->Name, Name) == 0)
    #else
      if (strcasecmp(p->Next->Element->Name, Name) == 0)
    #endif
        return p->Next->Element;

	  p = p->Next;
	}

  return NULL;
}
/*--------------------------------------------------------------------------------*/  
CDBField* CDBFieldList::Seek(int Index)
{
int Counter = 1;

  p = Items;

  while(p->Next != NULL)
	{
	  if (Index == Counter)
        return p->Next->Element;

	  Counter++;
	  p = p->Next;
	}

  return NULL;
}
/*--------------------------------------------------------------------------------*/  
CDBField* CDBFieldList::GetFirstItem()
{
  p = Items;

  if (p->Next != NULL)
	{
      return p->Next->Element;
	}
  else
	return NULL;
}
/*--------------------------------------------------------------------------------*/  
CDBField* CDBFieldList::GetItem()
{
  if (p->Next != NULL)
	{
	  return p->Next->Element;
	}
  else
	return NULL;
}
/*--------------------------------------------------------------------------------*/  
CDBField* CDBFieldList::GetNextItem()
{

  p = p->Next;
  if (p->Next != NULL)
	{
      return p->Next->Element;
	}
  else
	return NULL;
}
/*********************************************************************************/
// CBaseAdabasDB
/*********************************************************************************/

CBaseAdabasDB* AdabasDB;
/*---------------------------------------------------------------------------------*/
void initadabas(CBaseAdabasDB* AdabasDBPtr)
{
  AdabasDB = AdabasDBPtr;
}



