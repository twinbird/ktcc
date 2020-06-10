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
  fi
}

assert 0 '0;'
assert 42 '42;'
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 7 '-3+10;'
assert 15 '-(-3*+5);'
assert 1 '1==1;'
assert 0 '1==0;'
assert 1 '1 == 1;'
assert 0 '1 == 0;'
assert 1 '1 != 0;'
assert 0 '0 != 0;'
assert 1 '0 < 1;'
assert 0 '1 < 0;'
assert 1 '0 <= 0;'
assert 0 '0 <= -1;'
assert 1 '1 > 0;'
assert 0 '1 > 1;'
assert 1 '1 >= 1;'
assert 0 '1 >= 2;'
assert 3 'a = 3;'
assert 22 '5 * 6 - 8;'
assert 14 'a = 3; b = 5 * 6 - 8; a + b / 2;'
assert 5 'bar = 2 + 3;'
assert 6 'foo = 1; bar = 2 + 3; foo + bar;'
assert 1 'return 1;'
assert 5 'a = 1; return 5; b = 3;'
assert 1 'if (1 > 0) return 1; return 2;'
assert 2 'if (1 < 0) return 1; return 2;'
assert 3 'if (1 > 0)
            if (2 > 0)
              return 3;'

echo OK
