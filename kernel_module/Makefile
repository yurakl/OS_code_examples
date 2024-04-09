CONFIG_MODULE_SIG=n
CONFIG_MODULE_SIG_ALL=n

obj-m += skm_1.o skm_2.o skm_3.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
