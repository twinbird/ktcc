#include "ktcc.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

// パースされた抽象構文木
Node *code[100];

// パースされた関数
Node *funcs[100];

// ローカル変数のリスト
LVar *locals;

// 次のトークンが期待している記号の時には、
// トークンを1つ読み進めてtrue.その他はfalse
static bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    return false;
  }
  token = token->next;
  return true;
}

// 次のトークンが識別子の時には、
// トークンを1つ読み進めて消費したトークンを返す
static Token *consume_kind(TokenKind kind) {
  Token *t = token;
  if (t->kind != kind) {
    return NULL;
  }
  token = t->next;
  return t;
}

// 次のトークンが期待している記号の時には、トークンを1つ読み進める。
// その他はエラー
static void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    error_at(token->str, "'%s'ではありません", op);
  }
  token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// その他はエラー
static int expect_number() {
  if (token->kind != TK_NUM) {
    error_at(token->str, "数ではありません");
  }
  int val = token->val;
  token = token->next;
  return val;
}

static bool at_eof() {
  return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurにつなげる
static Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

static bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

static bool is_alnum(char p) {
  return ('a' <= p && p <= 'z') || ('0' <= p && p <= '9') || (p == '_');
}

static bool is_word(char *p, char *word) {
  int len = strlen(word);
  if (strncmp(p, word, len) == 0 && !is_alnum(p[len])) {
    return true;
  }
  return false;
}

// 入力文字列pをトークナイズして返す
Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") ||
        startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '=' ||
        *p == '{' || *p == '}' || *p == ',' || *p == '&' || *p == '[' ||
        *p == ']') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (is_word(p, "sizeof")) {
      cur = new_token(TK_SIZEOF, cur, p, 6);
      p += 6;
      continue;
    }

    if (is_word(p, "return")) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }

    if (is_word(p, "if")) {
      cur = new_token(TK_IF, cur, p, 2);
      p += 2;
      continue;
    }

    if (is_word(p, "else")) {
      cur = new_token(TK_ELSE, cur, p, 4);
      p += 4;
      continue;
    }

    if (is_word(p, "while")) {
      cur = new_token(TK_WHILE, cur, p, 5);
      p += 5;
      continue;
    }

    if (is_word(p, "for")) {
      cur = new_token(TK_FOR, cur, p, 3);
      p += 3;
      continue;
    }

    if (is_word(p, "int")) {
      cur = new_token(TK_INT, cur, p, 3);
      p += 3;
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      int len = 0;
      char *q = p;
      while (is_alnum(*q)) {
        len++;
        q++;
      }
      cur = new_token(TK_IDENT, cur, p, len);
      p += len;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

static Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

static LVar *new_lvar(LVar *list, Token *tok, Type *ty, bool is_arg) {
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = list;
  lvar->name = tok->str;
  lvar->len = tok->len;
  lvar->ty = ty;
  lvar->offset = list == NULL ? 8 : list->offset + 8;
  lvar->is_arg = is_arg;

  return lvar;
}

static Type *new_type(TypeKind kind, Type *ptr_to, int array_size) {
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = kind;
  ty->ptr_to = ptr_to;
  ty->array_size = array_size;

  return ty;
}

// 以降のトークンが型の表現の前半部分(識別子まで)の定義の場合、トークンを消費して型を返す。
// その他はNULL
static Type *consume_type_prefix() {
  if (consume_kind(TK_INT)) {
    // 型
    Type *ty = new_type(INT, NULL, 0);

    // ポインタ?
    while (consume("*")) {
      Type *t = new_type(PTR, ty, 0);
      ty = t;
    }

    return ty;
  }
  return NULL;
}

static LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }
  return NULL;
}

static Node *expr();

static Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_kind(TK_IDENT);
  if (tok) {
    if (consume("(")) {
      // 関数
      Node *node = new_node(ND_FUNC, NULL, NULL);
      // 引数
      if (!consume(")")) {
        for (int i = 0; i < 6; i++) {
          node->args = list_add(node->args, expr());
          if (!consume(",")) {
            expect(")");
            break;
          }
        }
      }
      strncpy(node->func_name, tok->str, tok->len);
      node->func_name[tok->len + 1] = '\0';
      return node;
    } else {
      // 変数参照
      LVar *lvar = find_lvar(tok);
      if (!lvar) {
        error_at(token->str, "宣言されていない変数が見つかりました");
      }
      Node *var_node = new_node(ND_LVAR, NULL, NULL);
      var_node->lvar = lvar;

      if (consume("[")) {
        // 配列の添え字での参照
        int idx = expect_number();
        expect("]");
        Node *offset_node = new_node_num(idx);
        Node *ptr_node = new_node(ND_ADD, var_node, offset_node);
        Node *deref_node = new_node(ND_DEREF, ptr_node, NULL);
        return deref_node;
      }

      return var_node;
    }
  }

  return new_node_num(expect_number());
}

