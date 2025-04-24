.text
.globl tsearch_find
.type tsearch_find, @function
tsearch_find:
    pushq   %rbp
    movq    %rsp, %rbp
    movq    (%rdi), %r8           
    cmpq    $0, %r8               
    je      exit_search           
    movzbl  (%rsi), %r10d         
    jmp     begin_compare         

string_compare:
    movzbl  %al, %ecx             
    movzbl  (%rdx), %edx          
    subl    %edx, %ecx            
    cmpl    $0, %ecx              
    jle     try_left             
    movq    48(%r8), %r8          
    
verify_node:
    cmpq    $0, %r8               
    je      exit_search
    
begin_compare:
    movq    %r8, %rdx             
    movq    %rsi, %r9             
    movl    %r10d, %eax          
    testb   %r10b, %r10b          
    je      finish_compare        
    
byte_loop:                        
    cmpb    %al, (%rdx)          
    jne     string_compare       
    incq    %r9                   
    incq    %rdx                 
    movzbl  (%r9), %eax         
    testb   %al, %al            
    jne     byte_loop           
    
finish_compare:
    movzbl  (%rdx), %ecx         
    negl    %ecx                 
    
try_left:
    cmpl    $0, %ecx             
    jns     success              
    movq    40(%r8), %r8         
    jmp     verify_node          
    
success:
    movq    32(%r8), %r8         
    
exit_search:
    movq    %r8, %rax
    leave
    ret
