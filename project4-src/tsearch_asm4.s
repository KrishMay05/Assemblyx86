.text
.globl tsearch_find
tsearch_find:
    pushq   %rbp
    movq    %rsp, %rbp
    movq    (%rdi), %rdi           
    cmpq    $0, %rdi               
    je      exit_search            
    movq    %rsi, %rcx             

begin_loop:                        
    movq    (%rcx), %rax          
    bswap   %rax
    movq    8(%rcx), %r8          
    bswap   %r8
    movq    16(%rcx), %r9         
    bswap   %r9
    movq    24(%rcx), %r10        
    bswap   %r10

    # Load and swap node bytes
    movq    (%rdi), %rdx          
    bswap   %rdx
    movq    8(%rdi), %r11         
    bswap   %r11
    movq    16(%rdi), %r12        
    bswap   %r12
    movq    24(%rdi), %r13        
    bswap   %r13
    
    # Perform comparisons
    cmpq    %rdx, %rax            
    jne     handle_mismatch       
    cmpq    %r11, %r8             
    jne     handle_mismatch       
    cmpq    %r12, %r9             
    jne     handle_mismatch       
    cmpq    %r13, %r10            
    jne     handle_mismatch       

key_match:                         
    movq    32(%rdi), %rdi        
    jmp     exit_search           

handle_mismatch:                   
    movq    48(%rdi), %rsi        
    movq    40(%rdi), %rdi        
    cmpq    %rax, %rdx            
    cmovl   %rsi, %rdi            
    
verify_node:                      
    testq   %rdi, %rdi            
    je      exit_search           
    jmp     begin_loop            

exit_search:                       
    movq    %rdi, %rax            
    leave
    ret
