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
  printf("main:\n");

  // 変数26個分を確保しておく
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n"); // 26 * 8 = 208

  for (int i = 0; code[i]; i++) {
    gen(code[i]);
    // 式の評価結果をpop
    printf("  pop rax\n");
  }
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("	ret\n");

  return 0;
}
