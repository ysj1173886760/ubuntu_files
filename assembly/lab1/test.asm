dseg segment
data1 db 13H,26H
data2 dw 0
dseg ends
sseg segment stack
sktop db 20 dup(0)
cseg segment
    assume cs:cseg, ds:dseg
    assume ss:sseg
start:  mov ax, dseg
        mov ds, ax
        mov ax, sseg
        mov ss, ax
        mov sp, length sktop
        mov al, data1
        add al, data1 + 1
        mov byte ptr data2, al
        mov ah, 4CH
        int 21H
cseg    ends
        end start
