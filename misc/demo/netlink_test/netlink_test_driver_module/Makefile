CURDIR = $(shell pwd)
KVERS = $(shell uname -r) #变量KVERS为当前linux版本值 

obj-m += test.o 

all:

	make -C /lib/modules/4.4.0-143-generic/build M=$(CURDIR) modules 

clean:
	rm -rf *.od
