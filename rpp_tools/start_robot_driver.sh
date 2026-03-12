#!/bin/bash
gnome-terminal --working-directory="$HOME/rpp_tools" -- bash -c "ros2 launch robot_bringup robot_driver.launch.py; exec bash"
