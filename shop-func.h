#ifdef WIN32

#ifndef VSL_ENGINE
  #include "vsl.h"
  extern "C" _declspec(dllexport) t_Value base_price(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_vat(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_discount(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_currency(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_language(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_productprev(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_productnext(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_productlink(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_catalognext(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_catalogprev(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_cataloglink(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_basketnext(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_basketprev(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_tobasket(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_basketinsert(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_basketupdate(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_ordersum(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_ordervat(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_orderrowsum(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_orderrowvat(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_basketsum(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_basketrowsum(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_basketrowvat(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_basketvat(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_basketdelete(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_basketorder(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_login(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_paymentlist(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_paymentend(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_maintenance(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_shippingcostlist(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_shippingcostnext(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_shippingcostend(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_templateroot(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_upload(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_importfile(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_categoryname(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_categorypicture(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_productcount(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_productshow(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_user(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_available(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_ininventory(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value base_tobasketwindow(t_Value *arg_list);


  extern "C" _declspec(dllexport) t_Value product_lookup(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value catalog_lookup(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value basket_lookup(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value payment_lookup(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value promotion_lookup(t_Value *arg_list);
  extern "C" _declspec(dllexport) t_Value shippingcost_lookup(t_Value *arg_list);
  
  extern "C" _declspec(dllexport) t_Value base_allow(t_Value *arg_list);
#endif

#endif

//
// how to display prices
//
#define PRICEDISP_NET      0       // exclude tax
#define PRICEDISP_GROSS    1       // include tax
#define PRICEDISPSTR_NET   "NET"   // strings used in the database
#define PRICEDISPSTR_GROSS "GROSS"

//
// how to calculate tax
//
#define TAXCALCMETH_RETAIL       0           // apply tax before quantity
#define TAXCALCMETH_WHOLESALE    1           // apply quantity before tax
#define TAXCALCMETHSTR_RETAIL    "RETAIL"
#define TAXCALCMETHSTR_WHOLESALE "WHOLESALE"

/**
  * Rounds a number to two decimal places.
  */
extern "C" double round( double d );

/**
  * Determines how prices should be displayed.  Returns PRICEDISP_NET or
  * PRICEDISP_GROSS.
  */
extern "C" int priceDisplay();

/**
  * Determines how tax should be calculated.  Returns TAXCALCMETH_RETAIL or
  * TAXCALCMETH_WHOLESALE.
  */
extern "C" int taxCalcMethod();
