#!/bin/bash

git add *.c *.h *.s
git commit -m "Commit project 4"
git push

echo ================== Running TreeSearch Recursive in C benchmark ================
gcc -o tsearch_bench_recursive_c tsearch_bench.c tsearch.c AVLTree.c tsearch_recursive.c
if [ $? -ne 0 ]; then
  echo "You need to implement tsearch_recursive.c"
  exit 1
fi
time ./tsearch_bench_recursive_c || exit 1

echo ================== Running TreeSearch Iterative in C benchmark ================
gcc -o tsearch_bench_iterative_c tsearch_bench.c tsearch.c AVLTree.c tsearch_iterative.c  
if [ $? -ne 0 ]; then
  echo "You need to implement tsearch_iterative.c"
  exit 1
fi
time ./tsearch_bench_iterative_c || exit 1

echo ================== Running ASM 1 benchmark ================
gcc -o tsearch_bench_asm1 tsearch_bench.c tsearch.c AVLTree.c tsearch_asm1.s 
if [ $? -ne 0 ]; then
  echo "You need to implement tsearch_asm1 and be faster than C benchmark"
  exit 1
fi
time ./tsearch_bench_asm1 || exit 1

echo ================== Running ASM 2 benchmark ================
gcc -o tsearch_bench_asm2 tsearch_bench.c tsearch.c AVLTree.c tsearch_asm2.s || exit 1
if [ $? -ne 0 ]; then
  echo "You need to implement tsearch_asm2 and be faster than ASM 1 benchmark"
  exit 1
fi
time ./tsearch_bench_asm2 || exit 1

echo ================== Running ASM 3 benchmark ================
gcc -o tsearch_bench_asm3 tsearch_bench.c tsearch.c AVLTree.c tsearch_asm3.s || exit 1
if [ $? -ne 0 ]; then
  echo "You need to implement tsearch_asm3 and be faster than ASM 2 benchmark"
  exit 1
fi
time ./tsearch_bench_asm3 || exit 1

echo ================== Running ASM 4 benchmark ================
gcc -o tsearch_bench_asm4 tsearch_bench.c tsearch.c AVLTree.c tsearch_asm4.s || exit 1
if [ $? -ne 0 ]; then
  echo "You need to implement tsearch_asm4 and be faster than ASM 3 benchmark"
  exit 1
fi
time ./tsearch_bench_asm4 || exit 1

echo ================== Running ASM 5 benchmark ================
gcc -o tsearch_bench_asm5 tsearch_bench.c tsearch.c AVLTree.c tsearch_asm5.s || exit 1
if [ $? -ne 0 ]; then
  echo "You need to implement tsearch_asm5 and be faster than ASM 4 benchmark"
  exit 1
fi
time ./tsearch_bench_asm5 || exit 1