static Node *size_of_expr(Node *n) {
  // ND_NUMなら4
  if (n->kind == ND_NUM) {
    return new_node_num(4);
  }

  // ND_LVARなら型によって分ける
  if (n->kind == ND_LVAR) {
    switch (n->lvar->ty->kind) {
    case INT:
      return new_node_num(4);
    case PTR:
      return new_node_num(8);
    default:
      error("不明な型です");
    }
  }
  // ND_DEREFならデリファレンスした先のサイズ
  if (n->kind == ND_DEREF) {
    switch (n->lhs->lvar->ty->ptr_to->kind) {
    case INT:
      return new_node_num(4);
    case PTR:
      return new_node_num(8);
    default:
      error("不明な型です");
    }
  }

  // その他の演算子なら左辺値のサイズ
  return size_of_expr(n->lhs);
}

static Node *unary() {
  if (consume_kind(TK_SIZEOF)) {
    return size_of_expr(unary());
  }
  if (consume("+")) {
    return primary();
  }
  if (consume("-")) {
    // 0-xとして置き換える
    return new_node(ND_SUB, new_node_num(0), primary());
  }
  if (consume("&")) {
    return new_node(ND_ADDR, unary(), NULL);
  }
  if (consume("*")) {
    return new_node(ND_DEREF, unary(), NULL);
  }
  return primary();
}

static Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

static Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

static Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<")) {
      node = new_node(ND_LT, node, add());
    } else if (consume("<=")) {
      node = new_node(ND_LE, node, add());
    } else if (consume(">")) {
      node = new_node(ND_LT, add(), node);
    } else if (consume(">=")) {
      node = new_node(ND_LE, add(), node);
    } else {
      return node;
    }
  }
}

static Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

static Node *assign() {
  Node *node = equality();
  if (consume("=")) {
    node = new_node(ND_ASSIGN, node, assign());
  }
  return node;
}

static Node *expr() {
  return assign();
}

static Node *stmt() {
  Node *node;

  if (consume_kind(TK_IF)) {
    node = new_node(ND_IF, NULL, NULL);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume_kind(TK_ELSE)) {
      node->els = stmt();
    } else {
      node->els = NULL;
    }
    return node;
  }

  if (consume_kind(TK_WHILE)) {
    node = new_node(ND_WHILE, NULL, NULL);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    return node;
  }

  if (consume_kind(TK_FOR)) {
    node = new_node(ND_FOR, NULL, NULL);
    expect("(");
    if (!consume(";")) {
      node->init = expr();
      expect(";");
    } else {
      node->init = NULL;
    }
    if (!consume(";")) {
      node->cond = expr();
      expect(";");
    } else {
      node->cond = NULL;
    }
    if (!consume(")")) {
      node->inc = expr();
      expect(")");
    } else {
      node->inc = NULL;
    }
    node->then = stmt();
    return node;
  }

  Type *ty = NULL;
  if ((ty = consume_type_prefix())) {
    // 変数名
    Token *tok = consume_kind(TK_IDENT);

    // 配列?
    if (consume("[")) {
      int n = expect_number();
      expect("]");

      Type *nt = new_type(ARRAY, ty, n);
      ty = nt;
    }

    locals = new_lvar(locals, tok, ty, false);
    expect(";");

    Node *node = new_node(ND_LVAR, NULL, NULL);
    node->lvar = locals;
    return node;
  }

  if (consume("{")) {
    node = new_node(ND_BLOCK, NULL, NULL);
    node->stmts = NULL;
    while (!consume("}")) {
      node->stmts = list_add(node->stmts, (void *)stmt());
    }
    return node;
  }

  if (consume_kind(TK_RETURN)) {
    node = new_node(ND_RETURN, expr(), NULL);
    expect(";");
    return node;
  } else {
    node = expr();
    expect(";");
    return node;
  }
}

static Node *func_def() {
  Node *node = new_node(ND_FUNC_DEF, NULL, NULL);

  node->return_type = consume_type_prefix();
  if (!node->return_type) {
    // 未指定ならINT
    node->return_type = new_type(INT, NULL, 0);
  }

  // 関数名
  Token *tok = consume_kind(TK_IDENT);
  strncpy(node->func_def_name, tok->str, tok->len);
  node->func_def_name[tok->len] = '\0';

  // 引数
  node->locals = NULL;
  expect("(");
  if (!consume(")")) {
    while (1) {
      // 型
      Type *ty = consume_type_prefix();
      if (!ty) {
        error_at(token->str, "型名が未指定です");
      }

      // 変数名
      tok = consume_kind(TK_IDENT);
      if (!tok) {
        error_at(token->str, "変数名が未指定です");
      }
      node->locals = new_lvar(node->locals, tok, ty, true);
      if (!consume(",")) {
        expect(")");
        break;
      }
    }
  }

  // ローカル変数を差し替えて定義本体を解析
  LVar *l = locals;
  locals = node->locals;
  node->func_body = stmt();
  node->locals = locals;
  locals = l;

  return node;
}

void program() {
  int i = 0;
  while (!at_eof()) {
    funcs[i++] = func_def();
  }
  funcs[i] = NULL;
}
