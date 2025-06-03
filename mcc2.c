
/* 
 * mcc                       by Nagisa Neco Ishiura    *  
 *                                                     */ 
 
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>  
#include <assert.h> 
#include "lex.h"  
#include "tab.h"
#include "code.h" 
  
#define ARRAY_MAXDIMENSION 8 
#define STACK_FRAME_RESERVE 3 
  
/* mcc のデバッグ用トレース情報の選択 */ 
typedef enum {  
  mcc_TRACE_NO,  
  mcc_TRACE_LOW,   
  mcc_TRACE_MID,
  mcc_TRACE_HIGH, 
} mcc_trace_t;  
 
static mcc_trace_t mcc_trace = mcc_TRACE_NO; 
static int tmp_stack1747119801 = 12834;  
 
static void arg(int argc, char **argv, char source_f[], char object_f[], mcc_trace_t *trace);  
static void argerr(); 
static void at(char *checkpoint);  
static void syntax_error(lex_t *x, char *msg);
static void semantic_error(char *msg); 
static int  id_isfunc(char *id, tab_t *gt, tab_t *lt);  
 
static void preprocess(code_t *c); 
static void postprocess(code_t *c, tab_t *gt);  
static void parse_program(code_t *c, lex_t *x, tab_t *gt); 
static void parse_declaration_head(code_t *c, lex_t *x, itab_basetype_t *type, int *ptrlevel, char *id);  
static void parse_variable_declaration_tail(code_t *c, lex_t *x, tab_t *t, itab_cls_t cls, itab_basetype_t type, int ptrlevel, char *id); 
static void parse_function_declaration_tail(code_t *c, lex_t *x, tab_t *t, itab_basetype_t type, int ptrlevel, char *id);  
static void parse_function_body(code_t *c, lex_t *x, tab_t *gt, tab_t *lt, int argc);
static void parse_variable_declaration(code_t *c, lex_t *x, tab_t *t, itab_cls_t cls); 
static void parse_statement(code_t *c, lex_t *x, tab_t *gt, tab_t *lt);  
static void parse_expression(code_t *c, lex_t *x, tab_t *gt, tab_t *lt); 
static void parse_expression2(code_t *c, lex_t *x, tab_t *gt, tab_t *lt); 
static void parse_expression3(code_t *c, lex_t *x, tab_t *gt, tab_t *lt);  
static void parse_expression4(code_t *c, lex_t *x, tab_t *gt, tab_t *lt); 
static void parse_expression5(code_t *c, lex_t *x, tab_t *gt, tab_t *lt);  
static void parse_variable_reference(code_t *c, lex_t *x, tab_t *gt, tab_t *lt); 
static void parse_array_index(code_t *c, lex_t *x, tab_t *gt, tab_t *lt, tab_t *t, int i);  
static void parse_assign(code_t *c, lex_t *x, tab_t *gt, tab_t *lt);
static void parse_if(code_t *c, lex_t *x, tab_t *gt, tab_t *lt); 
static void parse_while(code_t *c, lex_t *x, tab_t *gt, tab_t *lt);  
static void parse_return(code_t *c, lex_t *x, tab_t *gt, tab_t *lt); 
static void parse_call(code_t *c, lex_t *x, tab_t *gt, tab_t *lt); 
static void parse_lhs_expression(code_t *c, lex_t *x, tab_t *gt, tab_t *lt);  
 
  
int main(int argc, char **argv) 
{  
  char source_f[FILENAME_MAX]; /* mini-C プログラムのファイル名 */
  char object_f[FILENAME_MAX]; /* VSM コードのファイル名 */ 
  
  /* (0) 変数 x, c, gt, lex_trace の宣言をこの下に書き込む */ 
  lex_t *x;
  code_t *c;
  tab_t *gt;
  lex_trace_t lex_trace;
 
  /* (1) 引数の解析 */  
  arg(argc, argv, source_f, object_f, &mcc_trace); 
  
  /* (2)〜(7)の処理をこの下に書き込む */ 
  x = lex_new(source_f);
  c = code_new();
  gt = tab_new(lex_TOKEN_MAXLEN);

  switch(mcc_trace) {
    case mcc_TRACE_NO: lex_trace = lex_TRACE_NO; break;
    case mcc_TRACE_LOW: lex_trace = lex_TRACE_BY_CHAR; break;
    case mcc_TRACE_MID: lex_trace = lex_TRACE_BY_TOKEN; break;
    case mcc_TRACE_HIGH: lex_trace = lex_TRACE_BY_TOKEN; break;
    default: assert(0); //エラー
    }
  lex_trace_set(x, lex_trace);

  preprocess(c);
  lex_get(x);
  parse_program(c,x,gt);
  postprocess(c,gt);
  code_write(c, object_f);
  tab_delete(gt);
  code_delete(c);
  lex_delete(x);
  
  return 0;
} 
  
 
static void argerr() 
/* 引数エラー (コマンドラインシンタックスを表示) */  
{ 
  fprintf(stderr, "syntax: mcc [-t TRACE_LEVEL][-o file] PROG.mc\n");  
  exit(EXIT_FAILURE); 
}  

