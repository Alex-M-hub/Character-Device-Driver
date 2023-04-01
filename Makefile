obj-m := driver.o #we're building a kerner module called "driver.o"
KDIR := /lib/modules/$(shell uname -r)/build #sets the KDIR variable to the path of the Linux kernel source code.
PWD := $(shell pwd) #sets the PWD variable to the current directory where the Makefile is located.
default: #this splecifies the default target for the Makefile
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules
#This command tells the Makefile to build the kernel module using the kernel source code located
# 		in KDIR and using the PWD directory as the location for the module source code
