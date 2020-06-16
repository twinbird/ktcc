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

assert 0 'int main() { return 0; }'
assert 42 'int main() { return 42; }'
assert 21 "int main() { return 5+20-4; }"
assert 41 " int main() { return 12 + 34 - 5 ; }"
assert 47 'int main() { return 5+6*7; }'
assert 15 'int main() { return 5*(9-6); }'
assert 4 'int main() { return (3+5)/2; }'
assert 7 'int main() { return -3+10; }'
assert 15 'int main() { return -(-3*+5); }'
assert 1 'int main(){return 1==1;}'
assert 0 'int main() { return 1==0;}'
assert 1 'int main() { return 1 == 1; }'
assert 0 'int main() { return 1 == 0; } '
assert 1 'int main() { return 1 != 0; } '
assert 0 'int main() { return 0 != 0; }'
assert 1 'int main() { return 0 < 1; }'
assert 0 'int main() { return 1 < 0;}'
assert 1 'int main(){return 0 <= 0;}'
assert 0 'int main() { return 0 <= -1; }'
assert 1 'int main() { return 1 > 0;}'
assert 0 'int main() { return 1 > 1;}'
assert 1 'int main() { return 1 >= 1;}'
assert 0 'int main() { return 1 >= 2;}'
assert 3 'int main() { int a; return a = 3;}'
assert 22 'int main() { return 5 * 6 - 8;}'
assert 14 'int main() { int a; int b; a = 3; b = 5 * 6 - 8; return a + b / 2; }'
assert 5 'int main() { int bar; return bar = 2 + 3; }'
assert 6 'int main() { int foo; int bar; foo = 1; bar = 2 + 3; return foo + bar; }'
assert 5 'int main() { int a; int b; a = 1; return 5; b = 3; }'
assert 1 'int main() { if (1 > 0) return 1; return 2; }'
assert 2 'int main() { if (1 < 0) return 1; return 2; }'
assert 3 'int main() {
            if (1 > 0)
              if (2 > 0)
                return 3;
          }'
assert 2 'int main() {
            if (1 < 0)
              return 1;
            else
              return 2;
          }'
assert 3 'int main() {
            int i;
            i = 0;
            while (i < 3) 
              i = i + 1;
            return i;
          }'
assert 4 'int main() {
            int i;
            i = 0;
            while (i = i + 1)
              if (i == 4)
                return i;
          }'
assert 0 'int main() {
            int i;
            i = 0;
            while (0)
              return i + 1;
            return i;
          }'
assert 3 'int main() {
            int i;
            for (i = 0; i < 3; i = i + 1)
              1;
            return i;
          }'
assert 6 'int main() {
            int i;
            int ret;
            ret = 0;
            for (i = 0; i < 4; i = i + 1) {
              ret = ret + i;
            }
            return ret;
          }'
assert 4 'int foo() {
            return 2;
          }
          int main() {
            return foo() + foo();
          }'
assert 1 'int foo() { return 1; } int main() { return foo(); }'
assert 2 'int foo(int a) { return 1 + a; } int main() { return foo(1); }'
assert 4 'int bar(int a, int b) { return 1 + a + b; } int main() { return bar(1, 2); }'
assert 21 'int foo(int a, int b, int c, int d, int e, int f) { return a+b+c+d+e+f; } int main() { return foo(1,2,3,4,5,6);}'
assert 5 'int fib(int n) { if (n == 1) return 1; if (n == 2) return 1; return fib(n-2) + fib(n-1);} int main() {return fib(5); }'
assert 3 'int main() { int x; int y; x = 3; y = &x; return *y; }'
assert 3 'int main() { int x; int *y; y = &x; *y = 3; return x; }'

echo OK
