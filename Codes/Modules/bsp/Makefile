# Makefile for BSP drivers

# Cross-compilation settings
CROSS_COMPILE ?= arm-linux-gnueabihf-
ARCH ?= arm

# Target module name
TARGET ?= bsp

# Source and include directories
SRC_DIR := src
INCLUDE_DIR := $(PWD)/include
EXTERNAL_INCLUDE_DIR := $(PWM)/include

# Kernel build system variables
ifeq ($(KERNELRELEASE),)
KDIR ?= ${HOME}/linux
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean

else

obj-m := $(TARGET).o

$(TARGET)-objs := \
	$(SRC_DIR)/osa/bsp_osa.o \
	$(SRC_DIR)/bsp.o \
	$(SRC_DIR)/cpld/bsp_cpld.o \
	# $(SRC_DIR)/cpld/bsp_cpld_spi.o \
	# $(SRC_DIR)/cpld/bsp_cpld_i2c.o

# Use relative path for include directory
ccflags-y := -I$(INCLUDE_DIR) -I$(EXTERNAL_INCLUDE_DIR)

endif

# Generate dependency files
%.d: %.c
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,$$*$\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(wildcard $(SRC_DIR)/*.d)

.PHONY: clean
