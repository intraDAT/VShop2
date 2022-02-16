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






/*
 *
 * decryptpassword function
 *
 * Version 0.0
 *
 */



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
/*
 *
 * skey_addbin dual-system addition
 *
 * Version 0.0
 *
 */



int
skey_addbin (skey_addbin_t *addbin, \
	     char *first, \
	     char *second, \
	     char **sum, \
	     unsigned int *first_size, \
	     unsigned int *second_size, \
	     unsigned int *sum_size)
{
  if ((first == NULL) || (second == NULL) || (addbin == NULL))
    {
      free (addbin);
      return (-1);
    }

  if (*first_size == 0)
    {
      *first_size = strlen (first);
      addbin->first += *first_size - 1;
    }
  
  if (*second_size == 0)
    {
      *second_size = strlen (second);
      addbin->second += *second_size - 1;
    }

  if (*first_size >= *second_size)
    addbin->size = *first_size + 1;

  else
    addbin->size = *second_size + 1;

  if (skey_getmem (skey_getmem_new (),
		  (void **)  &addbin->mem.start,
		   (void **) &addbin->mem.ptr,
		   sum,
		   &addbin->mem.size,
		   &addbin->size) != 0)
    {
      skey_dest_addbin (skey_dest_new (),
			(void **) &addbin->mem.start,
			first,
			second,
			*sum,
			first_size,
			second_size,
			sum_size);
      free (addbin);
      return (-1);
    }

  if (*first_size >= *second_size)
    {
      addbin->first = first;
      addbin->second = second;
      addbin->first_size = first_size;
      addbin->second_size = second_size;
    }
  
  else
    {
      addbin->second = first;
      addbin->first = second;
      addbin->first_size = second_size;
      addbin->second_size = first_size;
    }

  addbin->overflow = '0';

  for (addbin->offset = 0; addbin->offset <= (*addbin->second_size - 1); (addbin->offset)++)
    {
      if (addbin->overflow == '0')
	{
	  if ((*(addbin->first - addbin->offset) == '0') &&
	      (*(addbin->second - addbin->offset) == '0'))
	    {
	      *(*sum - addbin->offset) = '0';
	    }

	  else if ((*(addbin->first - addbin->offset) != \
		    *(addbin->second - addbin->offset)))
	    *(*sum - addbin->offset) = '1';
		  
	  else if ((*(addbin->first - addbin->offset) == '1') && \
		   (*(addbin->second - addbin->offset) == '1'))
	    {
	      addbin->overflow = '1';
	      *(*sum - addbin->offset) = '0';
	    }
	}
	      
      else 
	{
	  if ((*(addbin->first - addbin->offset) == '0') &&
	      (*(addbin->second - addbin->offset) == '0'))
	    {
	      addbin->overflow = '0';
	      *(*sum - addbin->offset) = '1';
	    }

	  else if ((*(addbin->first - addbin->offset) != \
		    *(addbin->second - addbin->offset)))
	    {
	      *(*sum - addbin->offset) = '0';
	    }

	  else if ((*(addbin->first - addbin->offset) == '1') && \
		   (*(addbin->second - addbin->offset) == '1'))
	    {
	      *(*sum - addbin->offset) = '1';
	    }
	}
    }

  if ((*addbin->first_size - *addbin->second_size) > 0)
    {
      for (addbin->offset; addbin->offset <= (*addbin->first_size - 1); (addbin->offset)++)
	{
	  if ((addbin->overflow == '1') &&
	      (*(addbin->first - addbin->offset) == '1'))
	    {
	      *(*sum - addbin->offset) = '0';
	    }

	  else if ((addbin->overflow == '1') &&
		   (*(addbin->first - addbin->offset) == '0'))
	    {
	      addbin->overflow = '0';
	      *(*sum - addbin->offset) = '1';
	    }
	  
	  else 
	    *(*sum - addbin->offset) = *(addbin->first - addbin->offset);
	}
    }

  if (addbin->overflow == '1')
    {
      *(*sum - (addbin->offset)) = '1';
      *sum -= (addbin->offset);
      *sum_size = addbin->offset + 1;
    }
  
  else
    {
      *sum -= (addbin->offset - 1);
      *sum_size = addbin->offset;
    }

  addbin->result = NULL;

  if (skey_mem_alloc ((void **) &addbin->result, sum_size) != 0)
    {
      skey_dest_addbin (skey_dest_new (), \
			(void **) &addbin->mem.start, \
			first, \
			second, \
			*sum, \
			first_size, \
			second_size, \
			NULL);
      free (addbin);
      return (-1);
    }

  memcpy ((void *) addbin->result, (const void *) *sum, (size_t) *sum_size);

  *sum = addbin->result;

  skey_dest_addbin (skey_dest_new (), \
		    (void **) &addbin->mem.start, \
		    first, \
		    second, \
		    *sum, \
		    first_size, \
		    second_size, \
		    NULL);
  free (addbin);
  return (0);
}
/*
 *
 * skey_addbin initialisation
 *
 * Version 0.0
 *
 */



skey_addbin_t *
skey_addbin_new ()
{
  return ((skey_addbin_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_addbin_t)));
}
/*
 *
 * skey_compare, result 1 the "first" number is bigger
 * else the "second" one
 *
 * Version 0.0
 *
 */



int
skey_compare (skey_compare_t *compare, 
	      char *first, 
	      char *second, 
	      unsigned *first_size, 
	      unsigned *second_size)
{
  if (!(((*first == '0') || (*first == '1')) && ((*second == '0') || (*second == '1'))))
    {
      printf ("function left in wrong position \n");
      free (compare);
      return (-1);
    }

  if ((first == NULL) || (second == NULL) || (first_size == NULL) || (second_size == NULL) || (compare == NULL))
    {
      printf ("function left in wrong position \n");
      free (compare);
      return (-1);
    }

  if (*first_size == 0)
    {
      *first_size = strlen (first);
      first += strlen (first) - 1;
    }
  
  if (*second_size == 0)
    {
      *second_size = strlen (second);
      second += strlen (second) - 1;
    }

  while ((*(first - *first_size + compare->first_offset + 1) == '0') &&  \
	 ((*first_size - compare->first_offset - 1) >= 0) && \
	 ((*first_size - compare->first_offset - 1) <= *first_size))
    (compare->first_offset)++;

  while ((*(second - *second_size + compare->second_offset + 1) == '0') && \
	 ((*second_size - compare->second_offset - 1) >= 0) && \
	 ((*second_size - compare->second_offset - 1) <= *second_size))
    (compare->second_offset)++;
  
  if ((*first_size - compare->first_offset) > (*second_size - compare->second_offset))
    {
      free (compare);
      return (1);
    }

  else if ((*first_size - compare->first_offset) < (*second_size - compare->second_offset))
    {
      free (compare);
      return (2);
    }

  else
    {
      for (compare->offset = 0; compare->offset <= (*second_size - 1 - compare->second_offset); (compare->offset)++)
	{
	  if (*(first - *first_size + compare->first_offset + compare->offset + 1) != 
	      *(second - *second_size + compare->second_offset + compare->offset + 1))
	    {
	      if (*(first - *first_size + compare->first_offset + compare->offset + 1) == '1')
		{
		  free (compare);
		  return (1);
		}
	      else
		{
		  free (compare);
		  return (2);
		}
	    }

	  else if ((*second_size - compare->offset - compare->second_offset - 1) == 0)
	    {
	      free (compare);
	      return (1);
	    }
	}  
    }

  free (compare);

  return (-1);
}
/*
 *
 * skey_compare initialisation
 *
 * Version 0.0
 *
 */



skey_compare_t *
skey_compare_new ()
{
  return ((skey_compare_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_compare_t)));
}
/*
 *
 * skey_new initialices the skey structure
 *
 * Version 0.0
 *
 */



int
skey_convert (skey_convert_t *convert, 
	      char *in, 
	      char **out, 
	      unsigned int *in_size)
{
  if (in == NULL)
    return (-1);

  if (*in_size == 0)
    {
      *in_size = strlen (in);
      in += *in_size;
    }

  convert->size = *in_size / 8 + 7;

  
  if (skey_getmem (skey_getmem_new (),
		   (void **) &convert->mem.start, 
		   (void **) &convert->mem.ptr, 
		   out, 
		   &convert->mem.size, 
		   &convert->size) != 0)
    {
      skey_dest_convert (skey_dest_new (),
			 (void **) &convert->mem.start, 
			 in, 
			 NULL, 
			 in_size, 
			 NULL);
      free (convert);
      return (-1);
    }

  *out -= (convert->size - 1);
  convert->offset2 = 0;

  for (convert->offset = 0; convert->offset2 <= (8 * (*in_size/8) + *in_size - 8 * (*in_size/8)); (convert->offset)++)
    {
      if (*(in - convert->offset2) == '1')
	(char ) *(*out + convert->offset) |= (char ) 0x01;
      
      (convert->offset2)++;
 
      if (*(in - convert->offset2) == '1')
	(char ) *(*out + convert->offset) |= (char )  0x02;

      (convert->offset2)++;

      if (*(in - convert->offset2) == '1')
	(char ) *(*out + convert->offset) |= (char )  0x04;

      (convert->offset2)++;

      if (*(in - convert->offset2) == '1')
	(char ) *(*out + convert->offset) |= (char )  0x08;

      (convert->offset2)++;

      if (*(in - convert->offset2) == '1')
	(char ) *(*out + convert->offset) |= (char )  0x10;

      (convert->offset2)++;

      if (*(in - convert->offset2) == '1')
	(char ) *(*out + convert->offset) |= (char )  0x20;

      (convert->offset2)++;

      if (*(in - convert->offset2) == '1')
	(char ) *(*out + convert->offset) |= (char )  0x40;

      (convert->offset2)++;

      if (*(in - convert->offset2) == '1')
	(char ) *(*out + convert->offset) |= (char )  0x80;

      (convert->offset2)++;
    }

  skey_dest_convert (skey_dest_new (),
		     (void **) &convert->mem.start, 
		     in, 
		     *out, 
		     in_size, 
		     NULL);
  free (convert);

  return (0);
}
/*
 *
 * skey_convert initialisation
 *
 * Version 0.0
 *
 */



