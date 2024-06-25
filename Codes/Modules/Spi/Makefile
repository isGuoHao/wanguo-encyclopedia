# 内核目录配置
KER_SRC_PATH := ""
KER_OBJ_PATH := ""

# 编译参数
ARCH := 
CROSS_COMPILE := 
EXTRA_CFLAGS += -I${KER_SRC_PATH}/include

# 模块名
TARGET := spi-test

obj-m := ${TARGET}.o

# 模块源文件
${TARGET}.o := spi_read_write.o

# 模块编译目标
all:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} ${EXTRA_CFLAGS} -C ${KER_OBJ_PATH} M=$(PWD) modules

# 清理目标
clean:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} ${EXTRA_CFLAGS} -C ${KER_OBJ_PATH} M=$(PWD) clean
	
