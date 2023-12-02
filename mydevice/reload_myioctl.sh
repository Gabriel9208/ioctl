#!/bin/bash

if [[ $# -eq 0 ]]; then 
    echo "reload_myioctl.sh: Miss argument."
    exit 1
fi

sudo rmmod /home/nachos/ioctl/mydevice/myioctl_driver.ko
sudo insmod /home/nachos/ioctl/mydevice/myioctl_driver.ko

sudo rm /dev/myioctl
sudo mknod /dev/myioctl c $1 0

sudo chmod 666 /dev/myioctl