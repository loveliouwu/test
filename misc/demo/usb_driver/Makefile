MODULE_NAME:=lyp_drv

RESMAN_CORE_OBJS:=lyp.o

$(MODULE_NAME) -objs:=$(RESMAN_CORE_OBJS)

obj-m:= lyp_drv.o

PWD:=$(shell pwd)

CURDIR = $(shell pwd)
KVERS = $(shell uname -r) #变量KVERS为当前linux版本值 

$(MODULE_NAME)-objs := lyp.o cf_log.o netlink_transfer.o 

KERNELDIR := /lib/modules/4.4.0-146-generic/build

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *~
install:
	
