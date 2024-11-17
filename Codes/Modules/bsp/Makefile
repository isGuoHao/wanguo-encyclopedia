# 定义所有子模块的路径
SUBDIRS := drivers/osa drivers/pdc drivers/prc

# 默认目标
all: $(SUBDIRS)

# 规则来递归编译每个子模块
$(SUBDIRS):
	$(MAKE) -C $@ modules


# 单独编译指定模块
%: 
	$(MAKE) -C drivers/$@ modules;


# 单独清理指定模块
%-clean:
	$(MAKE) -C drivers/$* clean

# 清理规则
clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done


# 安装规则
install:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install; \
	done

# 单独安装指定模块
%-install:
	$(MAKE) -C drivers/$* install


# 卸载规则
uninstall:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir uninstall; \
	done

# 单独卸载指定模块
%-uninstall:
	$(MAKE) -C drivers/$* uninstall


.PHONY: all clean clean-% $(SUBDIRS) install uninstall %-install %-uninstall