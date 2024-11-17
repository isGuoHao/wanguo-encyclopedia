# 引用子目录中的 Kbuild 文件
TARGET := pdc
obj-m := $(TARGET).o

# 定义要编译的目标模块
$(TARGET)-objs := pdc_core.o \
					cpld/pdc_cpld.o \
					cpld/pdc_cpld_i2c.o \
					cpld/pdc_cpld_spi.o \
					led/pdc_led.o \
					led/pdc_led_gpio.o \
					led/pdc_led_pwm.o \
					osa/pdc_osa.o

# 添加头文件路径
ccflags-y += -I$(src)/../include \
			-I$(srctree)include