static void arg( 
  int argc,   
  char **argv,  
  char source_f[],  
  char object_f[],   
  mcc_trace_t *mcc_trace 
)  
/* 引数の解析 */ 
{  
  char *suffix;
  int i = 1; 
  strcpy(object_f, "");  
  if (i>=argc) argerr(); 
  while (argv[i][0]=='-') { 
    if (strcmp(argv[i],"-t")==0) {  
      i++; 
      if (i>=argc) argerr();  
      switch(atoi(argv[i])) { 
        case 0: *mcc_trace = mcc_TRACE_NO; break;  
        case 1: *mcc_trace = mcc_TRACE_LOW; break;
        case 2: *mcc_trace = mcc_TRACE_MID; break; 
        case 3: *mcc_trace = mcc_TRACE_HIGH; break;  
        default:  *mcc_trace = mcc_TRACE_NO; 
      } 
      i++;  
    } 
    else if (strcmp(argv[i],"-o")==0) {  
      i++; 
      if (i>=argc) argerr();  
      strcpy(object_f, argv[i]);
      i++; 
    }  
    else argerr(); 
    if (i>=argc) argerr(); 
  }  
  if (i>=argc) argerr(); 
  strcpy(source_f, argv[i]);  
 
  if (strlen(object_f)==0) {  
    strcpy(object_f,source_f);
    suffix = object_f+strlen(object_f)-3; 
    if (strcmp(suffix,".mc")==0) {  
      strcpy(suffix,".vsm"); 
    } 
    else {  
      strcpy(object_f+strlen(object_f),".vsm"); 
    }  
  } 
}  

 
static void at(char *checkpoint)  
/* デバッグ用: チェックポイントの表示 (主に関数の入口用) */ 
{ 
  if (mcc_trace>=mcc_TRACE_MID) {  
    fprintf(stderr, "at %s\n", checkpoint); 
  }  
} 
  

static void end(char *checkpoint)   
/* デバッグ用: チェックポイントの表示 (主に関数の出口用) */  
{  
  if (mcc_trace>=mcc_TRACE_MID) { 
    fprintf(stderr, "end %s\n", checkpoint);  
  } 
}     
 
  
static void syntax_error(lex_t *x, char *msg)
/* 文法エラー */ 
{  
  fprintf(stderr, "%s:%d: %s (last token '%s')\n",  
    x->filename, x->linenum, msg, x->token); 
  exit(EXIT_FAILURE);  
} 
  
 
static void semantic_error(char *msg)  
/* 意味エラー */
{ 
  fprintf(stderr, "%s\n", msg);  
  exit(EXIT_FAILURE); 
} 
  
 
static void preprocess(code_t *c)  
{ 
  at("preprocess");  
  code_append(c, opcode_ISP, 0, 0);
  code_append(c, opcode_LC, 0, 0);
  code_append(c, opcode_SB, 1, 0);
  code_append(c, opcode_CALL, 0, 0);
  code_append(c, opcode_EXIT, 0, 0);
}
 
