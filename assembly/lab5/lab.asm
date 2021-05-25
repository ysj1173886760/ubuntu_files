dseg segment
data1 dw -30
data2 dw -15
sum dw 0
parameter dw 0
dseg ends

sseg segment stack
sktop db 20 dup(0)
sseg ends

cseg segment
    assume cs: cseg, ds:dseg, ss:sseg

abs_p proc near
    mov bx, word ptr [parameter]
    mov ax, word ptr [bx]
    cmp ax, 0
    jge res
    neg ax
    mov word ptr [bx], ax
res: ret
abs_p endp

sum_p proc near
    mov ax, word ptr [data1]
    add ax, word ptr [data2]
    mov word ptr[sum], ax
    ret
sum_p endp

start:
    mov ax, dseg
    mov ds, ax
    mov ax, sseg
    mov ss, ax
    mov word ptr [parameter], offset data1
    call abs_p
    mov word ptr [parameter], offset data2
    call abs_p
    call sum_p
result:
    mov ah, 4ch
    mov al, 0
    int 21h

cseg ends
end start