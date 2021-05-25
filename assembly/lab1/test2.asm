dseg segment
data1 db 12H, -10H
data2 dw 12H, -10H
data3 dd 12H, -10H
data4 db 'ABCD'
data5 dw 'AB', 'EF'
data6 dd 'AB'
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
