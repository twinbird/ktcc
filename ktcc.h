#ifndef _KTCC_H_
#define _KTCC_H_

#include "ktcc.h"
#include <stdbool.h>
#include <stdio.h>

// トークンの種類
typedef enum {
  TK_RESERVED,  // 記号
  TK_IDENT,     // 識別子
  TK_NUM,       // 整数トークン
  TK_CHARACTER, // 文字トークン
  TK_STRING,    // 文字列トークン
  TK_RETURN,    // 予約語:return
  TK_IF,        // 予約語:if
  TK_ELSE,      // 予約語:else
  TK_WHILE,     // 予約語:while
  TK_FOR,       // 予約語:for
  TK_EOF,       // 入力終わりを示すトークン
  TK_INT,       // 予約語:int
  TK_CHAR,      // 予約語:char
  TK_STRUCT,    // 予約語:struct
  TK_SIZEOF,    // 予約語:sizeof
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

// 構造体のメンバ
typedef struct StructMember StructMember;

// 変数の型
typedef enum {
  INT,
  CHAR,
  PTR,
  ARRAY,
  STRUCT,
} TypeKind;
typedef struct Type Type;
struct Type {
  Type *next;            // 次の型定義
  TypeKind kind;         // 型の種類
  struct Type *ptr_to;   // ポインタの場合: 何を指す型か
  size_t array_size;     // 配列の長さ
  char *name;            // 構造体名
  int len;               // 構造体名の長さ
  StructMember *members; // 構造体のメンバのリスト
};
extern Type *types;

struct StructMember {
  StructMember *next; // 次の構造体のメンバかNULL
  char *name;         // メンバの名前
  int len;            // メンバの名前の長さ
  Type *type;         // メンバの型
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
  GVar *next;     // 次の変数かNULL
  Type *ty;       // 変数の型
  char *name;     // 変数名
  int len;        // 変数名の長さ
  int init_int;   // intの初期値
  char init_char; // charの初期値
};
extern GVar *globals;

// 文字列定数
typedef struct StrLiteral StrLiteral;
struct StrLiteral {
  StrLiteral *next; // 次の変数かNULL
  char *str;        // 文字列
  int len;          // 文字列の長さ
  char *label;      // アセンブリコードのラベル
};
extern StrLiteral *str_literals;

// 抽象構文木ノードの種類
typedef enum {
  ND_ADD,         // +
  ND_SUB,         // -
  ND_MUL,         // *
  ND_DIV,         // /
  ND_EQ,          // ==
  ND_NE,          // !=
  ND_LT,          // <と>
  ND_LE,          // <=と>=
  ND_ASSIGN,      // =
  ND_LVAR,        // ローカル変数
  ND_GVAR,        // グローバル変数
  ND_STR_LITERAL, // 文字列リテラル
  ND_NUM,         // 整数
  ND_RETURN,      // return
  ND_IF,          // if
  ND_WHILE,       // while
  ND_FOR,         // for
  ND_BLOCK,       // { ... } で表現するブロック
  ND_FUNC,        // 関数
  ND_FUNC_DEF,    // 関数定義
  ND_ADDR,        // &
  ND_DEREF,       // *
  ND_MEM_REF,     // .
} NodeKind;

typedef struct ArgsList ArgsList;
typedef struct StmtsList StmtsList;

// 抽象構文木のノード
typedef struct Node Node;
struct Node {
  NodeKind kind;       // ノードの型
  Node *lhs;           // 左辺
  Node *rhs;           // 右辺
  int val;             // 値(kindがND_NUMの場合のみ利用する)
  LVar *lvar;          // ローカル変数(kindがND_LVARの場合)
  GVar *gvar;          // グローバル変数(kindがND_GVARの場合)
  Node *init;          // 初期化式(for)
  Node *inc;           // インクリメント式(for)
  Node *cond;          // 条件式
  Node *then;          // 条件式がtrueの場合の文
  Node *els;           // 条件式がfalseの場合の文
  StmtsList *stmts;    // ブロック内の文
  char *func_name;     // 実行する関数名
  ArgsList *args;      // 関数の引数
  char *func_def_name; // 関数定義名
  Node *func_body;     // 関数本体
  Type *return_type;   // 関数の戻り値の型
  LVar *locals;        // 関数定義内の変数(kind: ND_FUNC_DEFの場合)
  StrLiteral *str_literal; // 文字列リテラル(kind: ND_STR_LITERALの場合)
  char *mem_ref_name;      // .で参照するメンバ名(ND_MEM_REFの場合)
};

// 引数のリスト
struct ArgsList {
  ArgsList *next;
  Node *data;
};
extern ArgsList *args_add(ArgsList *l, Node *data);

// 文のリスト
struct StmtsList {
  StmtsList *next;
  Node *data;
};
extern StmtsList *stmts_add(StmtsList *l, Node *data);

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
void error(char *fmt, ...);

// コード生成して標準出力へ出力
extern void gen(Node *node);

// 初期化されていないグローバル変数のコードを生成して標準出力へ出力
extern void gen_global(GVar *globals);

// 初期化されるグローバル変数のコードを生成して標準出力へ出力
extern void gen_init_global(GVar *globals);

// 文字列定数のコードを生成して標準出力へ出力
extern void gen_str_literal_data(StrLiteral *literals, int count);

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

// 文字列をn文字コピーして末尾にNULLを入れて返す
char *strndup(const char *s, size_t n);

StructMember *member_add(StructMember *list, char *name, int len, Type *t);
Type *struct_type_add(Type *list, char *name, int len, StructMember *m);
Type *find_type(Type *list, char *name, int len);
Type *member_type(Type *t, char *mem_name);

int align(int sz);
#endif
