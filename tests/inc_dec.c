int main() {
  int n;
  n = 3;
  ++n;
  if (++n != 5) {
    return 0;
  }
  --n;
  if (n++ != 4) {
    return 0;
  }
  n--;
  return n;
}
