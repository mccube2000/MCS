# MCS
A Modular Customized System(Operating System).

## 构建&运行
构建：
```bash
scons build
```

运行：
```bash
scons --run vhd
```

调试：
```bash
scons --run vhd-debug
```

## Todo List

### bug
* [ ] [暂无]

### kernel
* [ ] 线程实现
* [ ] 单链表
* [ ] 线程调度
* [ ] TSS处理
* [ ] 用户态切换
* [ ] GDT重新规划
* [ ] hash map

### boot
* [ ] [暂无]

## Done List

### kernel
* [x] 资源结构
* [x] 项目结构
* [x] 任务切换
* [x] 任务结构
* [x] 时钟中断处理
* [x] 日期时间处理
* [x] 设置PIT时钟
* [x] 获取RTC时间
* [x] 初始化内存
* [x] 初始化鼠标键盘
* [x] 初始化PIC
* [x] 初始化GDT IDT
* [x] 初始化调色板
* [x] 获取BIOS信息

### boot
* [x] BIOS中断e820内存地图获取
* [x] 拓展引导区
* [x] 内存规划
* [x] 分页模式
* [x] 读取硬盘数据到内存
* [x] 设置GDT
* [x] 32位模式
* [x] 切换显示模式
* [x] 通过BIOS中断获取部分信息
