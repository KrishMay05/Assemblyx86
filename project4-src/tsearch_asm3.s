.text
.globl tsearch_find
tsearch_find:
    pushq   %rbp                    
    movq    %rsp, %rbp
    movq    (%rdi), %rdi           
    cmpq    $0, %rdi               
    je      done_searching         
    movq    %rsi, %rcx             
    
begin_comparison:                   
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
    
    # Compare all bytes
    cmpq    %rdx, %rax             
    jne     handle_mismatch        
    cmpq    %r11, %r8              
    jne     handle_mismatch        
    cmpq    %r12, %r9              
    jne     handle_mismatch        
    cmpq    %r13, %r10             
    jne     handle_mismatch        
    
match_success:                      
    movq    32(%rdi), %rdi         
    jmp     done_searching         
    
handle_mismatch:                    
    movq    %rax, %r9              
    cmpq    %rdx, %rax             
    jle     go_left                
    
go_right:                          
    movq    48(%rdi), %rdi         
    jmp     verify_child
    
go_left:                           
    movq    40(%rdi), %rdi         
    
verify_child:                       
testq   %rdi, %rdi             
    je      done_searching         
    jmp     begin_comparison       
    
done_searching:                     
    movq    %rdi, %rax             
    leave                          
    ret