static void postprocess(code_t *c, tab_t *gt)  
{ 
  at("postprocess"); 
  code_set(c, 0, opcode_ISP, gt->itab_vsize, 0);
  code_set(c, 1, opcode_LC, gt->itab_vsize, 0);
  int i = tab_itab_find(gt, "main");
  if (i==itab_FAIL) {
  semantic_error("body of ’main’ not defined");
  }
  else if (gt->itab[i].role == itab_role_VAR) {
  semantic_error("’main’ is declared as a variable");
  }
  else if (gt->itab[i].role == itab_role_FUNC) {
  code_set(c, 3, opcode_CALL, gt->itab[i].address, 0);  
  }
  else {
  assert(0);
}
}
 
static void parse_program(code_t *c, lex_t *x, tab_t *gt)  
{ 
  itab_basetype_t type;
  int ptrlevel;
  char id[lex_TOKEN_MAXLEN+1];
  at("parse_program");  
  while (x->type!=token_EOF) {
    parse_declaration_head(c, x, &type, &ptrlevel, id);
      if (x->type==token_LPAREN) {
        parse_function_declaration_tail (c, x, gt, type, ptrlevel, id);
      }
      else {
        parse_variable_declaration_tail (c, x, gt, itab_cls_GLOBAL, type, ptrlevel, id);
        if (x->type==token_SEMICOLON) {
          lex_get(x);
        }
        else {
          syntax_error(x, "';' expected");
        }
      }
  }
}
 
  
static void parse_declaration_head(code_t *c, lex_t *x, itab_basetype_t *type, int *ptrlevel, char *id) 
{ 
   at("parse_declaration_head");  
  if (x->type == token_KW_INT) { 
    *type = itab_basetype_INT; 
  }
  else if (x->type == token_KW_CHAR) { 
    *type = itab_basetype_CHAR; 
  }
  else { 
    syntax_error(x, "type name expected"); 
  }
  lex_get(x);
  *ptrlevel = 0;
  while (x->type == token_STAR) {
    (*ptrlevel)++;
    lex_get(x);
  }
  if (x->type == token_ID) {
    strcpy(id, x->token);
    lex_get(x);
  }
  else { 
    syntax_error(x, "identifier expected"); 
  }
  //fprintf(stderr, "type='%c', ptrlevel=%i, id=\"%s\"\n", *type, *ptrlevel, id);
}  
 
  
static void
parse_variable_declaration_tail(code_t *c, lex_t *x, tab_t *t, itab_cls_t cls, itab_basetype_t type, int ptrlevel, char *id) 
{  
  int dimension = 0;
  int max[ARRAY_MAXDIMENSION];
  int elementsize[ARRAY_MAXDIMENSION];
  int size, d;
  at("parse_variable_declaration_tail"); 
  while(x->type == token_LBRACK) {
    assert(dimension < ARRAY_MAXDIMENSION);
    lex_get(x);
    if (x->type == token_INT) {
      max[dimension] = x->val;
      dimension++; 
      lex_get(x);
    }
    else { 
      syntax_error(x, "array size integer expected"); 
    }
    if (x->type == token_RBRACK) { 
      lex_get(x); 
    }
    else { 
      syntax_error(x, "']' expected"); 
    }
  }
    
    
  if (dimension==0) { size = 1; }
  else {
    elementsize[dimension-1] = 1;
    for (d = dimension-2; 0 <= d; d--) {
      elementsize[d] = max[d+1] * elementsize[d+1];
    }
    size = max[0] * elementsize[0];
  }
  int i = tab_itab_new(t, id);
  if (i < 0) {
    char errmsg[lex_TOKEN_MAXLEN+256];
    sprintf(errmsg, "multiple definition of %s", id);
    syntax_error(x, errmsg);
  }
  t->itab[i].role = itab_role_VAR;
  t->itab[i].cls = cls;
  t->itab[i].basetype = type;
  t->itab[i].ptrlevel = ptrlevel;
  t->itab[i].argc = 0;
  t->itab[i].size = size;
  t->itab[i].address = t->itab_vsize;
  t->itab_vsize += size;
  if (dimension == 0) {
    t->itab[i].aref = -1;
  }
  else {
    for (d = 0; d < dimension; d++) {
      int a = tab_atab_append(t, max[d], elementsize[d]);
      if (d == 0) t->itab[i].aref = a;
    }
  }
} 
  
 
static void parse_function_declaration_tail
(code_t *c, lex_t *x, tab_t *gt, itab_basetype_t type, int ptrlevel, char *id) 
{  
  tab_t *lt;
  int argc = 0;
  int i;
  at("parse_function_declaration_tail");

  lt = tab_new(lex_TOKEN_MAXLEN);

  if (x->type == token_LPAREN) {
    lex_get(x);
  } else {
    syntax_error(x, "'(' expected");
  }

  while (x->type != token_RPAREN) {
    argc++;
    parse_variable_declaration(c, x, lt, itab_cls_ARG);
    if (x->type != token_RPAREN) {
      if (x->type == token_COMMA) {
        lex_get(x);
      } else {
        syntax_error(x, "',' or ')' expected");
      }
    }
  }
  lex_get(x);

  i = tab_itab_new(gt, id);
  if (i < 0) {
    char errmsg[lex_TOKEN_MAXLEN+256];
    sprintf(errmsg, "multiple definition of %s", id);
    syntax_error(x, errmsg);
  }
  gt->itab[i].role = itab_role_FUNC;
  gt->itab[i].cls = itab_cls_GLOBAL;
  gt->itab[i].basetype = type;
  gt->itab[i].ptrlevel = ptrlevel;
  gt->itab[i].argc = argc;
  gt->itab[i].aref = -1;
  gt->itab[i].address = c->size;

  if (mcc_trace >= mcc_TRACE_HIGH) {
    fprintf(stderr, "== dump list of gt ==\n");
    tab_dump(gt);
  }

  parse_function_body(c, x, gt, lt, argc);
  gt->itab[i].size = c->size - gt->itab[i].address;

  tab_delete(lt);
}  
 
