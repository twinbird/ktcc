int main() {
  int a;
  a = 0;

  if (1 > 0)
    a = a + 1;
  a = a + 1;
  if (1 > 0)
    if (2 > 0)
      a = a + 1;
  if (0 > 1)
    a = a + 10;
  else
    a = a + 1;

  return a;
}
