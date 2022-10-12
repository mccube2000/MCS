# MCS
A Modular Customized System(Operating System).

## Todo List

### kernel
* [ ] hash map
* [ ] TSS处理
* [ ] 任务结构
* [ ] GDT重新规划
* [ ] 时钟中断处理
* [ ] 进程调度
* [ ] 进程结构
* [ ] 资源结构

### boot
* [ ] [暂无]

## Done List

### kernel
* [x]  日期时间处理
* [x]  设置PIT时钟
* [x]  获取RTC时间
* [x]  初始化内存
* [x]  初始化鼠标键盘
* [x]  初始化PIC
* [x]  初始化GDT IDT
* [x]  初始化调色板
* [x]  获取BIOS信息

### boot
* [x]  BIOS中断e820内存地图获取
* [x]  拓展引导区
* [x]  内存规划
* [x]  分页模式
* [x]  读取硬盘数据到内存
* [x]  设置GDT
* [x]  32位模式
* [x]  切换显示模式
* [x]  通过BIOS中断获取部分信息
