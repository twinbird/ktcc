#!/bin/bash

assert() {
  expected="$1"
  input="$2"

  ./ktcc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert_func() {
  expected="$1"
  input="$2"

  echo -E "$3" > tmp.c
  ./ktcc "$input" > tmp.s
  cc -S tmp.c -o tmp2.s
  cc tmp.s tmp2.s -o tmp
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 'main() { return 0; }'
assert 42 'main() { return 42; }'
assert 21 "main() { return 5+20-4; }"
assert 41 " main() { return 12 + 34 - 5 ; }"
assert 47 'main() { return 5+6*7; }'
assert 15 'main() { return 5*(9-6); }'
assert 4 'main() { return (3+5)/2; }'
assert 7 'main() { return -3+10; }'
assert 15 'main() { return -(-3*+5); }'
assert 1 'main(){return 1==1;}'
assert 0 'main() { return 1==0;}'
assert 1 'main() { return 1 == 1; }'
assert 0 'main() { return 1 == 0; } '
assert 1 'main() { return 1 != 0; } '
assert 0 'main() { return 0 != 0; }'
assert 1 'main() { return 0 < 1; }'
assert 0 'main() { return 1 < 0;}'
assert 1 'main(){return 0 <= 0;}'
assert 0 'main() { return 0 <= -1; }'
assert 1 'main() { return 1 > 0;}'
assert 0 'main() { return 1 > 1;}'
assert 1 'main() { return 1 >= 1;}'
assert 0 'main() { return 1 >= 2;}'
assert 3 'main() { return a = 3;}'
assert 22 'main() { return 5 * 6 - 8;}'
assert 14 'main() { a = 3; b = 5 * 6 - 8; return a + b / 2; }'
assert 5 'main() { return bar = 2 + 3; }'
assert 6 'main() { foo = 1; bar = 2 + 3; return foo + bar; }'
assert 5 'main() { a = 1; return 5; b = 3; }'
assert 1 'main() { if (1 > 0) return 1; return 2; }'
assert 2 'main() { if (1 < 0) return 1; return 2; }'
assert 3 'main() {
            if (1 > 0)
              if (2 > 0)
                return 3;
          }'
assert 2 'main() {
            if (1 < 0)
              return 1;
            else
              return 2;
          }'
assert 3 'main() {
            i = 0;
            while (i < 3) 
              i = i + 1;
            return i;
          }'
assert 4 'main() {
            i = 0;
            while (i = i + 1)
              if (i == 4)
                return i;
          }'
assert 0 'main() {
            i = 0;
            while (0)
              return i + 1;
            return i;
          }'
assert 3 'main() {
            for (i = 0; i < 3; i = i + 1)
              1;
            return i;
          }'
assert 6 'main() {
            ret = 0;
            for (i = 0; i < 4; i = i + 1) {
              ret = ret + i;
            }
            return ret;
          }'
assert_func 1 'main() { return foo(); }' 'int foo() { return 1; }'
assert_func 21 'main() { return foo(1,2,3,4,5,6);}' 'int foo(int a, int b, int c, int d, int e, int f) { return a+b+c+d+e+f; }'
assert_func 2 'main() { return foo(1); }' 'int foo(int a) { return 1 + a; }'

echo OK
