# 内核目录配置
KER_SRC_PATH ?=
KER_OBJ_PATH ?=

# 编译参数
ARCH ?= arm 
CROSS_COMPILE ?= arm-linux-gnueabi-
EXTRA_CFLAGS += -I${KER_SRC_PATH}/include

# 模块名
TARGET := spi-test

obj-m := ${TARGET}.o

# 模块源文件
${TARGET}-objs := spi_test.o

# 模块编译目标
all:
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} ${EXTRA_CFLAGS} -C ${KER_OBJ_PATH} M=$(PWD) modules

# 清理目标
clean:
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} ${EXTRA_CFLAGS} -C ${KER_OBJ_PATH} M=$(PWD) clean
	
