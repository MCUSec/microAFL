#!/bin/bash
# cd /sys/devices/system/cpu
# echo performance | tee cpu*/cpufreq/scaling_governor
# cd ${HOME}/microAFL
sudo mkdir -p /mnt/ram
sudo mount -t ramfs -o size=512m ramfs /mnt/ram # size should be greater than TRACESIZE in common.h
sudo chmod -R 777 /mnt/ram
