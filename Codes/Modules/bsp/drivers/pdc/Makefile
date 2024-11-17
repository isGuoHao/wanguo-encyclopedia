# 内核源代码路径
KERNELDIR ?= /lib/modules/$(shell uname -r)/build

# 编译规则
modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules

# 清理规则
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean
	
.PHONY: modules clean