skey_convert_t *
skey_convert_new ()
{
  return ((skey_convert_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_convert_t)));
}
/*
 *
 * crypt with key
 *
 * Version 0.0
 *
 */



int
skey_crypt (skey_crypt_t *crypt, 
	    char *in, 
	    char *key,
	    char **out, 
	    unsigned int *in_size, 
	    unsigned int *key_size)
{
  if ((in == NULL) || (key == NULL) || (crypt == NULL))
    {
      free (crypt);
      return (-1);
    }

  if (*in_size == 0)
    {
      *in_size = strlen (in);
      in += (*in_size - 1);
    }

  if (*key_size == 0)
    {
      *key_size = strlen (key);
      key += (*key_size - 1);
    }

  if ((*in_size - ((*in_size / 8) * 8)) > 0)
    crypt->size = ((*in_size / 8) * 8) + 8;

  else
    crypt->size = *in_size + 1;

  if (skey_getmem (skey_getmem_new (), 
		   (void **) &crypt->mem.start, 
		   (void **) &crypt->mem.ptr, 
		   out, 
		   &crypt->mem.size, 
		   &crypt->size) != 0)
    {
      skey_dest_crypt (skey_dest_new (), 
		       (void **) &crypt->mem.start,
		       in, 
		       key, 
		       *out, 
		       in_size, 
		       key_size, 
		       &crypt->size);
      free (crypt);
      return (-1);
    }

  if (skey_getmem (skey_getmem_new (), 
		   (void **) &crypt->mem.start, 
		   (void **) &crypt->mem.ptr, 
		   &crypt->in, 
		   &crypt->mem.size, 
		   &crypt->size) != 0)
    {
      skey_dest_crypt (skey_dest_new (), 
		       (void **) &crypt->mem.start,
		       in, 
		       key, 
		       *out, 
		       in_size, 
		       key_size, 
		       &crypt->size);
      free (crypt);
      return (-1);
    }

  crypt->in -= (crypt->size - 1);

  memset ((void *) crypt->in, '0', crypt->size);
  memcpy ((void *) (crypt->in + crypt->size - *in_size), 
	  (const void *) (in - *in_size + 1), (size_t) *in_size);

  crypt->in += (crypt->size - 1);

  while (*in_size >= crypt->offset)
    {
      if (crypt->offset2 < *key_size)
	{
	  if (*(crypt->in - crypt->offset) != 
	      *(key - crypt->offset2))
	    *(*out - crypt->offset) = '1';
	  else
	    *(*out - crypt->offset) = '0';
	}
      else
	{
	  crypt->offset2 = 0;

	  if (*(crypt->in - crypt->offset) != 
	      *(key - crypt->offset2))
	    *(*out - crypt->offset) = '1';
	  else
	    *(*out - crypt->offset) = '0';
	}

      (crypt->offset)++;
      (crypt->offset2)++;
    }

  skey_dest_crypt (skey_dest_new (), 
		   (void **) &crypt->mem.start, 
		   in, 
		   key, 
		   *out, 
		   in_size, 
		   key_size,
		   &crypt->size);
  free (crypt);

  return (0);
}
/*
 *
 * skey_crypt initialisation
 *
 * Version 0.0
 *
 */



skey_crypt_t *
skey_crypt_new ()
{
  return ((skey_crypt_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_crypt_t)));
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



