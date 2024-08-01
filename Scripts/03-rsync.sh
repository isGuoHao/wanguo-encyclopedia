#!/bin/bash

# 配置部分
WATCH_DIR="/path/to/watch"  # 要监视的目录
BACKUP_TYPE="local"         # 备份类型: "local" 或 "remote"
LOCAL_BACKUP_DIR="/path/to/local/backup"  # 本地备份目录
REMOTE_BACKUP_USER="user"  # 远程备份用户名
REMOTE_BACKUP_HOST="host"  # 远程备份主机
REMOTE_BACKUP_DIR="/path/to/remote/backup"  # 远程备份目录

# 确保 WATCH_DIR 和备份目录存在
if [ ! -d "$WATCH_DIR" ]; then
    echo "Watch directory $WATCH_DIR does not exist."
    exit 1
fi

if [ "$BACKUP_TYPE" == "remote" ]; then
    if [ -z "$REMOTE_BACKUP_USER" ] || [ -z "$REMOTE_BACKUP_HOST" ] || [ -z "$REMOTE_BACKUP_DIR" ]; then
        echo "Remote backup configuration is incomplete."
        exit 1
    fi
fi

# 监视指定目录并进行备份
inotifywait -m -r -e modify,create,delete,move "$WATCH_DIR" | while read -r directory events filename; do
    if [[ "$events" =~ (MODIFY|CREATE|DELETE|MOVE) ]]; then
        echo "Change detected in $directory$filename. Starting backup..."

        # 备份操作
        if [ "$BACKUP_TYPE" == "local" ]; then
            rsync -av --delete "$WATCH_DIR/" "$LOCAL_BACKUP_DIR/"
        elif [ "$BACKUP_TYPE" == "remote" ]; then
            rsync -av --delete "$WATCH_DIR/" "$REMOTE_BACKUP_USER@$REMOTE_BACKUP_HOST:$REMOTE_BACKUP_DIR/"
        else
            echo "Unknown backup type: $BACKUP_TYPE"
            exit 1
        fi

        echo "Backup completed!"
    else
        echo "Ignored event: $events for $directory$filename"
    fi
done

