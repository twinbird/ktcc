int putchar(int c) {
  return write(1, &c, 1);
}

int strlen(char *str) {
  int n;
  n = 0;
  while (*str != '\0')
    n++;
  return n;
}

int puts(char *str) {
  int n;
  n = strlen(str);
  if (write(1, str, n) < 0) {
    return -1;
  }
  if (putchar('\0') < 0) {
    return -1;
  }
  return 0;
}
