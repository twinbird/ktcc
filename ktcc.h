#ifndef _KTCC_H_
#define _KTCC_H_

#include "ktcc.h"
#include <stdbool.h>
#include <stdio.h>

#define MAX_FUNCTION_NAME_LENGTH 255
#define MAX_VARIABLE_NAME_LENGTH 255

typedef struct List List;
struct List {
  List *next;
  void *data;
};
extern List *list_add(List *l, void *data);

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_RETURN,   // 予約語:return
  TK_IF,       // 予約語:if
  TK_ELSE,     // 予約語:else
  TK_WHILE,    // 予約語:while
  TK_FOR,      // 予約語:for
  TK_EOF,      // 入力終わりを示すトークン
  TK_INT,      // 予約語:int
  TK_CHAR,     // 予約語:char
  TK_SIZEOF,   // 予約語:sizeof
} TokenKind;

// トークン
typedef struct Token Token;
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ
};

// 変数の型
typedef enum {
  INT,
  CHAR,
  PTR,
  ARRAY,
} TypeKind;
typedef struct Type Type;
struct Type {
  TypeKind kind;
  struct Type *ptr_to; // ポインタの場合: 何を指す型か
  size_t array_size;
};

// ローカル変数
typedef struct LVar LVar;
struct LVar {
  LVar *next;  // 次の変数かNULL
  Type *ty;    // 変数の型
  char *name;  // 変数名
  int len;     // 変数名の長さ
  int offset;  // RBPからのオフセット
  bool is_arg; // 関数定義の引数ならtrue
};
extern LVar *locals;

// グローバル変数
typedef struct GVar GVar;
struct GVar {
  GVar *next; // 次の変数かNULL
  Type *ty;   // 変数の型
  char *name; // 変数名
  int len;    // 変数名の長さ
};
extern GVar *globals;

// 抽象構文木ノードの種類
typedef enum {
  ND_ADD,      // +
  ND_SUB,      // -
  ND_MUL,      // *
  ND_DIV,      // /
  ND_EQ,       // ==
  ND_NE,       // !=
  ND_LT,       // <と>
  ND_LE,       // <=と>=
  ND_ASSIGN,   // =
  ND_LVAR,     // ローカル変数
  ND_GVAR,     // グローバル変数
  ND_NUM,      // 整数
  ND_RETURN,   // return
  ND_IF,       // if
  ND_WHILE,    // while
  ND_FOR,      // for
  ND_BLOCK,    // { ... } で表現するブロック
  ND_FUNC,     // 関数
  ND_FUNC_DEF, // 関数定義
  ND_ADDR,     // &
  ND_DEREF,    // *
} NodeKind;

// 抽象構文木のノード
typedef struct Node Node;
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // 値(kindがND_NUMの場合のみ利用する)
  LVar *lvar;    // ローカル変数(kindがND_LVARの場合)
  GVar *gvar;    // グローバル変数(kindがND_GVARの場合)
  Node *init;    // 初期化式(for)
  Node *inc;     // インクリメント式(for)
  Node *cond;    // 条件式
  Node *then;    // 条件式がtrueの場合の文
  Node *els;     // 条件式がfalseの場合の文
  List *stmts;   // ブロック内の文
  char func_name[MAX_FUNCTION_NAME_LENGTH];     // 実行する関数名
  List *args;                                   // 関数の引数
  char func_def_name[MAX_FUNCTION_NAME_LENGTH]; // 関数定義名
  Node *func_body;                              // 関数本体
  Type *return_type;                            // 関数の戻り値の型
  LVar *locals; // 関数定義内の変数(kind: ND_FUNC_DEFの場合)
};

// 現在着目しているトークン
extern Token *token;

// 入力プログラム
extern char *user_input;

// 構文解析された入力プログラム
extern Node *code[100];

// 構文解析された関数群
extern Node *funcs[100];

// エラーレポート関数
extern void error_at(char *loc, char *fmt, ...);
void error(char *msg);

// コード生成して標準出力へ出力
extern void gen(Node *node);

// グローバル変数のコードを生成して標準出力へ出力
extern void gen_global();

// 標準入力文字列をトークナイズする
extern Token *tokenize();

// 構文解析の起点
extern void program();

// デバッグコメントを出力ソースへ書き残す
extern void debug_comment(char *msg);

// 型のために割り当てるバイトサイズを返す
extern int alloc_size(Type *ty);

// 型のために割り当てるバイトサイズを返す
int type_kind_size(TypeKind kind);

// ND_LVAR, ND_GVAR, ND_DEREFの示す変数の型を返す
Type *type_of(Node *node);
#endif
