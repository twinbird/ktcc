#include "ktcc.h"
#include <stdlib.h>

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

