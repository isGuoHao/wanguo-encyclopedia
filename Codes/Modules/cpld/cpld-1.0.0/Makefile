obj-m += cpld_driver.o

ARCH ?= arm
CROSS_COMPILE ?= 
KERNEL_BUILD_PATH ?= /lib/modules/$(shell uname -r)/build


all:
	make -C ${KERNEL_BUILD_PATH} M=$(PWD) modules

clean:
	make -C ${KERNEL_BUILD_PATH} M=$(PWD) modules clean
