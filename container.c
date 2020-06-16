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

