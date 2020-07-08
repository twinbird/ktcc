#include "ktcc.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// エラーレポート関数
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void debug_comment(char *msg) {
  printf("# %s\n", msg);
}

int type_kind_size(TypeKind kind) {
  switch (kind) {
  case CHAR:
    return 1;
    break;
  case INT:
    return 4;
    break;
  case PTR:
    return 8;
    break;
  default:
    error("不正な型が見つかりました");
  }
}

int align(int sz) {
  return sz + (sz % 4);
}

int alloc_size(Type *ty) {
  if (!ty) {
    error("不正な型が見つかりました");
  }
  if (ty->kind == ARRAY) {
    return ty->array_size * alloc_size(ty->ptr_to);
  }
  if (ty->kind == STRUCT) {
    StructMember *m = ty->members;
    int sz = 0;
    while (m) {
      sz += align(alloc_size(m->type));
      m = m->next;
    }
    return sz;
  }
  return type_kind_size(ty->kind);
}

Type *member_type(Type *t, char *mem_name) {
  if (t->kind != STRUCT) {
    error("メンバ参照する変数が構造体ではありません");
  }
  int sz = 0;
  StructMember *m = t->members;
  while (m) {
    if (!memcmp(m->name, mem_name, m->len)) {
      break;
    }
    m = m->next;
  }
  return m->type;
}

Type *type_of(Node *node) {
  switch (node->kind) {
  case ND_GVAR:
    return node->gvar->ty;
    break;
  case ND_LVAR:
    return node->lvar->ty;
    break;
  case ND_MEM_REF:
    return member_type(node->lhs->lvar->ty, node->mem_ref_name);
    break;
  default:
    return type_of(node->lhs);
    break;
  }
}

char *strndup(const char *s, size_t n) {
  char *p = memchr(s, '\0', n);
  if (p != NULL)
    n = p - s;
  p = malloc(n + 1);
  if (p != NULL) {
    memcpy(p, s, n);
    p[n] = '\0';
  }
  return p;
}
