#!/bin/bash

echo "[INFO] stopping rosbridge only ..."
pkill -f "rosbridge_websocket" || true
pkill -f "ros2 launch rosbridge_server rosbridge_websocket_launch.xml" || true

echo "[INFO] map runtime stopped"
