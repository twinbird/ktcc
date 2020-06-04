#ifndef _KTCC_H_
#define _KTCC_H_

#include "ktcc.h"

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_NUM,      // 整数トークン
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

// 抽象構文木ノードの種類
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_EQ,  // ==
  ND_NE,  // !=
  ND_LT,  // <と>
  ND_LE,  // <=と>=
  ND_NUM, // 整数
} NodeKind;

// 抽象構文木のノード
typedef struct Node Node;
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // 値(kindがND_NUMの場合のみ利用する)
};

// 現在着目しているトークン
extern Token *token;

// 入力プログラム
extern char *user_input;

// エラーレポート関数
extern void error_at(char *loc, char *fmt, ...);

// コード生成して標準出力へ出力
extern void gen(Node *node);

// 標準入力文字列をトークナイズする
extern Token *tokenize();

Node *expr();
#endif
