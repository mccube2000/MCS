[bits 32]

    global	_io_stihlt
    global	_io_in8,  _io_in16,  _io_in32
    global	_io_out
    global	_io_load_eflags, _io_store_eflags
    global	_load_gdtr, _load_idtr
    global	_load_cr0, _store_cr0, _load_cr2
    global	_load_tr, _save_context, _load_context
    global	_asm_page_fault, _asm_int_pit, _asm_int_keyboard, _asm_inthandler27, _asm_int_mouse
    extern	_page_fault, _int_pit, _int_keyboard, _inthandler27, _int_mouse

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

; _asm_int_pit:
;     PUSH	ES
;     PUSH	DS
;     PUSHAD
;     MOV		EAX,ESP
;     PUSH	EAX
;     MOV		AX,SS
;     MOV		DS,AX
;     MOV		ES,AX
;     CALL	_int_pit
;     POP		EAX
;     POPAD
;     POP		DS
;     POP		ES
;     IRETD

_asm_int_pit:
    pushad
    push ds
    push es
    push fs
    push gs
    mov eax, esp
    push eax

    mov ax, ss
    mov ds, ax
    mov es, ax

    call _int_pit

    pop eax                         ; 取出esp
    mov edi, [eax + 4 * 7]          ; 获取接下来popad时会被忽略的esp作为目标栈
    mov esi, esp                    ; 将iretd需要的参数复制到目标栈
    add esi, 0x30
    mov ecx, 4 * 3                  ; 复制3个
    rep movsb

    pop gs
    pop fs
    pop es
    pop ds
    popad ; popad会忽略esp，不会pop esp，只是 + 4
    mov esp, [esp - 4 * 5]
    iretd

_asm_int_keyboard:
    PUSH		ES
    PUSH		DS
    PUSHAD
    MOV		EAX,ESP
    PUSH		EAX
    MOV		AX,SS
    MOV		DS,AX
    MOV		ES,AX
    CALL	_int_keyboard
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

_asm_int_mouse:
    PUSH	ES
    PUSH	DS
    PUSHAD
    MOV		EAX,ESP
    PUSH	EAX
    MOV		AX,SS
    MOV		DS,AX
    MOV		ES,AX
    CALL	_int_mouse
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
