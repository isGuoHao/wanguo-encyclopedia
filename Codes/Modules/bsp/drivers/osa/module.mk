# 获取模块名
MODULE_NAME ?= $(shell grep 'MODULE_NAME := ' $(CURDIR)/Kbuild | awk -F':=' '{print $$2}' | tr -d '[:space:]')

# 依赖的模块符号表
EXTRA_SYMBOLS ?=

# 安装命令
INSTALL_CMD = \
	mkdir -p $(HEADER_INSTALL_DIR) $(MODULE_INSTALL_DIR) $(SYMBOL_INSTALL_DIR); \
	cp -a $(CURDIR)/include/* $(HEADER_INSTALL_DIR); \
	cp -a $(CURDIR)/$(MODULE_NAME).ko $(MODULE_INSTALL_DIR); \
	cp -a $(CURDIR)/Module.symvers $(SYMBOL_INSTALL_DIR);

# 卸载命令
UNINSTALL_CMD = \
	rm -rf $(HEADER_INSTALL_DIR); \
	rm -f $(MODULE_INSTALL_DIR)/$(MODULE_NAME).ko; \
	rm -rf $(SYMBOL_INSTALL_DIR);

# 包含通用 Makefile
include $(BSP_ROOT)/scripts/Makefiles/kernel-module.mk
