typedef int p_label;

typedef struct _if_hints {
  p_label else_label;
  p_label endif_label;
} _if_hints;

typedef struct _while_hints {
  p_label while_label;
  p_label wend_label;
} _while_hints;

typedef struct _for_hints {
  p_label for_label;
  p_label next_label;
} _for_hints;

typedef struct _do_hints {
  p_label do_label;
  p_label loop_label;
} _do_hints;

typedef struct _func_hints {
  int args;
} _func_hints;

long new_text_entry(char *txt);

void p_code_call();
void p_code_print(long txt);
void p_code_pushf(float f);
void p_code_pushi(long i);
void p_code_pushs(long s);
long p_code_new_label();
void p_code_label(long label);
void p_code_jmp(long lable, int mod);
void p_code_finish();
void op_code(int op, int mod);

extern int DebugOutput;
extern long Magic;
extern long current;
extern long begin;
extern long buflength;
extern char buf[64000];
extern char Path[256];


