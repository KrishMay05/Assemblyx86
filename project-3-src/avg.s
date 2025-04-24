.data
        .comm   a, 8              # long a;
        .comm   avg, 8            # long avg;
        .comm   forcon, 8         # long forcon;
        .comm   sum, 8            # long sum;
        .comm   num, 8

        .text
format1:
        .string "n?"

format2:
        .string "%ld"

format3:
        .string "SUM=%ld\n"

format4:
        .string "AVG=%ld\n"

format5:
        .string "TEST=%ld\n"      # Fixed newline here

format6:
        .string "FOR=%ld\n"      # Fixed newline here


.globl main
main:                           # main()
                                #
        pushq   %rbp            # Save frame pointer
        movq    %rsp, %rbp      #
                                #
        movq    $format1, %rdi  #   printf("n?");
        movq    $0, %rax        #
        call    printf          #
                                #
        movq    $format2, %rdi  #   scanf("%ld", &a); (for n)
        movq    $a, %rsi        #
        movq    $0, %rax        #
        call    scanf           #

        movq    a, %rdi         #
        movq    %rdi, forcon    #
                                #
        movq    $0, sum         #
        movq    $0, %rcx        # Use %rcx (64-bit register)


        movq    $format6, %rdi  #
        movq    forcon, %rsi      # Fix the reference here
        movq    $0, %rax        #
        call    printf          #

        # Loop to read numbers and calculate sum
        forloop:
        movq    num, %rdi         # Move sum into register %rdi
        movq    forcon, %rsi      # Move forcon into register %rsi
        cmpq    %rsi, %rdi        # Compare sum with forcon
        jge     endforloop        #

        movq    $format2, %rdi  #
        movq    $a, %rsi        #
        movq    $0, %rax        #
        call    scanf

        movq    a, %rdi         #
        addq    %rdi, sum       #
        incq    num

        movq    $format5, %rdi  #
        movq    num, %rsi      # Fix the reference here
        movq    $0, %rax        #
        call    printf          #

        jmp     forloop         #

        endforloop:

        # Print SUM
        movq    $format3, %rdi  #
        movq    sum, %rsi       #
        movq    $0, %rax        #
        call    printf          #

        # Calculate AVG (sum/num)
        movq    sum, %rax       # Move sum into %rax for division
        cqto                    # Sign extend %rax into %rdx
        idivq   num             # Divide sum by num
        movq    %rax, avg       # Store the result (avg)

        # Print AVG
        movq    $format4, %rdi  #
        movq    avg, %rsi       #
        movq    $0, %rax        #
        call    printf          #

        leave                   # Restore frame pointer
        ret                     # Return from main

