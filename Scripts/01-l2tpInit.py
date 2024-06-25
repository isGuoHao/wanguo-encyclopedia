import os
import sys
import psutil
import socket
import subprocess
import threading
import time
import ctypes

LINK_NAME = "xxx"	# 使用时修改为实际网络连接名
USER_NAME = "xxx"	# 使用时修改为实际用户名
USER_PSWD = "***"		# 使用时修改为实际密码

NETMASK = "255.255.255.255"	
GATEWAY = "192.168.x.x"	# 使用时修改为实际VPN IP

# 要检查的目标网络
TARGET_NETWORK_LIST = ["192.168.x.x", 
                       "192.168.x.x",
                      ]

# 添加路由
def add_route(target_network):
    command = f'route add {target_network} mask {NETMASK} {GATEWAY}'
    print("\nCMD:", command)
    result = subprocess.run(command, shell=True)
    if result.returncode == 0:
        return True
    else:
        return False

# 删除路由
def delete_route(target_network):
    command = f'route delete {target_network}'
    print("\nCMD:", command)
    result = subprocess.run(command, shell=True)
    if result.returncode == 0:
        return True
    else:
        return False

# 检查路由是否存在
def is_route_exist(target_network):
    command = 'route print'
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    output_lines = result.stdout.split('\n')
    for line in output_lines:
        if target_network in line:
            return True
    return False

# 遍历检查路由是否存在并添加路由
def route_init():
    for target_network in TARGET_NETWORK_LIST:
        # 如果路由已经存在，则直接删除，重新添加
        if is_route_exist(target_network) == True:
            delete_route(target_network)
        # 添加路由
        result = add_route(target_network)
        if result == True:
            print(f"Add route to {target_network} success")
        else:
            print(f"Add route to {target_network} failed")


# 连接到L2TP
def connect_to_l2tp():
    command = f'rasdial {LINK_NAME} {USER_NAME} {USER_PSWD}'
    result = subprocess.run(command, shell=True)
    return result.returncode
    if result.returncode == 0:
        return True
    else:
        return False


# 获取L2TP连接状态
def get_l2tp_status():
    interfaces = psutil.net_if_addrs()
    if LINK_NAME in interfaces:
        addrs = interfaces[LINK_NAME]
        for addr in addrs:
            if addr.family == socket.AF_INET or addr.family == socket.AF_INET6:
                return True
    return False


# 遍历检查路由是否存在并添加路由
def l2tp_init():
    # 获取L2TP连接状态
    result = get_l2tp_status()
    if result != True:
        print("\nl2tp is not active, try to reconnect to l2tp...")
        # 连接到L2TP
        result = connect_to_l2tp()
        if result == 0:
            print("Reconnect to l2tp success.")
            route_init()
        else:
            print("Reconnect to l2tp failed.")
    else:
        print("l2tp is active")


# 定时检测l2tp连接状态
def execute_l2tp_init_periodically():
    interval = 30  # 间隔时间，单位：秒
    while True:
        l2tp_init()
        time.sleep(interval)


# 判断是否具有管理员权限
def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        return False


### ---- 脚本入口 ---- 
if __name__ == "__main__":
    # 清屏
    os.system("cls")

    # 获取管理员权限
    if not is_admin():
        ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, " ".join(sys.argv), None, 1)
        sys.exit(0)


    # 设置后台线程
    thread = threading.Thread(target=execute_l2tp_init_periodically, daemon=True)
    thread.start()

    while True:
        time.sleep(100)
