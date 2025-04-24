.text
.globl tsearch_find
tsearch_find:
    pushq   %rbp
    movq    %rsp, %rbp
    movq    (%rdi), %rdi
    cmpq    $0, %rdi
    je      exit_tree

    movq    %rsi, %rcx
    movbe   (%rcx), %rax

begin_search:
    movbe   (%rdi), %rdx
    cmpq    %rdx, %rax
    je      match_found

traverse_tree:
    movq    48(%rdi), %rsi
    movq    40(%rdi), %rdi
    cmpq    %rax, %rdx
    cmovl   %rsi, %rdi

check_valid:
    testq   %rdi, %rdi
    je      exit_tree
    jmp     begin_search

match_found:
    movq    32(%rdi), %rdi

exit_tree:
    movq    %rdi, %rax
    leave
    ret

