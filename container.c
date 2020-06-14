#include "ktcc.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

List *list_add(List *l, void *data) {
  List *n = malloc(sizeof(List));
  n->data = data;
  n->next = NULL;
  if (l == NULL) {
    return n;
  }

  List *search = l;
  while (search->next != NULL) {
    search = search->next;
  }
  search->next = n;
  return l;
}

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
