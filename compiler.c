#include <stdlib.h>

#ifndef WIN32
  #include <unistd.h>
#else
  #include <io.h>

  #ifdef _DEBUG
    #define DEBUG
  #endif

#endif

#include <string.h>
#include "compiler.h"
#include "m_code.h"
#include "vsl.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern char *out_file_name;

long seg_text_ptr = 0;
char seg_text[64000];
unsigned char seg_exec[32768];
long label_tbl[600]={-1,};
long PC=0; 
long label=0;

int DebugOutput;
long Magic;

long current = 0;
long begin = 0;
long buflength = 0;
char buf[64000];
char Path[256];

/*----------------------------------------------------------------------------------*/
long new_text_entry(char *txt)
{
  long start=seg_text_ptr;
  strcpy(seg_text + seg_text_ptr, txt);
  seg_text_ptr += strlen(txt)+1;
  return start;
}
/*----------------------------------------------------------------------------------*/        
long p_code_new_label()
{
  return label++;
}
/*----------------------------------------------------------------------------------*/
void p_code_label(long label)
{
  label_tbl[label] = PC;
#ifdef DEBUG
  printf("L%ld\n", label);
#endif
}
/*----------------------------------------------------------------------------------*/
void op_code_debug(unsigned char opc)
{
  int op = opc>>4;
  int mod = opc & 15;
  seg_exec[PC] = (op<<4)|mod;
  switch (op) {
  case CALL:
    printf( "call\n");
    break;
  case PFX:
    printf( "pfx %d\n", mod);
    break;
  case LDC:
    printf( "ldc %d\n", mod);
    break;
  case PUSH:
    if ((Id_type)mod==Int_Value)
      printf( "push.i\n");
    else if ((Id_type)mod==String_Value)
      printf( "push.s\n");
    else if ((Id_type)mod==Float_Value)
      printf( "push.f\n");
    break;
  case PRINT:
    printf( "print\n");
    break;
  case FUNC:
    switch(mod) {
    case OP_LT   : printf( "OP_LT   \n"); break;
    case OP_LE   : printf( "OP_LE   \n"); break;
    case OP_EQ   : printf( "OP_EQ   \n"); break;
    case OP_GE   : printf( "OP_GE   \n"); break;
    case OP_GT   : printf( "OP_GT   \n"); break;
    case OP_NE   : printf( "OP_NE   \n"); break;
    case OP_PLUS : printf( "OP_PLUS \n"); break;
    case OP_MINUS: printf( "OP_MINUS\n"); break;
    case OP_TIMES: printf( "OP_TIMES\n"); break;
    case OP_DIV  : printf( "OP_DIV  \n"); break;
    case OP_MOD  : printf( "OP_MOD  \n"); break;
    case OP_NEG  : printf( "OP_NEG  \n"); break;
    case OP_AND  : printf( "OP_AND  \n"); break;
    case OP_OR   : printf( "OP_OR   \n"); break;
    case OP_NOT  : printf( "OP_NOT  \n"); break;
    }
    break;
  case JMP:
    switch(mod) {
    case JMP_UNC: printf( "jmp\n"); break;
    case JMP_Z:  printf( "jz\n"); break;
    case JMP_NZ: printf( "jnz\n"); break;
    }
    break;
  case EXIT:
    printf( "exit\n");
    break;
  }
}
/*----------------------------------------------------------------------------------*/
void op_code(int op, int mod)
{
#ifdef DEBUG
  printf( "0x%lx 0x%x ", PC, (op<<4)|mod);
  op_code_debug((op<<4)|mod);
#endif
  seg_exec[PC] = (op<<4)|mod;
  PC++;
}
/*----------------------------------------------------------------------------------*/
void p_code_call()
{
  op_code(CALL,0);
#ifdef DEBUG
  printf( "-> call\n");
#endif
}
/*----------------------------------------------------------------------------------*/
#ifdef WIN32
  void p_code_const(__int64 d)
#else
  void p_code_const(long long d)