void
skey_dest_addbin (skey_dest_t *addbin,
		  void **start,
		  char *first,
		  char *second,
		  char *sum,
		  unsigned int *first_size,
		  unsigned int *second_size,
		  unsigned int *sum_size)
{
  addbin->start = (skey_ptr_t *) *start;

  if (first_size != NULL)
    first -= (*first_size - 1);

  if (second_size != NULL)
    second -= (*second_size - 1);
  
  if (sum_size != NULL)
    sum -= (*sum_size - 1);

  for (; addbin->start->ptr != NULL; addbin->start = addbin->start->next);
  
  do
    {
      if ((addbin->start->ptr != NULL) &&
	  (addbin->start->ptr != first) &&
	  (addbin->start->ptr != second) &&
	  (addbin->start->ptr != sum))
	{
	  free (addbin->start->ptr);
	}
      if (addbin->start->next != NULL)
	free (addbin->start->next);

      if (addbin->start->last == NULL)
	{
	  free (addbin->start);
	  addbin->start = NULL;
	}

      else
	addbin->start = addbin->start->last;
    } while (addbin->start != NULL);

  if (first_size != NULL)
    first += (*first_size - 1);

  if (second_size != NULL)
    second += (*second_size - 1);
  
  free (addbin);
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



void
skey_dest_compare (skey_dest_t *compare,
		   void **start, 
		   char *first, 
		   char *second, 
		   unsigned int *first_size,
		   unsigned int *second_size)
{
  compare->start = (skey_ptr_t *) *start;

  if (first_size != NULL)
    first -= (*first_size - 1);

  if (second_size != NULL)
    second -= (*second_size - 1);

  for (; compare->start->ptr != NULL; compare->start = compare->start->next);
  
  do
    {
      if ((compare->start->ptr != NULL) &&
	  (compare->start->ptr != first) &&
	  (compare->start->ptr != second))
	{
	  free (compare->start->ptr);
	}
      if (compare->start->next != NULL)
	free (compare->start->next);

      if (compare->start->last == NULL)
	{
	  free (compare->start);
	  compare->start = NULL;
	}

      else
	compare->start = compare->start->last;
    } while (compare->start != NULL);

  if (first_size != NULL)
    first += (*first_size - 1);

  if (second_size != NULL)
    second += (*second_size - 1);
  
  free (compare);
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



void
skey_dest_convert (skey_dest_t *convert,
		   void **start, 
		   char *in, 
		   char *out, 
		   unsigned int *in_size,  
		   unsigned int *out_size)
{
  convert->start = (skey_ptr_t *) *start;

  if (in_size != NULL)
    in -= (*in_size - 1);

  if (out_size != NULL)
    out -= (*out_size - 1);

  for (; convert->start->ptr != NULL; convert->start = convert->start->next);
  
  do
    {
      if ((convert->start->ptr != NULL) &&
	  (convert->start->ptr != in) &&
	  (convert->start->ptr != out))
	{
	  free (convert->start->ptr);
	}
      if (convert->start->next != NULL)
	free (convert->start->next);

      if (convert->start->last == NULL)
	{
	  free (convert->start);
	  convert->start = NULL;
	}

      else
	convert->start = convert->start->last;
    } while (convert->start != NULL);

  if (in_size != NULL)
    in += (*in_size - 1);

  if (out_size != NULL)
    out += (*out_size - 1);

  free (convert);
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */


void
skey_dest_crypt (skey_dest_t *crypt, \
		 void **start, \
		 char *in, \
		 char *key, \
		 char *out, \
		 unsigned int *in_size, \
		 unsigned int *key_size, \
		 unsigned int *out_size)
{
  crypt->start = (skey_ptr_t *) *start;

  if (in_size != NULL)
    in -= (*in_size - 1);

  if (out_size != NULL)
    out -= (*out_size - 1);

  if (key_size != NULL)
    key -= (*key_size - 1);

  crypt->start->last = NULL;

  for (; crypt->start->ptr != NULL; crypt->start = crypt->start->next);
  
  do
    {
      if ((crypt->start->ptr != NULL) &&
	  (crypt->start->ptr != in) &&
	  (crypt->start->ptr != out) &&
	  (crypt->start->ptr != key))
	{
	  free (crypt->start->ptr);
	}
      if (crypt->start->next != NULL)
	free (crypt->start->next);

      if (crypt->start->last == NULL)
	{
	  free (crypt->start);
	  crypt->start = NULL;
	}

      else
	crypt->start = crypt->start->last;
    } while (crypt->start != NULL);

  if (in_size != NULL)
    in += (*in_size - 1);

  if (out_size != NULL)
    out += (*out_size - 1);

  if (key_size != NULL)
    key += (*key_size - 1);

  free (crypt);
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



void
skey_dest_divbin  (skey_dest_t *divbin, \
		   void **start, \
		   char *divisor, \
		   char *divident, \
		   char *quotient, \
		   char *remainder, \
		   unsigned int *divisor_size, \
		   unsigned int *divident_size, \
		   unsigned int *quotient_size, \
		   unsigned int *remainder_size)
{
  divbin->start = (skey_ptr_t *) *start;

  if (divisor_size != NULL)
    divisor -= (*divisor_size - 1);

  if (divident_size != NULL)
    divident -= (*divident_size - 1);
  
  if (quotient_size != NULL)
    quotient -= (*quotient_size - 1);

  if (remainder_size != NULL)
    remainder -= (*remainder_size - 1);

  divbin->start->last = NULL;

  for (; divbin->start->ptr != NULL; divbin->start = divbin->start->next);
  
  do
    {
      if ((divbin->start->ptr != NULL) &&
	  (divbin->start->ptr != divisor) &&
	  (divbin->start->ptr != divident) &&
	  (divbin->start->ptr != quotient) &&
	  (divbin->start->ptr != remainder))
	{
	  free (divbin->start->ptr);
	}

      if (divbin->start->next != NULL)
	free (divbin->start->next);

      if (divbin->start->last == NULL)
	{
	  free (divbin->start);
	  divbin->start = NULL;
	}

      else
	divbin->start = divbin->start->last;
    } while (divbin->start != NULL);

  if (divisor_size != NULL)
    divisor += (*divisor_size - 1);
  
  if (divident_size != NULL)
    divident += (*divident_size - 1);

  free (divbin);
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



void
skey_dest_mod  (skey_dest_t *mod,
		void **start,
		char *in,
		char *base,
		char *ascii_offset,
		char *out,
		unsigned int *in_size,
		unsigned int *base_size,
		unsigned int *ascii_offset_size,
		unsigned int *out_size)
{
  mod->start = (skey_ptr_t *) *start;
  if (in_size != NULL)
    in -= (*in_size - 1);

  if (base_size != NULL)
    base -= (*base_size - 1);
  
  if (ascii_offset_size != NULL)
    ascii_offset -= (*ascii_offset_size - 1);

  if (out_size != NULL)
    out -= (*out_size - 1);

  for (; mod->start->ptr != NULL; mod->start = mod->start->next);
  
  do
    {
      if ((mod->start->ptr != NULL) &&
	  (mod->start->ptr != in) &&
	  (mod->start->ptr != base) &&
	  (mod->start->ptr != ascii_offset) &&
	  (mod->start->ptr != out))
	{
	  free (mod->start->ptr);
	}
      if (mod->start->next != NULL)
	free (mod->start->next);

      if (mod->start->last == NULL)
	{
	  free (mod->start);
	  mod->start = NULL;
	}

      else
	mod->start = mod->start->last;
    } while (mod->start != NULL);

  if (in_size != NULL)
    in += (*in_size - 1);

  if (base_size != NULL)
    base += (*base_size - 1);
  
  if (ascii_offset_size != NULL)
    ascii_offset += (*ascii_offset_size - 1);

  free (mod);
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



void
skey_dest_mulbin  (skey_dest_t *mulbin,
		   void **start,
		   char *multiplicand,
		   char *multiplier,
		   char *product,
		   unsigned int *multiplicand_size, 
		   unsigned int *multiplier_size, 
		   unsigned int *product_size)
{
  mulbin->start = (skey_ptr_t *) *start;

  if (multiplicand_size != NULL)
    multiplicand -= (*multiplicand_size - 1);

  if (multiplier_size != NULL)
    multiplier -= (*multiplier_size - 1);
  
  if (product_size != NULL)
    multiplier -= (*product_size - 1);

  for (; mulbin->start->ptr != NULL; mulbin->start = mulbin->start->next);
  
  do
    {
      if ((mulbin->start->ptr != NULL) &&
	  (mulbin->start->ptr != multiplicand) &&
	  (mulbin->start->ptr != multiplier) &&
	  (mulbin->start->ptr != product))
	{
	  free (mulbin->start->ptr);
	}
      if (mulbin->start->next != NULL)
	free (mulbin->start->next);

      if (mulbin->start->last == NULL)
	{
	  free (mulbin->start);
	  mulbin->start = NULL;
	}

      else
	mulbin->start = mulbin->start->last;
    } while (mulbin->start != NULL);

  if (multiplicand_size != NULL)
    multiplicand += (*multiplicand_size - 1);

  if (multiplier_size != NULL)
    multiplier += (*multiplier_size - 1);

  free (mulbin);
}
/*
 *
 * skey_dest_new initialisation
 *
 * Version 0.0
 *
 */



skey_dest_t *
skey_dest_new ()
{
  return ((skey_dest_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_dest_t)));
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



void
skey_dest_read (skey_dest_t *read,
		void **start,
		char *in,
		char *out,
		unsigned int *in_size,
		unsigned int *out_size)
{
  read->start = (skey_ptr_t *) *start;

  if (in_size != NULL)
    in -= (*in_size - 1);

  if (out_size != NULL)
    out -= (*out_size - 1);

  for (; read->start->ptr != NULL; read->start = read->start->next);
  
  do
    {
      if ((read->start->ptr != NULL) &&
	  (read->start->ptr != in) &&
	  (read->start->ptr != out))
	{
	  free (read->start->ptr);
	}
      if (read->start->next != NULL)
	free (read->start->next);

      if (read->start->last == NULL)
	{
	  free (read->start);
	  read->start = NULL;
	}

      else
	read->start = read->start->last;
    } while (read->start != NULL);

  if (in_size != NULL)
    in += (*in_size - 1);

  if (out_size != NULL)
    out += (*out_size - 1);

  free (read);
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



void
skey_dest_subbin (skey_dest_t *subbin,
		  void **start,
		  char *minuend,
		  char *subtrahend,
		  char *difference,
		  unsigned int *minuend_size,
		  unsigned int *subtrahend_size,
		  unsigned int *difference_size)
{
  subbin->start = (skey_ptr_t *) *start;

  if (minuend_size != NULL)
    minuend -= (*minuend_size - 1);

  if (subtrahend_size != NULL)
    subtrahend -= (*subtrahend_size - 1);
  
  if (difference_size != NULL)
    difference -= (*difference_size - 1);

  for (; subbin->start->ptr != NULL; subbin->start = subbin->start->next);
  
  do
    {
      if ((subbin->start->ptr != NULL) &&
	  (subbin->start->ptr != minuend) &&
	  (subbin->start->ptr != subtrahend) &&
	  (subbin->start->ptr != difference))
	{
	  free (subbin->start->ptr);
	}
      if (subbin->start->next != NULL)
	free (subbin->start->next);

      if (subbin->start->last == NULL)
	{
	  free (subbin->start);
	  subbin->start = NULL;
	}
      
      else
	subbin->start = subbin->start->last;
    } while (subbin->start != NULL);

  /*  free ((skey_ptr_t *) *start); */

  if (minuend_size != NULL)
    minuend += (*minuend_size - 1);

  if (subtrahend_size != NULL)
    subtrahend += (*subtrahend_size - 1);

  free (subbin);
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



void
skey_dest_unmod  (skey_dest_t *unmod,
		  void **start,
		  char *in,
		  char *base,
		  char *ascii_offset,
		  char *out,
		  unsigned int *in_size,
		  unsigned int *base_size,
		  unsigned int *ascii_offset_size,
		  unsigned int *out_size)
{
  unmod->start = (skey_ptr_t *) *start;

  if (in_size != NULL)
    in -= (*in_size - 1);

  if (base_size != NULL)
    base -= (*base_size - 1);
  
  if (ascii_offset_size != NULL)
    ascii_offset -= (*ascii_offset_size - 1);

  if (out_size != NULL)
    out -= (*out_size - 1);

  unmod->start->last = NULL;

  for (; unmod->start->ptr != NULL; unmod->start = unmod->start->next);
  
  do
    {
      if ((unmod->start->ptr != NULL) &&
	  (unmod->start->ptr != in) &&
	  (unmod->start->ptr != base) &&
	  (unmod->start->ptr != ascii_offset) &&
	  (unmod->start->ptr != out))
	{
	  free (unmod->start->ptr);
	}

      if (unmod->start->next != NULL)
	free (unmod->start->next);

      if (unmod->start->last == NULL)
	{
	  free (unmod->start);
	  unmod->start = NULL;
	}
      
      else
	unmod->start = unmod->start->last;
    } while (unmod->start != NULL);

  if (in_size != NULL)
    in += (*in_size - 1);

  if (base_size != NULL)
    base += (*base_size - 1);
  
  if (ascii_offset_size != NULL)
    ascii_offset += (*ascii_offset_size - 1);

  free (unmod);
}
/*
 *
 * skey_divbin dual division with rest
 *
 * Version 0.0
 *
 */



int
skey_divbin (skey_divbin_t *divbin, 
	     char *divisor, 
	     char *divident, 
	     char **quotient, 
	     char **remainder, 
	     unsigned int *divisor_size, 
	     unsigned int *divident_size, 
	     unsigned int *quotient_size, 
	     unsigned int *remainder_size)
{
  if ((divisor == NULL) || (divident == NULL) || (divbin == NULL))
    {
      free (divbin);
      return (-1);
    }

  if (*divisor_size == 0)
    {
      *divisor_size = strlen (divisor);
      divisor += strlen (divisor) - 1;
    }

  if (*divident_size == 0)
    {
      *divident_size = strlen (divident);
      divident += strlen (divident) - 1;
    }

  if ((skey_rmlz (&divisor, divisor_size) != 0) ||
      (skey_rmlz (&divident, divident_size) != 0))
    {
      free (divbin);
      return (-1);
    }

  if (*divisor_size > *divident_size)
    {
      *remainder_size = *divident_size;
      *quotient_size = 1;

      if (skey_getmem (skey_getmem_new (), 
		       (void **) &divbin->mem.start, 
		       (void **) &divbin->mem.ptr, 
		       remainder, 
		       &divbin->mem.size, 
		       remainder_size) != 0)
	{
	  skey_dest_divbin  (skey_dest_new (), 
			     (void **) &divbin->mem.start, 
			     divisor, 
			     divident, 
			     NULL, 
			     *remainder, 
			     divisor_size, 
			     divident_size, 
			     NULL, 
			     remainder_size);
	    free (divbin);
	  return (-1);
	}
      
      if (skey_getmem (skey_getmem_new (), 
		       (void **) &divbin->mem.start, 
		       (void **) &divbin->mem.ptr, 
		       quotient, 
		       &divbin->mem.size, 
		       quotient_size) != 0)
	{
	  skey_dest_divbin  (skey_dest_new (), 
			     (void **) &divbin->mem.start, 
			     divisor, 
			     divident, 
			     *quotient, 
			     *remainder, 
			     divisor_size, 
			     divident_size, 
			     quotient_size, 
			     remainder_size);
	    free (divbin);
	  return (-1);
	}
      
      *remainder -= (*remainder_size - 1);
      *quotient -= (*quotient_size - 1);
      memcpy (*remainder, divident - (*remainder_size - 1), *remainder_size);
      memcpy (*quotient, "0", *quotient_size);
      skey_dest_divbin  (skey_dest_new (), 
			 (void **) &divbin->mem.start, 
			 divisor, 
			 divident, 
			 *quotient, 
			 *remainder, 
			 divisor_size, 
			 divident_size, 
			 NULL, 
			 NULL);
	free (divbin);
      return (0);
    }
   
  divbin->divident_size = *divident_size;
  
  if (skey_getmem (skey_getmem_new (), 
		   (void **) &divbin->mem.start, 
		   (void **) &divbin->mem.ptr, 
		   &divbin->divident, 
		   &divbin->mem.size, 
		   &divbin->divident_size) != 0)
    {
      skey_dest_divbin  (skey_dest_new (), 
			 (void **) &divbin->mem.start, 
			 divisor, 
			 divident, 
			 NULL, 
			 NULL, 
			 divisor_size, 
			 divident_size, 
			 NULL, 
			 NULL);
	free (divbin);
      return (-1);
    }
   
  if (skey_getmem (skey_getmem_new (), 
		   (void **) &divbin->mem.start, 
		   (void **) &divbin->mem.ptr, 
		   &divbin->divisor, 
		   &divbin->mem.size, 
		   &divbin->divident_size) != 0)
    {
      skey_dest_divbin  (skey_dest_new (), 
			 (void **) &divbin->mem.start, 
			 divisor, 
			 divident, 
			 NULL, 
			 NULL, 
			 divisor_size, 
			 divident_size, 
			 NULL, 
			 NULL);
	free (divbin);
      return (-1);
    }

  divbin->divisor_size = *divisor_size;
  divbin->divident_size = *divident_size;

  memcpy (divbin->divisor - divbin->divisor_size + 1, 
	  divisor - divbin->divisor_size + 1, 
	  divbin->divisor_size);
  memcpy (divbin->divident - divbin->divident_size + 1, 
	  divident - divbin->divident_size + 1, 
	  divbin->divident_size);

  if (skey_getmem (skey_getmem_new (), 
		   (void **) &divbin->mem.start, 
		   (void **) &divbin->mem.ptr, 
		   quotient, 
		   &divbin->mem.size, 
		   divident_size) != 0)
    {
      skey_dest_divbin  (skey_dest_new (), 
			 (void **) &divbin->mem.start, 
			 divisor, 
			 divident, 
			 *quotient, 
			 NULL, 
			 divisor_size, 
			 divident_size, 
			 quotient_size, 
			 NULL);
	free (divbin);
      return (-1);
    }

  *quotient -= (*divident_size - 1);

  divbin->first = divbin->divident;
  divbin->first -= (divbin->divident_size - divbin->divisor_size);
  divbin->first_size = divbin->divisor_size;
  divbin->first_ptr = &divbin->first;

  if (divbin->divisor_size < divbin->divident_size)
    {
      for (divbin->offset = 0; divbin->offset < (divbin->divident_size - divbin->divisor_size); (divbin->offset)++)
	*(divbin->divisor - divbin->divident_size + 1 + divbin->offset) = '0';
    }

  while (divbin->done == 0)
    {
      while ((divbin->first <= divbin->divident) &&
	     (skey_compare (skey_compare_new(), 
			    divbin->first, 
			    divbin->divisor, 
			    &divbin->first_size, 
			    &divbin->divisor_size) == 2) && 
	     (divbin->done == 0))
	{
	  if (divbin->first  < divbin->divident)
            {
	      (divbin->first)++;
	      (divbin->first_size)++;
	      *(*quotient + divbin->qoffset) = '0';
	      (divbin->qoffset)++;
            }
            
	  else
            {
	      *(*quotient + divbin->qoffset) = '0';
	      (divbin->qoffset)++;
	      if (divbin->first_size > 0)
		{
		  *remainder_size = divbin->first_size;

		  if (skey_getmem (skey_getmem_new (), 
				   (void **) &divbin->mem.start, 
				   (void **) &divbin->mem.ptr, 
				   remainder, &divbin->mem.size, 
				   &divbin->first_size) != 0)
		    {
		      skey_dest_divbin  (skey_dest_new (), 
					 (void **) &divbin->mem.start, 
					 divisor, 
					 divident, 
					 NULL, 
					 NULL, 
					 divisor_size, 
					 divident_size, 
					 NULL, 
					 NULL);
			free (divbin);
		      return (-1);
		    }

		  *remainder -= (*remainder_size - 1);

		  memcpy ((void *) *remainder, (const void *) (divbin->first - divbin->first_size + 1), (size_t ) divbin->first_size);
		}

	      divbin->done = 1;
            }
	}

      if (divbin->first > divbin->divident)
	divbin->done = 1;
      
      if (divbin->done == 0)
	{
	  *remainder = NULL;

	  if (skey_subbin (skey_subbin_new(), 
			   divbin->first, 
			   divbin->divisor, 
			   remainder, 
			   &divbin->first_size, 
			   &divbin->divisor_size, 
			   remainder_size) != 0)
	    {
	      skey_dest_divbin  (skey_dest_new (), 
				 (void **) &divbin->mem.start, 
				 divisor, 
				 divident, 
				 NULL, 
				 NULL, 
				 divisor_size, 
				 divident_size, 
				 NULL, 
				 NULL);
		free (divbin);
	      return (-1);
	    }

	  if (skey_store_ptr (skey_getmem_new (), 
			      (void **) &divbin->mem.start, 
			      (void **) &divbin->mem.ptr, 
			      *remainder, 
			      &divbin->mem.size) != 0)
	    {
	      skey_dest_divbin  (skey_dest_new (), 
				 (void **) &divbin->mem.start, 
				 divisor, 
				 divident, 
				 NULL, 
				 NULL, 
				 divisor_size, 
				 divident_size, 
				 NULL, 
				 NULL);
	      free (divbin);
	      return (-1);
	    }
			   
	  *(*quotient + divbin->qoffset) = '1';
	  (divbin->qoffset)++;
         
	  memcpy (divbin->first - *remainder_size + 1, *remainder, *remainder_size);

	  for (divbin->offset = 0; divbin->offset < (divbin->first_size - *remainder_size); (divbin->offset)++)
	    *(divbin->first - divbin->first_size + 1 + divbin->offset) = '0';

	  divbin->first++;
	  divbin->first_size = *remainder_size + 1;
	  
	  if (skey_rmlg (divbin->first_ptr, &divbin->first_size) != 0)
	    {
	      skey_dest_divbin  (skey_dest_new (), 
				 (void **) &divbin->mem.start, 
				 divisor, 
				 divident, 
				 NULL, 
				 NULL, 
				 divisor_size, 
				 divident_size, 
				 NULL, 
				 NULL);
		free (divbin);
	      return (-1);
	    }
	}
    }

  *quotient += (divbin->qoffset - 1);

  if (skey_rmlz (quotient, &divbin->qoffset) != 0)
    {
      skey_dest_divbin  (skey_dest_new (), 
			 (void **) &divbin->mem.start, 
			 divisor, 
			 divident, 
			 NULL, 
			 NULL, 
			 divisor_size, 
			 divident_size, 
			 NULL, 
			 NULL);
	free (divbin);
      return (-1);
    }

  *quotient -= (divbin->qoffset - 1);
  *quotient_size = divbin->qoffset;
  
  skey_mem_alloc ((void **) &divbin->free_ptr, &divbin->qoffset);
  memcpy ((void *)  divbin->free_ptr, (const void *) *quotient, (size_t ) divbin->qoffset);
  *quotient = divbin->free_ptr;

  skey_mem_alloc ((void **) &divbin->free_ptr, remainder_size);
  memcpy ((void *) divbin->free_ptr, (const void *) *remainder, (size_t ) *remainder_size);
  *remainder = divbin->free_ptr;
   
  skey_dest_divbin  (skey_dest_new (), 
		     (void **) &divbin->mem.start, 
		     divisor, 
		     divident, 
		     *quotient, 
		     *remainder, 
		     divisor_size, 
		     divident_size, 
		     NULL,
		     NULL);
    free (divbin);
  return (0);
}
/*
 *
 * skey_divbin initialisation
 *
 * Version 0.0
 *
 */



skey_divbin_t *
skey_divbin_new ()
{
  return ((skey_divbin_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_divbin_t)));
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



int
skey_getmem (skey_getmem_t *getmem, \
	     void **start, \
	     void **ptr, \
	     char **target, \
	     unsigned int *ptr_size, \
	     unsigned int *size)
{
  *ptr_size = sizeof (skey_ptr_t);

  if (*start == NULL)
    if (skey_mem_alloc ((void **) start, ptr_size) != 0)
      {
	free (getmem);
	return (-1);
      }

  getmem->start = (skey_ptr_t *) *start;
  getmem->ptr = getmem->start;
 
  if (getmem->start->ptr == NULL)
    {
      if ((getmem->start->next = (void *) malloc (*ptr_size)) == NULL)
	{
	  free (getmem);
	  return (-1);
	}

      memset (getmem->start->next, '\0', *ptr_size);
	
      if (skey_mem_alloc ((void **) target, size) != 0)
	{
	  free (getmem);
	  return (-1);
	}

      getmem->start->ptr = (char *) *target;
      getmem->start->last = NULL;
      getmem->start->next->ptr = NULL;
      getmem->start->next->next = NULL;
      getmem->start->next->last = getmem->start;
    }

  else 
    {
      for (getmem->ptr = getmem->start; getmem->ptr->ptr != NULL; getmem->ptr = getmem->ptr->next);

      if ((getmem->ptr->next = (void *) malloc (*ptr_size)) == NULL)
	{
	  free (getmem);
	  return (-1);
	}

      memset (getmem->ptr->next, '\0', *ptr_size);

      if (skey_mem_alloc ((void **) target, size) != 0)
	{
	  free (getmem);
	  return (-1);
	}

      getmem->ptr->ptr = (char *) *target;
      getmem->ptr->next->ptr = NULL;
      getmem->ptr->next->next = NULL;
      getmem->ptr->next->last = getmem->ptr;
    }

  *target += (*size - 1);

  free (getmem);
  return (0);
}
/*
 *
 * skey_getmem initialisation
 *
 * Version 0.0
 *
 */



skey_getmem_t *
skey_getmem_new ()
{
  return ((skey_getmem_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_getmem_t)));
}
/*
 *
 * memory allocator function
 *
 * Version 0.0
 *
 */



int
skey_mem_alloc (void **ptr, unsigned int *size)
{
  *ptr = NULL;

  if ((*ptr = (void *) malloc ((size_t) *size + 1)) == NULL)
    return (-1);

    memset ((void *) *ptr, '\0', *size + 1);

  return (0);
}
/*
 *
 * skey_mod modulo conversion
 *
 * Version 0.0
 *
 */



int
skey_mod (skey_mod_t *mod, 
	  char *in, 
	  char *base, 
	  char *ascii_offset, 
	  char **out, 
	  unsigned int *in_size, 
	  unsigned int *base_size, 
	  unsigned int *ascii_offset_size, 
	  unsigned int *out_size)
{
  if ((in == NULL) || (base == NULL) || (out == NULL))
    {
      free (mod);
      return (-1);
    }

  if ((*base_size > 5) || (*base_size < 2))
    {
      free (mod);
      return (-1);
    }

  if (*in_size == 0)
    {
      *in_size = strlen (in);
      in += *in_size - 1;
    }

  if (*base_size == 0)
    {
      *base_size = strlen (base);
      base += *base_size - 1;
    }

  *out_size = (*in_size / 8 * 31) ;

  *out = NULL;

  if (skey_getmem (skey_getmem_new (), 
		   (void **) &mod->mem.start, 
		   (void **) &mod->mem.ptr, 
		   out, 
		   &mod->mem.size, 
		   out_size) != 0)
    {
      skey_dest_mod  (skey_dest_new (), 
		      (void **) &mod->mem.start, 
		      in, 
		      base, 
		      ascii_offset, 
		      *out, 
		      in_size, 
		      base_size, 
		      ascii_offset_size, 
		      NULL);
      free (mod);
      return (-1);
    }

  mod->out = *out;

  memset ((void *) (*out - *out_size + 1), '0', *out_size);

  mod->offset = 0;

  mod->divident_size = *in_size;

  if (skey_getmem (skey_getmem_new (), 
		   (void **) &mod->mem.start, 
		   (void **) &mod->mem.ptr, 
		   &mod->divident, 
		   &mod->mem.size, 
		   &mod->divident_size) != 0)
    {
      skey_dest_mod  (skey_dest_new (), 
		      (void **) &mod->mem.start, 
		      in, 
		      base, 
		      ascii_offset, 
		      *out, 
		      in_size, 
		      base_size, 
		      ascii_offset_size, 
		      out_size);
      free (mod);
      return (-1);
    }

  memcpy ((void *) (mod->divident - mod->divident_size + 1), 
	  (const void *) (in - *in_size + 1), mod->divident_size);

  mod->quotient = mod->divident;
  mod->quotient_size = mod->divident_size;
  mod->one = '0';
  mod->one_size = 1;

  while (skey_compare(skey_compare_new (),
		      mod->quotient, 
		      &mod->one, 
		      &mod->quotient_size, 
		      &mod->one_size) == 1)
    {
      mod->quotient = NULL;
      mod->remainder = NULL;

      if (skey_divbin (skey_divbin_new (), 
		       base, 
		       mod->divident, 
		       &mod->quotient, 
		       &mod->remainder, 
		       base_size, 
		       &mod->divident_size, 
		       &mod->quotient_size, 
		       &mod->remainder_size) != 0)
	{
 	  skey_dest_mod  (skey_dest_new (), 
			  (void **) &mod->mem.start, 
			  in, 
			  base, 
			  ascii_offset, 
			  *out, 
			  in_size, 
			  base_size, 
			  ascii_offset_size, 
			  out_size);
	  free (mod);
	  return (-1);
	}

      if (skey_store_ptr (skey_getmem_new (), 
			  (void **) &mod->mem.start, 
			  (void **) &mod->mem.ptr, 
			  mod->quotient, 
			  &mod->mem.size) != 0)
	{
	  skey_dest_mod  (skey_dest_new (), 
			  (void **) &mod->mem.start, 
			  in, 
			  base, 
			  ascii_offset, 
			  *out, 
			  in_size, 
			  base_size, 
			  ascii_offset_size, 
			  out_size);
	  free (mod);
	  return (-1);
	}

      if (skey_store_ptr (skey_getmem_new (), 
			  (void **) &mod->mem.start, 
			  (void **) &mod->mem.ptr, 
			  mod->remainder, 
			  &mod->mem.size) != 0)
	{
	  skey_dest_mod  (skey_dest_new (), 
			  (void **) &mod->mem.start, 
			  in, 
			  base, 
			  ascii_offset, 
			  *out, 
			  in_size, 
			  base_size, 
			  ascii_offset_size, 
			  out_size);
	  free (mod);
	  return (-1);
	}

      mod->quotient += (mod->quotient_size - 1);
      mod->divident = mod->quotient;
      mod->divident_size = mod->quotient_size;
      mod->remainder += (mod->remainder_size - 1);
      mod->result = NULL;

      if ( skey_addbin (skey_addbin_new (), 
			mod->remainder, 
			ascii_offset, 
			&mod->result, 
			&mod->remainder_size, 
			ascii_offset_size, 
			&mod->result_size) != 0)
	{
	  skey_dest_mod  (skey_dest_new (), 
			  (void **) &mod->mem.start, 
			  in, 
			  base, 
			  ascii_offset, 
			  *out, 
			  in_size, 
			  base_size, 
			  ascii_offset_size, 
			  out_size);
	  free (mod);
	  return (-1);
	}

      if (skey_store_ptr (skey_getmem_new (), 
			  (void **) &mod->mem.start, 
			  (void **) &mod->mem.ptr, 
			  mod->result, 
			  &mod->mem.size) != 0)
	{
	  skey_dest_mod  (skey_dest_new (), 
			  (void **) &mod->mem.start, 
			  in, 
			  base, 
			  ascii_offset, 
			  *out, 
			  in_size, 
			  base_size, 
			  ascii_offset_size, 
			  out_size);
	  free (mod);
	  return (-1);
	}
      
      mod->out_offset = mod->out;
      mod->out_offset -= (mod->result_size - 1);
      memcpy ((void *) mod->out_offset, 
	      (const void *) mod->result, (size_t ) mod->result_size);

      if (skey_compare(skey_compare_new (), 
		       mod->quotient, 
		       &mod->one, 
		       &mod->quotient_size, 
		       &mod->one_size) == 1) 
	mod->out -= 8;

      else
	mod->out -= 7;
    }
  

  *out = malloc(strlen(mod->out)+1);
  strcpy(*out,mod->out);

  skey_dest_mod  (skey_dest_new (), 
		  (void **) &mod->mem.start, 
		  in, 
		  base, 
		  ascii_offset, 
		  *out, 
		  in_size, 
		  base_size, 
		  ascii_offset_size, 
		  NULL);
  free (mod);
  return (0);
}
/*
 *
 * skey_mod initialisation
 *
 * Version 0.0
 *
 */



skey_mod_t *
skey_mod_new ()
{
  return ((skey_mod_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_mod_t)));
}
/*
 *
 * skey_mulbin dual-system multiplikation
 *
 * Version 0.0
 *
 */



int
skey_mulbin (skey_mulbin_t *mulbin,
	     char *multiplicand,
	     char *multiplier,
	     char **product,
	     unsigned int *multiplicand_size,
	     unsigned int *multiplier_size,
	     unsigned int *product_size)
{
  if ((multiplicand == NULL) || (multiplier == NULL) || (mulbin == NULL))
    {
      free (mulbin);
      return (-1);
    }

  if (*multiplicand_size == 0)
    {
      *multiplicand_size = strlen (multiplicand);
      multiplicand += strlen (multiplicand) - 1;
    }

  if (*multiplier_size == 0)
    {
      *multiplier_size = strlen (multiplier);
      multiplier += strlen (multiplier) - 1;
    }
   
  *product_size = *multiplier_size +  *multiplicand_size;


  if (skey_getmem(skey_getmem_new (),
		  (void **) &mulbin->mem.start,
		  (void **) &mulbin->mem.ptr,
		  product,
		  &mulbin->mem.size,
		  product_size) != 0)
    {
      skey_dest_mulbin  (skey_dest_new (),
			 (void **) &mulbin->mem.start,
			 multiplicand,
			 multiplier,
			 *product,
			 multiplicand_size, 
			 multiplier_size, 
			 product_size);
      free (mulbin);
      return (-1);
    }

  mulbin->multiplicand = multiplicand;
  mulbin->multiplier = multiplier;
  mulbin->multiplicand_size = *multiplicand_size;
  mulbin->multiplier_size = *multiplier_size;

  mulbin->ptr_size = mulbin->multiplier_size;

  if (skey_getmem(skey_getmem_new (),
		  (void **) &mulbin->mem.start,
		  (void **) &mulbin->mem.ptr,
		  &mulbin->ptr,
		  &mulbin->mem.size,
		  &mulbin->ptr_size) != 0)
    {
      skey_dest_mulbin  (skey_dest_new (),
			 (void **) &mulbin->mem.start,
			 multiplicand,
			 multiplier,
			 *product,
			 multiplicand_size, 
			 multiplier_size, 
			 product_size);
      free (mulbin);       
      return (-1);
    }

  mulbin->product = mulbin->ptr;
  mulbin->ptr -= (mulbin->ptr_size - 1);
  mulbin->product_size = mulbin->ptr_size;

  mulbin->result_size = *product_size;

  if (skey_getmem(skey_getmem_new (),
		  (void **) &mulbin->mem.start,
		  (void **) &mulbin->mem.ptr,
		  &mulbin->result,
		  &mulbin->mem.size,
		  &mulbin->result_size) != 0)
    {
      skey_dest_mulbin  (skey_dest_new (),
			 (void **) &mulbin->mem.start,
			 multiplicand,
			 multiplier,
			 *product,
			 multiplicand_size, 
			 multiplier_size, 
			 product_size);
      free (mulbin);
      return (-1);
    }

  memset ((void *) (mulbin->result - mulbin->result_size + 1), '\0', (size_t) mulbin->result_size );

  mulbin->size = mulbin->result_size;

  for (mulbin->offset = 0; mulbin->offset <= (mulbin->multiplier_size - 1); (mulbin->offset)++)
    {
      if ((*(mulbin->multiplier - mulbin->offset) == '1') &&
	  (mulbin->offset > 0))
	{ 
	  mulbin->ptr_size = (mulbin->offset + mulbin->multiplicand_size + 1);

	  mulbin->realloc = (char *) realloc((void *) mulbin->ptr,(size_t ) (mulbin->ptr_size ));
	  
	  if (skey_realptr (skey_dest_new (),
			    (void **) &mulbin->mem.start,
			    mulbin->ptr,
			    mulbin->realloc) != 0)
	    {
	      skey_dest_mulbin  (skey_dest_new (),
				 (void **) &mulbin->mem.start,
				 multiplicand,
				 multiplier,
				 *product,
				 multiplicand_size,
				 multiplier_size,
				 product_size);
	      free (mulbin);
	      return (-1);
	    }

	  mulbin->ptr_size -= 1;
	  mulbin->ptr = mulbin->realloc;
	  mulbin->product_size = mulbin->ptr_size;
	  mulbin->product = mulbin->ptr;

	  mulbin->product += (mulbin->multiplicand_size - 1);
 
	  (mulbin->ptr_size)++;
	  memset ( mulbin->ptr, '\0', mulbin->ptr_size);
	  (mulbin->ptr_size)--;
	 
	  memcpy ((void *) (mulbin->product - mulbin->multiplicand_size + 1),
		  (void *) (mulbin->multiplicand - mulbin->multiplicand_size + 1),
		  (size_t) mulbin->multiplicand_size);

	  for (mulbin->k = mulbin->offset; mulbin->k > 0; (mulbin->k)--)
	    *(mulbin->product + mulbin->k) = '0';

	  mulbin->product += mulbin->offset;

	  *product = NULL;
	  
	  if (skey_addbin (skey_addbin_new(),
			   mulbin->product,
			   mulbin->result,
			   product,
			   &mulbin->product_size,
			   &mulbin->size,
			   product_size) != 0)
	    {
	      skey_dest_mulbin  (skey_dest_new (),
				 (void **) &mulbin->mem.start,
				 multiplicand,
				 multiplier,
				 *product,
				 multiplicand_size, 
				 multiplier_size, 
				 product_size);
	      free (mulbin);
	      return (-1);
	    }
	  
	  if (skey_store_ptr (skey_getmem_new (),
			      (void **) &mulbin->mem.start,
			      (void **) &mulbin->mem.ptr,
			      *product,
			      &mulbin->mem.size) != 0)
	    {
	      skey_dest_mulbin  (skey_dest_new (),
				 (void **) &mulbin->mem.start,
				 multiplicand,
				 multiplier,
				 *product,
				 multiplicand_size, 
				 multiplier_size, 
				 product_size);
	      free (mulbin);
	      return (-1);
	    }
	  
	  *product += (*product_size - 1);

	  memcpy ((void *) (mulbin->result - *product_size + 1),
		  (const void *) (*product - *product_size + 1), (size_t ) *product_size);
	  mulbin->size = *product_size;
	}

      if ((*(mulbin->multiplier) == '1') && (mulbin->offset == 0))
	{
	  memcpy ((void *) (mulbin->result - mulbin->multiplicand_size + 1),
		  (const void *) (mulbin->multiplicand - (mulbin->multiplicand_size - 1)),
		  (size_t ) mulbin->multiplicand_size);

	  memcpy ((void *) (*product - mulbin->multiplicand_size + 1),
		  (void *) (mulbin->multiplicand - mulbin->multiplicand_size + 1),
		  (size_t) mulbin->multiplicand_size);

	  mulbin->size = mulbin->multiplicand_size;
	}
  
      if ((*(mulbin->multiplier) == '0') && (mulbin->offset == 0))
	{
	  memcpy ((void *) mulbin->result, "0", 1);

	  memcpy ((void *) *product, "0", 1);

	  mulbin->size = 1;
	}
    }

  if (skey_rmlz (product, &mulbin->size) != 0)
    {
      skey_dest_mulbin  (skey_dest_new (),
			 (void **) &mulbin->mem.start,
			 multiplicand,
			 multiplier,
			 *product,
			 multiplicand_size, 
			 multiplier_size, 
			 &mulbin->size);
      free (mulbin);
      return (-1);
    }
  
  mulbin->ptr = *product;
  
  if (skey_mem_alloc ((void **) product, &mulbin->size) != 0)
    {
      skey_dest_mulbin  (skey_dest_new (),
			 (void **) &mulbin->mem.start,
			 multiplicand,
			 multiplier,
			 *product,
			 multiplicand_size, 
			 multiplier_size, 
			 &mulbin->size);
      free (mulbin);
      return (-1);
    }
  
  memcpy ((void *) *product, (const void *) (mulbin->ptr - mulbin->size + 1), (size_t ) mulbin->size);
  *product_size = mulbin->size;

  skey_dest_mulbin  (skey_dest_new (),
		     (void **) &mulbin->mem.start,
		     multiplicand,
		     multiplier,
		     *product,
		     multiplicand_size, 
		     multiplier_size,
		     product_size);

  free (mulbin);
  return (0);
}
/*
 *
 * skey_mulbin initialisation
 *
 * Version 0.0
 *
 */



skey_mulbin_t *
skey_mulbin_new ()
{
  return ((skey_mulbin_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_mulbin_t)));
}
/*
 *
 * skey_read reads 
 *
 * Version 0.0
 *
 */



int
skey_read (skey_read_t *read, \
	   char *in, \
	   char **out, \
	   unsigned int *in_size, \
	   unsigned int *out_size)
{
  if (in == NULL)
    return (-1);

  if (*in_size == 0)
    {
      *in_size = strlen (in);
      in += *in_size;
    }

  read->size = *in_size * 8;
  *out_size = read->size;
  
  if (skey_getmem (skey_getmem_new (),
		   (void **) &read->mem.start,
		   (void **) &read->mem.ptr,
		   out,
		   &read->mem.size,
		   &read->size) != 0)
    {
      skey_dest_read (skey_dest_new (),
		      (void **) &read->mem.start,
		      in,
		      *out,
		      in_size,
		      &read->size);
      free (read);
      return (-1);
    }

  read->offset2 = 0;

  for (read->offset = 0; read->offset2 <= (read->size - 1); (read->offset)++)
    {
      if (((char ) *(in + read->offset) & (char ) 0x01) == (char ) 0x01)
	*(*out - read->offset2) = '1';

      else
	*(*out - read->offset2) = '0';
	
      (read->offset2)++;
	
      if (((char ) *(in + read->offset) & (char )  0x02) == (char ) 0x02)
	*(*out - read->offset2) = '1';

      else
	*(*out - read->offset2) = '0';

      (read->offset2)++;

      if (((char ) *(in + read->offset) & (char )  0x04) == (char ) 0x04)
	*(*out - read->offset2) = '1';

      else
	*(*out - read->offset2) = '0';

      (read->offset2)++;
      
      if (((char ) *(in + read->offset) & (char )  0x08) == (char ) 0x08)
	*(*out - read->offset2) = '1';

      else
	*(*out - read->offset2) = '0';

      (read->offset2)++;

      if (((char ) *(in + read->offset) & (char ) 0x10) == (char ) 0x10)
	*(*out - read->offset2) = '1';

      else
	*(*out - read->offset2) = '0';
	
      (read->offset2)++;
  
      if (((char ) *(in + read->offset) & (char )  0x20) == (char ) 0x20)
	*(*out - read->offset2) = '1';
    
      else
	*(*out - read->offset2) = '0';

      (read->offset2)++;

      if (((char ) *(in + read->offset) & (char )  0x40) == (char ) 0x40)
	*(*out - read->offset2) = '1';

      else
	*(*out - read->offset2) = '0';

      (read->offset2)++;
      
      if (((char ) *(in + read->offset) & (char )  0x80) == (char ) 0x80)
	*(*out - read->offset2) = '1';

      else
	*(*out - read->offset2) = '0';

      (read->offset2)++;
    }

  skey_dest_read (skey_dest_new (),
		  (void **) &read->mem.start,
		  in,
		  *out,
		  in_size,
		  &read->size);
  free (read);
  
  return (0);
}
/*
 *
 * skey_convert initialisation
 *
 * Version 0.0
 *
 */



skey_read_t *
skey_read_new ()
{
  return ((skey_read_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_read_t)));
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



int
skey_realptr (skey_dest_t *realptr, \
	      void **start, \
	      void *ptr_old, \
	      void *ptr_new)
{
  realptr->start = (skey_ptr_t *) *start;

  for  (;((realptr->start->ptr != ptr_old) && (realptr->start->next != NULL)); realptr->start = realptr->start->next);

  if (realptr->start->ptr == ptr_old)
    {
      realptr->start->ptr = ptr_new;
      free (realptr);
      return (0);
    }

  free (realptr);
  return (-1);
}
/*
 *
 * skey_rmlz remove leading zero's
 *
 * Version 0.0
 *
 */



int
skey_rmlg (char **num, unsigned int *size)
{
  while ((*(*num - (*size - 1)) == '0') && (*size > 1))
    {
      *size = *size - 1;
    }

  return (0);
}
/*
 *
 * skey_rmlz remove leading zero's
 *
 * Version 0.0
 *
 */



int
skey_rmlz (char **num, unsigned int *size)
{
  while ((*(*num - (*size - 1)) == '0') && (*size > 1))
    {
      *(*num - (*size - 1)) = '\0';
      *size = *size - 1;
    }

  return (0);
}
/*
 *
 * memory management function
 *
 * Version 0.0
 *
 */



int
skey_store_ptr (skey_getmem_t *store,
		void **start,
		void **ptr,
		void *target,
		unsigned int *ptr_size)
{
  store->start = (skey_ptr_t *) *start;
  store->ptr = (skey_ptr_t *) *ptr;

  store->ptr = store->start;
  *ptr_size = sizeof (skey_ptr_t);
  
  if (store->start == NULL)
    {
      free (store);
      return (-1);
    }

  if (store->start->ptr == NULL)
    {
      if ((store->start->next = (void *) malloc (*ptr_size)) == NULL)
	{
	  free (store);
	  return (-1);
	}

      memset (store->start->next, '\0', *ptr_size);

      store->start->ptr = (char *) target;
      store->start->last = NULL;
      store->start->next->ptr = NULL;
      store->start->next->next = NULL;
      store->start->next->last = store->start;
    }

  else 
    {
      for (store->ptr = store->start; store->ptr->ptr != NULL; store->ptr = store->ptr->next);

      if ((store->ptr->next = (void *) malloc (*ptr_size)) == NULL)
	{
	  free (store);
	  return (-1);
	}

      memset (store->ptr->next, '\0', *ptr_size);

      store->ptr->ptr = (char *) target;
      store->ptr->next->ptr = NULL;
      store->ptr->next->next = NULL;
      store->ptr->next->last = store->ptr;
    }

  free (store);
  return (0);
}
/*
 *
 * skey_subbin dual-system subtraction
 *
 * Version 0.0
 *
 */



int
skey_subbin (skey_subbin_t *subbin, 
	     char *minuend, 
	     char *subtrahend, 
	     char **difference, 
	     unsigned int *minuend_size, 
	     unsigned int *subtrahend_size, 
	     unsigned int *difference_size)
{
  if ((minuend != NULL) && (subtrahend != NULL) && (subbin != NULL))
    {
      subbin->minuend = minuend;
      subbin->subtrahend = subtrahend;
    }

  else
    {
      free (subbin);
      return (-1);
    }

  if (*minuend_size == 0)
    {
      *minuend_size = strlen (minuend);
      subbin->minuend += *minuend_size - 1;
    }
  
  if (*subtrahend_size == 0)
    {
      *subtrahend_size = strlen (subtrahend);
      subbin->subtrahend += *subtrahend_size - 1;
    }

  if (*minuend_size < *subtrahend_size)
    {
      free (subbin);
      return (-1);
    }

  if (skey_getmem (skey_getmem_new (), 
		   (void **) &subbin->mem.start, 
		   (void **) &subbin->mem.ptr, 
		   &subbin->new_subtrahend, 
		   &subbin->mem.size, 
		   minuend_size) != 0)
    {
      skey_dest_subbin (skey_dest_new (), 
			(void **) &subbin->mem.start, 
			minuend, 
			subtrahend, 
			*difference, 
			minuend_size, 
			subtrahend_size, 
			NULL);
      free (subbin);
      return (-1);
    }
  
  for (subbin->offset = 0; subbin->offset <= (*subtrahend_size - 1); (subbin->offset)++)
    {
      if (*(subbin->subtrahend - subbin->offset) == '0')
	*(subbin->new_subtrahend - subbin->offset) = '1';

      else
	*(subbin->new_subtrahend - subbin->offset) = '0';
    }

  for (subbin->offset = *subtrahend_size; subbin->offset <= (*minuend_size - 1); (subbin->offset)++)
    *(subbin->new_subtrahend - subbin->offset) = '1';
  
  *difference = NULL;
  
  if (skey_addbin (skey_addbin_new(), 
		   subbin->minuend, 
		   subbin->new_subtrahend, 
		   difference, 
		   minuend_size, 
		   minuend_size, 
		   difference_size) != 0)
    {
      skey_dest_subbin (skey_dest_new (), 
			(void **) &subbin->mem.start, 
			minuend, 
			subtrahend, 
			*difference, 
			minuend_size, 
			subtrahend_size, 
			difference_size);
      free (subbin);
      return (-1);
    }

  if (skey_store_ptr (skey_getmem_new (), 
		      (void **) &subbin->mem.start, 
		      (void **) &subbin->mem.ptr, 
		      *difference, 
		      &subbin->mem.size) != 0)
    {
      skey_dest_subbin (skey_dest_new (), 
			(void **) &subbin->mem.start, 
			minuend, 
			subtrahend, 
			*difference, 
			minuend_size, 
			subtrahend_size, 
			NULL);
      free (subbin);
      return (-1);
    }

  subbin->minuend = (char *) *difference;
  subbin->one = '1';
  subbin->subtrahend = &subbin->one;
  subbin->size = 1;
  subbin->minuend += *difference_size - 1;
  subbin->minuend_size = *difference_size;

  *difference = NULL;

  if (skey_addbin (skey_addbin_new(), 
		   subbin->minuend, 
		   subbin->subtrahend, 
		   difference, 
		   &subbin->minuend_size, 
		   &subbin->size, 
		   difference_size) != 0)
    {
      skey_dest_subbin (skey_dest_new (), 
			(void **) &subbin->mem.start, 
			minuend, 
			subtrahend, 
			*difference, 
			minuend_size, 
			subtrahend_size, 
			difference_size);
      free (subbin);
      return (-1);
    }

  if (skey_store_ptr (skey_getmem_new (), 
		      (void **) &subbin->mem.start, 
		      (void **) &subbin->mem.ptr, 
		      *difference, 
		      &subbin->mem.size) != 0)
    {
      skey_dest_subbin (skey_dest_new (), 
			(void **) &subbin->mem.start, 
			minuend, 
			subtrahend, 
			*difference, 
			minuend_size, 
			subtrahend_size, 
			NULL);
      free (subbin);
      return (-1);
    }

  *(*difference) = '0';
  *difference += (*difference_size - 1);

  if (skey_rmlz (difference, difference_size))
    {
      skey_dest_subbin (skey_dest_new (), 
			(void **) &subbin->mem.start, 
			minuend, 
			subtrahend, 
			*difference, 
			minuend_size, 
			subtrahend_size, 
			difference_size);
      free (subbin);
      return (-1);
    }

  *difference -= (*difference_size - 1);

  if (skey_mem_alloc ((void **) &subbin->new_subtrahend, difference_size) != 0)
    {
      skey_dest_subbin (skey_dest_new (), 
			(void **) &subbin->mem.start, 
			minuend, 
			subtrahend, 
			*difference, 
			minuend_size, 
			subtrahend_size, 
			difference_size);
      free (subbin);
      return (-1);
    }

  memcpy ((void *) subbin->new_subtrahend, (const void *) *difference, (size_t) *difference_size);

  *difference = subbin->new_subtrahend;

  skey_dest_subbin (skey_dest_new (), 
		    (void **) &subbin->mem.start, 
		    minuend, 
		    subtrahend, 
		    *difference, 
		    minuend_size, 
		    subtrahend_size, 
		    NULL);

  free (subbin);

  return (0);
}
/*
 *
 * skey_subbin initialisation
 *
 * Version 0.0
 *
 */



skey_subbin_t *
skey_subbin_new ()
{
  return ((skey_subbin_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_addbin_t)));
}
/*
 *
 * skey_unmod reverse modulo conversion
 *
 * Version 0.0
 *
 */



int
skey_unmod (skey_unmod_t *unmod,
	    char *in, 
	    char *base, 
	    char *ascii_offset, 
	    char **out, 
	    unsigned int *in_size, 
	    unsigned int *base_size, 
	    unsigned int *ascii_offset_size, 
	    unsigned int *out_size)
{
  if ((in == NULL) || (base == NULL) || (out == NULL) || (ascii_offset == NULL))
    {
      free (unmod);
      return (-1);
    }

  if ((*base_size > 5) || (*base_size < 2))
    {
      free (unmod);
      return (-1);
    }

  if (*in_size == 0)
    {
      *in_size = strlen (in);
      in += *in_size - 1;
    }

  if (*base_size == 0)
    {
      *base_size = strlen (base);
      base += *base_size - 1;
    }
  
  unmod->out_size = (*in_size / 8  + 8 )* *base_size;
  
  if (skey_getmem (skey_getmem_new (),
		   (void **) &unmod->mem.start, 
		   (void **) &unmod->mem.ptr, 
		   &unmod->out, 
		   &unmod->mem.size, 
		   &unmod->out_size) != 0)
    {
      skey_dest_unmod  (skey_dest_new (),
			(void **) &unmod->mem.start, 
			in, 
			base, 
			ascii_offset, 
			*out, 
			in_size, 
			base_size, 
			ascii_offset_size, 
			NULL);
      free (unmod);
      return (-1);
    }

  unmod->in_ptr = &unmod->in;
  unmod->offset2 = 8;
  unmod->sum = &unmod->zero;;
  unmod->sum_size = 1;

  if (skey_getmem (skey_getmem_new (),
		   (void **) &unmod->mem.start, 
		   (void **) &unmod->mem.ptr, 
		   unmod->in_ptr, 
		   &unmod->mem.size, 
		   in_size) != 0)
    {
      skey_dest_unmod  (skey_dest_new (),
			(void **) &unmod->mem.start, 
			in, 
			base, 
			ascii_offset, 
			*out, 
			in_size, 
			base_size, 
			ascii_offset_size, 
			NULL);
      free (unmod);
      return (-1);
    }

  memcpy ((void *) (unmod->in - *in_size + 1), 
	  (const void *) (in - *in_size + 1), (size_t ) *in_size);

  unmod->one = '1';
  unmod->zero = '0';

  for (unmod->offset = 0; unmod->offset < skey_unmod_test (in_size); (unmod->offset)++)
    {
      if ((((int ) (*in_size - unmod->offset)) < 8  && (((int ) (*in_size  - unmod->offset * 8)) > 0))) 
	{
	  unmod->offset2 = *in_size - unmod->offset;
	  unmod->in += (8 - unmod->offset2);
	}

      skey_rmlz(unmod->in_ptr, &unmod->offset2);

      if (skey_subbin (skey_subbin_new (), 
		       unmod->in, 
		       ascii_offset, 
		       &unmod->in2, 
		       &unmod->offset2, 
		       ascii_offset_size, 
		       &unmod->in2_size) != 0)
	{
	  skey_dest_unmod  (skey_dest_new (),
			    (void **) &unmod->mem.start, 
			    in, 
			    base, 
			    ascii_offset, 
			    *out, 
			    in_size, 
			    base_size, 
			    ascii_offset_size, 
			    NULL);
	  free (unmod);
	  return (-1);
	}

      if (skey_store_ptr (skey_getmem_new (), 
			  (void **) &unmod->mem.start, 
			  (void **) &unmod->mem.ptr, 
			  unmod->in2, 
			  &unmod->mem.size) != 0)
	{
	  skey_dest_unmod  (skey_dest_new (),
			    (void **) &unmod->mem.start, 
			    in, 
			    base, 
			    ascii_offset, 
			    *out, 
			    in_size, 
			    base_size, 
			    ascii_offset_size, 
			    NULL);
	  free (unmod);
	  return (-1);
	}

      unmod->in2 += (unmod->in2_size - 1);
      
      if (unmod->offset >= 1)
	{
	  if (skey_mulbin (skey_mulbin_new (), 
			   base, 
			   unmod->base, 
			   &unmod->position, 
			   base_size, 
			   &unmod->base_size, 
			   &unmod->position_size) != 0)
	    {
	      skey_dest_unmod  (skey_dest_new (),
				(void **) &unmod->mem.start, 
				in, 
				base, 
				ascii_offset, 
				*out, 
				in_size, 
				base_size, 
				ascii_offset_size, 
				NULL);
	      free (unmod);
	      return (-1);
	    }

	  if (skey_store_ptr (skey_getmem_new (), 
			      (void **) &unmod->mem.start, 
			      (void **) &unmod->mem.ptr, 
			      unmod->position, 
			      &unmod->mem.size) != 0)
	    {
	      skey_dest_unmod  (skey_dest_new (),
				(void **) &unmod->mem.start, 
				in, 
				base, 
				ascii_offset, 
				*out, 
				in_size, 
				base_size, 
				ascii_offset_size, 
				NULL);
	      free (unmod);
	      return (-1);
	    }

	  unmod->position += (unmod->position_size - 1);
	  unmod->base = unmod->position;
	  unmod->base_size = unmod->position_size;
	}
      
      else
	{
	  unmod->base = &unmod->one;
	  unmod->base_size = 1;
	  unmod->position = &unmod->one;
	  unmod->position_size = 1;
	}

      if (skey_mulbin (skey_mulbin_new (), 
		       unmod->in2, 
		       unmod->position, 
		       &unmod->product, 
		       &unmod->in2_size, 
		       &unmod->position_size, 
		       &unmod->product_size) != 0)
	{
	  skey_dest_unmod  (skey_dest_new (),
			    (void **) &unmod->mem.start, 
			    in, 
			    base, 
			    ascii_offset, 
			    *out, 
			    in_size, 
			    base_size, 
			    ascii_offset_size, 
			    NULL);
	  free (unmod);
	  return (-1);
	}

      if (skey_store_ptr (skey_getmem_new (), 
			  (void **) &unmod->mem.start, 
			  (void **) &unmod->mem.ptr, 
			  unmod->product, 
			  &unmod->mem.size) != 0)
	{
	  skey_dest_unmod  (skey_dest_new (),
			    (void **) &unmod->mem.start, 
			    in, 
			    base, 
			    ascii_offset, 
			    *out, 
			    in_size, 
			    base_size, 
			    ascii_offset_size, 
			    NULL);
	  free (unmod);
	  return (-1);
	}

/* Hier war  (unmod->base_size = 1) */
      if (unmod->base_size == 1)
	{
	  unmod->base = unmod->position;
	  unmod->base_size = unmod->position_size;
	}

      unmod->product += (unmod->product_size - 1);

      if (skey_addbin (skey_addbin_new (), 
		       unmod->product, 
		       unmod->sum, 
		       out, 
		       &unmod->product_size, 
		       &unmod->sum_size, 
		       out_size) != 0)
	{
	  skey_dest_unmod  (skey_dest_new (),
			    (void **) &unmod->mem.start, 
			    in, 
			    base, 
			    ascii_offset, 
			    *out, 
			    in_size, 
			    base_size, 
			    ascii_offset_size, 
			    out_size);
	  free (unmod);
	  return (-1);
	}

      if (skey_store_ptr (skey_getmem_new (), 
			  (void **) &unmod->mem.start, 
			  (void **) &unmod->mem.ptr,
			  *out, 
			  &unmod->mem.size) != 0)
	{
	  skey_dest_unmod  (skey_dest_new (),
			    (void **) &unmod->mem.start, 
			    in, 
			    base, 
			    ascii_offset, 
			    *out, 
			    in_size, 
			    base_size, 
			    ascii_offset_size, 
			    NULL);
	  free (unmod);
	  return (-1);
	}

      unmod->sum = (*out + *out_size - 1);
      unmod->sum_size = *out_size;
      
      unmod->in -= 8;
      unmod->offset2 = 8;
    }

  skey_dest_unmod  (skey_dest_new (),
		    (void **) &unmod->mem.start,
		    in, 
		    base, 
		    ascii_offset, 
		    *out, 
		    in_size, 
		    base_size, 
		    ascii_offset_size,
		    NULL);
  free (unmod);

  return (0);
}

/*
 *
 * skey_unmod initialisation
 *
 * Version 0.0
 *
 */

skey_unmod_t *
skey_unmod_new ()
{
  return ((skey_unmod_t *) calloc ((size_t ) 1, (size_t ) sizeof (skey_unmod_t)));
}

/*
 *
 * skey_unmod reverse modulo conversion
 *
 * Version 0.0
 *
 */

int
skey_unmod_test (unsigned int *i)
{
  if ((*i - (*i / 8) * 8) == 0)
    {
      return (*i / 8);
    }

  else
    {
      return (*i / 8 + 1);
    }
}


#ifdef __cplusplus 
}
#endif
