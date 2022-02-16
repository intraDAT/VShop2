/*
 *
 * decryptpassword function
 *
 * Version 0.0
 *
 */

#include "skey.h"

#ifdef __cplusplus 
extern "C" {
#endif

int
decryptpassword (char **out, char *in, char *key) 
{
  char *ascii_offset;
  char *base;
  char *crypt_buf;
  char *crypt_ptr;
  char *in_ptr;
  char *key_ptr;
  char *out_ptr;
  char *out_buf;
  char *ptr;
  char *result;
  char *constant_ptr;

  unsigned int crypt_buf_size;
  unsigned int in_ptr_size;
  unsigned int key_ptr_size;
  unsigned int ascii_offset_size;
  unsigned int base_size;
  unsigned int compare_size;
  unsigned int in_size;
  unsigned int key_size;
  unsigned int offset;
  unsigned int out_size;
  unsigned int result_size;
  
  in_size = strlen (in);
  key_size = strlen (key);

  if (skey_read (skey_read_new(), 
		 in, 
		 &in_ptr, 
		 &in_size, 
		 &in_ptr_size) != 0)
    return (-1);

  if (skey_read (skey_read_new(), 
		 key, 
		 &key_ptr, 
		 &key_size, 
		 &key_ptr_size) != 0)
    {
      in_ptr -= (in_ptr_size - 1);
      free (in_ptr);
      return (-1);
    }

  for (offset = 7; offset <= key_ptr_size; offset += 8)
    {
      *(key_ptr - offset) = '1';
    }

  base = key_ptr;
  base_size = 5;

  ptr = key_ptr - 2;
  
  constant_ptr = "11010";
  constant_ptr += base_size - 1;

  if (skey_compare (skey_compare_new (), 
		    constant_ptr, 
		    ptr, 
		    &base_size, 
		    &base_size) == 2)
    base_size--;

  base = ptr;
  compare_size = 1;

  if (skey_compare (skey_compare_new (), 
		    "0", 
		    ptr, 
		    &compare_size, 
		    &base_size) == 1)
    base = "11010";

  ascii_offset = "1100001";
  ascii_offset += 6;
  ascii_offset_size = 7;

  if (skey_unmod (skey_unmod_new (), 
		  in_ptr, 
		  base, 
		  ascii_offset, 
		  &out_ptr, 
		  &in_ptr_size,  
		  &base_size, 
		  &ascii_offset_size, 
		  &out_size) != 0)
    {
      in_ptr -= (in_ptr_size - 1);
      key_ptr -= (key_ptr_size - 1);
      free (in_ptr);
      free (key_ptr);
      out = NULL;
      return (-1);
    }

  out_ptr += (out_size - 1);

  if (skey_subbin (skey_subbin_new (),
		   out_ptr,
		   ptr,
		   &result,
		   &out_size,
		   &base_size,
		   &result_size) != 0)
    {
      in_ptr -= (in_ptr_size - 1);
      key_ptr -= (key_ptr_size - 1);
      out_ptr -= (out_size - 1);  
      free (in_ptr);
      free (key_ptr);
      free (out_ptr);
      out = NULL;
      return (-1);
    }
  
  crypt_buf_size = (result_size + (result_size - (result_size / 8) * 8));
  crypt_buf = (char *) malloc ((size_t ) (crypt_buf_size + 1));
  memset ((void *) crypt_buf, '0', (size_t ) crypt_buf_size - 1);
  *(crypt_buf + crypt_buf_size - 1) = '\0';
  memcpy ((void *) (crypt_buf + (result_size - (result_size / 8) * 8)), (const void *) result, (size_t ) result_size);
  crypt_buf += (crypt_buf_size - 1);

  if (skey_crypt (skey_crypt_new (), 
		  crypt_buf, 
		  key_ptr, 
		  &crypt_ptr, 
		  &crypt_buf_size,
		  &key_ptr_size) != 0)
    {
      crypt_buf -= (crypt_buf_size - 2);
      in_ptr -= (in_ptr_size - 1);
      key_ptr -= (key_size - 1);
      out_ptr -= (in_ptr_size - 1);
      free (crypt_buf);
      free (in_ptr);
      free (key_ptr);
      free (result);
      free (out_ptr);   
      free (key_ptr);
      out = NULL;
      return (-1);
    }

  if (skey_convert (skey_convert_new (),
		    crypt_ptr,
		    out,
		    &result_size) != 0)
    {
      crypt_buf -= (crypt_buf_size - 2);
      in_ptr -= (in_ptr_size - 1);
      key_ptr -= (key_ptr_size - 1);
      crypt_ptr -= in_ptr_size;
      free (crypt_buf);
      free (in_ptr);
      free (key_ptr);
      free (crypt_ptr);
      free (result);
      return (-1);
    }

  realloc ((void *) *out, (size_t ) (result_size / 8));
  *(*out + (result_size / 8)-1) = '\0';

  crypt_buf -= (crypt_buf_size - 2);
  in_ptr -= (in_ptr_size - 1);
  key_ptr -= (key_ptr_size - 1);
  crypt_ptr -= result_size;
  result -= (result_size - 1);
  out_ptr -= (out_size - 1);
  free (in_ptr);
  free (key_ptr);
  free (out_ptr);   

  return (0);
}

#ifdef __cplusplus 
}
#endif
