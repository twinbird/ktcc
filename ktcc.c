#include "ktcc.h"
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません。\n");
    return 1;
  }

  // tokenize
  user_input = argv[1];
  token = tokenize();

  // parse
  program();

  // code generate
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");

  for (int i = 0; funcs[i]; i++) {
    gen(funcs[i]);
  }

  return 0;
}
