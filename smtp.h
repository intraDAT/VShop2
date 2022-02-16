/*
 *
 * smtp header file
 *
 * Version 0.0
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#include "smtp_network.h"
/* #include "maillinglist.h" */

/*
 * --------------------
 *
 * definitions
 *
 */

#ifndef SMTP_STANDART
#define SMTP_STANDART 1
#endif

#ifndef SMTP_TEST
#define SMTP_TEST 2
#endif

#ifndef SMTP_MULTI_PART
#define SMTP_MULTI_PART 3
#endif

#ifndef SMTP_8BIT
#define SMTP_8BIT 4
#endif

/* --------------------- */

#ifndef SMTP_TXT
#define SMTP_TXT 1
#endif

#ifndef SMTP_BIN
#define SMTP_BIN 2
#endif
/*
 * --------------------
 *
 * stacklist structure
 *
 */

typedef struct smtp_stack
{
  char *stack;
  struct smtp_stack *last;
} smtp_stack_t;


/*
 * --------------------
 *
 * email union
 *
 */

typedef union val
{
  unsigned int num;
  char arry[3];
} val64;

/*
 * --------------------
 *
 * header structure
 *
 */

typedef struct smtpheader
{
  int size;
  int type;
  char *filePath;
  char *content;
  char *name;
  char *filename;
  struct smtpheader *next;
} headstr;

/* --------------------- */

#include "smtp_proto.h"

