core_base_address   equ 0x00040000  ; 内核加载的起始内存地址
core_start_sector   equ 0x00000001  ; 内核的起始逻辑扇区号
sector_count        equ 100         ; 加载扇区数量
cyls                equ 0x0ff0      ; 引导扇区设置
leds                equ 0x0ff1      ; led灯
vmode               equ 0x0ff2      ; 关于颜色的信息
scrnX               equ 0x0ff4      ; 屏幕X分辨率
scrnY               equ 0x0ff6      ; 屏幕Y分辨率
vram                equ 0x0ff8      ; 图像缓冲区的起始地址

; 显示模式
; 0x100       ;  640 x  400 x 8bit 彩色
; 0x101       ;  640 x  480 x 8bit 彩色
; 0x103       ;  800 x  600 x 8bit 彩色
; 0x105       ; 1024 x  768 x 8bit 彩色
; 0x107       ; 1280 x 1024 x 8bit 彩色
vbe_mode            equ 0x105       ; 1024 x  768 x 8bit 彩色

section mbr vstart=0x7c00
; 初始化寄存器
    mov ax, cs
    mov sp, 0x7c00

; 保存led信息
    mov ah, 0x02
    int 0x16                        ; keyboard BIOS
    mov [leds], al

; 设置VBE显示模式
    ; 检查VBE
    mov ax, 0x9000
    mov es, ax
    mov di, 0
    mov ax, 0x4f00
    int 0x10
    cmp ax, 0x004f
    jne scrn320
    ; je scrn320
    
    ; 检查VBE的版本
    mov ax, [es: di + 4]
    cmp ax, 0x0200
    jb scrn320                      ; if (AX < 0x0200) goto scrn320

    ; 取得画面模式信息
    mov cx, vbe_mode
    mov ax, 0x4f01
    int 0x10
    cmp ax,0x004f
    jne scrn320

    ; 画面模式信息的确认
    cmp byte [es: di + 0x19], 8     ; 颜色数必须为8
    jne scrn320
    cmp byte [es: di + 0x1b], 4     ; 颜色的指定方法必须为4(4是调色板模式)
    jne scrn320
    mov ax, [es: di]                ; 模式属性bit7不是1就不能加上0x4000
    and ax, 0x0080
    jz scrn320                      ; 模式属性的bit7是0，所以放弃

    ; 画面设置
    mov bx, vbe_mode + 0x4000
    mov ax, 0x4f02
    int 0x10
    mov byte [vmode], 8
    mov ax, [es: di + 0x12]
    mov [scrnX], ax
    mov ax, [es: di + 0x14]
    mov [scrnY], ax
    mov eax, [es: di + 0x28]
    mov [vram], eax
    jmp start

; VGA显卡，320 x 200 x 8bit
scrn320:
    ; 保存显示信息
    mov byte [vmode], 8
    mov word [scrnX], 320
    mov word [scrnY], 200
    mov dword [vram], 0xa0000

    ; 转化图像模式
    mov al, 0x13
    mov ah, 0x00
    int 0x10

start:

; GDT
    lgdt [gdt]

; A20
    in al, 0x92                     ; 南桥芯片内的端口
    or al, 0000_0010b
    out 0x92, al                    ; 打开A20

; 关闭中断
    cli                             ; 还未进行中断处理,暂时关闭中断

; PE
    mov eax, cr0
    or eax, 1
    mov cr0, eax                    ; 设置PE位

; 进入保护模式
    ; 清流水线并串行化处理器
    jmp dword 0x0008: flush         ; 16位的描述符选择子：32位偏移, 选择GDT对应的第1项

[bits 32]

flush:
    mov eax, 0x0010                 ; 加载数据段(4GB)选择子, 选择GDT对应的第2项
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax                     ; 加载堆栈段(4GB)选择子
    mov esp, 0x7c00                 ; 堆栈指针


