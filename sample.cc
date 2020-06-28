int main() {
  char c;
  c = 'H';
  write(4, &c, 2);
  c = 'e';
  write(4, &c, 2);
  c = 'l';
  write(4, &c, 2);
  c = 'l';
  write(4, &c, 2);
  c = 'o';
  write(4, &c, 2);
  c = ',';
  write(4, &c, 2);
  c = ' ';
  write(4, &c, 2);
  c = 'W';
  write(4, &c, 2);
  c = 'o';
  write(4, &c, 2);
  c = 'r';
  write(4, &c, 2);
  c = 'l';
  write(4, &c, 2);
  c = 'd';
  write(4, &c, 2);
  c = '\n';
  write(4, &c, 2);

  return 1;
}
