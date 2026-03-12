#!/bin/bash
set -e

source /opt/ros/humble/setup.bash

HTML_DIR="/home/rpp/rpp_tools/html"
HTTP_PORT=8080
ROSBRIDGE_PORT=9090

echo "[INFO] checking rosbridge on ${ROSBRIDGE_PORT} ..."
if ss -ltn | grep -q ":${ROSBRIDGE_PORT} "; then
  echo "[INFO] rosbridge already running"
else
  echo "[INFO] starting rosbridge_server ..."
  nohup ros2 launch rosbridge_server rosbridge_websocket_launch.xml >/tmp/rosbridge.log 2>&1 &
  sleep 3
fi

echo "[INFO] checking http server on ${HTTP_PORT} ..."
if ss -ltn | grep -q ":${HTTP_PORT} "; then
  echo "[INFO] http server already running"
else
  echo "[INFO] starting http server in ${HTML_DIR} ..."
  cd "$HTML_DIR"
  nohup python3 -m http.server ${HTTP_PORT} >/tmp/map_http.log 2>&1 &
  sleep 1
fi

echo "[INFO] map service ready"
echo "[INFO] rosbridge: 127.0.0.1:${ROSBRIDGE_PORT}"
echo "[INFO] http:      127.0.0.1:${HTTP_PORT}"
echo "[INFO] page:      http://127.0.0.1:${HTTP_PORT}/robot_map_local.html"
