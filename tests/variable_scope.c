int x;
int foo() {
  int x;
  x = 5;
  return x;
}
int main() {
  x = 1;
  int y;
  int z;
  y = 1;
  z = 1;
  return x + y + z + foo();
}