; 加载内核至内存
    mov ecx, 100                     ; 32位模式下的LOOP使用ECX，读取扇区数量
    mov eax, core_start_sector
    mov ebx, core_base_address      ; 起始地址
    @2:
        call read_hard_disk_0
        inc eax
        loop @2                     ; 循环读

; 准备页表，一级页表
    mov ebx, 0x00010000             ; 页目录表PDT的物理地址
    mov edx, ebx
    or edx, 0x00000003
    mov [ebx + 0xffc], edx          ; 对应页目录表自己的目录项

    mov ecx, 0x3ff
    mov eax, 0x40f003
    sub ebx, 4                      ; 页目录表PDT的物理地址 - 4
    cpd:
        mov [ebx + ecx * 4], eax    ; 从第0项到第0x3fe项
        sub eax, 0x1000
        loop cpd

    mov ebx, 0x00011000             ; 初始化第一个目录项，对应0-0x3ff_fff物理内存
    mov ecx, 0x00000003
    .b0
        xor eax, eax
        xor esi, esi
        .b1:
            mov edx, eax
            or edx, ecx
            mov [ebx + esi * 4], edx;登记页的物理地址
            add eax, 0x1000         ; 下一个相邻页的物理地址
            inc esi
            cmp esi, 0x400          ; 仅低端1MB内存对应的页才是有效的
            jl .b1
    cmp ecx, 0x00000003             ; 显存页初始化完成后，跳转至pgo
    jne pgo
    mov ebx, 0x00391000             ; 显存对应页
    mov ecx, 0xe0000003             ; 显存物理地址0xe0000_000
    jmp .b0

pgo:
    ; 令CR3寄存器指向页目录，并正式开启页功能 
    mov eax, 0x00010000             ; PCD = PWT = 0
    mov cr3, eax

         ;将GDT的线性地址映射到从0x80000000开始的相同位置 
        ;  sgdt [gdt]
        ;  mov ebx,[gdt+2]
        ;  add dword [gdt+2],0x80000000      ;GDTR也用的是线性地址
        ;  lgdt [gdt]

    mov eax,cr0
    or eax,0x80000000
    mov cr0,eax                        ;开启分页机制

    jmp core_base_address
    hh:
        hlt
        jmp hh
read_hard_disk_0:
    ; EAX=逻辑扇区号
    ; DS:EBX=目标缓冲区地址
    ; 返回：EBX=EBX+512
    push eax
    push ecx
    push edx

    push eax
    
    mov dx, 0x1f2
    mov al, 1
    out dx, al          ; 读取的扇区数

    inc dx              ; 0x1f3
    pop eax
    out dx, al          ; LBA地址7~0

    inc dx              ; 0x1f4
    mov cl, 8
    shr eax, cl
    out dx, al          ; LBA地址15~8

    inc dx              ; 0x1f5
    shr eax, cl
    out dx, al          ; LBA地址23~16

    inc dx              ; 0x1f6
    shr eax, cl
    or al, 0xe0         ; 第一硬盘  LBA地址27~24
    out dx, al

    inc dx              ; 0x1f7
    mov al, 0x20        ; 读命令
    out dx, al

    .waits:
        in al, dx
        and al, 0x88
        cmp al, 0x08
        jnz .waits      ; 不忙，且硬盘已准备好数据传输 

    mov ecx, 256        ; 总共要读取的字数
    mov dx, 0x1f0

    .readw:
        in ax, dx
        mov [ebx], ax
        add ebx, 2
        loop .readw

    pop edx
    pop ecx
    pop eax

    ret

gdt:
    dw     3 * 8 - 1
    dd     gdt0                     ; GDT的物理/线性地址

gdt0:
    dd      0, 0
    dd      0x0000ffff, 0x00cf9a00  ; 代码段
    dd      0x0000ffff, 0x00cf9200  ; 数据段
    dd      0, 0

times 510-($-$$) db 0
db 0x55, 0xaa