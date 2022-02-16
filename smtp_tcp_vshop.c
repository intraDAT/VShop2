/*
 *
 * tcp_connect c-function derived from "tcp_connect function of R.Stevens"
 *
 * Version 0.0
 *
 */

#include "smtp_network.h"

#ifdef __cplusplus 
extern "C" {
#endif

int
tcp_connect (const char *hostname_ip, const char *service)
{
  char **h_addr_list;

  int sockfd;

  /* === DEBUG === */
  struct in_addr test;
  /* ============= */

  struct sockaddr_in server;
  
  struct servent *serv_str;
  
  struct hostent *host_str;

  memset (&server, '\0', sizeof (struct sockaddr_in));


  if ((host_str =  gethostbyname (hostname_ip)) == NULL)
    {
      if ((host_str = gethostbyaddr (hostname_ip, 4, AF_INET)) == NULL)
	      return (-1);
    }
  
  if ((serv_str = getservbyname (service, "tcp")) == NULL)
    return (-1);
  
  h_addr_list = host_str->h_addr_list;
  
  /* === DEBUG === */
  memcpy ((void *) &test.s_addr,(void *) *host_str->h_addr_list, host_str->h_length);
  inet_ntoa (test);
  /* ============= */

  do
    {
      if ((sockfd = socket (host_str->h_addrtype, SOCK_STREAM, 0)) < 0)
	continue;

      server.sin_family = host_str->h_addrtype;
      server.sin_port = serv_str->s_port;
      memcpy ( (void *) &server.sin_addr.s_addr, (void *) *h_addr_list,host_str->h_length);

      /* === DEBUG === */
      inet_ntoa (server.sin_addr);
      /* ============= */

      if (connect (sockfd, (struct sockaddr *) &server, sizeof (server)) == 0)
	break;

      close (sockfd);
    }while (h_addr_list++ != NULL);

  if (h_addr_list == NULL)
    return (-1);

  /*  free (serv_str); */

  return (sockfd);
}

#ifdef __cplusplus 
}
#endif
/*
 *
 * tcp_disconnect
 *
 * Version 0.0
 *
 */

#ifdef __cplusplus 
extern "C" {
#endif

int
tcp_disconnect (int sockfd)
{
  return (close (sockfd));
}

#ifdef __cplusplus 
}
#endif
/*
 *
 * tcp_read
 *
 * Version 0.0
 *
 */

#ifdef __cplusplus 
extern "C" {
#endif

ssize_t
tcp_read (int sockfd, char *buf)
{
  size_t nleft;

  ssize_t nread;

  nleft = 1;

  while (nleft > 0)
    {
      #ifdef WIN32
        if ((nread = recv(sockfd,buf, nleft, 0)) == SOCKET_ERROR)
      #else
        if ((nread = read (sockfd, buf, nleft)) < 0)
      #endif
    	 {
         #ifndef WIN32
	          if (errno == EINTR)
	            nread = 0;
	          else
         #endif
	          return (-1);
	      }
     else 
      if (nread == 0)
	      break;

      nleft = 0;


    } 

  return (nread);
}

#ifdef __cplusplus 
}
#endif
/*
 *
 * tcp_write
 *
 * Version 0.0
 *
 */

#ifdef __cplusplus 
extern "C" {
#endif

ssize_t
tcp_write (int sockfd, const void *buf, size_t nwrite)
{
  size_t nleft;

  ssize_t nwritten;

  int errno;

  char *ptr;

  ptr = (void *) buf;

  nleft = nwrite;


  while (nleft > 0)
    {
      errno = 0;

      #ifdef WIN32
        if ((nwritten = send (sockfd, ptr, nleft,0)) == SOCKET_ERROR )
      #else
        if ((nwritten = write (sockfd, ptr, nleft)) <= 0)
      #endif
        {
          #ifndef WIN32
  	        if (errno == EINTR)
	            nwritten = 0;
	          else
          #endif
	            return (-1);
        }

      nleft -= nwritten;
      ptr += nwritten;
    }

  return (nwritten);
}

#ifdef __cplusplus 
}
#endif
