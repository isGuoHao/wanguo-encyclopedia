# 内核源代码路径
KERNELDIR ?= /lib/modules/$(shell uname -r)/build

# 获取内核版本
KERNEL_VERSION := $(shell $(MAKE) -C $(KERNELDIR) kernelversion | grep -v make)

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

# 默认目标
all: modules

# 编译规则
modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) KBUILD_EXTRA_SYMBOLS="$(EXTRA_SYMBOLS)" modules

# 清理规则
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean

# 安装目标
install: modules
ifeq ($(V),1)
	echo "\nInstalling modules to $(DESTDIR)"
	$(INSTALL_CMD)
else
	@echo "\nInstalling modules to $(DESTDIR)"
	@$(INSTALL_CMD)
endif

# 卸载目标
uninstall:
ifeq ($(V),1)
	echo "\nUninstalling modules from $(DESTDIR)"
	$(UNINSTALL_CMD)
else
	@echo "\nUninstalling modules from $(DESTDIR)"
	@$(UNINSTALL_CMD)
endif

.PHONY: all modules clean install uninstall
