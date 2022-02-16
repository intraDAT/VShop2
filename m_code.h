/* Major commands */
#define CALL 1  /* Stack: . . . no_args(int) func_name(string) -> result  */
#define PFX 2   /* _accu = _accu << 4 | mod */
#define LDC 3   /* accu = _accu << 4 | mod; _accu = 0 */
#define PUSH 4  /* Stack: - -> accu */
#define PRINT 5
#define FUNC 6
#define JMP 7
#define SPECIAL 8
#define POP 9
#define EXIT 15

/* Modifiers FUNC */
#define OP_LT 1
#define OP_LE 2
#define OP_EQ 3
#define OP_GE 4
#define OP_GT 5
#define OP_NE 6

#define OP_PLUS 7
#define OP_MINUS 8
#define OP_TIMES 9
#define OP_DIV 10
#define OP_MOD 11
#define OP_NEG 12

/* Maybe obsolete */
#define OP_AND 13
#define OP_OR  14
#define OP_NOT 15

/* modifiers JMP */
#define JMP_UNC 1
#define JMP_Z   2
#define JMP_NZ  3

/* modifiers SPECIAL */
#define FOR_INIT 1 /* Stack: start end step -> end step start */
#define FOR_TEST 2 /* Stack: end step start -> end step start res
		      if step>0 
		        res = (start < end)
		      else
		        res = (start > end)
		   */
		        
#define FOR_LOOP 3 /* stack: end step start -> end step start+step */
#define FOR_VAL  4

#define PRODUCT_INIT 5 
#define PRODUCT_LOOP 6

#define CATALOG_INIT 7 
#define CATALOG_LOOP 8

#define BASKET_INIT 9 
#define BASKET_LOOP 10

#define PROMOTION_INIT 11 
#define PROMOTION_LOOP 12

#define PRODUCT_INIT2 13 

#define PRINT_STACK 14 

#define RELEVANT_INIT 15 
#define RELEVANT_LOOP 16


/* p_code file format */

typedef struct p_exec_head {
  long magic;
  long n_labels;
  long ofs_labels;
  long n_strings;
  long ofs_strings;
  long n_exec;
  long ofs_exec;
} p_exec_head;

  


