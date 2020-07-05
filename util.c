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

int alloc_size(Type *ty) {
  if (!ty) {
    error("不正な型が見つかりました");
  }
  if (ty->kind == ARRAY) {
    return ty->array_size * alloc_size(ty->ptr_to);
  }
  return type_kind_size(ty->kind);
}

Type *type_of(Node *node) {
  switch (node->kind) {
  case ND_GVAR:
    return node->gvar->ty;
    break;
  case ND_LVAR:
    return node->lvar->ty;
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
