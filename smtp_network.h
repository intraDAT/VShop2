/*
 *
 * network functions  header file
 *
 * Version 0.0
 *
 */
#include <stdio.h>
/*
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
*/
#ifndef WIN32
  #include <netdb.h>
  #include <signal.h>
/*  #include <sockaddrcom.h>*/
  #ifndef SUN
  #include <tcpd.h>
  #endif
  #include <unistd.h>

  #include <arpa/inet.h>
  #ifndef SUN
  #include <asm/errno.h>
  #endif

  /* #include <linux/in.h> */

  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <errno.h>

#else
  #include <io.h>
  #include <winsock.h>

  typedef int ssize_t;

#endif

  /* -------------------- */

#include "smtp_network_proto.h"
               
