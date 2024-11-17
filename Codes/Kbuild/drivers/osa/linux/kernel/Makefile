# 引用子目录中的 Kbuild 文件
TARGET := osa
obj-m := $(TARGET).o

# 定义要编译的目标模块
$(TARGET)-objs := 	osa_fileops.o \
					osa_vmalloc.o \
					osa_addr.o \
					osa_init.o \
					osa_atomic.o \
					osa_barrier.o \
					osa_cache.o \
					osa_debug.o \
					osa_device.o \
					osa_interrupt.o \
					osa_math.o \
					osa_mutex.o \
					osa_proc.o \
					osa_schedule.o \
					osa_semaphore.o \
					osa_spinlock.o \
					osa_string.o \
					osa_task.o \
					osa_timer.o \
					osa_wait.o \
					osa_workqueue.o \
					osa_notifier.o \
					osa_platform.o

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
