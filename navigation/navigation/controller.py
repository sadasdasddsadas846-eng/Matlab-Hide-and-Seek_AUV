"""
AUV Controller Module
Handles low-level control of the underwater drone.
"""

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist


class AUVController(Node):
    """Controller node for AUV movement."""

    def __init__(self):
        super().__init__('auv_controller')
        self.publisher_ = self.create_publisher(Twist, '/cmd_vel', 10)
        self.get_logger().info('AUV Controller initialized')

    def move(self, linear_x=0.0, linear_y=0.0, linear_z=0.0,
             angular_x=0.0, angular_y=0.0, angular_z=0.0):
        """Send velocity command to AUV."""
        msg = Twist()
        msg.linear.x = linear_x
        msg.linear.y = linear_y
        msg.linear.z = linear_z
        msg.angular.x = angular_x
        msg.angular.y = angular_y
        msg.angular.z = angular_z
        self.publisher_.publish(msg)

    def stop(self):
        """Stop all AUV movement."""
        self.move()


def main(args=None):
    rclpy.init(args=args)
    controller = AUVController()
    rclpy.spin(controller)
    controller.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()

