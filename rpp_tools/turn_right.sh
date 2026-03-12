#!/bin/bash

# 右转指令脚本 (turn_right.sh)
ros2 topic pub -r 10 /cmd_vel geometry_msgs/Twist '{linear: {x: 0.0}, angular: {z: -0.8}}'
