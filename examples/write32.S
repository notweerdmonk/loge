    .global write32

    .data
buffer:
    .space 0x100

    .text
write32:
    push    %rdi
    push    %rdx
    movq    $buffer, %rdi
    xor     %rax, %rax
    call    strncpy
    pop     %rdx
    pop     %rdi
    movq    $buffer, %rsi
    leaq    do_write32, %rax
    movq    $0x2300000000, %rcx # compatibility mode CS
    orq     %rcx, %rax
    pushq   %rax
    retfl

do_write32:
    xor     %eax, %eax
    movl    $4, %eax
    movl    %edi, %ebx
    movl    %esi, %ecx
    int     $0x80

    xor     %eax, %eax
    movl    $1, %eax
    xor     %ebx, %ebx
    int     $0x80