static void parse_function_body(code_t *c, lex_t *x, tab_t *gt, tab_t *lt, int argc)  
{
  at("parse_function_body"); 
  if (x->type == token_LBRACE) { 
    lex_get(x); 
  }  
  else { 
    syntax_error(x, "'{' expected"); 
  } 
 
  while (x->type == token_KW_INT || x->type == token_KW_CHAR) {
    parse_variable_declaration(c, x, lt, itab_cls_LOCAL);
    if (x->type == token_SEMICOLON) { 
      lex_get(x); 
    }
    else { 
      syntax_error(x, "';' expected"); 
    }
  }

  if (mcc_trace >= mcc_TRACE_HIGH) {  
    fprintf(stderr, "== dump list of lt ==\n"); 
    tab_dump(lt);  
  } 
  code_append(c, opcode_ISP, lt->itab_vsize + STACK_FRAME_RESERVE, 0);
  while (x->type != token_RBRACE) {
    parse_statement(c, x, gt, lt);
  }
  code_append(c, opcode_RET, 0, 0);
  if (x->type == token_RBRACE) { 
    lex_get(x); 
  }
  else { 
    syntax_error(x, "'}' expected"); 
  }
}  
 
static void parse_variable_declaration(code_t *c, lex_t *x, tab_t *t, itab_cls_t cls) 
{  
  int ptrlevel;
  itab_basetype_t type;
  char id[lex_TOKEN_MAXLEN+1];
  at("parse_variable_declaration"); 
  parse_declaration_head(c, x, &type, &ptrlevel, id);
  parse_variable_declaration_tail(c, x, t, cls, type, ptrlevel, id);
}  
 
  
static void
parse_statement(code_t *c, lex_t *x, tab_t *gt, tab_t *lt) 
{  
  at("parse_statement"); 
   if (x->type == token_SEMICOLON) {
    lex_get(x);
   }
   else if (x->type == token_ID) {
    if(id_isfunc(x->token, gt, lt)) {
      parse_call(c, x, gt, lt);
      code_append(c, opcode_ISP, -1, 0);
    }
    else {parse_assign(c, x, gt, lt);}
  }
   else if (x->type == token_KW_IF) {
     parse_if(c, x, gt, lt);
   }
   else if (x->type == token_KW_WHILE) {
    parse_while(c, x, gt, lt);
   }
   else if (x->type == token_SEMICOLON) {
    lex_get(x);
   }
   else if (x->type == token_LBRACE) {
    lex_get(x);
    while(x->type!=token_RBRACE) {
      parse_statement(c,x,gt,lt);
    }
    lex_get(x);
   }
   else if (x->type == token_KW_RETURN) {
    parse_return(c,x,gt,lt);
   }
   else if(x->type==token_STAR) {
    parse_assign(c, x, gt, lt);
   }
   else{
    syntax_error(x,"'?;' expected");
   }
} 
  
