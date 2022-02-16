/*
 *
 * smtp function data send the DATA
 * command
 *
 * Version 0.0
 *
 */

#include "smtp.h"

#include <stdio.h>

/* --- */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* --- */

#ifdef __cplusplus 
extern "C" {
#endif

  /*------------------------------------------------------------------------------*/
  /*
   *
   * text data function
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/
  int
  smtp_data (int sockfd, char *data, char *buf)
  {
    char buffer[3625];

    char *data_buf;
    char *ptr;
    char *strptr;

    char encoding[] = 
    {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', \
      'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', \
      'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', \
      '5', '6', '7', '8', '9', '+', '/'
    };

    val64 cbuf;

    unsigned int counter;
    unsigned int counter64;
    unsigned int linecount;
    unsigned int slotcount;
    int rest;
    int ret;

    /* --- * /
       int fd;
       / * --- */
      
    counter = 0;
    counter64 = 0;
    linecount = 0;
    slotcount = 0;
    rest = 0;
    ret = 0;

    data_buf = NULL;
    strptr = NULL;
    ptr = data;

    /* --- * /
       fd = open ("/tmp/email______", O_RDWR|O_CREAT|O_APPEND);
       / * --- */

    cbuf.num = 0;

    while (ptr < (data + strlen (data)))
      {
	memset (&buffer, '\0', sizeof (buffer));
	counter = 0;

	while ((ptr < (data + strlen (data))) &&
	       (counter < sizeof (buffer)))
	  {
	    cbuf.num = 0;

	    if ((ptr + 3) <= (data + strlen (data)))
	      memcpy ((void *) &cbuf.arry[0], (void *) ptr, 3);

	    else
	      memcpy ((void *) &cbuf.arry[0], (void *) ptr, ((data + (strlen (data))) - ptr));

	    if ((counter < sizeof (buffer)) &&
		(linecount < (76 - 1)))
	      {
		buffer[counter] = encoding[(cbuf.arry[0] & 0xfc) >> 2];
		counter++;
		linecount++;
		counter64++;
	      }

	    else if ((counter < sizeof (buffer)) &&
		     (linecount == (76 - 1)))
	      {
		if ((counter < (sizeof (buffer) - 2)) &&
		    (linecount == (76 - 1)))
		  {
		    buffer[counter] = 0x0d;
		    counter++;
		    buffer[counter] = 0x0a;
		    counter++;
		    linecount = 0;
		  }

		else
		  {
		    if (tcp_write (sockfd, &buffer, strlen ((char *) &buffer)) < 0)
		      return (-1);
		   
		    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
		    counter = 0;
		    buffer[counter] = 0x0d;
		    counter++;
		    buffer[counter] = 0x0a;
		    counter++;
		    linecount = 0;
		  }
		buffer[counter] = encoding[(cbuf.arry[0] & 0xfc) >> 2];
		counter++;
		linecount++;
		counter64++;
	      }

	    else if (counter > ((sizeof (buffer) - 2)))
	      {
		if (tcp_write (sockfd, &buffer, strlen ((char *) &buffer)) < 0)
		  return (-1);
	       
		memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
		counter = 0;
		buffer[counter] = encoding[(cbuf.arry[0] & 0xfc) >> 2];
		counter++;
		linecount++;
		counter64++;
	      }

	    if ((counter < sizeof (buffer)) &&
		(linecount < (76 - 1)))
	      {
		buffer[counter] = encoding[((cbuf.arry[0] & 3) << 4) | ((cbuf.arry[1] & 0xf0) >> 4)];
		counter++;
		linecount++;
		counter64++;
	      }

	    else if ((counter < sizeof (buffer)) &&
		     (linecount == (76 - 1)))
	      {
		if ((counter < (sizeof (buffer) - 2)) &&
		    (linecount == (76 - 1)))
		  {
		    buffer[counter] = 0x0d;
		    counter++;
		    buffer[counter] = 0x0a;
		    counter++;
		    linecount = 0;
		  }

		else
		  {
		    if (tcp_write (sockfd, &buffer, strlen ((char *) &buffer)) < 0)
		      return (-1);
		   
		    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
		    counter = 0;
		    buffer[counter] = 0x0d;
		    counter++;
		    buffer[counter] = 0x0a;
		    counter++;
		    linecount = 0;
		  }

		buffer[counter] = encoding[((cbuf.arry[0] & 3) << 4) | ((cbuf.arry[1] & 0xf0) >> 4)];
		counter++;
		linecount++;
		counter64++;
	      }

	    else if (counter > ((sizeof (buffer) - 2)))
	      {
		if (tcp_write (sockfd, &buffer, strlen ((char *) &buffer)) < 0)
		  return (-1);
	       
		memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
		counter = 0;
		buffer[counter] = encoding[((cbuf.arry[0] & 3) << 4) | ((cbuf.arry[1] & 0xf0) >> 4)];
		counter++;
		linecount++;
		counter64++;
	      }

	    if ((counter < sizeof (buffer)) &&
		(linecount < (76 - 1)))
	      {
		buffer[counter] = encoding[((cbuf.arry[1] & 0xF) << 2) | ((cbuf.arry[2] & 0xc0) >> 6)];
		counter++;
		linecount++;
		counter64++;
	      }

	    else if ((counter < sizeof (buffer)) &&
		     (linecount == (76 - 1)))
	      {
		if ((counter < (sizeof (buffer) - 2)) &&
		    (linecount == (76 - 1)))
		  {
		    buffer[counter] = 0x0d;
		    counter++;
		    buffer[counter] = 0x0a;
		    counter++;
		    linecount = 0;
		  }

		else
		  {
		    if (tcp_write (sockfd, &buffer, strlen ((char *) &buffer)) < 0)
		      return (-1);
		   
		    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
		    counter = 0;
		    buffer[counter] = 0x0d;
		    counter++;
		    buffer[counter] = 0x0a;
		    counter++;
		    linecount = 0;
		  }

		buffer[counter] = encoding[((cbuf.arry[1] & 0xF) << 2) | ((cbuf.arry[2] & 0xc0) >> 6)];
		counter++;
		linecount++;
		counter64++;
	      }

	    else if (counter > ((sizeof (buffer) - 2)))
	      {
		if (tcp_write (sockfd, &buffer, strlen ((char *) &buffer)) < 0)
		  return (-1);
	       
		memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
		counter = 0;
		buffer[counter] = encoding[((cbuf.arry[1] & 0xF) << 2) | ((cbuf.arry[2] & 0xc0) >> 6)];
		counter++;
		linecount++;
		counter64++;
	      }

	    if ((counter < sizeof (buffer)) &&
		(linecount < (76 - 1)))
	      {
		buffer[counter] = encoding[cbuf.arry[2] & 0x3F];
		counter++;
		linecount++;
		counter64++;
	      }

	    else if ((counter < sizeof (buffer)) &&
		     (linecount == (76 - 1)))
	      {
		if ((counter < (sizeof (buffer) - 2)) &&
		    (linecount == (76 - 1)))
		  {
		    buffer[counter] = 0x0d;
		    counter++;
		    buffer[counter] = 0x0a;
		    counter++;
		    linecount = 0;
		  }

		else
		  {
		    if (tcp_write (sockfd, &buffer, strlen ((char *) &buffer)) < 0)
		      return (-1);
		   
		    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
		    counter = 0;
		    buffer[counter] = 0x0d;
		    counter++;
		    buffer[counter] = 0x0a;
		    counter++;
		    linecount = 0;
		  }

		buffer[counter] = encoding[cbuf.arry[2] & 0x3F];
		counter++;
		linecount++;
		counter64++;
	      }

	    else if (counter > ((sizeof (buffer) - 2)))
	      {
		if (tcp_write (sockfd, &buffer, strlen ((char *) &buffer)) < 0)
		  return (-1);
	       
		memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
		counter = 0;
		buffer[counter] = encoding[cbuf.arry[2] & 0x3F];
		counter++;
		linecount++;
		counter64++;
	      }

	    ptr = ptr + 3;
	  }

	if (counter > 0)
	  {
	    if (tcp_write (sockfd, &buffer, strlen ((char *) &buffer)) < 0)
	      return (-1);
	    
	    /* --- * /
	       write (fd, &buffer, strlen ((char *) &buffer));
	       close (fd);
	       / * --- */

	    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	    counter = 0;
	  }
      }

    if ((rest = counter64 % 3) > 0)
      {
	if (!((counter + rest + 7) < sizeof (buffer)))
	  {
	    if (tcp_write (sockfd, &buffer, strlen ((char *) &buffer)) < 0)
	      return (-1);

	    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	    counter = 0;
	  }

	for (; rest != 0; rest--)
	  {
	    if (linecount < (76 - 1))
	      {
		buffer[counter] = '=';
		counter++;
		linecount++;
		linecount = 0;
	      }
	    else
	      {
		buffer[counter] = 0x0d;
		counter++;
		buffer[counter] = 0x0a;
		counter++;
		linecount = 0;
		buffer[counter] = '=';
		counter++;
		linecount++;
	      }
	  }
      }

    buffer[counter] = 0x0d;
    counter++;
    buffer[counter] = 0x0a;
    counter++;
    buffer[counter] = 0x0d;
    counter++;
    buffer[counter] = 0x0a;
    counter++;
    buffer[counter] = '.';
    counter++;
    buffer[counter] = 0x0d;
    counter++;
    buffer[counter] = 0x0a;

    if (tcp_write (sockfd, &buffer, strlen ((char *) &buffer)) < 0)
      return (-1);

    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
    ptr = &buffer[0];

    do 
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer + sizeof (buffer) - 1))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer, ' ')) == NULL)
      return (-1);

    *strptr = '\0';
  
    ret = atoi ((const char *) &buffer);
  
    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';
  
    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    if (ret != 354)
      return (ret);

    if (tcp_write (sockfd, data_buf, strlen (data_buf)) < 0)
      return (-1);

    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
    ptr = &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer + sizeof (buffer) - 1))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' '); 
  
    if ((strptr = strchr ((const char *) &buffer, ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memset (buf, '\0', (size_t) sizeof (buffer));
    strcpy(buf,ptr);
    /*memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));*/

    return (ret);
  }
  /*------------------------------------------------------------------------------*/
  /*
   *
   * text data function
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/
  int
  smtp_8_data (int sockfd, char *data, char *buf)
  {
    char buffer[1024];

    char *data_buf;
    char *ptr;
    char *strptr;
    int ret;

    /* --- * /
       int fd;
       / * --- */

    ret = 0;

    data_buf = NULL;
    strptr = NULL;
    ptr = data;

    /* --- * /
       fd = open ("/tmp/email______", O_RDWR|O_CREAT|O_APPEND);
       / * --- */

    if (tcp_write (sockfd, ptr, strlen (ptr)) < 0)
      return (-1);

    /* --- * /
       memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
       sprintf (buffer, "DAS IST DER BUFFER INHALT: >\"%s\"<", ptr);
       write (fd, buffer, strlen (buffer) + 1);
       close (fd);
       / * --- */

    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
    sprintf (buffer, "\r\n\r\n.\r\n");
    
    if (tcp_write (sockfd, &buffer, strlen (buffer)) < 0)
      return (-1);

    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
    ptr = &buffer[0];

    do 
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer + sizeof (buffer) - 1))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer, ' ')) == NULL)
      return (-1);

    *strptr = '\0';
  
    ret = atoi ((const char *) &buffer);
  
    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';
  
    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    if (ret != 354)
      return (ret);

    if (tcp_write (sockfd, data_buf, strlen (data_buf)) < 0)
      return (-1);

    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
    ptr = &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer + sizeof (buffer) - 1))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' '); 
  
    if ((strptr = strchr ((const char *) &buffer, ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memset (buf, '\0', (size_t) sizeof (buffer));
    strcpy(buf,ptr);
    /*memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));*/

    return (ret);
  }
  /*------------------------------------------------------------------------------*/
  /*
   *
   * binary data function
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/
  int
  smtp_bin_data (int sockfd, char *data, int size, char *buf)
  {
    char buffer[3625];

    char *data_buf;
    char *ptr;
    char *strptr;

    char encoding[] = 
    {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', \
      'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', \
      'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', \
      '5', '6', '7', '8', '9', '+', '/'
    };

    val64 cbuf;

    unsigned int counter;
    unsigned int counter64;
    unsigned int linecount;
    unsigned int slotcount;
    unsigned int tripcount;
    int copysize;
    int datasize;
    int i;
    int rest;
    int ret;

    /* --- * /
       int fd;
       / * --- */
      
    counter = 0;
    counter64 = 0;
    linecount = 0;
    slotcount = 0;
    tripcount = 0;
    copysize = 0;
    datasize = 0;
    i = 0;
    rest = 0;
    ret = 0;

    data_buf = NULL;
    strptr = NULL;
    ptr = data;

    /* --- * /
       fd = open ("/tmp/email______", O_RDWR|O_CREAT|O_APPEND);
       / * --- */

    cbuf.num = 0;

    memset (buffer, '\0', sizeof (buffer));

    while (counter64 < (size - 1))
      {
	while (slotcount < (sizeof (buffer) - 6))
	  {
	    cbuf.num = 0;
		
	    if ((copysize = size - counter64) >= 3)
	      copysize = 3;
		
	    memcpy ((void *) &cbuf.arry[0], (void *) (ptr + counter64), copysize);

	    if ((linecount + 3) <= (76 - 2))
	      {
		buffer[slotcount++] = encoding[(cbuf.arry[0] & 0xfc) >> 2];
		counter++;
		linecount++;
	      }

	    else
	      {
		buffer[slotcount++] = '\r';
		buffer[slotcount++] = '\n';
		linecount = 0;
		buffer[slotcount++] = encoding[(cbuf.arry[0] & 0xfc) >> 2];
		counter++;
		linecount++;
	      }
	    
	    if ((linecount + 3) <= (76 - 2))
	      {
		buffer[slotcount++] = encoding[(((cbuf.arry[0] & 3) << 4) | ((cbuf.arry[1] & 0xf0) >> 4))];
		counter++;
		linecount++;
	      }

	    else
	      {
		buffer[slotcount++] = '\r';
		buffer[slotcount++] = '\n';
		linecount = 0;
		buffer[slotcount++] = encoding[(((cbuf.arry[0] & 3) << 4) | ((cbuf.arry[1] & 0xf0) >> 4))];
		counter++;
		linecount++;
	      }

	    if ((linecount + 3) <= (76 - 2))
	      {
		if (copysize < 2)
		  {
		    counter64 += copysize;
		    break;
		  }

		buffer[slotcount++] = encoding[(((cbuf.arry[1] & 0x0f) << 2) | ((cbuf.arry[2] & 0xc0) >> 6))];
		counter++;
		linecount++;
	      }

	    else
	      {
		if (copysize < 2)
		  {
		    counter64 += copysize;
		    break;
		  }

		buffer[slotcount++] = '\r';
		buffer[slotcount++] = '\n';
		linecount = 0;
		buffer[slotcount++] = encoding[(((cbuf.arry[1] & 0x0f) << 2) | ((cbuf.arry[2] & 0xc0) >> 6))];
		counter++;
		linecount++;
	      }

	    if ((linecount + 3) <= (76 - 2))
	      {
		if (copysize < 3)
		  {
		    counter64 += copysize;
		    break;
		  }


		buffer[slotcount++] = encoding[(cbuf.arry[2] & 0x3f)];
		counter++;
		linecount++;
	      }
	    
	    else
	      {
		if ((copysize < 3) <= 0)
		  {
		    counter64 += copysize;
		    break;
		  }

		buffer[slotcount++] = '\r';
		buffer[slotcount++] = '\n';
		counter++;
		linecount = 0;
		buffer[slotcount++] = encoding[(cbuf.arry[2] & 0x3f)];
		counter++;
		linecount++;
	      }

	    counter64 += copysize;

	  }

	if (tcp_write (sockfd, buffer, slotcount) < 0)
	  return (-1);
	
	memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	slotcount = 0;
      }

	
    if (copysize < 2)
      i = 2;
    
    else
      i = 1;

    if ((slotcount + i + strlen ("----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg") + 7) > sizeof (buffer))
      {
	if (tcp_write (sockfd, buffer, slotcount) < 0)
	  return (-1);
	    
	memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	slotcount = 0;
	linecount = 0;
      }

    for (; i > 0; i--)
      buffer[slotcount++] = '=';

    buffer[slotcount++] = '\r';
    buffer[slotcount++] = '\n';
    strcpy (&buffer[slotcount], "----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg");
    slotcount += strlen ("----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg");
    buffer[slotcount++] = '\r';
    buffer[slotcount++] = '\n';
    buffer[slotcount++] = '.';
    buffer[slotcount++] = '\r';
    buffer[slotcount++] = '\n';
    
    if (tcp_write (sockfd, buffer, slotcount) < 0)
      return (-1);
    
    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
    ptr = &buffer[0];
	
    do 
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
		
	    ptr++;
		
	    if ((void *) ptr > (void *) (&buffer + sizeof (buffer) - 1))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer, ' ')) == NULL)
      return (-1);

    *strptr = '\0';
  
    ret = atoi ((const char *) &buffer);
  
    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';
  
    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    if (ret != 354)
      return (ret);

    if (tcp_write (sockfd, buffer, slotcount) < 0)
      return (-1);

    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
    ptr = &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer + sizeof (buffer) - 1))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' '); 
  
    if ((strptr = strchr ((const char *) &buffer, ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memset (buf, '\0', (size_t) sizeof (buffer));
    strcpy(buf,ptr);
    /*memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));*/

    return (ret);

  }
  /*------------------------------------------------------------------------------*/
  /*
   *
   * binary data function without additional smtp server conversation
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/
  int
  smtp_b_data (int sockfd, char *data, int size, char *buf)
  {
    char buffer[3625];

    char *data_buf;
    char *ptr;
    char *strptr;

    char encoding[] = 
    {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', \
      'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', \
      'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', \
      '5', '6', '7', '8', '9', '+', '/'
    };

    val64 cbuf;

    unsigned int counter;
    unsigned int counter64;
    unsigned int linecount;
    unsigned int slotcount;
    int copysize;
    int datasize;
    int i;
    int rest;
    int ret;

    /* --- * /
       int fd;
       / * --- */
      
    counter = 0;
    counter64 = 0;
    linecount = 0;
    slotcount = 0;
    copysize = 0;
    datasize = 0;
    i = 0;
    rest = 0;
    ret = 0;

    data_buf = NULL;
    strptr = NULL;
    ptr = data;

    /* --- * /
       fd = open ("/tmp/email______", O_RDWR|O_CREAT|O_APPEND);
       / * --- */

    cbuf.num = 0;

    memset (buffer, '\0', sizeof (buffer));

    while (counter64 < (size - 1))
      {
	while (slotcount < (sizeof (buffer) - 6))
	  {
	    cbuf.num = 0;
		
	    if ((copysize = size - counter64) >= 3)
	      copysize = 3;
		
	    memcpy ((void *) &cbuf.arry[0], (void *) (ptr + counter64), copysize);

	    if ((linecount + 3) <= (76 - 2))
	      {
		buffer[slotcount++] = encoding[(cbuf.arry[0] & 0xfc) >> 2];
		counter++;
		linecount++;
	      }

	    else
	      {
		buffer[slotcount++] = '\r';
		buffer[slotcount++] = '\n';
		linecount = 0;
		buffer[slotcount++] = encoding[(cbuf.arry[0] & 0xfc) >> 2];
		counter++;
		linecount++;
	      }
	    
	    if ((linecount + 3) <= (76 - 2))
	      {
		buffer[slotcount++] = encoding[(((cbuf.arry[0] & 3) << 4) | ((cbuf.arry[1] & 0xf0) >> 4))];
		counter++;
		linecount++;
	      }

	    else
	      {
		buffer[slotcount++] = '\r';
		buffer[slotcount++] = '\n';
		linecount = 0;
		buffer[slotcount++] = encoding[(((cbuf.arry[0] & 3) << 4) | ((cbuf.arry[1] & 0xf0) >> 4))];
		counter++;
		linecount++;
	      }

	    if ((linecount + 3) <= (76 - 2))
	      {
		if (copysize < 2)
		  {
		    counter64 += copysize;
		    break;
		  }

		buffer[slotcount++] = encoding[(((cbuf.arry[1] & 0x0f) << 2) | ((cbuf.arry[2] & 0xc0) >> 6))];
		counter++;
		linecount++;
	      }

	    else
	      {
		if (copysize < 2)
		  {
		    counter64 += copysize;
		    break;
		  }

		buffer[slotcount++] = '\r';
		buffer[slotcount++] = '\n';
		linecount = 0;
		buffer[slotcount++] = encoding[(((cbuf.arry[1] & 0x0f) << 2) | ((cbuf.arry[2] & 0xc0) >> 6))];
		counter++;
		linecount++;
	      }

	    if ((linecount + 3) <= (76 - 2))
	      {
		if (copysize < 3)
		  {
		    counter64 += copysize;
		    break;
		  }


		buffer[slotcount++] = encoding[(cbuf.arry[2] & 0x3f)];
		counter++;
		linecount++;
	      }
	    
	    else
	      {
		if ((copysize < 3) <= 0)
		  {
		    counter64 += copysize;
		    break;
		  }

		buffer[slotcount++] = '\r';
		buffer[slotcount++] = '\n';
		counter++;
		linecount = 0;
		buffer[slotcount++] = encoding[(cbuf.arry[2] & 0x3f)];
		counter++;
		linecount++;
	      }

	    counter64 += copysize;

	  }

	if (tcp_write (sockfd, buffer, slotcount) < 0)
	  return (-1);
	
	memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	slotcount = 0;
      }
	
    if (copysize < 2)
      i = 2;
    
    else
      i = 1;

    if ((slotcount + i + strlen ("----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg") + 7) > sizeof (buffer))
      {
	if (tcp_write (sockfd, buffer, slotcount) < 0)
	  return (-1);
	    
	memset ((void *) &buffer, '\0', (size_t) sizeof (buffer));
	slotcount = 0;
	linecount = 0;
      }

    for (; i > 0; i--)
      buffer[slotcount++] = '=';
    
    buffer[slotcount++] = '\r';
    buffer[slotcount++] = '\n';
    
    if (tcp_write (sockfd, buffer, slotcount) < 0)
      return (-1);

    return (0);
  }
  /*------------------------------------------------------------------------------*/
  /*
   *
   * smtp set_error function
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/
  int
  smtp_set_error (char *mail_server_ip, char *sender, char *recipient, char *answer, char *function, int res)
  {
    char date[80];
    char *ptr;
    char *ret;
    int ret_size;
    struct tm *tm_ptr;
    time_t TimeT;

  
    ptr = answer;

    while ((char *) ptr <= (char *) (answer + strlen (answer)))
      {
	if ((*ptr == '\r') || (*ptr == '\n'))
	  *ptr = ' ';
	ptr++;
      }
  
    memset (&date[0], '\0', sizeof (date));

    time(&TimeT);
    tm_ptr = localtime (&TimeT);
  
    strftime(&date[0], (size_t ) sizeof (date), (const char *) "SMTP function -- date: %a %b %d %H:%M:%S %Y", (const struct tm *) tm_ptr);

    ret_size = strlen (date) + strlen (" \n\t-- mailserver: ") + strlen (mail_server_ip) +
      strlen (" \n\t-- sender: ") + strlen (sender) + 
      strlen (" \n\t-- recipient: ") + strlen (recipient) +
      strlen (" \n\t-- SMTP server answer: ") + strlen (answer) +
      strlen (" \n\t-- SMTP function: ") + strlen (function) +
      strlen (" \n\t-- result code: ") + 6;

    if ((ret = (char *) malloc (ret_size)) == NULL)
      return (-1);
  
    memset ((void *) ret, '\0', ret_size);

    ptr = ret;
    memcpy ((void *) ptr, (void *) &date[0], strlen (&date[0]));

    ptr += strlen (&date[0]);
    memcpy ((void *) ptr, (void *) " \n\t-- mailserver: ", strlen (" \n\t-- mailserver: "));
                           
    ptr += strlen (" \n\t-- mailserver: ");
    memcpy ((void *) ptr, (void *) mail_server_ip, strlen (mail_server_ip));

    ptr += strlen (mail_server_ip);
    memcpy ((void *) ptr, (void *) " \n\t-- sender: ", strlen (" \n\t-- sender: "));

    ptr += strlen (" \n\t-- sender: ");
    memcpy ((void *) ptr, (void *) sender, strlen (sender));
  
    ptr += strlen (sender);
    memcpy ((void *) ptr, (void *) " \n\t-- recipient: ", strlen (" \n\t-- recipient: "));

    ptr += strlen (" \n\t-- recipient: ");
    memcpy ((void *) ptr, (void *) recipient, strlen (recipient));

    ptr += strlen (recipient);
    memcpy ((void *) ptr, (void *) " \n\t-- SMTP server answer: ", strlen (" \n\t-- SMTP server answer: "));

    ptr += strlen (" \n\t-- SMTP server answer: ");
    memcpy ((void *) ptr, (void *) answer, strlen (answer));

    ptr += strlen (answer);
    memcpy ((void *) ptr, (void *) " \n\t-- SMTP function: ", strlen (" \n\t-- SMTP function: "));

    ptr += strlen (" \n\t-- SMTP function: ");
    memcpy ((void *) ptr, (void *) function, strlen (function));
  
    ptr += strlen (function);
    memcpy ((void *) ptr, (void *) " \n\t-- result code: ", strlen (" \n\t-- result code: "));
  
    ptr += strlen (" \n\t-- result code: ");
  
#ifdef WIN32
    _snprintf (ptr, 3, "%i", res);
#else 
    snprintf (ptr, 3, "%i", res);
#endif
  
    ptr += strlen (ptr);
    *ptr = '\n';
    ptr++;
  
    ptr = answer;
  
  
    if (strlen (ret) < 1023)
      {
	memset ((void *) answer, '\0', 1023);
	memcpy ((void *) answer, (const void *) ret, strlen (ret));
      }

    free (ret);

    return (0);
  }

  /*------------------------------------------------------------------------------*//*
   *
   * smtp function "establishe" establishe a tcp connection
   * between client and smtp server
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/
  int
  smtp_establishe (char *servername_ip, int *sockfd)
  {
    char buffer[1024];

    char *ptr;
    char *strptr;

    int ret;

    if ((*sockfd = tcp_connect (servername_ip, "smtp")) < 0)
      return (421);

    else
      {
	memset ((void *) &buffer[0], '\0', 1024);

	ptr = (char *) &buffer[0];

	do 
	  {
	    while (tcp_read (*sockfd, ptr) == 1)
	      {
		if (*ptr == '\n')
		  break;
	      
		ptr++;
	      
		if ((void *) ptr > (void *) (&buffer[0] + 1023))
		  return (-1);
	      }

	    if (buffer[3] != ' ')
	      {
		memset ((void *) &buffer, '\0', (size_t) 1024);
		ptr = &buffer[0];
	      }
	  }while (buffer[3] != ' ');

	if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
	  return (-1);

	*strptr = '\0';

	ret = atoi ((const char *) &buffer[0]);

	return (ret);
      }
  }

  /*------------------------------------------------------------------------------*//*
   *
   * smtp function expn send the EXPN
   * command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/int
  smtp_expn (int sockfd, char *who, char *buf)
  {
    char buffer[1024];
  
    char *bufptr;
    char *ptr;
    char *strptr;

    int ret;
    int expanding;

    memset ((void *) &buffer[0], '\0', 1024);
    memcpy (&buffer[0], "EXPN ", strlen ("EXPN "));
    ptr = (char *) (&buffer[0] + strlen ("EXPN "));
    memcpy (ptr, who, strlen (who));
    *(ptr + strlen (who)) = '\r';
    *(ptr + strlen (who) + 1) = '\n';

    if (tcp_write (sockfd, &buffer[0], strlen ((const char *) &buffer[0])) < 0)
      return (-1);

    memset ((void *) &buffer[0], '\0', 1024);
    ptr = (char *) &buffer[0];
  
    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer[0] + 1023))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) 1024);
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    expanding = 0;
    bufptr = buf;

    do
      {
	if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
	  return (-1);

	if (((char *) &buffer[0] - (char *) strptr) > 2)
	  {
	    if ((strptr = strchr ((const char *) &buffer[0], '-')) == NULL)
	      return (-1);

	    expanding = 1;
	  }

	*strptr = '\0';

	ret = atoi ((const char *) &buffer[0]);

	ptr = strptr + 1;

	if ((strptr = strrchr (ptr, '\n')) == NULL)
	  return (-1);

	*strptr = '\0';

	memcpy ((void *) bufptr, (const void *) ptr, (size_t ) strlen (ptr));
	bufptr += strlen (ptr);

      }while (expanding == 1);

    return (ret);
  }
  /*------------------------------------------------------------------------------*/
  /*
   *
   * header generation
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/
  int 
  smtp_header (char *sender, char *recipient, char *subject, int type, int sockfd , char *buf)
  {
    char buffer[1024];
  
    char *ptr;
    char *strptr;

    int ret;
    int Length;


    memset ((void *) &buffer[0], '\0', 1024);
  
    memcpy (&buffer[0], "DATA\n", strlen ("DATA\n"));
  
    ptr = (char *) (&buffer[0] + strlen ("DATA\n"));

    if (tcp_write (sockfd, &buffer[0], strlen ((const char *) &buffer[0])) < 0)
      return (-1);

    memset ((void *) &buffer[0], '\0', 1024);
    ptr = (char *) &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer[0] + 1023))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) 1024);
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer[0]);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    if (type == SMTP_8BIT)
      {
	Length = strlen ("From: ") + strlen (sender) + 2 + strlen ("To: ") + strlen (recipient) + 2 + 
	  strlen ("Subject: ") + strlen (subject) + 2 + strlen ("MIME-Version: 1.0\r\nContent-Type: text/plain; charset=ISO-8859-1") +
	  2 + strlen ("Content-Transfer-Encoding: 8bit") + 5;

	if ((ptr = (char *) malloc (Length)) == NULL)
	  return (-1);
  
	memset((void*) ptr,'\0',Length);

	strptr = ptr;

	memcpy ((void *) ptr, (const void *) "From: ", (size_t ) strlen ("From: "));
	ptr += strlen ("From: ");
	memcpy ((void *) ptr, (const void *) sender, (size_t ) strlen (sender));
	ptr += strlen (sender);
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "To: ", (size_t ) strlen ("To: "));
	ptr += strlen ("To: ");
	memcpy ((void *) ptr, (const void *) recipient, (size_t ) strlen (recipient));
	ptr += strlen (recipient);
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "Subject: ", (size_t ) strlen ("Subject: "));
	ptr += strlen ("Subject: ");
	memcpy ((void *) ptr, (const void *) subject, (size_t ) strlen (subject));
	ptr += strlen (subject);
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "MIME-Version: 1.0\r\nContent-Type: text/plain; charset=ISO-8859-1", \
		strlen ("MIME-Version: 1.0\r\nContent-Type: text/plain; charset=ISO-8859-1"));
	ptr += strlen ("MIME-Version: 1.0\r\nContent-Type: text/plain; charset=ISO-8859-1");
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "Content-Transfer-Encoding: 8bit", \
		strlen ("Content-Transfer-Encoding: 8bit"));
	ptr += strlen ("Content-Transfer-Encoding: 8bit");
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;	
      }

    if ((type == SMTP_STANDART) || (type == SMTP_TEST))
      {
	Length = strlen ("From: ") + strlen (sender) + 2 + strlen ("To: ") + strlen (recipient) + 2 + 
	  strlen ("Subject: ") + strlen (subject) + 2 + strlen ("MIME-Version: 1.0\r\nContent-Type: text/plain; charset=ISO-8859-1") +
	  2 + strlen ("Content-Transfer-Encoding: base64") + 5;

	if ((ptr = (char *) malloc (Length)) == NULL)
	  return (-1);
  
	memset((void*) ptr,'\0',Length);

	strptr = ptr;

	memcpy ((void *) ptr, (const void *) "From: ", (size_t ) strlen ("From: "));
	ptr += strlen ("From: ");
	memcpy ((void *) ptr, (const void *) sender, (size_t ) strlen (sender));
	ptr += strlen (sender);
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "To: ", (size_t ) strlen ("To: "));
	ptr += strlen ("To: ");
	memcpy ((void *) ptr, (const void *) recipient, (size_t ) strlen (recipient));
	ptr += strlen (recipient);
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "Subject: ", (size_t ) strlen ("Subject: "));
	ptr += strlen ("Subject: ");
	memcpy ((void *) ptr, (const void *) subject, (size_t ) strlen (subject));
	ptr += strlen (subject);
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "MIME-Version: 1.0\r\nContent-Type: text/plain; charset=ISO-8859-1", \
		strlen ("MIME-Version: 1.0\r\nContent-Type: text/plain; charset=ISO-8859-1"));
	ptr += strlen ("MIME-Version: 1.0\r\nContent-Type: text/plain; charset=ISO-8859-1");
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "Content-Transfer-Encoding: base64", \
		strlen ("Content-Transfer-Encoding: base64"));
	ptr += strlen ("Content-Transfer-Encoding: base64");
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
      }

    if (type == SMTP_MULTI_PART)
      {
	Length = strlen ("From: ") + strlen (sender) + 2 + strlen ("To: ") + strlen (recipient) + 2 + 
	  strlen ("Subject: ") + strlen (subject) + 2 + 
	  strlen ("Content-Type: multipart/mixed;\r\n\t boundary=\"----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg\"\r\n") + 5;

	if ((ptr = (char *) malloc (Length)) == NULL)
	  return (-1);
  
	memset((void*) ptr,'\0',Length);

	strptr = ptr;

	memcpy ((void *) ptr, (const void *) "From: ", (size_t ) strlen ("From: "));
	ptr += strlen ("From: ");
	memcpy ((void *) ptr, (const void *) sender, (size_t ) strlen (sender));
	ptr += strlen (sender);
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "To: ", (size_t ) strlen ("To: "));
	ptr += strlen ("To: ");
	memcpy ((void *) ptr, (const void *) recipient, (size_t ) strlen (recipient));
	ptr += strlen (recipient);
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "Subject: ", (size_t ) strlen ("Subject: "));
	ptr += strlen ("Subject: ");
	memcpy ((void *) ptr, (const void *) subject, (size_t ) strlen (subject));
	ptr += strlen (subject);
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "Content-Type: multipart/mixed;\r\n\t boundary=\"----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg\"\r\n\r\n", \
		strlen ("Content-Type: multipart/mixed;\r\n\t boundary=\"----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg\"\r\n\r\n"));
	ptr += strlen ("Content-Type: multipart/mixed;\r\n\t boundary=\"----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg\"\r\n\r\n");
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	
      }

    if (tcp_write (sockfd, strptr, strlen ((char *) strptr)) < 0)
      return (-1);

    return (0);
  }
  /*------------------------------------------------------------------------------*/
  /*
   *
   * sub_header generation
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/
  int 
  smtp_subheader (char *content, char *name, char *filename, int sockfd , char *buf)
  {
    char *ptr;
    char *strptr;

    int Length;
    
    if (filename == NULL)
      {
	Length = strlen ("----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg\r\n") + 2 + 
	  strlen ("MIME-Version: 1.0\r\nContent-Type: ") + strlen (content) + strlen (";") + 2 +
	  strlen ("Content-Transfer-Encoding: base64\r\n") + 5;
      }

    else
      {
	Length = strlen ("----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg\r\n") + 2 + 
	  strlen ("MIME-Version: 1.0\r\nContent-Type: ") + strlen (content) + strlen (";") + 2 + strlen ("\t name=\"") + 
	  strlen (name) + strlen ("\"") + 2 + 
	  strlen ("Content-Transfer-Encoding: base64\r\nContent-Disposition: attachment") + 2 +
	  strlen ("\t filename=\"") + strlen (filename) + strlen ("\"") + 5;
      }

    if ((ptr = (char *) calloc (1, Length)) == NULL)
      return (-1);
  
    strptr = ptr;

    memcpy ((void *) ptr, (const void *) "----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg\r\n", (size_t ) strlen ("----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg\r\n"));
    ptr += strlen ("----------Next_Part_Of_Message---------@*@~@efh564gh54hdfg\r\n");
    *(ptr++) = 0x0d;
    *(ptr++) = 0x0a;
    memcpy ((void *) ptr, (const void *) "MIME-Version: 1.0\r\nContent-Type: ", (size_t ) strlen ("Content-Type: "));
    ptr += strlen ("MIME-Version: 1.0\r\nContent-Type: ");
    memcpy ((void *) ptr, (const void *) content, (size_t ) strlen (content));
    ptr += strlen (content);
    memcpy ((void *) ptr, (const void *) ";", (size_t ) strlen (";"));
    ptr += strlen (";");
    *(ptr++) = 0x0d;
    *(ptr++) = 0x0a;

    if (filename != NULL)
      {
	memcpy ((void *) ptr, (const void *) "\t name=\"", (size_t ) strlen ("\t name=\""));
	ptr += strlen ("\t name=\"");
	memcpy ((void *) ptr, (const void *) name, (size_t ) strlen (name));
	ptr += strlen (name);
	memcpy ((void *) ptr, (const void *) "\"", (size_t ) strlen ("\""));
	ptr += strlen ("\"");
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
      }

    if (filename != NULL)
      {
	memcpy ((void *) ptr, (const void *) "Content-Transfer-Encoding: base64\r\nContent-Disposition: attachment", (size_t ) strlen ("Content-Transfer-Encoding: base64\r\nContent-Disposition: attachment"));
	ptr += strlen ("Content-Transfer-Encoding: base64\r\nContent-Disposition: attachment");
	*(ptr++) = 0x0d;
	*(ptr++) = 0x0a;
	memcpy ((void *) ptr, (const void *) "\t filename=\"", strlen ("\t filename=\""));
	ptr += strlen ("\t filename=\"");
	memcpy ((void *) ptr, (const void *) filename, (size_t ) strlen (filename));
	ptr += strlen (filename);
	memcpy ((void *) ptr, (const void *) "\"", (size_t ) strlen ("\""));
	ptr += strlen ("\"");
      }

    else
      {
	memcpy ((void *) ptr, (const void *) "Content-Transfer-Encoding: base64\r\n", (size_t ) strlen ("Content-Transfer-Encoding: base64\r\n"));
	ptr += strlen ("Content-Transfer-Encoding: base64\r\n");
      }

    *(ptr++) = 0x0d;
    *(ptr++) = 0x0a;
    *(ptr++) = 0x0d;
    *(ptr++) = 0x0a;
  
    if (tcp_write (sockfd, strptr, strlen ((char *) strptr)) < 0)
      return (-1);

    return (0);
  }
  /*------------------------------------------------------------------------------*//*
   *
   * smtp function "helo" send the HELO
   * command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/
  int
  smtp_helo (int sockfd, char *whoami, char *buf)
  {
    char buffer[1024];
  
    char *ptr;
    char *strptr;

    int ret;
    int length;
  
    memset ((void *) &buffer[0], '\0', 1024);
  
    memcpy (&buffer[0], "EHLO ", strlen ("EHLO "));
  
    ptr = (char *) (&buffer[0] + strlen ("EHLO "));
  
    if (strchr (whoami, '@') != NULL)
      length = (strchr (whoami, '@') - whoami);
    else 
      length = strlen(whoami);
  
    memcpy (ptr, whoami, length);
    *(ptr + length) = '\r';
    *(ptr + length + 1) = '\n';
  
    if (tcp_write (sockfd, &buffer[0], strlen ((const char *) &buffer[0])) < 0)
      return (-1);

    memset ((void *) &buffer[0], '\0', 1024);
    ptr = (char *) &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer[0] + 1023))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) 1024);
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer[0]);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    return (ret);
  }

  /*------------------------------------------------------------------------------*/
  /*
   *
   * function loggs the mail session
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  void
  smtp_log (smtp_stack_t *log, smtp_stack_t *recipient, smtp_stack_t *server, FILE *fstream)
  {
    smtp_stack_t *ptr;

    if (log != NULL)
      {
	do
	  {
	    ptr = log;
	  
	    if ((fstream != NULL) && (strlen (log->stack) != 0))
	      {
		fwrite((const void *) log->stack, (size_t ) strlen (log->stack), 1, fstream);
	      }

	    if (log->stack != NULL)
	      free (log->stack);
	  
	    log = log->last;
	    free (ptr);
	  }while (log->last != NULL);

	ptr = log;
	  
	if ((fstream != NULL) && (strlen (log->stack) != 0))
	  {
	    fwrite((const void *) log->stack, (size_t ) strlen (log->stack), 1, fstream);
	  }

	if (log->stack != NULL)
	  free (log->stack);
	  
	log = log->last;
	free (ptr);  
      }

    if (recipient != NULL)
      {
	do
	  {
	    ptr = recipient;
      
	    if (recipient->stack != NULL)
	      free (recipient->stack);

	    recipient = recipient->last;
	    free (ptr);
	  }while (recipient->last != NULL);
      }

    if (server != NULL)
      {
	do
	  {
	    ptr = server;
      
	    if (server->stack != NULL)
	      free (server->stack);

	    server = server->last;
	    free (ptr);
	  }while (server->last != NULL);
      }
  }

  /*------------------------------------------------------------------------------*/
  /*
   *
   * smtp function "mail" send the MAIL
   * command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  int
  smtp_mail (int sockfd, char *reverse_path, char *buf)
  {
    char buffer[1024];
  
    char *ptr;
    char *strptr;

    int ret;

    memset ((void *) &buffer[0], '\0', 1024);
    memcpy (&buffer[0], "MAIL FROM:", strlen ("MAIL FROM:"));
    ptr = (char *) (&buffer[0] + strlen ("MAIL FROM:"));
    *ptr = '<';
    ptr++;
    memcpy (ptr, reverse_path, strlen (reverse_path));
    *(ptr + strlen (reverse_path)) = '>';
    *(ptr + strlen (reverse_path) + 1) = '\r';
    *(ptr + strlen (reverse_path) + 2) = '\n';

    if (tcp_write (sockfd, &buffer[0], strlen ((const char *) &buffer[0])) < 0)
      return (-1);

    memset ((void *) &buffer[0], '\0', 1024);
    ptr = (char *) &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer[0] + 1023))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) 1024);
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer[0]);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    return (ret);
  }
  /*------------------------------------------------------------------------------*/
  /*
   *
   * smtp function "quit" send the QUIT
   * command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  int
  smtp_quit (int sockfd, char *buf)
  {
    char buffer[1024];
  
    char *ptr;
    char *strptr;

    int ret;

    memset ((void *) &buffer[0], '\0', 1024);
    memcpy (&buffer[0], "QUIT", strlen ("QUIT"));
    ptr = (char *) (&buffer[0] + strlen ("QUIT"));
    *ptr = '\r';
    *(ptr + 1) = '\n';

    if (tcp_write (sockfd, &buffer[0], strlen ((const char *) &buffer[0])) < 0)
      return (-1);

    memset ((void *) &buffer[0], '\0', 1024);
    ptr = (char *) &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer[0] + 1023))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) 1024);
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');
  
    if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer[0]);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    return (ret);
  }

  /*------------------------------------------------------------------------------*/
  /*
   *
   * smtp function "rcpt" send the RCPT
   * command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  int
  smtp_rcpt (int sockfd, char *recipient, char *buf)
  {
    char buffer[1024];
  
    char *ptr;
    char *strptr;

    int ret;

    memset ((void *) &buffer[0], '\0', 1024);
    memcpy (&buffer[0], "RCPT TO:", strlen ("RCPT TO:"));
    ptr = (char *) (&buffer[0] + strlen ("RCPT TO:"));
    *ptr = '<';
    ptr++;
    memcpy (ptr, recipient, strlen (recipient));
    *(ptr + strlen (recipient)) = '>';
    *(ptr + strlen (recipient) + 1) = '\r';
    *(ptr + strlen (recipient) + 2) = '\n';

    if (tcp_write (sockfd, &buffer[0], strlen ((const char *) &buffer[0])) < 0)
      return (-1);

    memset ((void *) &buffer[0], '\0', 1024);
    ptr = (char *) &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer[0] + 1023))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) 1024);
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
      return (-1);
  
    *strptr = '\0';

    ret = atoi ((const char *) &buffer[0]);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    return (ret);
  }

  /*------------------------------------------------------------------------------*/
  /*
   *
   * smtp function "rset" send the RSET
   * command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  int
  smtp_rset (int sockfd, char *buf)
  {
    char buffer[1024];
  
    char *ptr;
    char *strptr;

    int ret;

    memset ((void *) &buffer[0], '\0', 1024);
    memcpy (&buffer[0], "RSET", strlen ("RSET"));
    ptr = (char *) (&buffer[0] + strlen ("RSET"));
    *ptr = '\r';
    *(ptr + 1) = '\n';

    if (tcp_write (sockfd, &buffer[0], strlen ((const char *) &buffer[0])) < 0)
      return (-1);

    memset ((void *) &buffer[0], '\0', 1024);
    ptr = (char *) &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer[0] + 1023))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) 1024);
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer[0]);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    return (ret);
  }

  /*------------------------------------------------------------------------------*/
  /*
   *
   * smtp function "saml" saml the SAML
   * command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  int
  smtp_saml (int sockfd, char *reverse_path, char *buf)
  {
    char buffer[1024];
  
    char *ptr;
    char *strptr;

    int ret;

    memset ((void *) &buffer[0], '\0', 1024);
    memcpy (&buffer[0], "SAML FROM:", strlen ("MAI FROM:"));
    ptr = (char *) (&buffer[0] + strlen ("SAML FROM:"));
    *ptr = '<';
    ptr++;
    memcpy (ptr, reverse_path, strlen (reverse_path));
    *(ptr + strlen (reverse_path)) = '>';
    *(ptr + strlen (reverse_path) + 1) = '\r';
    *(ptr + strlen (reverse_path) + 2) = '\n';

    if (tcp_write (sockfd, &buffer[0], strlen ((const char *) &buffer[0])) < 0)
      return (-1);

    memset ((void *) &buffer[0], '\0', 1024);
    ptr = (char *) &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer[0] + 1023))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) 1024);
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer[0]);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    return (ret);
  }

  /*------------------------------------------------------------------------------*/
  /*
   *
   * smtp function "send" send the SEND
   * command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  int
  smtp_send (int sockfd, char *reverse_path, char *buf)
  {
    char buffer[1024];
  
    char *ptr;
    char *strptr;

    int ret;

    memset ((void *) &buffer[0], '\0', 1024);
    memcpy (&buffer[0], "SEND FROM:", strlen ("MAI FROM:"));
    ptr = (char *) (&buffer[0] + strlen ("SEND FROM:"));
    memcpy (ptr, reverse_path, strlen (reverse_path));
    *(ptr + strlen (reverse_path)) = '\r';
    *(ptr + strlen (reverse_path) + 1) = '\n';

    if (tcp_write (sockfd, &buffer[0], strlen ((const char *) &buffer[0])) < 0)
      return (-1);

    memset ((void *) &buffer[0], '\0', 1024);
    ptr = (char *) &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer[0] + 1023))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) 1024);
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer[0]);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    return (ret);
  }

  /*------------------------------------------------------------------------------*/
  /*
   *
   * smtp stat machine for sending mail
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  int WSAReturn(int ret, smtp_stack_t *log, smtp_stack_t* ptr, smtp_stack_t *recip,  
		smtp_stack_t *server, char *mail_server_ip, char *recipient, 
		char *sender, FILE *fstream)
  {

#ifdef WIN32
    WSACleanup( );  
#endif

    if (ret == 0)
      {
	if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	  return (-1);
      
	if ((ptr->stack = (char *) malloc (strlen ("no errors\n") + 1)) == NULL)
	  return (-1);
      
	strcpy(ptr->stack,"no errors\n");
	ptr->last = NULL;

	ptr->last = log;
	log = ptr;
      }

    else
      {
	if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	  return (-1);
      
	if ((ptr->stack = (char *) malloc (strlen ("error in sending mail\n") + 1)) == NULL)
	  return (-1);
      
	strcpy(ptr->stack,"error in sending mail\n");
	ptr->last = NULL;
      
	ptr->last = log;
	log = ptr;
      }

    smtp_log (log, recip, server, fstream);
    return ret;
  }
  /*------------------------------------------------------------------------------*/
  /*
   *
   * smtp sm_mail function
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  int
  smtp_sm_mail (char *mail_server_ip, char *sender, char *recipient, char *subject, char **TmpPtr, headstr *header, int type, FILE *fstream)
  {
    int e_count;
    int i;
    int res;
    int statret;
    int sockfd;
    int fd;

    unsigned int readSize;

    char* data;

    smtp_stack_t *ptr;
    smtp_stack_t *log;
    smtp_stack_t *recip;
    smtp_stack_t *server;
    
    headstr *hptr;

    struct stat status;

    /* --- * /
       int rfd;
       / * --- */

#ifdef WIN32
    WORD wVersionRequested;      
    WSADATA wsaData;
#endif

#ifdef WIN32
    wVersionRequested = MAKEWORD( 2, 0 );
    WSAStartup( wVersionRequested, &wsaData ) ;
#endif

    log = NULL;
    recip = NULL;
    server = NULL;
    hptr = header;
    data = TmpPtr[0];

    /* --- * /
       rfd = open ("/tmp/email______", O_RDWR|O_CREAT|O_APPEND);
       write (rfd, "DER ANFANG\n", strlen ("DER ANFANG\n"));
       / * --- */

    /* --- * /
    printf ("<FONT SIZE=\"3\" FACE=\"arial,helv,helvetica\"><B>BIS HIER HIN UND NICHT WEITER</B></FONT>");
    / * --- */

    if (type == SMTP_MULTI_PART)
      {
	i = 0;

	while (hptr != NULL)
	  {
	    if (hptr->type == SMTP_TXT)
	      {
		if ((fd = open (hptr->filePath,O_RDONLY) <= 0))
		  {
		    res = 0;
		    return (-1);
		  }

		stat (hptr->filePath,&status);
		
		if ((TmpPtr[i] = calloc (1, status.st_size)) == NULL)
		  {
		    res = 0;
		    return (-1);
		  }

		readSize = 0;
		
		while ((readSize += read(fd, (TmpPtr[i] + readSize), (status.st_size - readSize))) < status.st_size);
	      }
	  }
      }

    if (mail_server_ip != NULL)
      res = smtp_establishe (mail_server_ip, &sockfd);

    else
      return (-1);
  
    if (res == 220)
      {
	if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	  return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);

	if ((ptr->stack = (char *) malloc (1024)) == NULL)
	  {
	    tcp_disconnect (sockfd);
	    free (ptr);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }

	ptr->last = 0;

	memset ((void *) ptr->stack, '\0', strlen ("successfull connected to mail server") + 1);
	strcpy (ptr->stack, "successfull connected to mail server");

	smtp_set_error (mail_server_ip, sender, recipient, ptr->stack, "ESTABLISH", res);

	ptr->last = log;
	log = ptr;
      }

    else
      {
	if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	  {
	    tcp_disconnect (sockfd);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }  
      
	if ((ptr->stack = (char *) malloc (1024)) == NULL)
	  {
	    tcp_disconnect (sockfd);
	    free (ptr);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }

	ptr->last = NULL;

	memset ((void *) ptr->stack, '\0', strlen ("could not connected to mail server") + 1);
	strcpy(ptr->stack,"could not connected to mail server");

	smtp_set_error (mail_server_ip, sender, recipient, ptr->stack, "ESTABLISH", res);

	ptr->last = log;
	log = ptr;
      }

    if (res != 220)
      {
        tcp_disconnect (sockfd);
        return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
      }	

    e_count = 0;


    /* 
     * HELO
     * --------------------
     */


    do
      {
	if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	  {
	    tcp_disconnect (sockfd);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }  

	if ((ptr->stack = (char *) malloc (1024)) == NULL)
	  {
	    tcp_disconnect (sockfd);
	    free (ptr);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }

	ptr->last = NULL;

	memset ((void *) ptr->stack, '\0', 1024);

	res = smtp_helo (sockfd, sender, ptr->stack);

	smtp_set_error (mail_server_ip, sender, recipient, ptr->stack, "HELO", res);

	if (res != 250)
	  e_count++;

	ptr->last = log;
	log = ptr;
      
      } while ((e_count < 3) && (res != 250));

    if (res != 250)
      {

	do
	  {
	    if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	      {
	        tcp_disconnect (sockfd);
	        return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
              }		

	    if ((ptr->stack = (char *) malloc (1024)) == NULL)
	      {
	        tcp_disconnect (sockfd);
		free (ptr);
		return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	      }

	    ptr->last = NULL;

	    memset ((void *) ptr->stack, '\0', 1024);

	    res = smtp_quit (sockfd, ptr->stack);

	    if (res != 250)
	      e_count++;

	    ptr->last = log;
	    log = ptr;
      
	  } while ((e_count < 3) && (res != 250));
        tcp_disconnect (sockfd);
	return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
      }

    e_count = 0;

    /* 
     * MAIL
     * --------------------
     */
    do
      {
	if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	  {
	    tcp_disconnect (sockfd);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }  

	if ((ptr->stack = (char *) malloc (1024)) == NULL)
	  {
	    tcp_disconnect (sockfd);
	    free (ptr);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }

	ptr->last = NULL;

	memset ((void *) ptr->stack, '\0', 1024);

	res = smtp_mail (sockfd, recipient, ptr->stack);

	smtp_set_error (mail_server_ip, sender, recipient, ptr->stack, "MAIL", res);

	if (res != 250)
	  e_count++;

	ptr->last = log;
	log = ptr;
      
      } while ((e_count < 3) && (res != 250));
  
    if (res != 250)
      {

	do
	  {
	    if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	      {
	        tcp_disconnect (sockfd);
	        return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
              }		

	    if ((ptr->stack = (char *) malloc (1024)) == NULL)
	      {
	        tcp_disconnect (sockfd);
		free (ptr);
		return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	      }

	    ptr->last = NULL;

	    memset ((void *) ptr->stack, '\0', 1024);

	    res = smtp_quit (sockfd, ptr->stack);

	    if (res != 250)
	      e_count++;

	    ptr->last = log;
	    log = ptr;
      
	  } while ((e_count < 3) && (res != 250));

        tcp_disconnect (sockfd);
	return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
      }

    e_count = 0;

    /* 
     * RCPT
     * --------------------
     */

    do
      {
	if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	  {
	    tcp_disconnect (sockfd);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }  

	if ((ptr->stack = (char *) malloc (1024)) == NULL)
	  {
	    tcp_disconnect (sockfd);
	    free (ptr);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }

	ptr->last = NULL;

	memset ((void *) ptr->stack, '\0', 1024);

	res = smtp_rcpt (sockfd, recipient, ptr->stack);

	statret = res;

	smtp_set_error (mail_server_ip, sender, recipient, ptr->stack, "RCPT", res);

	if (res != 250)
	  e_count++;

	ptr->last = log;
	log = ptr;
      
      } while ((e_count < 3) && (res != 250));
  
    if (res != 250)
      {

	do
	  {
	    if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	      {
	        tcp_disconnect (sockfd);
	        return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
              } 		

	    if ((ptr->stack = (char *) malloc (1024)) == NULL)
	      {
	        tcp_disconnect (sockfd);
		free (ptr);
		return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	      }

	    ptr->last = NULL;

	    memset ((void *) ptr->stack, '\0', 1024);

	    res = smtp_quit (sockfd, ptr->stack);

	    if (res != 250)
	      e_count++;

	    ptr->last = log;
	    log = ptr;
      
	  } while ((e_count < 3) && (res != 250));
        tcp_disconnect (sockfd);
	return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
      }

    e_count = 0;

    /* 
     * DATA
     * --------------------
     */

    if (type != SMTP_TEST)
      {
	do
	  {
	    if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	      {
	        tcp_disconnect (sockfd);
	        return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	      }	

	    if ((ptr->stack = (char *) malloc (1024)) == NULL)
	      {
	        tcp_disconnect (sockfd);
		free (ptr);
		return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	      }

	    ptr->last = NULL;

	    memset ((void *) ptr->stack, '\0', 1024);

	    if (smtp_header(sender, recipient, subject, type, sockfd, ptr->stack) != 0)
	      {
	        tcp_disconnect (sockfd);
	        return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	      }	

	    if (type == SMTP_8BIT)
	      {
		/* --- * /
		   write (rfd, data, strlen (data));
		   / * --- */

		res = smtp_8_data (sockfd, data, ptr->stack);

		/* --- * /
		   write (rfd, data, strlen (data));
		   / * --- */
		
		smtp_set_error (mail_server_ip, sender, recipient, ptr->stack, "DATA", res);
	      }
	      
	    else if (type == SMTP_STANDART)
	      {
		res = smtp_data (sockfd, data, ptr->stack);

		smtp_set_error (mail_server_ip, sender, recipient, ptr->stack, "DATA", res);
	      }

	    else if ((type != SMTP_8BIT) && (type != SMTP_STANDART))
	      {
		i = 0;

		while (TmpPtr[i] != NULL)
		  {

		    if (smtp_subheader(header->content, header->name, header->name, sockfd, ptr->stack) != 0)
		      return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);

		    data = TmpPtr[i];
		    i++;

		    if (TmpPtr[i] != NULL)
		      {
			smtp_b_data (sockfd, data, header->size, ptr->stack);
		      }

		    else
		      {
			res = smtp_bin_data (sockfd, data, header->size, ptr->stack);
			smtp_set_error (mail_server_ip, sender, recipient, ptr->stack, "DATA", res);
		      }

		    header = header->next;

		  }
	      }

	    if (res != 250)
	      e_count++;

	    ptr->last = log;
	    log = ptr;
      
	  } while ((e_count < 3) && (res != 250));

	if (res != 250)
	  {
	    do
	      {
		if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
		  {
		    tcp_disconnect (sockfd);
		    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
		  }  

		if ((ptr->stack = (char *) malloc (1024)) == NULL)
		  {
		    tcp_disconnect (sockfd);
		    free (ptr);
		    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
		  }

		ptr->last = NULL;

		memset ((void *) ptr->stack, '\0', 1024);

		res = smtp_quit (sockfd, ptr->stack);

		if (res != 250)
		  e_count++;

		ptr->last = log;
		log = ptr;
      
	      } 
	    while ((e_count < 3) && (res != 250));
	    
            tcp_disconnect (sockfd);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }

	e_count = 0;
      }
    /* 
     * QUIT
     * --------------------
     */

    /* --- * /
       write (rfd, "\n im QUIT\n", strlen ("\n im QUIT\n") + 1);
       close (rfd);
       / * --- */

    do
      {
	if ((ptr = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
	  {
	    tcp_disconnect (sockfd);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }  


	if ((ptr->stack = (char *) malloc (1024)) == NULL)
	  {
	    tcp_disconnect (sockfd);
	    free (ptr);
	    return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
	  }

	ptr->last = NULL;

	memset ((void *) ptr->stack, '\0', 1024);

	res = smtp_quit (sockfd, ptr->stack);

	smtp_set_error (mail_server_ip, sender, recipient, ptr->stack, "QUIT", res);

	if (res != 221)
	  e_count++;

	ptr->last = log;
	log = ptr;
      
      } while ((e_count < 3) && (res != 221));

  
    if ((res != 221) || ((type == SMTP_TEST) && (statret != 250)))
      {
        tcp_disconnect (sockfd);
        return WSAReturn(-1, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
      }	
    tcp_disconnect (sockfd);
    return WSAReturn(0, log, ptr, recip, server, mail_server_ip, recipient, sender, fstream);
  }

  /*------------------------------------------------------------------------------*/
  /*
   *
   * smtp function "soml" soml the SOML
   * command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  int
  smtp_soml (int sockfd, char *reverse_path, char *buf)
  {
    char buffer[1024];
  
    char *ptr;
    char *strptr;

    int ret;

    memset ((void *) &buffer[0], '\0', 1024);
    memcpy (&buffer[0], "SOML FROM:", strlen ("SOML FROM:"));
    ptr = (char *) (&buffer[0] + strlen ("SOML FROM:"));
    *ptr = '<';
    ptr++;
    memcpy (ptr, reverse_path, strlen (reverse_path));
    *(ptr + strlen (reverse_path)) = '>';
    *(ptr + strlen (reverse_path) + 1) = '\r';
    *(ptr + strlen (reverse_path) + 2) = '\n';

    if (tcp_write (sockfd, &buffer[0], strlen ((const char *) &buffer[0])) < 0)
      return (-1);

    memset ((void *) &buffer[0], '\0', 1024);
    ptr = (char *) &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer[0] + 1023))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) 1024);
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer[0]);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    return (ret);
  }

  /*------------------------------------------------------------------------------*/
  /*
   *
   * stack push command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  char *
  smtp_spop (smtp_stack_t *top)
  {
    char *ret;
    smtp_stack_t *new;

    ret = top->stack;
    new = top->last;
    free (top);
 
    return (ret);
  }

  /*------------------------------------------------------------------------------*/
  /*
   *
   * stack push command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  int
  smtp_spush (smtp_stack_t *top, char *stack)
  {
    smtp_stack_t *new;

    if ((new = (smtp_stack_t *) malloc (sizeof (smtp_stack_t))) == NULL)
      return (-1);

    new->last = top;
    new->stack = stack;
    top = new;

    return (0);
  }


  /*------------------------------------------------------------------------------*/
  /*
   *
   * smtp function "vrfy" send the VRFY
   * command
   *
   * Version 0.0
   *
   */
  /*------------------------------------------------------------------------------*/

  int
  smtp_vrfy (int sockfd, char *who, char *buf)
  {
    char buffer[1024];
  
    char *ptr;
    char *strptr;

    int ret;

    memset ((void *) &buffer[0], '\0', 1024);
    memcpy (&buffer[0], "VRFY ", strlen ("VRFY "));
    ptr = (char *) (&buffer[0] + strlen ("VRFY "));
    memcpy (ptr, who, strlen (who));
    *(ptr + strlen (who)) = '\r';
    *(ptr + strlen (who) + 1) = '\n';

    if (tcp_write (sockfd, &buffer[0], strlen ((const char *) &buffer[0])) < 0)
      return (-1);

    memset ((void *) &buffer[0], '\0', 1024);
    ptr = (char *) &buffer[0];

    do
      {
	while (tcp_read (sockfd, ptr) == 1)
	  {
	    if (*ptr == '\n')
	      break;
	  
	    ptr++;
	  
	    if ((void *) ptr > (void *) (&buffer[0] + 1023))
	      return (-1);
	  }

	if (buffer[3] != ' ')
	  {
	    memset ((void *) &buffer, '\0', (size_t) 1024);
	    ptr = &buffer[0];
	  }
      }while (buffer[3] != ' ');

    if ((strptr = strchr ((const char *) &buffer[0], ' ')) == NULL)
      return (-1);

    *strptr = '\0';

    ret = atoi ((const char *) &buffer[0]);

    ptr = strptr + 1;

    if ((strptr = strrchr (ptr, '\n')) == NULL)
      return (-1);

    *strptr = '\0';

    memcpy ((void *) buf, (const void *) ptr, (size_t ) strlen (ptr));

    return (ret);
  }
  /*------------------------------------------------------------------------------*/
#ifdef __cplusplus 
}
#endif
