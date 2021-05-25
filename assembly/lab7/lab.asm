dseg segment
text db "can a canner use a can can a can$"
string db "can$"
count db 0
parameter1 db 0
dseg ends

sseg segment stack
sktop db 20 dup(0)
sseg ends

cseg segment
    assume cs:cseg, ds:dseg, ss:sseg

check proc near
    mov si, byte ptr [parameter1]
    mov di, 0
    jmp compare
l1:
    mov al, byte ptr [text + si]
    cmp al, '$'
    je return
    cmp al, byte ptr [string + di]
    jne return
    inc si
    inc di
compare: 
    mov al, byte ptr [string + di]
    cmp al, '$'
    jne l1
    add byte ptr [count], 1
return:
    ret
check endp

start:
    mov ax, dseg
    mov ds, ax
    mov ax, sseg
    mov ss, ax

    mov bx, 0

l2:
    mov si, bx
    mov al, byte ptr [text + si]
    cmp al, '$'
    je break
    cmp al, byte ptr [string]
    jne not_eq
    mov byte ptr [parameter1], si
    call check
not_eq:
    inc bx
    jmp l2

break:
    mov ah, 4ch
    mov al, 0
    int 21h
cseg ends
end start