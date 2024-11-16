# 引用子目录中的 Kbuild 文件
TARGET := osa
obj-m := $(TARGET).o

# 定义要编译的目标模块
$(TARGET)-objs := 	osal_fileops.o \
					osal_vmalloc.o \
					osal_addr.o \
					osal_init.o \
					osal_atomic.o \
					osal_barrier.o \
					osal_cache.o \
					osal_debug.o \
					osal_device.o \
					osal_interrupt.o \
					osal_math.o \
					osal_mutex.o \
					osal_proc.o \
					osal_schedule.o \
					osal_semaphore.o \
					osal_spinlock.o \
					osal_string.o \
					osal_task.o \
					osal_timer.o \
					osal_wait.o \
					osal_workqueue.o \
					osal_notifier.o \
					osal_platform.o

$(TARGET)-objs +=   media/base.o \
					media/media.o

$(TARGET)-objs +=   mmz/media-mem.o \
					mmz/mmz-userdev.o \
					mmz/allocator.o

# $(TARGET)-objs += 	mmz/cma_allocator.o


# 添加头文件路径
ccflags-y += 	-I$(src) \
				-I$(src)/../../include \
				-I$(src)/media \
				-I$(srctree)include
