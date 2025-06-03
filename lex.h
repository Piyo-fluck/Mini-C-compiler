#ifndef __LEX_H_INCLUDED
#define __LEX_H_INCLUDED

/*
 * 繧ｳ繝ｳ繝代う繝ｩ縺ｮ蟄怜唱隗｣譫舌Ν繝ｼ繝√Φ縺ｮ繝倥ャ繝
 */


#include <stdio.h>

/* 繝医�繧ｯ繝ｳ縺ｮ譛螟ｧ譁�ｭ玲焚 */
#define lex_TOKEN_MAXLEN 127

/* 繝医�繧ｯ繝ｳ縺ｮ繧ｿ繧､繝� */
#define token_BEGIN_ 0
typedef enum {
  token_UNDEF = token_BEGIN_, /* 譛ｪ螳夂ｾｩ */
  token_EOF, /* end of file */
  token_ID, /* 隴伜挨蟄� (螟画焚蜷阪ｄ髢｢謨ｰ蜷�) */
  token_INT, /* 謨ｴ謨ｰ繝ｪ繝�Λ繝ｫ */
  token_CHAR, /* 譁�ｭ励Μ繝�Λ繝ｫ */
  token_KW_CHAR, /* 繧ｭ繝ｼ繝ｯ繝ｼ繝� char */
  token_KW_ELSE, /* 繧ｭ繝ｼ繝ｯ繝ｼ繝� else */
  token_KW_IF, /* 繧ｭ繝ｼ繝ｯ繝ｼ繝� if */
  token_KW_INT, /* 繧ｭ繝ｼ繝ｯ繝ｼ繝� int */
  token_KW_RETURN, /* 繧ｭ繝ｼ繝ｯ繝ｼ繝� return */
  token_KW_WHILE, /* 繧ｭ繝ｼ繝ｯ繝ｼ繝� while */
  token_PLUS, /* 貍皮ｮ怜ｭ� + */
  token_MINUS, /* 貍皮ｮ怜ｭ� - */
  token_STAR, /* 貍皮ｮ怜ｭ� * */
  token_SLASH, /* 貍皮ｮ怜ｭ� / */
  token_PERCENT, /* 貍皮ｮ怜ｭ� % */
  token_AND, /* 貍皮ｮ怜ｭ� & */
  token_EQ, /* 貍皮ｮ怜ｭ� = */
  token_EQEQ, /* 貍皮ｮ怜ｭ� == */
  token_NE, /* 貍皮ｮ怜ｭ� != */
  token_GT, /* 貍皮ｮ怜ｭ� > */
  token_GE, /* 貍皮ｮ怜ｭ� >= */
  token_LT, /* 貍皮ｮ怜ｭ� < */
  token_LE, /* 貍皮ｮ怜ｭ� <= */
  token_COMMA, /* 險伜捷 , */
  token_SEMICOLON, /* 險伜捷 ; */
  token_LPAREN, /* 險伜捷 ( */
  token_RPAREN, /* 險伜捷 ) */
  token_LBRACE, /* 險伜捷 { */
  token_RBRACE, /* 險伜捷 } */
  token_LBRACK, /* 險伜捷 [ */
  token_RBRACK, /* 險伜捷 ] */
  token_END_, /* terminator */
} lex_type_t;
#define token_MAX_ token_END_


/* lex 縺ｮ繝�ヰ繝�げ逕ｨ縺ｮ繝医Ξ繝ｼ繧ｹ蜃ｺ蜉帙�繝｢繝ｼ繝� */
typedef enum {
  lex_TRACE_NO, /* 菴輔ｂ縺励↑縺� */
  lex_TRACE_BY_CHAR, /* 譁�ｭ怜腰菴阪〒蜃ｺ蜉� */
  lex_TRACE_BY_TOKEN, /* 繝医�繧ｯ繝ｳ蜊倅ｽ阪〒蜃ｺ蜉� */
} lex_trace_t;


/* 蟄怜唱隗｣譫舌�繝��繧ｿ繧剃ｿ晄戟縺吶ｋ讒矩�菴� */
typedef struct {
  char  *filename; /* 蜃ｦ逅�ｸｭ縺ｮ繝輔ぃ繧､繝ｫ蜷� */
  FILE  *fp; /* 繝輔ぃ繧､繝ｫ縺ｫ繧｢繧ｯ繧ｻ繧ｹ縺吶ｋ縺溘ａ縺ｮ繝昴う繝ｳ繧ｿ */
  char  c; /* 迴ｾ蝨ｨ隱ｭ繧薙〒縺�ｋ 1 譁�ｭ� */
  int  linenum; /* 迴ｾ蝨ｨ隱ｭ繧薙〒縺�ｋ陦後�逡ｪ蜿ｷ */
  lex_type_t  type;  /* 逶ｴ霑代↓蛻�ｊ蜃ｺ縺励◆繝医�繧ｯ繝ｳ縺ｮ繧ｿ繧､繝� */
  char  token[lex_TOKEN_MAXLEN+1]; /* 繝医�繧ｯ繝ｳ縺昴�繧ゅ� */
  int  token_len; /* 繝医�繧ｯ繝ｳ髟ｷ */
  int  val; /* 謨ｴ謨ｰ繝ｪ繝�Λ繝ｫ繧�枚蟄励Μ繝�Λ繝ｫ縺ｮ蛟､ */
  lex_trace_t  trace; /* 繝�ヰ繝�げ逕ｨ縺ｮ繝医Ξ繝ｼ繧ｹ諠��ｱ蜃ｺ蜉帙�繝｢繝ｼ繝� */
} lex_t;


lex_t* lex_new(char* filename);
void lex_get (lex_t *x);
void lex_trace_set(lex_t *x, lex_trace_t trace);
char* lex_typename(lex_type_t type);
void lex_err(lex_t *x, char *msg);
void lex_delete(lex_t *x);

#endif
