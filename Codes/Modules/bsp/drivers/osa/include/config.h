#ifndef __CONFIG_H__
#define __CONFIG_H__

// 定义 OSA 版本号
#define OSA_VERSION    "1.0"

// 定义启用和禁用的宏
#define OSA_ENABLE  1  // 启用某个模块
#define OSA_DISABLE 0  // 禁用某个模块

// 配置选项
#define OSA_CONFIG_ARGS        OSA_ENABLE      // 启用参数解析模块
#define OSA_CONFIG_ATOMIC      OSA_ENABLE      // 启用原子操作模块
#define OSA_CONFIG_BARRIER     OSA_ENABLE      // 启用内存屏障模块
#define OSA_CONFIG_CACHE       OSA_ENABLE      // 启用缓存管理模块
#define OSA_CONFIG_DEBUG       OSA_ENABLE      // 启用调试模块
#define OSA_CONFIG_DEVFREQ     OSA_ENABLE      // 启用设备频率管理模块
#define OSA_CONFIG_DEVICE      OSA_ENABLE      // 启用设备管理模块
#define OSA_CONFIG_FILE        OSA_ENABLE      // 启用文件操作模块
#define OSA_CONFIG_INTERRUPT   OSA_ENABLE      // 启用中断处理模块
#define OSA_CONFIG_LIST        OSA_ENABLE      // 启用链表管理模块
#define OSA_CONFIG_MATH        OSA_ENABLE      // 启用数学运算模块
#define OSA_CONFIG_MEMORY      OSA_ENABLE      // 启用内存管理模块
#define OSA_CONFIG_MMAP        OSA_ENABLE      // 启用内存映射模块
#define OSA_CONFIG_MUTEX       OSA_ENABLE      // 启用互斥锁模块
#define OSA_CONFIG_PLATOFRM    OSA_ENABLE      // 启用平台驱动模块
#define OSA_CONFIG_PROC        OSA_ENABLE      // 启用 proc 文件系统模块
#define OSA_CONFIG_REBOOT      OSA_ENABLE      // 启用重启通知模块
#define OSA_CONFIG_SCHEDULE    OSA_ENABLE      // 启用调度模块
#define OSA_CONFIG_SEMAPHORE   OSA_ENABLE      // 启用信号量模块
#define OSA_CONFIG_SPINLOCK    OSA_ENABLE      // 启用自旋锁模块
#define OSA_CONFIG_STRING      OSA_ENABLE      // 启用字符串操作模块
#define OSA_CONFIG_TASK        OSA_ENABLE      // 启用任务管理模块
#define OSA_CONFIG_TIMER       OSA_ENABLE      // 启用定时器模块
#define OSA_CONFIG_WAIT        OSA_ENABLE      // 启用等待队列模块
#define OSA_CONFIG_WORKQUEUE   OSA_ENABLE      // 启用工作队列模块

#endif // __CONFIG_H__
