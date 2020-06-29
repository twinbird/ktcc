int int_size() {
  int x;
  return sizeof(x);
}

int ptr_size() {
  int *x;
  return sizeof(x);
}

int num_size() {
  return sizeof(3);
}

int deref_size() {
  int *x;
  return sizeof(*x);
}

int main() {
  return int_size() + ptr_size() + num_size() + deref_size();
}
