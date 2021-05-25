dseg segment
prompt_input db "input file name:$"
prompt_output db "output file name:$"
input_file db 20, 0, 20 dup(0)
output_file db 20, 0, 20 dup(0)
input_id dw 0
output_id dw 0
error_string db "file error!$"
buff db 1024 dup(0)
return db 10, 13, '$'
dseg ends

sseg segment stack
sktop db 20 dup(0)
sseg ends

cseg segment
    assume cs:cseg, ds:dseg, ss:sseg

print_string proc near
    mov ah, 09h
    int 21h
    ret
print_string endp

start:
    mov ax, dseg
    mov ds, ax
    mov ax, sseg
    mov ss, ax

    mov dx, offset prompt_input
    call print_string

    mov dx, offset input_file
    mov ah, 0ah
    int 21h

    mov dx, offset return
    call print_string

    mov bl, byte ptr [input_file + 1]
    sub bh, bh
    mov byte ptr [bx + input_file + 2], bh

    mov dx, offset input_file + 2
    mov ax, 3D00H
    int 21h
    jc error
    mov word ptr [input_id], ax

    mov dx, offset prompt_output
    call print_string

    mov dx, offset output_file
    mov ah, 0ah
    int 21h
    
    mov dx, offset return
    call print_string

    mov bl, output_file + 1
    sub bh, bh
    mov byte ptr [bx + output_file + 2], bh

    mov dx, offset output_file + 2
    mov ax, 3D01H
    int 21h
    jc error
    mov word ptr [output_id], ax

l1:
    mov bx, word ptr [input_id]
    mov cx, 1024
    mov dx, offset buff
    mov ah, 3fh
    int 21h
    jc error

    mov cx, ax
    mov bx, word ptr [output_id]
    mov dx, offset buff
    mov ah, 40h
    int 21h
    jc error
    cmp ax, 1024
    je l1
    
    mov ah, 3eh
    int 21h
    jc error
    mov bx, word ptr [input_id]
    mov ah, 3eh
    int 21h
    jc error
    jmp exit

error:
    mov dx, offset error_string
    call print_string

exit:
    mov ah, 4ch
    mov al, 0
    int 21h
cseg ends
end start