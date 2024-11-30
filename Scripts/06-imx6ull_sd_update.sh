#!/bin/sh

# Set the working path and directories
WORK_PATH="/home/root/sd_update"
IMAGES_DIR="${WORK_PATH}/images"
BOOT_MOUNT_POINT="${WORK_PATH}/mnt/boot"
ROOTFS_MOUNT_POINT="${WORK_PATH}/mnt/rootfs"

# TFTP server IP address
TFTP_SERVER="10.10.0.201"

# Network configuration
INTERFACE="eth0"  # 网卡名
DEVICE_IP="10.10.0.202"  # 设备 IP 地址
NETMASK="255.255.255.0"  # 子网掩码

# Device paths
BOOT_DEVICE="/dev/mmcblk0p1"
ROOTFS_DEVICE="/dev/mmcblk0p2"

# Ensure the directories exist
mkdir -p "${IMAGES_DIR}" "${BOOT_MOUNT_POINT}" "${ROOTFS_MOUNT_POINT}"

# Function to handle errors and exit
error_exit() {
    echo "Error: $1" 1>&2
    exit 1
}

# Function to set the IP address of the device
set_ip_address() {
    echo ""
    echo "Setting IP address for $INTERFACE to $DEVICE_IP with netmask $NETMASK..."
    ifconfig "$INTERFACE" "$DEVICE_IP" netmask "$NETMASK" up || error_exit "Failed to set IP address."
}

# Function to mount a partition
mount_partition() {
    local device=$1
    local mount_point=$2
    echo "Mounting $device to $mount_point..."
    mount "$device" "$mount_point" || error_exit "Failed to mount $device."
}

# Function to unmount a partition
unmount_partition() {
    local mount_point=$1
    echo "Unmounting $mount_point..."
    umount "$mount_point" || error_exit "Failed to unmount $mount_point."
}

# Function to download a file using TFTP and save it to the images directory
download_file() {
    local filename=$1
    local target_path="${IMAGES_DIR}/$filename"
    
    # Remove the existing file if it exists
    if [ -f "$target_path" ]; then
        echo "Removing existing $filename..."
        rm -f "$target_path"
    fi
    
    echo "Downloading $filename from TFTP server $TFTP_SERVER..."
    tftp "$TFTP_SERVER" -gr "$filename" -l "$target_path" || error_exit "Failed to download $filename."
}

# Function to update the kernel (zImage and dtb)
update_kernel() {
    echo ""
    echo "Starting update kernel..."
    
    # Mount the boot partition
    mount_partition "$BOOT_DEVICE" "${BOOT_MOUNT_POINT}"
    
    # Download zImage
    download_file zImage
    
    # Copy zImage to the boot partition
    cp "${IMAGES_DIR}/zImage" "${BOOT_MOUNT_POINT}/" || error_exit "Failed to copy zImage to boot partition."
    
    # Download dtb
    download_file imx6ull-14x14-evk.dtb
    
    # Copy dtb to the boot partition
    cp "${IMAGES_DIR}/imx6ull-14x14-evk.dtb" "${BOOT_MOUNT_POINT}/" || error_exit "Failed to copy dtb to boot partition."
    
    # Unmount the boot partition
    unmount_partition "${BOOT_MOUNT_POINT}"
    
    echo "Kernel update completed successfully."
}

# Function to update the rootfs
update_rootfs() {
    echo ""
    echo "Starting update rootfs..."
    
    # Mount the rootfs partition
    mount_partition "$ROOTFS_DEVICE" "${ROOTFS_MOUNT_POINT}"
    
    # Download rootfs.tar
    download_file rootfs.tar
    
    # Clean up existing rootfs directory
    echo "Cleaning up existing rootfs..."
    rm -rf "${ROOTFS_MOUNT_POINT}"/*
    
    # Extract rootfs.tar to rootfs partition without printing filenames
    echo "Extracting rootfs.tar..."
    tar -xvf "${IMAGES_DIR}/rootfs.tar" -C "${ROOTFS_MOUNT_POINT}" &> /dev/null || error_exit "Failed to extract rootfs.tar."
    
    # Unmount the rootfs partition
    unmount_partition "${ROOTFS_MOUNT_POINT}"
    
    echo "Rootfs update completed successfully."
}

# Function to update U-Boot
update_uboot() {
    echo ""
    echo "Starting update U-Boot..."
    
    # Download U-Boot image
    download_file u-boot-dtb.imx
    
    # Write U-Boot image to the first sector of the SD card
    echo "Writing U-Boot image to /dev/mmcblk0..."
    dd if="${IMAGES_DIR}/u-boot-dtb.imx" of=/dev/mmcblk0 bs=1024 seek=1 conv=fsync || error_exit "Failed to write U-Boot image."
    
    echo "U-Boot update completed successfully."
}

# Main script logic
# Set the IP address at the beginning of the script
set_ip_address

if [ $# -eq 0 ]; then
    echo "Usage: $0 {kernel|rootfs|uboot|all}"
    exit 1
fi

case $1 in
    kernel)
        update_kernel
        ;;
    rootfs)
        update_rootfs
        ;;
    uboot)
        update_uboot
        ;;
    all)
        update_uboot
        update_kernel
        update_rootfs
        ;;
    *)
        echo "Invalid argument: $1. Use 'kernel', 'rootfs', 'uboot', or 'all'."
        exit 1
        ;;
esac

echo "Update completed successfully."
echo ""
exit 0
