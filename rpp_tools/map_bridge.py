#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from nav_msgs.msg import OccupancyGrid
from rclpy.qos import QoSProfile, ReliabilityPolicy, DurabilityPolicy, HistoryPolicy

class MapBridge(Node):
    def __init__(self):
        super().__init__('map_bridge')

        sub_qos = QoSProfile(
            history=HistoryPolicy.KEEP_LAST,
            depth=1,
            reliability=ReliabilityPolicy.RELIABLE,
            durability=DurabilityPolicy.TRANSIENT_LOCAL
        )

        pub_qos = QoSProfile(
            history=HistoryPolicy.KEEP_LAST,
            depth=1,
            reliability=ReliabilityPolicy.RELIABLE,
            durability=DurabilityPolicy.VOLATILE
        )

        self.latest_map = None

        self.sub = self.create_subscription(
            OccupancyGrid,
            '/map',
            self.map_callback,
            sub_qos
        )

        self.pub = self.create_publisher(
            OccupancyGrid,
            '/web_map',
            pub_qos
        )

        self.timer = self.create_timer(1.0, self.timer_callback)
        self.get_logger().info('map_bridge started')

    def map_callback(self, msg):
        self.latest_map = msg
        self.pub.publish(msg)
        self.get_logger().info(
            f"republished /web_map: {msg.info.width}x{msg.info.height}"
        )

    def timer_callback(self):
        if self.latest_map is not None:
            self.pub.publish(self.latest_map)

def main():
    rclpy.init()
    node = MapBridge()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
