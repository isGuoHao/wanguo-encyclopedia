#!/bin/bash

# 定义要替换的关键字
old_keyword="$1"
new_keyword="$2"

# 使用find命令查找当前目录下所有包含旧关键字的文件和文件夹
# -depth选项确保先处理子文件夹中的项目
find . -depth -name "*$old_keyword*" | while read file; do
    # 计算新文件名
    new_file=$(echo "$file" | sed "s/$old_keyword/$new_keyword/g")
    
    # 检查新旧文件名是否相同，如果不同则重命名
    if [ "$file" != "$new_file" ]; then
        echo "Renaming '$file' to '$new_file'"
        mv "$file" "$new_file"
    fi
done
