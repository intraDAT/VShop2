#include "priceutil.h"
#include <stdio.h>
#include <stdlib.h>

double rnd( double d ) {
  char buf[ 64 ];
  sprintf( buf, "%.2f", d );
  return atof( buf );
}

double discountPrice( double price, double discount ) {
  return price - price * ( discount / 100 );
}

double gross( double price, double discount, double tax ) {
  return rnd( discountPrice( price, discount ) * ( 1 + tax / 100 ) );
}

double net( double price, double discount ) {
  return rnd( discountPrice( price, discount ) );
}

double rowNet( double price, double discount, double quantity ) {
  return rnd( discountPrice( price, discount ) * quantity );
}

double rowGross( double price, double discount, double quantity, double tax ) {
  return rnd( discountPrice( price, discount ) * quantity * ( 1 + tax / 100 ) );
}

double rowGrossRetail( double price, double discount, double quantity,
                       double tax ) {
  return rnd( gross( price, discount, tax ) * quantity );
}

double rowTaxWholesale( double price, double discount, double quantity,
                        double tax ) {
  return rnd( discountPrice( price, discount ) * quantity * ( tax / 100 ) );
}

double rowTaxRetail( double price, double discount, double quantity,
                     double tax ) {
  double t = gross( price, discount, tax ) - net( price, discount );
  return rnd( t * quantity );
}

/*
double taxAmount( price, discount, tax ) {
  
}

double discountAmount( price, discount ) {
}
*/
