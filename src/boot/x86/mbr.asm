core_base_address   equ 0x00040000  ;常数，内核加载的起始内存地址 
core_start_sector   equ 0x00000001  ;常数，内核的起始逻辑扇区号 
cyls                equ 0x0ff0      ; 引导扇区设置
leds                equ 0x0ff1      ; led灯
vmode               equ 0x0ff2      ; 关于颜色的信息
scrnX               equ 0x0ff4      ; 屏幕X分辨率
scrnY               equ 0x0ff6      ; 屏幕Y分辨率
vram                equ 0x0ff8      ; 图像缓冲区的起始地址

%include "src/boot/x86/utils.asm"

section mbr vstart=0x7c00
; 初始化寄存器
    mov ax, cs
    mov ss, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7c00

; 保存led信息
    mov ah,0x02
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
    mov dword [ebx + 0x08], 0x0000_ffff      ; 基地址为0，界限0xFFFFF，DPL=00
    mov dword [ebx + 0x0c], 0x00cf_9800      ; 4KB粒度，代码段描述符，向上扩展

    ; 创建2#描述符，保护模式下的数据段和堆栈段描述符 
    mov dword [ebx + 0x10], 0x0000_ffff      ; 基地址为0，界限0xFFFFF，DPL=00
    mov dword [ebx + 0x14], 0x00cf_9200      ; 4KB粒度，数据段描述符，向上扩展

    ; 初始化描述符表寄存器GDTR
    mov word [cs: pgdt], 23                 ; 描述符表的界限

    lgdt [cs: pgdt]

; A20
    in al, 0x92                      ; 南桥芯片内的端口
    or al, 0000_0010b
    out 0x92, al                     ; 打开A20

; 关闭中断
    cli                             ; 还未进行中断处理,暂时关闭中断

; PE
    mov eax, cr0
    or eax, 1
    mov cr0, eax                    ; 设置PE位

; 进入保护模式
    ; 清流水线并串行化处理器
    jmp dword 0x0008:flush          ; 16位的描述符选择子：32位偏移

[bits 32]

flush:
    mov eax, 0x00010                ; 加载数据段(4GB)选择子
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax                     ; 加载堆栈段(4GB)选择子
    mov esp, 0x7000                 ; 堆栈指针

pgdt    dw 0
        dd 0x00008000     ;GDT的物理/线性地址

times   510-($-$$)  db 0
db 0x55, 0xaa
