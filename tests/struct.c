
struct point {
  int x;
  int y;
};

int main() {
  struct point p;
  p.y = 1;

  if (p.y != 1) {
    puts("構造体に代入した値と参照した値が異なっています");
    exit(p.y);
  }

  return sizeof(p);
}
