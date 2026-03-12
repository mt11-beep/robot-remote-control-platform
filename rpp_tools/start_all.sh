#!/bin/bash

cd $HOME/rpp_tools


# 1. 机器人驱动
gnome-terminal --working-directory="$HOME/rpp_tools" --title="机器人驱动" -- bash -c "ros2 launch robot_bringup robot_driver.launch.py; exec bash"

# 2. 机械臂控制
gnome-terminal --working-directory="$HOME/rpp_tools" --title="机械臂控制" -- bash -c "ros2 launch robot_bringup arm.launch.py; exec bash"

# 3. Realsense相机（连接到192.168.11.10）
gnome-terminal --working-directory="$HOME/rpp_tools" --title="Realsense相机" -- bash -c "ssh -t rpp@192.168.11.10 'source ~/.rpprc && ros2 launch realsense2_camera rs_align_depth_launch.py'; exec bash"

# 4. YOLO视觉（连接到192.168.11.10）
gnome-terminal --working-directory="$HOME/rpp_tools" --title="YOLO视觉" -- bash -c "ssh -t rpp@192.168.11.10 'source ~/.rpprc && ros2 launch yolo_bringup yolov11.launch.py'; exec bash"

# 5. 抓取任务管理
gnome-terminal --working-directory="$HOME/rpp_tools" --title="抓取任务" -- bash -c "ros2 launch bt_task_manager_ros2 grasp_node.launch.py; exec bash"


# 6. 导航系统
gnome-terminal --working-directory="$HOME/rpp_tools" --title="导航系统" -- bash -c "ros2 launch navigation_2d fast_lio_navigation.launch.py use_rviz:=false; exec bash"
