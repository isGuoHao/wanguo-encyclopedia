# 如果内核源码路径不是默认的 /lib/modules/$(shell uname -r)/build，
# 你可以通过设置 KDIR 指向你的内核源码树。
KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
