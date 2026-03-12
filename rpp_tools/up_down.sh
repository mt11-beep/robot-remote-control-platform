#!/bin/bash

# 设置高度参数，默认0.1米
HEIGHT=${1:-0.1}

# 启动电机升降ROS节点并设置位置
echo "正在启动电机升降控制..."
ros2 launch motor_lift_ros motor_lift_ros.launch.py &
sleep 2
ros2 topic pub /motor_lift/set_position std_msgs/msg/Float32 "data: ${HEIGHT}" --once

echo "电机高度设置为: ${HEIGHT} 米"
