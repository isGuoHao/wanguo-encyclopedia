#ifndef __OSA_H__
#define __OSA_H__

// 包含内核头文件
#include <linux/kernel.h>         // 内核基本功能
#include <linux/module.h>         // 模块支持
#include <linux/types.h>          // 基本数据类型
#include <linux/string.h>         // 字符串操作

#include <linux/list.h>
#include <linux/seq_file.h>

// 包含配置文件
#include "config.h"

// 根据配置选项包含相应的头文件

#if OSA_CONFIG_ARGS
#include "osa_args.h"               // 参数解析模块
#endif

#if OSA_CONFIG_ATOMIC
#include "osa_atomic.h"             // 原子操作模块
#endif

#if OSA_CONFIG_BARRIER
#include "osa_barrier.h"            // 内存屏障模块
#endif

#if OSA_CONFIG_CACHE
#include "osa_cache.h"              // 缓存管理模块
#endif

#if OSA_CONFIG_DEBUG
#include "osa_debug.h"              // 调试模块
#endif


#if OSA_CONFIG_WAIT
#include "osa_wait.h"               // 等待队列模块
#endif

#if OSA_CONFIG_DEVFREQ
#include "osa_devfreq.h"            // 设备频率管理模块
#endif


#if OSA_CONFIG_LIST
#include "osa_list.h"               // 链表管理模块
#endif

#if OSA_CONFIG_DEVICE
#include "osa_device.h"             // 设备管理模块
#endif

#if OSA_CONFIG_FILE
#include "osa_file.h"               // 文件操作模块
#endif

#if OSA_CONFIG_INTERRUPT
#include "osa_interrupt.h"          // 中断处理模块
#endif

#if OSA_CONFIG_MATH
#include "osa_math.h"               // 数学运算模块
#endif

#if OSA_CONFIG_MEMORY
#include "osa_memory.h"             // 内存管理模块
#endif

#if OSA_CONFIG_MMAP
#include "osa_mmap.h"               // 内存映射模块
#endif

#if OSA_CONFIG_MUTEX
#include "osa_mutex.h"              // 互斥锁模块
#endif

#if OSA_CONFIG_PLATOFRM
#include "osa_platform.h"           // platform驱动模块
#endif

#if OSA_CONFIG_PROC
#include "osa_proc.h"               // proc 文件系统模块
#endif

#if OSA_CONFIG_REBOOT
#include "osa_reboot.h"             // 重启通知模块
#endif

#if OSA_CONFIG_SCHEDULE
#include "osa_schedule.h"           // 调度模块
#endif

#if OSA_CONFIG_SEMAPHORE
#include "osa_semaphore.h"          // 信号量模块
#endif

#if OSA_CONFIG_SPINLOCK
#include "osa_spinlock.h"           // 自旋锁模块
#endif

#if OSA_CONFIG_STRING
#include "osa_string.h"             // 字符串操作模块
#endif

#if OSA_CONFIG_TASK
#include "osa_task.h"               // 任务管理模块
#endif

#if OSA_CONFIG_TIMER
#include "osa_timer.h"              // 定时器模块
#endif

#if OSA_CONFIG_WORKQUEUE
#include "osa_workqueue.h"          // 工作队列模块
#endif

#endif // __OSA_H__
