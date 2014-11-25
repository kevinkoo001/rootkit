###########################################################
#	Last Updated: Nov 16, 2014
#	CSE509 System Security 2014 Fall @CS SBU
#	Written By: 
#		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
#		Yaohui Chen (yaohchen@cs.stonybrook.edu)
#
#	Description: Make the LKM, loadable kernel module
############################################################

obj-m += kcr.o  
kcr-objs	:=	./src/kcr.o ./src/helpers.o ./src/HJ_ls.o ./src/dev.o ./src/backdoor.o ./src/HJ_read.o ./src/HJ_proc.o

MAKE	:= make 
KERNEL	:= /lib/modules/$(shell uname -r)/build
PWD		:= $(shell pwd)

all:
	$(MAKE) -C $(KERNEL) M=$(PWD) modules
	
clean:
	rm -rf *.o *.ko *.symvers *.mod.* *.order *.cmd
	rm -rf src/*.o
	# make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
