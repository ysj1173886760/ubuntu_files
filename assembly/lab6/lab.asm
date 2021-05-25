dseg segment
data1 db 0
data2 db 0
return db 13, 10, '$'
dseg ends

sseg segment stack
sktop db 20 dup(0)
sseg ends

cseg segment
    assume cs:cseg, ds:dseg, ss:sseg

print proc near
    cmp al, 10
    js one
    mov bl, 10
    sub ah, ah
    div bl
    mov bl, ah
    mov dl, al
    or dl, 030H
    mov ah, 02H
    int 21h
    mov al, bl
one:
    or al, 030H
    mov dl, al
    mov ah, 02h
    int 21h
    call print_return
    ret
print endp

print_return proc near
    mov ah, 09h
    mov dx, offset return
    int 21h
    ret
print_return endp

start:
    mov ax, dseg
    mov ds, ax
    mov ax, sseg
    mov ss, ax

    mov ah, 01h
    int 21h
    and ax, 000fh
    mov byte ptr [data1], al
    
    call print_return

    mov ah, 01h
    int 21h
    and ax, 000fh
    mov byte ptr [data2], al

    call print_return

    ; +
    mov al, byte ptr [data1]
    add al, byte ptr [data2]
    call print

    ; -
    mov al, byte ptr [data1]
    sub al, byte ptr [data2]
    call print

    ; *
    mov al, byte ptr [data1]
    mul byte ptr [data2]
    call print

    ; /
    sub ah, ah
    mov al, byte ptr [data1]
    div byte ptr [data2]
    call print
    sub ah, ah
    mov al, byte ptr [data1]
    div byte ptr [data2]
    mov al, ah
    call print

    mov ah, 4ch
    mov al, 0
    int 21h
cseg ends
end start