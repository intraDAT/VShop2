/*
 * header file for skey
 *
 * Version 0.0
 */

/*
 *--------------------
 *
 * included header files
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "skey_definitions.h"

#ifdef __cplusplus 
  extern "C" {
#endif


/*
 * --------------------
 *
 * ptr structure
 *
 */

typedef struct skey_ptr
{
  char *ptr;
  struct skey_ptr *next;
  struct skey_ptr *last;
} skey_ptr_t;

/*
 * --------------------
 *
 *  mem structure
 *
 */

typedef struct skey_mem
{
  skey_ptr_t *start;
  skey_ptr_t *ptr;
  unsigned int size;
} skey_mem_t;

/*
 * --------------------
 *
 * skey addbin structure
 *
 */

typedef struct skey_addbin
{
  skey_mem_t mem;
  char overflow;
  char *first;
  char *second;
  char *result;
  unsigned int *first_size;
  unsigned int *second_size;
  unsigned int offset;
  unsigned int size;
}skey_addbin_t;

/*
 * --------------------
 *
 * skey structure skey_clearmem
 *
 */

typedef struct skey_clearmem
{
  skey_mem_t mem;
}skey_clearmem_t;

/*
 * --------------------
 *
 * skey structure skey_compare
 *
 */

typedef struct skey_compare
{
  skey_mem_t mem;
  unsigned int offset;
  unsigned first_offset;
  unsigned second_offset;
}skey_compare_t;

/*
 * --------------------
 *
 * skey structure skey_convert
 *
 */

typedef struct skey_convert
{
  skey_mem_t mem;
  unsigned int offset;
  unsigned int offset2;
  unsigned int size;
}skey_convert_t;

/*
 * --------------------
 *
 * skey structure skey_crypt
 *
 */

typedef struct skey_crypt
{
  skey_mem_t mem;
  char *in;
  unsigned int offset;
  unsigned int offset2;
  unsigned int size;
}skey_crypt_t;

/*
 * --------------------
 *
 * skey structure skey_divbin
 *
 */

typedef struct skey_divbin
{
  skey_mem_t mem;
  char *free_ptr;
  char *first;
  char **first_ptr;
  char *divident;
  char *divisor;
  unsigned int first_size;
  unsigned int divisor_size;
  unsigned int divident_size;
  unsigned int offset;
  unsigned int qoffset;
  int done;
}skey_divbin_t;

/*
 * --------------------
 *
 * skey structure getmem
 *
 */

typedef struct skey_getmem
{
  skey_ptr_t *start;
  skey_ptr_t *ptr;
}skey_getmem_t;

/*
 * --------------------
 *
 * skey structure skey_mulbin
 *
 */

typedef struct skey_mulbin
{
  skey_mem_t mem;
  char *multiplicand;
  char *multiplier;
  char *one;
  char *product;
  char *ptr;
  char *realloc;
  char *result;
  unsigned int k;
  unsigned int multiplicand_size;
  unsigned int multiplier_size;
  unsigned int product_size;
  unsigned int ptr_size;
  unsigned int result_size;
  unsigned int offset;
  unsigned int size;
}skey_mulbin_t;

/*
 * --------------------
 *
 * skey structure skey_subbin
 *
 */

typedef struct skey_subbin
{
  skey_mem_t mem;
  char one;
  char *minuend;
  char *subtrahend;
  char *new_subtrahend;
  unsigned int minuend_size;
  unsigned int offset;
  unsigned int size;
}skey_subbin_t;

/*
 * --------------------
 *
 * skey structure skey_read
 *
 */

typedef struct skey_read
{
  skey_mem_t mem;
  unsigned int size;
  unsigned int offset;
  unsigned int offset2;
}skey_read_t;

/*
 * --------------------
 *
 * skey structure skey_mod
 *
 */

typedef struct skey_mod
{
  skey_mem_t mem;
  char *divident;
  char *out;
  char *out_offset;
  char *quotient;
  char *remainder;
  char *result;
  unsigned int divident_size;
  unsigned int one_size;
  unsigned int offset;
  unsigned int out_size;
  unsigned int quotient_size;
  unsigned int remainder_size;
  unsigned int result_size;
  char one;
}skey_mod_t;

/*
 * --------------------
 *
 * skey structure skey_unmod
 *
 */
typedef struct skey_unmod
{
  skey_mem_t mem;
  char *base;
  char *in;
  char **in_ptr;
  char *in2;
  char *multiplier;
  char **mult_ptr;
  char *out;
  char *position;
  char *product;
  char *ptr;
  char *result;
  char *sum;
  unsigned int base_size;
  unsigned int in2_size;
  int multiplier_size;
  unsigned int offset;
  unsigned int offset2;
  unsigned int out_size;
  unsigned int product_size;
  unsigned int position_size;
  unsigned int sum_size;
  char one;
  char zero;
}skey_unmod_t;

/*
 * --------------------
 *
 * skey structure skey_dest
 *
 */

typedef struct skey_dest
{
  skey_ptr_t *start;
}skey_dest_t;

/* -------------------- */

#ifdef __cplusplus 
}
#endif

#include "skey_proto.h"

