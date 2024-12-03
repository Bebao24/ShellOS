[bits 64]

global LoadGDT
LoadGDT:
    lgdt [rdi]

    ; Reload data segment
    mov ax, 0x10
    mov es, ax
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    pop rdi
    mov rax, 0x08 ; Code segment
    push rax
    push rdi
    retfq

