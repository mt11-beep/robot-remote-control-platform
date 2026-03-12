#!/usr/bin/env python3
import threading

from flask import Flask, Response
import rclpy
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import CompressedImage

HOST = "127.0.0.1"
PORT = 8081
TOPIC = "/camera/front_camera/color/image_raw/compressed"

latest_frame = None
frame_count = 0
frame_lock = threading.Lock()


class CompressedImageSubscriber(Node):
    def __init__(self):
        super().__init__("compressed_image_snapshot_server")
        self.subscription = self.create_subscription(
            CompressedImage,
            TOPIC,
            self.image_callback,
            qos_profile_sensor_data
        )
        self.get_logger().info(f"Subscribed to {TOPIC} with sensor_data QoS")

    def image_callback(self, msg: CompressedImage):
        global latest_frame, frame_count
        with frame_lock:
            latest_frame = bytes(msg.data)
            frame_count += 1


def ros_spin():
    rclpy.init()
    node = CompressedImageSubscriber()
    try:
        rclpy.spin(node)
    finally:
        node.destroy_node()
        rclpy.shutdown()


app = Flask(__name__)


@app.route("/snapshot.jpg")
def snapshot():
    global latest_frame
    with frame_lock:
        frame = latest_frame

    if frame is None:
        return Response(status=503)

    return Response(
        frame,
        mimetype="image/jpeg",
        headers={
            "Cache-Control": "no-cache, no-store, must-revalidate",
            "Pragma": "no-cache",
            "Expires": "0"
        }
    )


@app.route("/stats")
def stats():
    with frame_lock:
        fc = frame_count
        size = 0 if latest_frame is None else len(latest_frame)
    return f"frame_count={fc}, latest_size={size}\n"


if __name__ == "__main__":
    ros_thread = threading.Thread(target=ros_spin, daemon=True)
    ros_thread.start()
    app.run(host=HOST, port=PORT, threaded=True)

