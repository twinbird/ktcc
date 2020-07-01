int x;
int y[20];
int z = 1;
char c = 'c';

int main() {
  y[0] = 1;
  y[1] = 2;
  x = y[0] + y[1] + z - 1;
  x = x + c - 'c';
  return x;
}
