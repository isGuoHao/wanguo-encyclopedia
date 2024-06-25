# spi驱动测试
## 记录如何在自己写的external module中，通过已知的设备树节点名，获取已经注册的spi设备地址

## 设计思路
2. 根据设备树节点名，获取到设备树节点地址
3. 根据设备树节点，在eMMC bus上查找到device地址
4. 根据device地址，使用container_of获取spi_device地址

## build and clean
传参指定ARCH（架构）、CROSS_COMPILE（交叉编译工具链）、KER_SRC_PATH（内核源码目录）、KER_OBJ_PATH（内核编译目录）即可

### 1. arm
```bash
	make KER_SRC_PATH=/home/xxx/kernel/linux-5.10.4 KER_OBJ_PATH=/home/xxx/kernel/build
	make KER_SRC_PATH=/home/xxx/kernel/linux-5.10.4 KER_OBJ_PATH=/home/xxx/kernel/build clean
```

### 2. riscv
```bash
	make ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- KER_SRC_PATH=/home/xxx/kernel/linux-5.10.4 KER_OBJ_PATH=/home/xxx/kernel/build
	make ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- KER_SRC_PATH=/home/xxx/kernel/linux-5.10.4 KER_OBJ_PATH=/home/xxx/kernel/build clean
```
