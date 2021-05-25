dseg segment
data dw 1, 2, 4, -3, 0, 7, -5, 0, -4
count dw 8
stack dw 20 dup(0)
dseg ends

sseg segment 
sktop db 20 dup(0)
sseg ends

cseg segment
    assume cs:cseg, ds:dseg, ss:sseg
start: 
    mov ax, dseg ;初始化
    mov ds, ax
    mov ax, sseg
    mov ss, ax
    
    mov word ptr [stack], 0000H ;i
    mov word ptr [stack + 2], 0000H ;pos sum
    mov word ptr [stack + 4], 0000H ;pos count
    mov word ptr [stack + 6], 0000H ;neg sum
    mov word ptr [stack + 8], 0000H ;neg count
    jmp compare

doit:
    mov bx, word ptr [stack]
    add bx, word ptr [stack]
    mov ax, word ptr [data + bx]
    cmp ax, 0
    jle smaller
    mov bx, word ptr [stack]
    add bx, word ptr [stack]
    mov ax, word ptr [data + bx]
    add word ptr [stack + 2], ax
    add word ptr [stack + 4], 1
    jmp update

smaller:
    mov bx, word ptr [stack]
    add bx, word ptr [stack]
    mov ax, word ptr [data + bx]
    cmp ax, 0
    jge update
    mov bx, word ptr [stack]
    add bx, word ptr [stack]
    mov ax, word ptr [data + bx]
    add word ptr [stack + 6], ax
    add word ptr [stack + 8], 1

update:
    add word ptr [stack], 1

compare:
    mov ax, word ptr [count]
    cmp word ptr [stack], ax 
    jle doit

result:
    mov ah, 4ch
    mov al, 0
    int 21h

cseg ends
end start