static void parse_expression(code_t *c, lex_t *x, tab_t *gt, tab_t *lt) 
{  
  at("parse_expression"); 
  parse_expression2(c,x,gt,lt);

  lex_type_t cmp_op = x->type;
  while(cmp_op == token_LT || cmp_op == token_GT || cmp_op == token_LE || cmp_op ==
    token_GE || cmp_op == token_EQEQ || cmp_op == token_NE) {
    lex_get(x);
    parse_expression2(c, x, gt, lt);
    switch(cmp_op) {
        case token_EQEQ: code_append(c, opcode_EQ, 0, 0); break;
        case token_NE:   code_append(c, opcode_NE, 0, 0); break;
        case token_GT:   code_append(c, opcode_GT, 0, 0); break;
        case token_GE:   code_append(c, opcode_GE, 0, 0); break;
        case token_LT:   code_append(c, opcode_LT, 0, 0); break;
        case token_LE:   code_append(c, opcode_LE, 0, 0); break;
        default: break;
    }
    cmp_op = x->type;
  }
}  

static void parse_expression2(code_t *c, lex_t *x, tab_t *gt, tab_t *lt) 
{  
  at("parse_expression2"); 
  lex_type_t plmi_op = x->type;
  if(plmi_op== token_PLUS || plmi_op == token_MINUS) {lex_get(x);}
  parse_expression3(c, x, gt, lt);
  if(plmi_op==token_MINUS) {
    code_append(c, opcode_INV, 0, 0);
  }

  plmi_op = x->type;
  while(plmi_op == token_PLUS || plmi_op == token_MINUS) {
    lex_get(x);
    parse_expression3(c, x, gt, lt);
    if(plmi_op == token_MINUS) {code_append(c, opcode_SUB, 0, 0);}
    else if (plmi_op == token_PLUS) {code_append(c, opcode_ADD, 0, 0);}
    else {;}
    plmi_op = x->type;
  }


} 
  
static void parse_expression3(code_t *c, lex_t *x, tab_t *gt, tab_t *lt) 
{  
  at("parse_expression3"); 
  lex_type_t multi_op = x->type;
  parse_expression4(c, x, gt, lt);

  lex_type_t buffo = x->type;
  while(buffo == token_STAR || buffo == token_SLASH || buffo == token_PERCENT) {
    lex_get(x);
    parse_expression4(c, x, gt, lt);
    if(buffo == token_STAR) {code_append(c, opcode_MUL, 0, 0);}
    else if (buffo == token_SLASH) {code_append(c, opcode_DIV, 0, 0);}
    else if(buffo == token_PERCENT) {code_append(c, opcode_MOD, 0, 0);}
    else {;}
    buffo = x->type;
  }
}  

static void parse_expression4(code_t *c, lex_t *x, tab_t *gt, tab_t *lt) 
{  
  at("parse_expression4"); 
  parse_expression5(c,x,gt,lt);
} 
  
static void parse_expression5(code_t *c, lex_t *x, tab_t *gt, tab_t *lt) 
{  
  at("parse_expression5"); 
  if(x->type==token_CHAR||x->type==token_INT) {
    code_append(c, opcode_LC, x->val, 0);
    lex_get(x);
  }
  else if (x->type == token_LPAREN) {
    lex_get(x);
    parse_expression(c,x,gt,lt);
    if (x->type == token_RPAREN) {
      lex_get(x);
    }
    else {syntax_error(x, "'11)' expected");}
  }
  else if (x->type == token_ID) {
    if (id_isfunc(x->token,gt,lt)) { parse_call(c, x, gt, lt); }
    else { parse_variable_reference(c, x, gt, lt); }
  }
  else {syntax_error(x, "syntax error");}  
}
 
