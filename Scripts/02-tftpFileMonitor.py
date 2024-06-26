import os
import time
import platform
import threading

def rename_file(directory, old_name, new_name):
    old_path = os.path.join(directory, old_name)
    new_path = os.path.join(directory, new_name)
    
    print(f'Renaming {old_name} to {new_name}')
    try:
        if os.path.exists(new_path):
            os.remove(new_path)
        os.rename(old_path, new_path)
    except Exception as e:
        print(f'Error occurred while renaming {old_name} to {new_name}: {e}')
        # 日志记录
        with open('error.log', 'a') as f:
            f.write(f'Error occurred while renaming {old_name} to {new_name}: {e}\n')

def check_and_rename_file(tftp_path):
    while True:
        try:
            files = os.listdir(tftp_path)
            for file in files:
                if "FIT_Image.bin" in file:
                    rename_file(tftp_path, file, 'img')
                elif file.endswith(".ko"):
                    rename_file(tftp_path, file, 'ko')
            time.sleep(5)
        except Exception as e:
            print(f'Error occurred in check_and_rename_file: {e}')
            # 日志记录
            with open('error.log', 'a') as f:
                f.write(f'Error occurred in check_and_rename_file: {e}\n')

def main():
    system = platform.system()
    if system == "Windows":
        tftp_path = r"D:\TFTP"
    elif system == "Linux":
        tftp_path = os.path.expanduser("~") + "/tmp"
    else:
        exit(-1)

    # 设置后台线程
    thread = threading.Thread(target=check_and_rename_file, args=(tftp_path,), daemon=True)
    thread.start()

    while True:
        time.sleep(100)

if __name__ == "__main__":
    main()
