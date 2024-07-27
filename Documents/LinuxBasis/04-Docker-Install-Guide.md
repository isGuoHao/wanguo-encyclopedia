# 04-Docker-Install-Guide

## 1. 安装docker
### 1.1 方法1: 脚本自动安装
如果遇到卡死的情况，可以直接打开链接 https://get.docker.com ，将其中的内容复制，并保存到脚本docekr_install.sh，然后执行命令 bash docker_install.sh --mirror Aliyun 等待安装完成即可
```bash
curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun
```
### 1.2 方法2: 命令行安装
#### 1.2.1 更新源，并安装必要依赖
```bash
apt update -y && apt upgrade -y
```

```bash
apt install apt-transport-https ca-certificates curl gnupg-agent software-properties-common
```
#### 1.2.2 导入源仓库的 GPG key
```bash
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
```
#### 1.2.3 添加 Docker APT 软件源
```bash
add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
```
#### 1.2.4 安装 Docker 最新版本
```bash
apt update -y && apt upgrade -y
```
```bash
apt install docker-ce docker-ce-cli containerd.io
```
#### 1.2.5 输入命令验证是否安装成功
查看是否显示Docker版本号。
```bash
docker version
```
#### 1.2.6 防止Docker自动更新
```bash
sudo apt-mark hold docker-ce
```
#### 1.2.7 允许非root用户使用Docker
默认情况下只有root权限才可以使用Docker，使用下列命令将用户加入到Docker组。
```bash
sudo usermod -aG docker ${USER}
```
#### 1.2.8 卸载 Docker
卸载前，先停止并删除容器、镜像，卷和网络。
```bash
docker container stop $(docker container ls -aq)
```
```bash
docker system prune -a --volumes
```
#### 1.2.9 删除软件、配置和软件包
```bash
sudo apt purge docker-ce
```
```bash
sudo apt autoremove
```

## 2. 基于ubuntu:22.04创建容器
### 2.1 拉取ubuntu:22.04镜像
```bash
docker pull ubuntu:22.04
```
### 2.2 创建并运行容器
```bash
docker ps -a | grep 'docker-ubuntu'
if [ $? -ne 0 ]; then
    docker run -idt --name docker-ubuntu -h docker-ubuntu --privileged -v /dev:/dev -v /home:/home -w /home/${USER} ubuntu:22.04
fi
```
### 2.3 拷贝物理机用户配置到docker
```bash
sudo docker cp /etc/passwd docker-ubuntu:/etc
```
```bash
sudo docker cp /etc/group docker-ubuntu:/etc
```
```bash
sudo docker cp /etc/sudoers docker-ubuntu:/etc
```
```bash
sudo docker cp /etc/shadow docker-ubuntu:/etc
```
```bash
sudo docker cp /etc/shadow- docker-ubuntu:/etc
```
### 2.4 使用指定用户进入容器
第一次启动时，容器内默认未安装zsh，所以需要使用/bin/bash启动；
后续容器中若安装了zsh，则可以将/bin/bash替换为/usr/bin/zsh
```bash
docker exec -it -u ${USER} -w /home/${USER} docker-ubuntu /bin/bash
```
### 2.5 启动容器
```bash
docker start docker-ubuntu
```
### 2.6 重新启动容器
```bash
docker restart docker-ubuntu
```
### 2.7 停止运行容器
```bash
docker stop docker-ubuntu
```
### 2.8 删除容器
```bash
docker rm docker-ubuntu
```
### 2.9 将容器保存为镜像
```bash
docker commit docker-ubuntu ${IMAGE_NAME}:{IMAGE_TAG}
```
### 2.10 将镜像保存至文件
```bash
docker save -o <保存的镜像文件名.tar> <镜像名称>
```
### 从文件中加载镜像
```bash
docker load -i <保存的镜像文件名.tar>
```