static void parse_variable_reference(code_t *c, lex_t *x, tab_t *gt, tab_t *lt)  
{ 
  at("parse_variable_reference"); 
  int var_index;
  char* var_name = malloc(sizeof(char)*strlen(x->token));
    strcpy(var_name, x->token);
  lex_get(x);
  if( (var_index=tab_itab_find(lt, var_name)) != itab_FAIL) {
        code_append(c, opcode_LV, 1, (STACK_FRAME_RESERVE + lt->itab[var_index].address));
      }
      else if ((var_index=tab_itab_find(gt, var_name)) != itab_FAIL) {
        code_append(c, opcode_LV, 0, gt->itab[var_index].address);
  }
  free(var_name);
}  
 
static void parse_array_index(code_t *c, lex_t *x, tab_t *gt, tab_t *lt, tab_t *t, int i)  
{ 
  at("parse_array_index");  
}
 
static void parse_assign(code_t *c, lex_t *x, tab_t *gt, tab_t *lt)  
{ 
  at("parse_assign"); 
//   左辺式」の解析は parse lhs expression(c, x, gt, lt) を呼び出して行なう (lhs は left-hand
// side (左辺) の略). この後, ”=” 記号を確認し, 「式」の解析に parse expression を呼び出し, 最後に
// ”;” を確認すればよい.
  parse_lhs_expression(c, x, gt, lt);
  lex_get(x);
  if(x->type == token_EQ) {lex_get(x);}
  else {syntax_error(x, "'=' expected");}

  parse_expression(c, x, gt, lt);
  if(x->type == token_SEMICOLON) {lex_get(x);}
  else {syntax_error(x, "';' expected");}

  code_append(c, opcode_SI, 0, 0);
  
}  
 
static void parse_if(code_t *c, lex_t *x, tab_t *gt, tab_t *lt)  
{ 
  at("parse_if");  
  // if 文 ::= "if" "(" 式 ")" 文 ( ε | "else" 文 )
  lex_get(x);
  if(x->type==token_LPAREN) {lex_get(x);}
  else {syntax_error(x, "'('expected");}

  parse_expression(c, x, gt, lt);

  if(x->type==token_RPAREN) {lex_get(x);}
  else {syntax_error(x, "')'expected");}

  //BZ L1
  int index_BZ_to_else = code_append(c, opcode_BZ, 1, 0);
  //解析
  parse_statement(c, x, gt, lt);
  //ブランチ先
  //code_set(c, index_BZ_to_else, opcode_BZ, c->size-index_BZ_to_else-1, 0);
  if(x->type!=token_KW_ELSE) {
    code_set(c, index_BZ_to_else, opcode_BZ, c->size - index_BZ_to_else - 1, 0);;}
  else {
    int index_B_to_end = code_append(c, opcode_B, 1, 0);

    // BZ は else の直前にジャンプ
    code_set(c, index_BZ_to_else, opcode_BZ, c->size - index_BZ_to_else-1, 0);

    lex_get(x); // "else"
    parse_statement(c, x, gt, lt);

    // B のジャンプ先を if-else 全体の終わりに
    code_set(c, index_B_to_end, opcode_B, c->size - index_B_to_end - 1, 0);
  }
}
 
static void parse_while(code_t *c, lex_t *x, tab_t *gt, tab_t *lt)  
{ 
  at("parse_while"); 
  //while 文 ::= "while" "(" 式 ")" 文
   lex_get(x);
  if(x->type==token_LPAREN) {lex_get(x);}
  else {syntax_error(x, "'('expected");}
  int index_while = c->size;
  parse_expression(c, x, gt, lt);

  if(x->type==token_RPAREN) {lex_get(x);}
  else {syntax_error(x, "')'expected");}

  int index_BZ_to_else = code_append(c, opcode_BZ, 1, 0);
  parse_statement(c, x, gt, lt);

  code_set(c, index_BZ_to_else, opcode_BZ, c->size-index_BZ_to_else, 0);
  code_append(c, opcode_B, index_while-c->size-1, 0);
  // while_index (式のコード)
  //    BZ L2
  //     (文のコード)
  //    B while_index
  // L2
}  
 
