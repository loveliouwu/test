#嵌入式Linux系统移植

####总线
按照相对于CPU的位置，总线可分为片内总线和片外总线
    片内：连接CPU内部个主要功能部件
    片外：CPU与存储器和I/O接口之间进行信息交换的通道
按功能分为数据总线、地质总线、控制总线

- 总线带宽：
    总线带宽 = （总线宽度/8） *  总线频率


- 对于ARM系列的处理器，定义了3种片内总线
    AHB：用于高性能、高时钟的系统模块连接，支持处理器、片上储存器、片外存储器以及低功耗外设宏功能单元之间的有效连接。
    ASB：用在并不要求AHB的高性能特征的地方
    APB：用于较低性能外设备的简单连接，一般是节在AHB或ASB系统纵向上的第二级总线。

- 存储器
    典型的嵌入式系统存储结构包括芯片内部的缓存（cache）、随机存储器（ARM）、只读存储器（ROM）、芯片外部的主存（Nor Flash、SRAM、DRAM）和外存（Nand Flash、SD）
    - cache:全部集成在嵌入式微处理器内，分为数据cache、指令cache、或混合cache
    - 主存：处理器能直接访问的存储器，用来存放系统和用户的程序和数据，嵌入式系统的主存可位于处理器内或者外，片内存储器存储容量小、速度快，片外存储器容量大，但速度稍慢，可以旋作主存的设备包括：Nor Flash、E2PROM等只读存储器和SRAM、DRAM、SDRAM等随机访问存储器。
    - 外存：外存是处理器不能直接访问的存储器，用来存放用户的各种信息，容量大，速度相对主存较慢，但它可以用来长期保存用户信息，在嵌入式系统中常用的外存有：Nand Flash、DOC、CF、SD、MMC
    - Nand Flash是闪存的一种，可独立成为外存，也可以组成其他各种类型的电子表，如USB盘，CF、SD、MMC等，与另一种Nor Flash相比，Nand Flash具有容量大，回写速度快的特点，因此主要用于外存，而Nor Flash读取速度快，稳定性更高，更适合作为主存。
