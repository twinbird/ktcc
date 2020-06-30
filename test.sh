#!/bin/bash

function compile() {
  ./ktcc "$file" > tmp.s
	./ktcc "mylibc/mylibc.c" > tmp_mylibc.s
	as -o mylibc.o mylibc/mylibc_asm.s tmp_mylibc.s
	cc -o tmp tmp.s mylibc.o
}

function assert_exit() {
  expected="$1"
  file="$2"

  compile $file

  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "[$file] => [OK]"
  else
    echo "[$file] => $expected expected, but got $actual"
    exit 1
  fi
}

function assert_out() {
  expected="$1"
  file="$2"

  compile $file

  ./tmp
  actual=`./tmp`

  if [ "$actual" = "$expected" ]; then
    echo "[$file] => [OK]"
  else
    echo "[$file] => '$expected' expected, but got '$actual'"
    exit 1
  fi
}

assert_exit 42 tests/exit_code.c
assert_exit 21 tests/plus_minus.c
assert_exit 47 tests/multiple.c
assert_exit 15 tests/parentheses.c
assert_exit 4 tests/divide.c
assert_exit 22 tests/minus_sign.c
assert_exit 6 tests/compare.c
assert_exit 14 tests/variable.c
assert_exit 4 tests/if.c
assert_exit 3 tests/while.c
assert_exit 6 tests/for.c
assert_exit 26 tests/function.c
assert_exit 3 tests/addr_ptr.c
assert_exit 3 tests/array_ptr.c
assert_exit 3 tests/array.c
assert_exit 20 tests/sizeof.c
assert_exit 3 tests/global_variable.c
assert_exit 8 tests/variable_scope.c
assert_exit 56 tests/char.c
assert_out 'Hello, world' tests/write.c
assert_exit 101 tests/str_literal.c

echo OK
