int main() {
  char x[3];
  x[0] = '0';
  x[1] = '\t';
  x[2] = -1;
  return x[0] + x[1] + x[2];
}
