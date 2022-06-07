bootloader
	bootloader是BIOS启动后，首先执行的磁盘程序，这个程序负责加载真正的操作系统，由于它的这个职能
	它可以为内核传递参数，可以管理多个操作系统的启动，可以查看基本的硬件信息，可以识别分区，可以操作磁盘
	
	目前常见的Bootloader有grub和uboot
	
linux内核的生成：
	内核的编译都是先进入各个目录，生成built-in.o ，然后在上层根据一定规则组合生成vmlinux（例如
	/arch/arm/kernel/vmlinux.lds）然后经过处理压缩得到最终文件
		例如使用arm-linux-gnu-ld-o vmlinux -T arch/arm/kernel/vmlinux.lds生成vmlinux是未压缩，带调试信息、
		符号表的最初的内核vmlinux文件。
		
Linux系统的启动
	一个linux系统永远由bootloader、kernel、文件系统三元素组成
	有的嵌入式linux经过适当的调整可以不需要bootloader
	文件系统至少要有一个，一般的要有两个 一个是initrd，另一个是实际运行的根文件系统
	
initrd文件系统：
	启动过程的过度根文件系统可以使initrd也可以是initramfs这两种格式的文件体系，这两个根文件系统内容一样，组织方式不一样
	内核启动的initrd里的第一个程序是/init，这是内核写死的。想要改变也可以，修改内核代码。我们说一种典型情况。这个init程序是一个脚本文件，任务流程如下：
	l  建立一个sysroot根目录。该目录用于挂载之后要启动的真实文件系统
	l  设置一下命令的环境变量，如此后面的命令都可以在环境变量指定的目录中搜索到了
	l  mount proc文件系统打破/proc，这是linux内核动态状态的一个表现和设置入口
	l  mount sysfs 到/sys，这是linux内核资源的有组织的表现和设置入口
	l  mount devtmpfs到/dev，这是临时表征当前设备节点的文件系统，可以加速系统的启动（/dev目录对当前用户程序的运行至关重要）
	l  准备一下/dev目录中的一些节点（例如stdin、stdout、stderr等fd，这里的fd是文件描述符的意思。对内核中已有的静态节点进行创建）
	l  提供为内核输入额外参数的机会
	l  解析内核启动参数，并执行（例如在udev开始之前执行一些准备），由此可以看出内核参数并不一定都是给内核来解析的
	l  启动systemd-udevd，并配置其要响应的行为
	l  然后循环处理$hookdir/initqueue下的任务
	l  mount根文件系统
	l  找到根文件系统的init程序
	l  停止systemd-udevd服务程序
	l  做一些清理操作
	l  切换到root的init程序，启动完成
	 
	可以看出整个initrd文件系统存在的目的就是挂载根文件系统。所以当根文件系统内核可以直接挂载的时候就不需要了。
	但是现在越来越复杂的网络和设备，已经让这个initrd的存在是必要的了。但是可以看出，嵌入式系统是万万不需要这个东西的。
	典型的，scsi内核就很难识别，但是现在的存储设备已经很多是scsi了。但是也可以将scsi编译进内核来让内核可以直接识别。
			 
Linux内核启动顺序：
	bootloader将内核加载到内核后，需要将控制权交给内核，第一步是要解压内核，由于内核是自解压的，解压的入口相关文件是
	arch/arm/boot/compressed/head.S完成解压之后就是搬运，因为解压并不把内核放在其最终执行的位置，移动之前可能还要做一些保存
	可能被其覆盖的代码的搬运工作。
	第二阶段从\arch\arm\kernel\head.S开始，是内核的实际功能地点，完成的工作有：cpu ID检查，machine ID(也就是开发板ID)检查，
	创建初始化页表，设置C代码运行环境，跳转到内核第一个真正的C函数startkernel开始执行。
    第三阶段从start_kernel开始，完全是C语言了。其首先用大内核锁锁住内核保证独占，然后调用平台相关的初始化操作
	（arch/arm/kernel/setup.c里的setup_arch()），在这里内核启动后可使用的所有内存被初始化，所以如果要预留不被内核使用的内存空间，
	应该在这里预留。还被初始化的有页表结构、MMU、中断、内存区域、计时器、slab、vfs等。然后跳到init内核进程（不是用户空间的init进程）
	
	init_task是进程0使用的进程描述符，也是Linux系统中第一个进程描述符，该进程的描述符在arch/arm/kernel/init_task.c。init_task是Linux内核中的
	第一个线程（0号进程），它贯穿于整个Linux系统的初始化过程中，该进程也是Linux系统中唯一一个没有用kernel_thread()函数创建的进程！
	在init_task进程执行后期，它会调用kernel_thread()函数创建第一个核心进程kernel_init，同时init_task进程继续对Linux系统初始化。
	在完成初始化后，init_task会退化为cpu_idle进程，当Core 0的就绪队列中没有其它进程时，该进程将会获得CPU运行。新创建的1号进程kernel_init
	将会逐个启动次CPU,最后寻找文件系统中的init程序，将其替换为自身，变成用户态的第一个进程。
	
module_init(init);
module_exit(exit);
        就构成了一个模块。容易看出来module_init和module_exit就是注册钩子函数的调用。
		模块内部定义的钩子函数式static的，目的是只是内部可见。__init和__exit是gcc的特性。作为一个编译器，
		提供回收明确表示无用代码的能力是很好的功能。标记为__init的函数会被放入.init.text代码段，
		这个段在模块加载完后会被回收节省内存，因为不会再用到。
		
模块的加载和卸载
       模块机制存在的意义就是动态加载与卸载，原则上内核模块在被使用的过程中不可以被卸载，但也可以强制。
	   而加载的时候也必须保证模块与运行中内核的相容。insmod、rmmod分别是约定的用户端加载和卸载模块的命令。
	   
