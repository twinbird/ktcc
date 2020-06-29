int foo(int a, int b, int c, int d, int e, int f) {
  return a + b + c + d + e + f;
}
int fib(int n) {
  if (n == 1)
    return 1;
  if (n == 2)
    return 1;
  return fib(n - 2) + fib(n - 1);
}
int main() {
  return foo(1, 2, 3, 4, 5, 6) + fib(5);
}
