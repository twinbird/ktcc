#include "ktcc.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_file(char *path) {
  FILE *fp = fopen(path, "r");
  if (!fp) {
    error("cannot open %s: %s", path, strerror(errno));
  }

  if (fseek(fp, 0, SEEK_END) == -1) {
    error("%s: fseek: %s", path, strerror(errno));
  }
  size_t size = ftell(fp);
  if (fseek(fp, 0, SEEK_SET) == -1) {
    error("%s: fseek: %s", path, strerror(errno));
  }

  // \n\0の分 + 2
  char *buf = calloc(1, size + 2);
  fread(buf, size, 1, fp);

  // ファイル終端を\n\0にする
  if (size == 0 || buf[size - 1] != '\n') {
    buf[size++] = '\n';
  }
  buf[size] = '\0';

  fclose(fp);
  return buf;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません。\n");
    return 1;
  }

  // tokenize
  user_input = read_file(argv[1]);
  token = tokenize();

  // parse
  program();

  // code generate
  printf(".intel_syntax noprefix\n");

  printf(".bss\n");
  gen_global(globals);

  printf(".text\n");
  printf(".globl main\n");

  gen_str_literal_data(str_literals, 0);

  for (int i = 0; funcs[i]; i++) {
    gen(funcs[i]);
  }

  return 0;
}
