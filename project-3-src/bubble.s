.text
.globl bubblesort

bubblesort:
    pushq %rbp
    movq  %rsp, %rbp

    movq  %rdx, %r8        # r8 = array pointer (a)
    movq  %rsi, %r9        # r9 = size of array (n)
    movq  %rdi, %r10       # r10 = ascending flag

    decq  %r9              # r9 = n - 1 (size of array - 1)

    movl  $0, %r11d        # i = 0 (outer loop counter)

outer_loop:
    cmpq  %r9, %r11        # if i >= n - 1, exit outer loop
    jge   end_outer_loop

    movq  %r9, %r12        # r12 = n - i - 1 (limit for inner loop)
    subq  %r11, %r12

    movl  $0, %r13d        # j = 0 (inner loop counter)

inner_loop:
    cmpq  %r12, %r13       # if j >= n - i - 1, exit inner loop
    jge   end_inner_loop

    movq  (%r8,%r13,8), %rax  # load array[j] into rax
    movq  8(%r8,%r13,8), %rbx # load array[j + 1] into rbx

    cmpq  $0, %r10          # check ascending flag
    jnz   ascending_sort    # if ascending is not 0, go to ascending sort

descending_sort:
    cmpq  %rax, %rbx        # compare array[j] and array[j+1]
    jle   no_swap           # if array[j] >= array[j+1], no swap needed
    jmp   do_swap

ascending_sort:
    cmpq  %rbx, %rax        # compare array[j+1] and array[j] for ascending
    jle   no_swap           # if array[j+1] >= array[j], no swap needed

do_swap:
    # Swap array[j] and array[j + 1]
    movq  %rax, 8(%r8,%r13,8) # store array[j] in array[j+1]
    movq  %rbx, (%r8,%r13,8)  # store array[j+1] in array[j]

no_swap:
    incl  %r13d            # increment inner loop counter j
    jmp   inner_loop       # repeat inner loop

end_inner_loop:
    incl  %r11d            # increment outer loop counter i
    jmp   outer_loop       # repeat outer loop

end_outer_loop:
    leave
    ret

