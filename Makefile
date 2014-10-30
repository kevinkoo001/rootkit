obj-m += evil.o  
evil-objs	:=	./src/evil.o ./src/helpers.o ./src/HJ_ls.o ./src/dev.o 
all:
			make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
			make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
