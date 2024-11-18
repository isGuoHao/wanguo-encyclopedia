# 定义所有子模块的路径
SUBDIRS := drivers/osa drivers/pdc drivers/prc

# 默认目标
all: $(SUBDIRS)

# 规则来递归编译每个子模块
$(SUBDIRS):
	$(MAKE) -C $@ -f module.mk modules


# 单独编译指定模块
%: 
	$(MAKE) -C drivers/$@ -f module.mk modules;


# 单独清理指定模块
%-clean:
	$(MAKE) -C drivers/$* -f module.mk clean

# 清理规则
clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f module.mk clean; \
	done


# 安装规则
install:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f module.mk install; \
	done

# 单独安装指定模块
%-install:
	$(MAKE) -C drivers/$* -f module.mk install


# 卸载规则
uninstall:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f module.mk uninstall; \
	done

# 单独卸载指定模块
%-uninstall:
	$(MAKE) -C drivers/$* -f module.mk uninstall


.PHONY: all clean clean-% $(SUBDIRS) install uninstall %-install %-uninstall
