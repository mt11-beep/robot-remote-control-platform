#!/bin/bash
# stop_all_terminals.sh

# 关闭所有ROS2启动的进程
pkill -f "robot_driver.launch.py"
pkill -f "arm.launch.py"
pkill -f "rs_align_depth_launch.py"
pkill -f "yolov11.launch.py"
pkill -f "controller_node.launch.py"
pkill -f "grasp_node.launch.py"
pkill -f "fast_lio_navigation.launch.py"

# 关闭SSH连接
pkill -f "ssh.*192.168.11.10"

echo "所有相关终端已关闭"
