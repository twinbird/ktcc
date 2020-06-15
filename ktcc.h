#ifndef _KTCC_H_
#define _KTCC_H_

#include "ktcc.h"

#define MAX_FUNCTION_NAME_LENGTH 255

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

// ローカル変数
typedef struct LVar LVar;
struct LVar {
  LVar *next; // 次の変数かNULL
  char *name; // 変数名
  int len;    // 変数名の長さ
  int offset; // RBPからのオフセット
};

extern LVar *locals;

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
  ND_NUM,      // 整数
  ND_RETURN,   // return
  ND_IF,       // if
  ND_WHILE,    // while
  ND_FOR,      // for
  ND_BLOCK,    // { ... } で表現するブロック
  ND_FUNC,     // 関数
  ND_FUNC_DEF, // 関数定義
} NodeKind;

// 抽象構文木のノード
typedef struct Node Node;
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // 値(kindがND_NUMの場合のみ利用する)
  int offset; // 変数へのRBPからのオフセット(kindがND_LVARの場合)
  Node *init;                               // 初期化式(for)
  Node *inc;                                // インクリメント式(for)
  Node *cond;                               // 条件式
  Node *then;                               // 条件式がtrueの場合の文
  Node *els;                                // 条件式がfalseの場合の文
  List *stmts;                              // ブロック内の文
  char func_name[MAX_FUNCTION_NAME_LENGTH]; // 実行する関数名
  List *args;                               // 関数の引数
  char func_def_name[MAX_FUNCTION_NAME_LENGTH]; // 関数定義名
  Node *func_body;                              // 関数本体
  LVar *def_args;                               // 関数定義の引数
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

// 標準入力文字列をトークナイズする
extern Token *tokenize();

// 構文解析の起点
void program();
#endif
