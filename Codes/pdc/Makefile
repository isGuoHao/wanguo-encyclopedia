# 内核源代码路径
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
# 获取内核版本
KERNEL_VERSION := $(shell $(MAKE) -C $(KERNELDIR) kernelversion | grep -v make)

# 获取模块名
MODULE_NAME ?= $(shell grep 'MODULE_NAME := ' $(CURDIR)/Kbuild | awk -F':=' '{print $$2}' | tr -d '[:space:]')

# 目标安装目录
DESTDIR ?= $(CURDIR)/_install
# 头文件
HEADER_INSTALL_DIR := $(DESTDIR)/include/$(MODULE_NAME)
# ko
MODULE_INSTALL_DIR := $(DESTDIR)/lib/$(KERNEL_VERSION)
# 符号表
SYMBOL_INSTALL_DIR := $(DESTDIR)/lib/$(KERNEL_VERSION)/symvers/$(MODULE_NAME)

# 依赖的模块符号表
TARGET_ROOT ?= "/"
ROOT_SYMBOL_DIR := $(TARGET_ROOT)/lib/$(KERNEL_VERSION)/symvers
# EXTRA_SYMBOLS ?= $(ROOT_SYMBOL_DIR)/osa/Module.symvers


# 默认目标
all: modules

# 编译规则
modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) KBUILD_EXTRA_SYMBOLS="$(EXTRA_SYMBOLS)" modules

# 清理规则
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean

install: modules
	@echo "Installing modules to $(DESTDIR)"
	@mkdir -p $(HEADER_INSTALL_DIR) $(MODULE_INSTALL_DIR) $(SYMBOL_INSTALL_DIR)
	@cp -a $(CURDIR)/include/* $(HEADER_INSTALL_DIR)
	@cp -a $(CURDIR)/$(MODULE_NAME).ko $(MODULE_INSTALL_DIR)
	@cp -a $(CURDIR)/Module.symvers $(SYMBOL_INSTALL_DIR)

# 卸载目标
uninstall:
	@echo "Uninstalling modules from $(DESTDIR)"
	@rm -rf $(DESTDIR)


.PHONY: all modules clean install uninstall
