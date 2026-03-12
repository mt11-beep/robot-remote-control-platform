#!/bin/bash

# 前进指令（0.3m/s，执行0.5秒后停止）
ros2 topic pub -r 10 /cmd_vel geometry_msgs/Twist '{linear: {x: 0.3}, angular: {z: 0.0}}'
