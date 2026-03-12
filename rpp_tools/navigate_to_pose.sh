#!/bin/bash

# 简化版本，使用预定义的常见角度

if [ $# -lt 3 ]; then
    echo "用法: $0 <x> <y> <yaw_degrees>"
    echo "支持的角度: 0, 90, 180, 270 度"
    exit 1
fi

X=$1
Y=$2
YAW=$3

case $YAW in
    0)
        Z=0.0; W=1.0 ;;
    90)
        Z=0.707; W=0.707 ;;
    180)
        Z=1.0; W=0.0 ;;
    270)
        Z=-0.707; W=0.707 ;;
    *)
        echo "错误: 不支持的角度 $YAW，请使用 0, 90, 180, 270"
        exit 1 ;;
esac

ros2 action send_goal /navigate_to_pose nav2_msgs/action/NavigateToPose "{
pose: {
  header: {frame_id: 'map'},
  pose: {
    position: {x: $X, y: $Y, z: 0.0},
    orientation: {x: 0.0, y: 0.0, z: $Z, w: $W}
  }
}
}" --feedback

echo "已发送: 位置($X, $Y), 朝向($YAW 度)"
