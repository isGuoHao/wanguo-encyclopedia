obj-m := $(TARGET).o

$(TARGET)-objs := 	linux/kernel/osal_fileops.o \
					linux/kernel/osal_vmalloc.o \
					linux/kernel/osal_addr.o \
					linux/kernel/osal_init.o \
					linux/kernel/osal_atomic.o \
					linux/kernel/osal_barrier.o \
					linux/kernel/osal_cache.o \
					linux/kernel/osal_debug.o \
					linux/kernel/osal_device.o \
					linux/kernel/osal_interrupt.o \
					linux/kernel/osal_math.o \
					linux/kernel/osal_mutex.o \
					linux/kernel/osal_proc.o \
					linux/kernel/osal_schedule.o \
					linux/kernel/osal_semaphore.o \
					linux/kernel/osal_spinlock.o \
					linux/kernel/osal_string.o \
					linux/kernel/osal_task.o \
					linux/kernel/osal_timer.o \
					linux/kernel/osal_wait.o \
					linux/kernel/osal_workqueue.o \
					linux/kernel/osal_notifier.o \
					linux/kernel/osal_platform.o

$(TARGET)-objs +=   linux/kernel/media/base.o \
					linux/kernel/media/media.o

$(TARGET)-objs +=   linux/kernel/mmz/media-mem.o \
					linux/kernel/mmz/mmz-userdev.o \
					linux/kernel/mmz/allocator.o

# $(TARGET)-objs += linux/kernel/mmz/cma_allocator.o


# Use relative path for include directory
ccflags-y := -I$(src)/../../../include \
			-I$(srctree)include