static void parse_return(code_t *c, lex_t *x, tab_t *gt, tab_t *lt)  
{ 
  at("parse_return");  
}
 
static void parse_call(code_t *c, lex_t *x, tab_t *gt, tab_t *lt)  
{ 
  at("parse_call"); 
  if(strcmp(x->token,"putchar")==0) {
    lex_get(x);
    if(x->type==token_LPAREN) {lex_get(x);}
    else {syntax_error(x, "'(' expected");}
    parse_expression(c, x, gt, lt);

    if(x->type==token_RPAREN) {lex_get(x);}
    else {syntax_error(x, "')' expected");}
    code_append(c, opcode_DUP, 0, 0);
    code_append(c, opcode_PUTC, 0, 0);
  }
  else if(strcmp(x->token,"putint")==0) {
    lex_get(x);
    if(x->type==token_LPAREN) {lex_get(x);}
    else {syntax_error(x, "'(' expected");}
    parse_expression(c, x, gt, lt);

    if(x->type==token_RPAREN) {lex_get(x);}
    else {syntax_error(x, "'1)' expected");}
    code_append(c, opcode_DUP, 0, 0);
    code_append(c, opcode_PUTI, 0, 0);
  }
  else if (strcmp(x->token,"getchar") == 0) {
    lex_get(x);
    if (x->type ==token_LPAREN) {lex_get(x);}
    else {syntax_error(x, "'(' expected");}

    if(x->type==token_RPAREN) {lex_get(x);}
    else {syntax_error(x, "')' expected");}
    code_append(c, opcode_GETC, 0, 0);
  }
  else if (strcmp(x->token,"getint") == 0) {
    lex_get(x);
    if (x->type ==token_LPAREN) {lex_get(x);}
    else {syntax_error(x, "'(' expected");}

    if(x->type==token_RPAREN) {lex_get(x);}
    else {syntax_error(x, "')' expected");}
    code_append(c, opcode_GETI, 0, 0);
  }
  else {
    semantic_error("Semantic error: function not defined.");
  }
}  
 
  
static void parse_lhs_expression(code_t *c, lex_t *x, tab_t *gt, tab_t *lt) 
{  
  at("parse_lhs_expression");
  int assg_index;
  if( (assg_index=tab_itab_find(lt, x->token)) != itab_FAIL) {
        code_append(c, opcode_LA, 1, (STACK_FRAME_RESERVE + lt->itab[assg_index].address));
  }
  else if ((assg_index=tab_itab_find(gt, x->token)) != itab_FAIL) {
        code_append(c, opcode_LA, 0, gt->itab[assg_index].address);
  }
} 
  
 
static int id_isfunc(char *id, tab_t *gt, tab_t *lt)  
/* id が関数名なら 1 を，変数名なら 0 を返す */
{ 
  int isfunc = 0; // id が関数のとき 1, 変数のとき 0
  if (tab_itab_find(lt,id)!=itab_FAIL) { //…(1)
  isfunc = 0; // 変数である …(2)
  }
  else if (!strcmp(id,"putchar")||!strcmp(id,"putint")||!strcmp(id,"getchar")||!strcmp(id,"getint")) {
  isfunc = 1; // 関数である
  }
  else {
    int func_index = tab_itab_find(gt,id);
    if (func_index != itab_FAIL) {
      if (gt->itab[func_index].role==itab_role_FUNC) { isfunc = 1; }
      else if (gt->itab[func_index].role==itab_role_VAR) { isfunc = 0; }
      else {semantic_error("internal error");}  // … (3)
    }
    else {semantic_error("id is not defined");}
  }
return isfunc;
}  
 
