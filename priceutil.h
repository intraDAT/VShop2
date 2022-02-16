#ifndef __PRICEUTIL_H
#define __PRICEUTIL_H

/**
  * Rounds a number to two decimal places.
  */
double rnd( double d );

/**
  * Subtracts a discount (in percent) from a price and returns the unrounded
  * result.
  */
double discountPrice( double price, double discount );

/**
  * Returns the rounded gross price.
  */
double gross( double price, double discount, double tax );

/**
  * Returns the rounded net price.
  */
double net( double price, double discount );

/**
  * Returns the rounded price of a row.
  */
double rowNet( double price, double discount, double quantity );

/**
  * Returns the rounded price of a row including tax.
  */
double rowGross( double price, double discount, double quantity, double tax );

/**
  * Returns the rounded price of a row including tax using additional rounding.
  */
double rowGrossRetail( double price, double discount, double quantity,
                       double tax );

/**
  * Returns the amount of tax for a product row.
  */
double rowTaxWholesale( double price, double discount, double quantity,
                        double tax );

/**
  * Returns the amount of tax for a product row calculating gross price before
  * multiplying by quantity.
  */
double rowTaxRetail( double price, double discount, double quantity,
                     double tax );

/*
double taxAmount( price, discount, tax ) {
  
}

double discountAmount( price, discount ) {
}
*/

#endif
