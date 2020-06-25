#include "ktcc.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

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

void error(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

void debug_comment(char *msg) {
  printf("# %s\n", msg);
}

int type_kind_size(TypeKind kind) {
  switch (kind) {
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

