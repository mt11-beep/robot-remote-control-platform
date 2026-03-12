#!/bin/bash

# 停止指令脚本 (stop.sh)
ros2 topic pub -1 /cmd_vel geometry_msgs/Twist '{linear: {x: 0.0}, angular: {z: 0.0}}'
