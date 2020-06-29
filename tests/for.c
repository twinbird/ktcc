int main() {
  int i;
  int ret;
  ret = 0;
  for (i = 0; i < 4; i = i + 1) {
    ret = ret + i;
  }
  return ret;
}
