# 01-Ubuntu Install Guide

## 1. 设置root密码并切换至root
```bash
sudo passwd root && su
```

## 2. 更新软件
```bash
apt update && apt upgrade -y 
```

## 3. 安装基础常用软件
```bash
apt install -y vim git python3 pip build-essential curl wget net-tools iputils-ping
```

## 4. 配置ssh
### 4.1 安装openssh
```bash
apt install -y openssh-server openssh-client
```
### 4.2 ssh配置文件修改
```bash
sudo vim /etc/ssh/sshd_config
```
#### 4.2.1 密码登录
```bash
# 设置密码登录
PasswordAuthentication yes
```
#### 4.2.2 root登录
```bash
# 允许root登录
PermitRootLogin yes
```
### 4.3 重启sshd服务
```bash
systemctl restart sshd
```

## 5. 配置samba
### 5.1 安装samba
```bash
apt install -y samba
```
### 5.2 修改samba配置
```bash
sudo vim /etc/samba/smb.conf
```
#### 5.2.1 配置USER_NAME共享目录
```bash
[USER_NAME]
comment = wnguo share path
browseable = yes
path = /home/USER_NAME
create mask = 0700
directory mask = 0700
valid users = USER_NAME
force user = USER_NAME
force group = USER_NAME
public = yes
available = yes
writable = yes
```
### 5.3 添加samba用户
执行如下命令，并根据提示设置密码
```bash
smbpasswd -a ${USER}
```
### 5.4 重启smbd服务
```bash
systemctl restart smbd
```

## 6 安装zsh && oh-my-zsh
### 6.1 安装zsh
```bash
apt install -y zsh
```
### 6.2 下载并安装oh-my-zsh
```bash
sh -c "$(curl -fsSL https://gitee.com/mirrors/oh-my-zsh/raw/master/tools/install.sh)"
```
### 6.3 安装插件
(1) 命令高亮
```bash
git clone https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting
```
(2) 命令提示
```bash
git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions
```
(3) 命令补全
```bash
git clone https://github.com/zsh-users/zsh-completions ${ZSH_CUSTOM:=~/.oh-my-zsh/custom}/plugins/zsh-completions
```
### 6.4 配置zshrc
```bash
vim ~/.zshrc
```
#### 6.4.1 修改主题
```bash
ZSH_THEME="jonathan"
```
#### 6.4.2 开启插件
```bash
plugins=(git 
sudo 
extract 
zsh-autosuggestions 
zsh-syntax-highlighting
zsh-completions
)
```
#### 6.4.3 设置常用alias
```bash
alias cls='clear'
alias cp='cp -rf'
alias rf='rm -rf'
alias grepc='find . -iname "*.c" | xargs grep -rn --color=auto '
alias greph='find . -iname "*.h" | xargs grep -rn --color=auto '
```
