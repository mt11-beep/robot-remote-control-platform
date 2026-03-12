# ROS2 Pub GUI

A Linux Qt-based GUI for remote robot control, map visualization, video display, and command execution.

## 1. Overview

This project provides a Qt GUI running on a server-side Linux machine for controlling and monitoring a robot remotely.

Main functions include:

- Robot connection
- Manual motion control
- Automatic control / command triggering
- Map display in embedded web view
- Video display in embedded web view
- Log display
- Robot-side helper script integration

This project follows a **server + robot** deployment structure:

- **Server side**: runs the Qt GUI
- **Robot side**: runs ROS2 nodes, map/video services, and helper scripts in `rpp_tools`

---

## 2. Project Structure

```text
.
├── main.cpp
├── mainwindow.cpp
├── mainwindow.h
├── mainwindow.ui
├── ros2_pub_gui.pro
├── ros2_pub_gui_zh_CN.ts
├── README.md
├── .gitignore
└── rpp_tools
    ├── backward.sh
    ├── forward.sh
    ├── loop_scraping.sh
    ├── map_bridge.py
    ├── navigate_to_pose.sh
    ├── start_all.sh
    ├── start_map_service.sh
    ├── start_robot_driver.sh
    ├── start_video_service.sh
    ├── stop_all_terminals.sh
    ├── stop_map_runtime.sh
    ├── stop_map_service.sh
    ├── stop.sh
    ├── stop_video_service.sh
    ├── turn_left.sh
    ├── turn_right.sh
    ├── up_down.sh
    ├── video_mjpeg_server.py
    ├── video_mjpeg_server0.py
    ├── zhiling.txt
    └── html
        ├── map_meta.json
        ├── map.png
        ├── robot_map_local.html
        ├── robot_map_local1.html
        ├── roslib.min.js
        ├── test_connect.html
        └── video.html
