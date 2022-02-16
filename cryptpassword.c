/*
 *
 * cryptpassword function
 *
 * Version 0.0
 *
 */

#include "skey.h"

#ifdef __cplusplus 
extern "C" {
#endif

int
cryptpassword (char **out, char *in, char *key) 
{
  char *ascii_offset;
  char *base;
  char *crypt_ptr;
  char *in_ptr;
  char *in_ptr_buf;
  char *key_ptr;
  char *out_ptr;
  char *ptr;
  char *result;
  char *constant_ptr;

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

  in_ptr_buf = (char * ) malloc ((size_t ) in_size + 2);
  memset ((void *) in_ptr_buf, ' ', in_size + 2);
  memcpy ((void *) in_ptr_buf, (const void *) in, (size_t ) in_size);
  *(in_ptr_buf  + in_size + 1) = '\0';
  (in_size)++;

  if (skey_read (skey_read_new(), 
		 in_ptr_buf,
		 &in_ptr, 
		 &in_size, 
		 &in_ptr_size) != 0)
    return (-1);

  free (in_ptr_buf); 

  if (skey_read (skey_read_new(), 
		 key, 
		 &key_ptr, 
		 &key_size, 
		 &key_ptr_size) != 0)
    {
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

  if (skey_crypt (skey_crypt_new (), 
		  in_ptr, 
		  key_ptr, 
		  &crypt_ptr, 
		  &in_ptr_size, 
		  &key_ptr_size) != 0)
    {
      in_ptr -= (in_ptr_size - 1);
      key_ptr -= (key_ptr_size - 1);
      free (in_ptr);
      free (key_ptr);
      return (-1);
    }

  if (skey_addbin (skey_addbin_new (), 
		   crypt_ptr, 
		   ptr, 
		   &result, 
		   &in_ptr_size, 
		   &base_size, 
		   &result_size) != 0)
    {
      in_ptr -= (in_ptr_size - 1);
      key_ptr -= (key_ptr_size - 1);
      crypt_ptr -= (in_ptr_size - 1);
      free (in_ptr);
      free (key_ptr);
      free (crypt_ptr);
      return (-1);
    }

  result += (result_size - 1);
  ascii_offset = "1100001";
  ascii_offset += 6;
  ascii_offset_size = 7;

  if (skey_mod (skey_mod_new (), 
		result, 
		base, 
		ascii_offset, 
		&out_ptr, 
		&result_size,  
		&base_size, 
		&ascii_offset_size,
		&out_size) != 0)
    {
      in_ptr -= (in_ptr_size - 1);
      key_ptr -= (key_ptr_size - 1);
      crypt_ptr -= in_ptr_size;
      result -= (result_size - 1);
      free (in_ptr);
      free (key_ptr);
      free (crypt_ptr);
      free (result);
      return (-1);
    }

  out_size = strlen (out_ptr);
  out_ptr += (out_size - 1); 

  if (skey_convert (skey_convert_new (),
		    out_ptr,
		    out,
		    &out_size) != 0)
    {
      in_ptr -= (in_ptr_size - 1);
      key_ptr -= (key_ptr_size - 1);
      crypt_ptr -= in_ptr_size;
      result -= (result_size - 1);
      free (in_ptr);
      free (key_ptr);
      free (crypt_ptr);
      free (result);
      return (-1);
    }    

  in_ptr -= (in_ptr_size - 1);
  key_ptr -= (key_ptr_size - 1);
  crypt_ptr -= in_ptr_size;
  result -= (result_size - 1);
  free (in_ptr);
  free (key_ptr);
  free (crypt_ptr);
  free (result);
    
  return (0);
}

#ifdef __cplusplus 
}
#endif





