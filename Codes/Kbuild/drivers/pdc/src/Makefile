# 引用子目录中的 Kbuild 文件
TARGET := bsp
obj-m := $(TARGET).o

# 定义要编译的目标模块
$(TARGET)-objs := bsp_core.o \
					cpld/bsp_cpld.o \
					cpld/bsp_cpld_i2c.o \
					cpld/bsp_cpld_spi.o \
					led/bsp_led.o \
					led/bsp_led_gpio.o \
					led/bsp_led_pwm.o \
					osa/bsp_osa.o

# 添加头文件路径
ccflags-y += -I$(src)/../include \
			-I$(srctree)include
