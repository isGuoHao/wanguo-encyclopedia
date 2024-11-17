obj-m := $(TARGET).o

$(TARGET)-objs := 	linux/kernel/osa_fileops.o \
					linux/kernel/osa_vmalloc.o \
					linux/kernel/osa_addr.o \
					linux/kernel/osa_init.o \
					linux/kernel/osa_atomic.o \
					linux/kernel/osa_barrier.o \
					linux/kernel/osa_cache.o \
					linux/kernel/osa_debug.o \
					linux/kernel/osa_device.o \
					linux/kernel/osa_interrupt.o \
					linux/kernel/osa_math.o \
					linux/kernel/osa_mutex.o \
					linux/kernel/osa_proc.o \
					linux/kernel/osa_schedule.o \
					linux/kernel/osa_semaphore.o \
					linux/kernel/osa_spinlock.o \
					linux/kernel/osa_string.o \
					linux/kernel/osa_task.o \
					linux/kernel/osa_timer.o \
					linux/kernel/osa_wait.o \
					linux/kernel/osa_workqueue.o \
					linux/kernel/osa_notifier.o \
					linux/kernel/osa_platform.o

$(TARGET)-objs +=   linux/kernel/media/base.o \
					linux/kernel/media/media.o

$(TARGET)-objs +=   linux/kernel/mmz/media-mem.o \
					linux/kernel/mmz/mmz-userdev.o \
					linux/kernel/mmz/allocator.o

# $(TARGET)-objs += linux/kernel/mmz/cma_allocator.o


# Use relative path for include directory
ccflags-y := -I$(src)/../../../include \
			-I$(srctree)include


