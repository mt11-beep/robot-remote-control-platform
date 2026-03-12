#!/bin/bash
set -e

source /opt/ros/humble/setup.bash

VIDEO_PORT=8081
SCRIPT="/home/rpp/rpp_tools/video_mjpeg_server.py"

echo "[INFO] checking video snapshot server on ${VIDEO_PORT} ..."
if ss -ltn | grep -q ":${VIDEO_PORT} "; then
  echo "[INFO] video snapshot server already running"
else
  echo "[INFO] starting video snapshot server ..."
  nohup python3 "${SCRIPT}" >/tmp/video_snapshot.log 2>&1 &
  sleep 2
fi

echo "[INFO] video service ready"
echo "[INFO] page:          http://127.0.0.1:8080/video.html"
echo "[INFO] front snap:    http://127.0.0.1:${VIDEO_PORT}/front_snapshot.jpg"
echo "[INFO] yolo snap:     http://127.0.0.1:${VIDEO_PORT}/yolo_snapshot.jpg"
echo "[INFO] stats:         http://127.0.0.1:${VIDEO_PORT}/stats"
