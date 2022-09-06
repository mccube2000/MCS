core_base_address   equ 0x00040000  ;常数，内核加载的起始内存地址 
core_start_sector   equ 0x00000001  ;常数，内核的起始逻辑扇区号 
cyls                equ 0x0ff0      ; 引导扇区设置
leds                equ 0x0ff1      ; led灯
vmode               equ 0x0ff2      ; 关于颜色的信息
scrnX               equ 0x0ff4      ; 屏幕X分辨率
scrnY               equ 0x0ff6      ; 屏幕Y分辨率
vram                equ 0x0ff8      ; 图像缓冲区的起始地址

section mbr vstart=0x7c00
; 初始化寄存器
    mov ax, cs
    mov ss, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7c00

; 保存led信息
    mov ah, 0x02
    int 0x16                        ; keyboard BIOS
    mov [leds], al

; VGA显卡，320x200x8bit
    ; 保存显示信息
    mov byte [vmode], 8
    mov word [scrnX], 320
    mov word [scrnY], 200
    mov dword [vram], 0xa_0000

    ; 转化图像模式
    mov al, 0x13
    mov ah, 0x00
    int 0x10

    ; mov byte [vmode], 16
	; mov word [scrnX], 80
	; mov word [scrnY], 25
    ; mov dword [vram], 0xb_8000

; 设置GDT
    ; 计算GDT所在的逻辑段地址
    mov eax, [cs: pgdt + 0x02]      ; GDT的32位物理地址 
    xor edx, edx
    mov ebx, 16
    div ebx                         ; 分解成16位逻辑地址 

    mov ds, eax                     ; 令DS指向该段以进行操作
    mov ebx, edx                    ; 段内起始偏移地址 

    ; 跳过0#号描述符的槽位

    ; 创建1#描述符，保护模式下的代码段描述符
    mov dword [ebx + 0x08], 0x0000_ffff         ; 基地址为0，界限0xFFFFF，DPL=00
    mov dword [ebx + 0x0c], 0x00cf_9800         ; 4KB粒度，代码段描述符，向上扩展

    ; 创建2#描述符，保护模式下的数据段和堆栈段描述符 
    mov dword [ebx + 0x10], 0x0000_ffff         ; 基地址为0，界限0xFFFFF，DPL=00
    mov dword [ebx + 0x14], 0x00cf_9200         ; 4KB粒度，数据段描述符，向上扩展

    ; 初始化描述符表寄存器GDTR
    mov word [cs: pgdt], 23         ; 描述符表的界限

    lgdt [cs: pgdt]

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
    jmp dword 0x0008: flush         ; 16位的描述符选择子：32位偏移

[bits 32]

flush:
    mov eax, 0x00010                ; 加载数据段(4GB)选择子
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax                     ; 加载堆栈段(4GB)选择子
    mov esp, 0x7000                 ; 堆栈指针

;以下加载系统核心程序
    mov edi, core_base_address
    mov eax, core_start_sector
    mov ebx, edi                    ; 起始地址
    call read_hard_disk_0           ; 以下读取程序的起始部分（一个扇区）

    ; 以下判断整个程序有多大
    ; mov eax, [edi]                ; 核心程序尺寸
    mov eax, 102400                 ; 核心程序尺寸102400
    xor edx, edx
    mov ecx, 512                    ; 512字节每扇区
    div ecx

    or edx, edx
    jnz @1                          ; 未除尽，因此结果比实际扇区数少1
    dec eax                         ; 已经读了一个扇区，扇区总数减1
    @1:
        or eax, eax                 ; 考虑实际长度≤512个字节的情况
        jz pge                      ; EAX=0 ?

    ; 读取剩余的扇区
    mov ecx, eax                    ;32位模式下的LOOP使用ECX
    mov eax,core_start_sector
    inc eax                         ; 从下一个逻辑扇区接着读
    @2:
        call read_hard_disk_0
        inc eax
        loop @2                     ; 循环读，直到读完整个内核

; 开启分页
    pge:
        ; 创建系统内核的页目录表PDT
        mov ebx, 0x00020000         ; 页目录表PDT的物理地址

        ; 在页目录内创建指向页目录表自己的目录项
        mov dword [ebx + 4092], 0x00020003
        mov edx, 0x00021003         ; MBR空间有限，后面尽量不使用立即数

        ; 在页目录内创建与线性地址0x00000000对应的目录项
        mov [ebx + 0x000], edx      ; 写入目录项（页表的物理地址和属性）      
                                    ; 此目录项仅用于过渡。

        ; 在页目录内创建与线性地址0x80000000对应的目录项
        mov [ebx + 0x800], edx      ;写入目录项（页表的物理地址和属性）

        ;创建与上面那个目录项相对应的页表，初始化页表项 
        mov ebx, 0x00021000         ; 页表的物理地址
        xor eax, eax                ; 起始页的物理地址 
        xor esi, esi

    .b1:
        mov edx, eax
        or edx, 0x00000003             
        mov [ebx + esi * 4], edx    ; 登记页的物理地址
        add eax, 0x1000             ; 下一个相邻页的物理地址 
        inc esi
        cmp esi, 256                ; 仅低端1MB内存对应的页才是有效的 
        jl .b1

    ; 令CR3寄存器指向页目录，并正式开启页功能 
    mov eax, 0x00020000             ; PCD = PWT = 0
    mov cr3, eax

    ; 将GDT的线性地址映射到从0x80000000开始的相同位置 
    sgdt [pgdt]
    mov ebx, [pgdt + 2]
    add dword [pgdt + 2], 0x80000000; GDTR也用的是线性地址
    lgdt [pgdt]

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax                    ; 开启分页机制

    ;将堆栈映射到高端，这是非常容易被忽略的一件事。应当把内核的所有东西
    ;都移到高端，否则，一定会和正在加载的用户任务局部空间里的内容冲突，
    ;而且很难想到问题会出在这里。 
    add esp, 0x80000000

    jmp 0x80040004

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

pgdt dw 0
     dd 0x00008000                  ; GDT的物理/线性地址

times 510-($-$$) db 0
db 0x55, 0xaa
