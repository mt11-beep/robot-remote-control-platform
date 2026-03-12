#!/bin/bash

echo "[INFO] stopping http server on 8080 ..."
pkill -f "python3 -m http.server 8080" || true

echo "[INFO] stopping rosbridge ..."
pkill -f "rosbridge_websocket" || true
pkill -f "ros2 launch rosbridge_server rosbridge_websocket_launch.xml" || true

echo "[INFO] done"
