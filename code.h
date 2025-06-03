#ifndef __CODE_H_INCLUDED
#define __CODE_H_INCLUDED


#define code_MAXSIZE 1024  /* VSM 繧ｳ繝ｼ繝我ｸｭ縺ｮ譛螟ｧ蜻ｽ莉､謨ｰ */


/* 蜻ｽ莉､繧ｳ繝ｼ繝� */
#define opcode_BEGIN_ 0
typedef enum {
  opcode_NOP = opcode_BEGIN_,
  opcode_ISP, 
  opcode_LA, 
  opcode_LV, 
  opcode_LC, 
  opcode_LI, 
  opcode_DUP,
  opcode_SI, 
  opcode_SV, 
  opcode_SB, 
  opcode_B, 
  opcode_BZ, 
  opcode_CALL, 
  opcode_RET, 
  opcode_GETC, 
  opcode_GETI, 
  opcode_PUTC, 
  opcode_PUTI, 
  opcode_ADD, 
  opcode_SUB, 
  opcode_MUL, 
  opcode_DIV, 
  opcode_MOD, 
  opcode_INV,
  opcode_EQ, 
  opcode_NE, 
  opcode_GT, 
  opcode_LT, 
  opcode_GE, 
  opcode_LE,
  opcode_EXIT, 
  opcode_END_, 
} opcode_t;
#define opcode_MAX_ opcode_END_


/* 蜻ｽ莉､ */
typedef struct {
  opcode_t opcode;  /* 蜻ｽ莉､繧ｳ繝ｼ繝� */
  int operand[2];  /* 繧ｪ繝壹Λ繝ｳ繝� (0縲�2蛟�) */
} insn_t;


/* VSM 繧ｳ繝ｼ繝� */
typedef struct {
  int  size;  /* 繧ｳ繝ｼ繝峨�繧ｵ繧､繧ｺ */
  insn_t  insn[code_MAXSIZE];  /* 蜻ｽ莉､ */
} code_t;


code_t* code_new();
void code_delete(code_t *c);
void code_read(code_t *c, char filename[]);
void code_write(code_t *c, char filename[]);
int code_append(code_t *c, opcode_t op, int p0, int p1);
void code_set(code_t *c, int i, opcode_t op, int p0, int p1);
char* insn_mnemonic(opcode_t); 
int insn_n_operands(opcode_t op);


#endif

