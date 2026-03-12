#!/bin/bash

echo "[INFO] stopping video snapshot server on 8081 ..."
pkill -f "video_mjpeg_server.py" || true

echo "[INFO] done"
