#!/bin/bash

# 定义要替换的关键字
old_keyword="$1"
new_keyword="$2"

# 遍历当前目录下的所有文件
find . -type f | while read file; do
    # 检查文件是否可读
    if [ -r "$file" ]; then
        # 使用sed命令进行替换
        sed -i "s/$old_keyword/$new_keyword/g" "$file"
        echo "Replaced '$old_keyword' with '$new_keyword' in '$file'"
    else
        echo "Cannot read file: $file"
    fi
done
