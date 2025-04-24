.text
.globl tsearch_find
tsearch_find:
    pushq   %rbp
    movq    %rsp, %rbp             
    pushq   %r15                   
    subq    $8, %rsp
    
    movq    %rsi, -8(%rbp)         
    movq    (%rdi), %r15           
    cmpq    $0, %r15               
    je      tree_empty
    
search_begin:
    movq    %r15, %rsi             
    movq    -8(%rbp), %rdi         
    call    avl_compareKeys
    cmpq    $0, %rax
    jl      go_left_child
    je      key_found
    
go_right_child:
    movq    48(%r15), %r15
    cmpq    $0, %r15
    je      tree_empty
    jmp     search_begin
    
go_left_child:
    movq    40(%r15), %r15
    cmpq    $0, %r15
    je      tree_empty
    jmp     search_begin
    
key_found:
    movq    32(%r15), %r15
    jmp     exit_search
    
tree_empty:
    xorq    %r15, %r15            
    
exit_search:
    movq    %r15, %rax            
    movq    -16(%rbp), %r15       
    movq    %rbp, %rsp            
    popq    %rbp                  
    ret
