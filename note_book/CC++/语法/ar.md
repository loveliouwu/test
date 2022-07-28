1.
        ar r : 编译成静态库或者添加到静态库
                ar r out.a test1.c  //将test1.c编译成库
                ar r out.a test2.c  //将test2.c添加到库
                或者
                ar r out.a test1.o //将test1.o编译成库
                ar r out.a test2.o //将test2.o添加到库

        ar x : 从归档文件中分解文件
                ar x out.a 
                //之后会分解出test1.o  test2.o 