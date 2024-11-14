# Makefile for CPLD drivers

# Cross-compilation settings
CROSS_COMPILE ?= arm-linux-gnueabihf-
ARCH ?= arm

# Target module name
TARGET ?= proc_test

# Source and include directories
SRC_DIR := src
INCLUDE_DIR := include

# Output directory for intermediate files
OBJ_DIR := obj

# Kernel source directory
KDIR ?= /home/wanguo/02-BuildKit/.build/Code/linux_6.9.8

# Kernel build system variables
ifneq ($(KERNELRELEASE),)
obj-m := $(TARGET).o

$(TARGET)-objs := $(SRC_DIR)/proc.o

# Use relative path for include directory
ccflags-y := -I$(PWD)/$(INCLUDE_DIR)

else

PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
	rm -rf $(OBJ_DIR)

endif
