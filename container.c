#include "ktcc.h"
#include <stdlib.h>
#include <string.h>

ArgsList *args_add(ArgsList *l, Node *data) {
  ArgsList *n = malloc(sizeof(ArgsList));
  n->data = data;
  n->next = NULL;
  if (l == NULL) {
    return n;
  }

  ArgsList *search = l;
  while (search->next != NULL) {
    search = search->next;
  }
  search->next = n;
  return l;
}

StmtsList *stmts_add(StmtsList *l, Node *data) {
  StmtsList *n = malloc(sizeof(StmtsList));
  n->data = data;
  n->next = NULL;
  if (l == NULL) {
    return n;
  }

  StmtsList *search = l;
  while (search->next != NULL) {
    search = search->next;
  }
  search->next = n;
  return l;
}

StructMember *member_add(StructMember *list, char *name, int len, Type *t) {
  StructMember *sm = calloc(1, sizeof(StructMember));
  sm->name = name;
  sm->len = len;
  sm->type = t;
  sm->next = list;
  return sm;
}

Type *struct_type_add(Type *list, char *name, int len, StructMember *m) {
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = STRUCT;
  ty->name = name;
  ty->len = len;
  ty->members = m;
  ty->next = list;
  return ty;
}

Type *find_type(Type *list, char *name, int len) {
  Type *tmp = list;
  while (tmp != NULL) {
    if (!memcmp(tmp->name, name, tmp->len)) {
      return tmp;
    }
    tmp = tmp->next;
  }
  return NULL;
}
