#!/bin/bash

# Function to print messages
print_info() {
    echo -e "\e[32m[INFO] $1\e[0m"
}

print_error() {
    echo -e "\e[31m[ERROR] $1\e[0m"
}

# 1. 设置root密码并切换至root
print_info "设置root密码"
sudo passwd root

print_info "切换至root用户"
su - root

# 2. 更新软件
print_info "更新软件包列表和升级已有软件"
apt update && apt upgrade -y

# 3. 安装基础常用软件
print_info "安装基础常用软件"
apt install -y vim git python3 pip build-essential curl wget net-tools iputils-ping

# 4. 配置ssh
print_info "安装并配置SSH"
apt install -y openssh-server openssh-client

# 修改ssh配置文件
print_info "修改SSH配置文件"
sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config
sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# 重启ssh服务
print_info "重启SSH服务"
systemctl restart sshd

# 5. 配置samba
print_info "安装并配置Samba"
apt install -y samba

# 修改Samba配置文件
print_info "修改Samba配置文件"
echo "
[wanguo]
comment = samba share path
browseable = yes
path = /home/wanguo
create mask = 0700
directory mask = 0700
valid users = wanguo
force user = wanguo
force group = wanguo
public = yes
available = yes
writable = yes
" >> /etc/samba/smb.conf

# 添加Samba用户
read -p "请输入Samba用户密码: " smbpasswd
(echo $smbpasswd; echo $smbpasswd) | smbpasswd -s -a wanguo

# 重启Samba服务
print_info "重启Samba服务"
systemctl restart smbd

# 6. 安装zsh && oh-my-zsh
print_info "安装zsh和oh-my-zsh"
apt install -y zsh

# 下载并安装oh-my-zsh
sh -c "$(curl -fsSL https://gitee.com/mirrors/oh-my-zsh/raw/master/tools/install.sh)"

# 安装常用插件
print_info "安装zsh常用插件"
git clone https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting
git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions
git clone https://github.com/zsh-users/zsh-completions ${ZSH_CUSTOM:=~/.oh-my-zsh/custom}/plugins/zsh-completions

# 修改zsh配置文件
print_info "修改zsh配置文件"
sed -i 's/ZSH_THEME="robbyrussell"/ZSH_THEME="jonathan"/' ~/.zshrc
sed -i 's/plugins=(git)/plugins=(git z sudo extract zsh-autosuggestions zsh-syntax-highlighting zsh-completions)/' ~/.zshrc

# 添加alias
echo "
# Custom aliases
alias cls='clear'
alias cp='cp -rf'
alias rf='rm -rf'
alias grepc='find . -iname \"*.c\" | xargs grep -rn --color=auto '
alias greph='find . -iname \"*.h\" | xargs grep -rn --color=auto '
" >> ~/.zshrc

print_info "完成所有配置，重载zsh配置"
source ~/.zshrc

print_info "配置完毕，请进行后续的检查和配置完善"
