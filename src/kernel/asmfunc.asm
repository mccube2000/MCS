[bits 32]

    global	_io_stihlt
    global	_io_in8,  _io_in16,  _io_in32
    global	_io_out
    global	_io_load_eflags, _io_store_eflags
    global	_load_gdtr, _load_idtr
    global	_load_cr0, _store_cr0, _load_cr2
    global	_load_tr, _save_context, _load_context
    global	_asm_page_fault, _asm_inthandler20, _asm_inthandler21, _asm_inthandler27, _asm_inthandler2c
    extern	_page_fault, _inthandler20, _inthandler21, _inthandler27, _inthandler2c

SECTION .text

_io_stihlt:	; void io_stihlt(void);
    STI
    HLT
    RET

_io_in8:	; int io_in8(int port);
    MOV		EDX,[ESP+4]		; port
    MOV		EAX,0
    IN		AL,DX
    RET

_io_in16:	; int io_in16(int port);
    MOV		EDX,[ESP+4]		; port
    MOV		EAX,0
    IN		AX,DX
    RET

_io_in32:	; int io_in32(int port);
    MOV		EDX,[ESP+4]		; port
    IN		EAX,DX
    RET

_io_out:	; void io_out8(int port, int data);
    MOV		EDX,[ESP+4]		; port
    MOV		AL,[ESP+8]		; data
    OUT		DX,AL
    RET

_io_load_eflags:	; int io_load_eflags(void);
    PUSHFD		; PUSH EFLAGS 
    POP		EAX
    RET

_io_store_eflags:	; void io_store_eflags(int eflags);
    MOV		EAX,[ESP+4]
    PUSH	EAX
    POPFD		; POP EFLAGS 
    RET

_load_gdtr:		; void load_gdtr(int limit, int addr);
    MOV		AX,[ESP+4]		; limit
    MOV		[ESP+6],AX
    LGDT	[ESP+6]
    RET

_load_idtr:		; void load_idtr(int limit, int addr);
    MOV		AX,[ESP+4]		; limit
    MOV		[ESP+6],AX
    LIDT	[ESP+6]
    RET

_load_cr0:		; int load_cr0(void);
    MOV		EAX,CR0
    RET

_load_cr2:		; int load_cr2(void);
    MOV		EAX,CR2
    RET

_store_cr0:		; void store_cr0(int cr0);
    MOV		EAX,[ESP+4]
    MOV		CR0,EAX
    RET

_load_tr:		; void load_tr(int tr);
    LTR		[ESP+4]			; tr
    RET

_asm_page_fault:
    PUSH		ES
    PUSH		DS
    PUSHAD
    MOV		EAX,ESP
    PUSH		EAX
    MOV		AX,SS
    MOV		DS,AX
    MOV		ES,AX
    CALL	_page_fault
    POP		EAX
    POPAD
    POP		DS
    POP		ES
    add esp, 4
    IRETD

; _asm_inthandler20:
;     PUSH	ES
;     PUSH	DS
;     PUSHAD
;     MOV		EAX,ESP
;     PUSH	EAX
;     MOV		AX,SS
;     MOV		DS,AX
;     MOV		ES,AX
;     CALL	_inthandler20
;     POP		EAX
;     POPAD
;     POP		DS
;     POP		ES
;     IRETD

_asm_inthandler20:
    pushad
    push ds
    push es
    push fs
    push gs
    push ss
    mov eax, esp
    push eax

    mov ax, ss
    mov ds, ax
    mov es, ax

    call _inthandler20

    pop eax
    pop ss
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iretd

_asm_inthandler21:
    PUSH		ES
    PUSH		DS
    PUSHAD
    MOV		EAX,ESP
    PUSH		EAX
    MOV		AX,SS
    MOV		DS,AX
    MOV		ES,AX
    CALL	_inthandler21
    POP		EAX
    POPAD
    POP		DS
    POP		ES
    IRETD

_asm_inthandler27:
    PUSH	ES
    PUSH	DS
    PUSHAD
    MOV		EAX,ESP
    PUSH	EAX
    MOV		AX,SS
    MOV		DS,AX
    MOV		ES,AX
    CALL	_inthandler27
    POP		EAX
    POPAD
    POP		DS
    POP		ES
    IRETD

_asm_inthandler2c:
    PUSH	ES
    PUSH	DS
    PUSHAD
    MOV		EAX,ESP
    PUSH	EAX
    MOV		AX,SS
    MOV		DS,AX
    MOV		ES,AX
    CALL	_inthandler2c
    POP		EAX
    POPAD
    POP		DS
    POP		ES
    IRETD

_save_context:
	push es
    push cs
    push ss
    push ds
    push fs
    push gs
    pushad
	mov eax, cr3
    push eax
	pushfd
	mov eax, esp
	add esp, 4 * 16
	ret
    ; pushad
    ; mov eax, cr3
    ; push eax
    ; pushfd
    ; push es
    ; push cs
    ; push ss
    ; push ds
    ; push fs
    ; push gs
    ; mov ebx, [esp + 4 * 17]
    ; mov ecx, 15
    ; cpy:
    ;     mov [ebx + ecx * 4], [esp + ecx * 4]
    ;     loop cpy
    ; add esp, 4 * 16
    ; ll:
    ; jmp ll
    ; ret

_load_context:
    add esp, 4
    pop gs
    pop fs
    pop ds
    pop ss
    pop cs
    pop es
    popfd
    pop eax
    mov cr3, eax
    popad
    ; add esp, 4 * 16

    ret