#endif
{
int mask;
int sh=28;
int bits=0;

  while(sh>0) 
    {
      mask = 15 << sh;
      if (mask & d)
        bits = 1;
      if (bits)
        op_code(PFX,(mask & d)>>sh);
      sh -= 4;
    }
  op_code(LDC,d % 16);
}
/*----------------------------------------------------------------------------------*/  
void p_code_print(long txt)
{
  p_code_const(txt);
  op_code(PRINT,0);

/*
  if (DebugOutput == 1)
    {
      printf( "<font color=\"#FFFFFF\">%s</font>\n", seg_text+txt);
    }
*/

#ifdef DEBUG
  printf( "-> print %ld (%s) \n", txt, seg_text+txt);
#endif
}
/*----------------------------------------------------------------------------------*/
void p_code_pushf(float f)
{
float d;
long *x;

  d = f;

  x = &d;

  p_code_const(*x);
  op_code(PUSH, Float_Value);
#ifdef DEBUG
  printf( "-> push.f %f\n", f);
#endif
}
/*----------------------------------------------------------------------------------*/
void p_code_pushi(long i)
{
  p_code_const(i);
  op_code(PUSH, Int_Value);
#ifdef DEBUG
  printf( "-> push.i %ld\n", i);
#endif
}
/*----------------------------------------------------------------------------------*/
void p_code_pushs(long s)
{
  p_code_const(s);
  op_code(PUSH, String_Value);
#ifdef DEBUG
  printf( "-> push.s %ld (%s) \n", s, seg_text+s);
#endif
}
/*----------------------------------------------------------------------------------*/
void p_code_jmp(long label, int mod)
{
  switch(mod) 
    {
      case JMP_UNC: 
        p_code_const(label);
        op_code(JMP, JMP_UNC);
        #ifdef DEBUG
          printf( "jmp L%ld (PC=%ld)\n", label, label_tbl[label] );
        #endif
        break;
      case JMP_Z: 
        p_code_const(label);
        op_code(JMP, JMP_Z);
      #ifdef DEBUG
        printf( "jz L%ld (PC=%ld)\n", label, label_tbl[label] );
      #endif
        break;
      case JMP_NZ: 
        p_code_const(label);
        op_code(JMP, JMP_NZ);
      #ifdef DEBUG
        printf( "jnz L%ld (PC=%ld)\n", label, label_tbl[label] );
      #endif
        break;
    }
}
/*----------------------------------------------------------------------------------*/
void p_code_finish() 
{
#ifdef DEBUG
  int i;
#endif
  int fd;
  p_exec_head p_header;
  op_code(EXIT, 0);
 
  p_header.magic = Magic;
  p_header.n_labels = label;
  p_header.n_strings = seg_text_ptr;
  p_header.n_exec = PC;
  p_header.ofs_labels = sizeof(p_header);
  p_header.ofs_strings = p_header.ofs_labels + sizeof(label_tbl[0])*label;
  p_header.ofs_exec = p_header.ofs_strings + seg_text_ptr;

  #ifndef WIN32
    #ifdef HPUX
      if ((fd = open(out_file_name, 400 | 2,775)) != -1)
        {
    #else
      if ((fd = open(out_file_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP | S_IWGRP |S_IROTH)) != -1)
        {
    #endif
          write(fd, &p_header, sizeof(p_header));
          write(fd, label_tbl, sizeof(label_tbl[0])*label);
          write(fd, seg_text, seg_text_ptr);
          write(fd, seg_exec, PC);
          close(fd);
        }
  #else
    if ((fd = _open(out_file_name, O_CREAT | O_WRONLY | O_TRUNC | _O_BINARY, S_IREAD | S_IWRITE)) != -1)
      {
        _write(fd, &p_header, sizeof(p_header));
        _write(fd, label_tbl, sizeof(label_tbl[0])*label);
        _write(fd, seg_text, seg_text_ptr);
        _write(fd, seg_exec, PC);
        _close(fd);
      }
  #endif

#ifdef DEBUG
  printf( "  %ld Labels:\n\n", label);
  i=0;
  while(i<label) 
    {
      printf( "L%d = PC %ld\n", i, label_tbl[i]);
      i++;
    }

  i=0;
  printf( "\n  %ld Strings:\n\n", seg_text_ptr);
  while(i<seg_text_ptr) 
    {
      printf( "%d = (%s)\n", i, seg_text+i);
      i+=strlen(seg_text+i)+1;
    }

  i=0;
  printf( "\n  %ld Program:\n\n", PC);
  while(i<PC) 
    {
      printf( "0x%x = 0x%x ", i, seg_exec[i]);
      op_code_debug(seg_exec[i]);
      i++;
    }

#endif

  exit(0);
  /*printf("OK\n");*/

}
