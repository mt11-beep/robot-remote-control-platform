#!/bin/bash

# 进入工作目录
cd $HOME/rpp_tools

# 发布循环抓取命令
echo "正在发布循环抓取命令..."
ros2 topic pub /grasp_node/cmd std_msgs/msg/String "data: 'loop'" -1

echo "命令发布完成"
