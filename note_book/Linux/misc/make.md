make是Linux下的一款程序自动维护工具，配合makefile的使用，就能够根据程序中模块的修改情况，自动判断应该对那些模块重新编译，从而保证软件是由最新的模块构成。当程序的源文件改变后，它能保证所有受影响的文件都将重新编译，而不受影响的文件则不予编译。


makefile 文件格式
	
	target:依赖的文件1.o 依赖的文件2 依赖的文件3\
	依赖的文件4 依赖的文件5
	依赖的文件1：文件1.c
		gcc -o 文件1.c  //注意：gcc前面要有一个tab

objects = main.o kbd.o command.o display.o \
insert.o search.o files.o utils.o
	target:$(objects)    //可以用一个变量名来表示依赖的文件

	
每个Makefile中都应该写一个清空目标文件（.o和执行文件）的规则，这不仅便于重编译
，也很利于保持文件的清洁。

	一般的风格都是：

	clean:
	　　rm edit $(objects)
	
	更为稳健的做法是：

	.PHONY : clean
	clean :
	　　-rm edit $(objects)
	
	前面说过，.PHONY意思表示clean是一个“伪目标”，。而在rm命令前面加了一个小减号的
意思就是，也许某些文件出现问题，但不要管，继续做后面的事。当然，clean的规则不要
放在文件的开头，不然，这就会变成make的默认目标，相信谁也不愿意这样。不成文的规
矩是——“clean从来都是放在文件的最后”。

注释。Makefile中只有行注释，和UNIX的Shell脚本一样，其注释是用“#”字符，这个
就像C/C++中的“//”一样。如果你要在你的Makefile中使用“#”字符，可以用反斜框进
行转义，如：“\#”。

在Makefile中的命令，必须要以[Tab]键开始,比如 rm   gcc  

当然，你可以使用别的文件名来书写Makefile，比如：“Make.Linux”，“Make.Solaris
”，“Make.AIX”等，如果要指定特定的Makefile，你可以使用make的“- f”和“--fil
e”参数，如：make -f Make.Linux或make --file Make.AIX
	make -f abc
	make -f abc clean 
	
	
引用其它的Makefile

在Makefile使用include关键字可以把别的Makefile包含进来，这很像C语言的#include，
被包含的文件会原模原样的放在当前文件的包含位置。include的语法是：

include <filename>

filename可以是当前操作系统Shell的文件模式（可以保含路径和通配符）

在 include前面可以有一些空字符，但是绝不能是[Tab]键开始。include和<filename>可
以用一个或多个空格隔开。举个例子，你有这样几个Makefile：a.mk、b.mk、c.mk，还有
一个文件叫foo.make，以及一个变量$(bar)，其包含了e.mk和 f.mk，那么，下面的语句：


include foo.make *.mk $(bar)

等价于：

include foo.make a.mk b.mk c.mk e.mk f.mk

make 命令开始时，会把找寻include所指出的其它Makefile，并把其内容安置在当前的位
置。就好像C/C++的#include指令一样。如果文件都没有指定绝对路径或是相对路径的话，
make会在当前目录下首先寻找，如果当前目录下没有找到，那么，make还会在下面的几个
目录下找：

1、如果make执行时，有“-I”或“--include-dir”参数，那么make就会在这个参数所指
定的目录下去寻找。
2、如果目录<prefix>/include（一般是：/usr/local/bin或/usr/include）存在的话，m
ake也会去找。

如果有文件没有找到的话，make会生成一条警告信息，但不会马上出现致命错误。它会继
续载入其它的文件，一旦完成makefile的读取，make会再重试这些没有找到，或是不能读
取的文件，如果还是不行，make才会出现一条致命信息。如果你想让make不理那些无法读
取的文件，而继续执行，你可以在 include前加一个减号“-”。如：

-include <filename>
其表示，无论include过程中出现什么错误，都不要报错继续执行。和其它版本make兼容的
相关命令是sinclude，其作用和这一个是一样的。

VPATH特殊变量
VPATH = src:../headers
上面的的定义指定两个目录，“src”和“../headers”，make会按照这个顺序进行搜索。
目录由“冒号”分隔。（当然，当前目录永远是最高优先搜索的地方）




	make [-f makefile文件名][选项][宏定义][目标]
	这里用[]括起来的表示是可选的。命令行选项由破折号“–”指明，后面跟选项，如
		make –e
		如果需要多个选项，可以只使用一个破折号
		make –kr
		也可以每个选项使用一个破折号
		make –k –r
		甚至混合使用也行
		make –e –kr
		
	–k：
	如果使用该选项，即使make程序遇到错误也会继续向下运行；如果没有该选项，在遇到第一个错误时make程序马上就会停止，那么后面的错误情况就不得而知了。我们可以利用这个选项来查出所有有编译问题的源文件。

	–n：
	该选项使make程序进入非执行模式，也就是说将原来应该执行的命令输出，而不是执行。

	–f ：
	指定作为makefile的文件的名称。 如果不用该选项，那么make程序首先在当前目录查找名为makefile的文件，如果没有找到，它就会转而查找名为Makefile的文件。如果您在Linux下使用GNU Make的话，它会首先查找GNUmakefile，之后再搜索makefile和Makefile。按照惯例，许多Linux程序员使用Makefile，因为这样能使Makefile出现在目录中所有以小写字母命名的文件的前面。
	
	一般情况下，makefile会跟项目的源文件放在同一个目录中。另外，系统中可以有多个makefile，一般说来一个项目使用一个makefile就可以了；如果项目很大的话，我们就可以考虑将它分成较小的部分，然后用不同的makefile来管理项目的不同部分。
	
在makefile中，注释以#为开头，至行尾结束。


obj-y：把由foo.c 或者 foo.s 文件编译得到foo.o 并连接进内核.
obj-m: 则表示该文件作为模块编译.
除了y、m以外的obj-x 形式的目标都不会被编译。




编译时，编译器需要的是语法的正确，函数与变量的声明的正确。对于后者，通常是你需
要告诉编译器头文件的所在位置（头文件中应该只是声明，而定义应该放在C/C++文件中）
，只要所有的语法正确，编译器就可以编译出中间目标文件。一般来说，每个源文件都应
该对应于一个中间目标文件（O文件或是OBJ文件）。

链接时，主要是链接函数和全局变量，所以，我们可以使用这些中间目标文件（O文件或是
OBJ文件）来链接我们的应用程序。链接器并不管函数所在的源文件，只管函数的中间目标
文件（Object File），在大多数时候，由于源文件太多，编译生成的中间目标文件太多，
而在链接时需要明显地指出中间目标文件名，这对于编译很不方便，所以，我们要给中间
目标文件打个包，在Windows下这种包叫“库文件”（Library File)，也就是 .lib 文件
，在UNIX下，是Archive File，也就是 .a 文件。