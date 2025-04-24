#!/bin/bash

git add *.c *.h *.s
git commit -m "Commit project 5"
git push

echo ================== Running TreeSearch Iterative in C benchmark ================
gcc -g -static -o tsearch_bench_iterative_c tsearch_bench_better.c tsearch.c AVLTree.c tsearch_iterative.c profil.c || exit 1  
if [ $? -ne 0 ]; then
  echo "You need to implement tsearch_iterative.c"
  exit 1
fi
time ./tsearch_bench_iterative_c || exit 1

echo ================== Running ASM 6 benchmark ================
gcc -g -static -o tsearch_bench_asm6 tsearch_bench_better.c tsearch.c AVLTree.c tsearch_asm6.s profil.c || exit 1
if [ $? -ne 0 ]; then
  echo "You need to implement tsearch_asm6"
  exit 1
fi
time ./tsearch_bench_asm6 || exit 1




