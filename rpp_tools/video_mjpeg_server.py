#!/usr/bin/env python3
import threading
import io

from flask import Flask, Response
import rclpy
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import CompressedImage, Image

import numpy as np
from PIL import Image as PILImage

HOST = "127.0.0.1"
PORT = 8081

FRONT_TOPIC = "/camera/front_camera/color/image_raw/compressed"
YOLO_TOPIC = "/yolo/dbg_image"

front_frame = None
yolo_frame = None
front_count = 0
yolo_count = 0

front_lock = threading.Lock()
yolo_lock = threading.Lock()


def ros_image_to_jpeg_bytes(msg: Image) -> bytes | None:
    """
    把 sensor_msgs/Image 转成 JPEG bytes
    支持常见编码：rgb8 / bgr8 / rgba8 / bgra8 / mono8
    """
    try:
        width = msg.width
        height = msg.height
        encoding = msg.encoding.lower()
        data = msg.data

        if encoding == "rgb8":
            arr = np.frombuffer(data, dtype=np.uint8).reshape((height, width, 3))
            img = PILImage.fromarray(arr, mode="RGB")

        elif encoding == "bgr8":
            arr = np.frombuffer(data, dtype=np.uint8).reshape((height, width, 3))
            arr = arr[:, :, ::-1]  # BGR -> RGB
            img = PILImage.fromarray(arr, mode="RGB")

        elif encoding == "rgba8":
            arr = np.frombuffer(data, dtype=np.uint8).reshape((height, width, 4))
            img = PILImage.fromarray(arr, mode="RGBA").convert("RGB")

        elif encoding == "bgra8":
            arr = np.frombuffer(data, dtype=np.uint8).reshape((height, width, 4))
            arr = arr[:, :, [2, 1, 0, 3]]  # BGRA -> RGBA
            img = PILImage.fromarray(arr, mode="RGBA").convert("RGB")

        elif encoding == "mono8":
            arr = np.frombuffer(data, dtype=np.uint8).reshape((height, width))
            img = PILImage.fromarray(arr, mode="L").convert("RGB")

        else:
            print(f"[WARN] unsupported yolo image encoding: {msg.encoding}")
            return None

        buf = io.BytesIO()
        img.save(buf, format="JPEG", quality=85)
        return buf.getvalue()

    except Exception as e:
        print(f"[ERROR] ros_image_to_jpeg_bytes failed: {e}")
        return None


class MultiImageServer(Node):
    def __init__(self):
        super().__init__("multi_image_snapshot_server")

        self.front_sub = self.create_subscription(
            CompressedImage,
            FRONT_TOPIC,
            self.front_callback,
            qos_profile_sensor_data
        )

        self.yolo_sub = self.create_subscription(
            Image,
            YOLO_TOPIC,
            self.yolo_callback,
            qos_profile_sensor_data
        )

        self.get_logger().info(f"Subscribed front topic: {FRONT_TOPIC}")
        self.get_logger().info(f"Subscribed yolo topic:  {YOLO_TOPIC}")

    def front_callback(self, msg: CompressedImage):
        global front_frame, front_count
        with front_lock:
            front_frame = bytes(msg.data)
            front_count += 1

    def yolo_callback(self, msg: Image):
        global yolo_frame, yolo_count
        jpeg_bytes = ros_image_to_jpeg_bytes(msg)
        if jpeg_bytes is None:
            return
        with yolo_lock:
            yolo_frame = jpeg_bytes
            yolo_count += 1


def ros_spin():
    rclpy.init()
    node = MultiImageServer()
    try:
        rclpy.spin(node)
    finally:
        node.destroy_node()
        rclpy.shutdown()


app = Flask(__name__)


@app.route("/front_snapshot.jpg")
def front_snapshot():
    with front_lock:
        frame = front_frame
    if frame is None:
        return Response(status=503)
    return Response(
        frame,
        mimetype="image/jpeg",
        headers={
            "Cache-Control": "no-cache, no-store, must-revalidate",
            "Pragma": "no-cache",
            "Expires": "0",
            "Access-Control-Allow-Origin": "*"
        }
    )


@app.route("/yolo_snapshot.jpg")
def yolo_snapshot():
    with yolo_lock:
        frame = yolo_frame
    if frame is None:
        return Response(status=503)
    return Response(
        frame,
        mimetype="image/jpeg",
        headers={
            "Cache-Control": "no-cache, no-store, must-revalidate",
            "Pragma": "no-cache",
            "Expires": "0",
            "Access-Control-Allow-Origin": "*"
        }
    )


@app.route("/stats")
def stats():
    with front_lock:
        fc = front_count
        fs = 0 if front_frame is None else len(front_frame)
    with yolo_lock:
        yc = yolo_count
        ys = 0 if yolo_frame is None else len(yolo_frame)

    return f"front_count={fc}, front_size={fs}, yolo_count={yc}, yolo_size={ys}\n"


if __name__ == "__main__":
    ros_thread = threading.Thread(target=ros_spin, daemon=True)
    ros_thread.start()
    app.run(host=HOST, port=PORT, threaded=True)
