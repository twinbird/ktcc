int putchar(int c) {
  return write(1, &c, 1);
}

int strlen(char *str) {
  int n;
  n = 0;
  while (*str != '\0')
    n = n + 1;
  return n;
}

