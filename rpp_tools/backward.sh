#!/bin/bash

# 后退指令脚本 (backward.sh)
ros2 topic pub -r 10 /cmd_vel geometry_msgs/Twist '{linear: {x: -0.3}, angular: {z: 0.0}}'
