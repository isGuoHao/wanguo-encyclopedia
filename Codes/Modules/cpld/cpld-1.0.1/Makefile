# Makefile for CPLD drivers

# Cross-compilation settings
CROSS_COMPILE ?= arm-linux-gnueabihf-
ARCH ?= arm

# Target module name
TARGET ?= cpld

# Source and include directories
SRC_DIR := src
INCLUDE_DIR := include

# Kernel build system variables
ifneq ($(KERNELRELEASE),)
obj-m := $(TARGET).o

$(TARGET)-objs := $(SRC_DIR)/cpld_core.o \
                  $(SRC_DIR)/cpld_spi.o \
                  $(SRC_DIR)/cpld_i2c.o \
                  $(SRC_DIR)/cpld_i3c.o

# Use relative path for include directory
ccflags-y := -I$(PWD)/$(INCLUDE_DIR)

else

KDIR ?= /path/to/your/kernel/source
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	rm -rf $(SRC_DIR)/*.o $(TARGET).ko *.mod.c .*.cmd .*.flags .*.order .*.symvers .tmp_versions

endif
