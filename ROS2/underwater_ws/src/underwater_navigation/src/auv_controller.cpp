#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <nav_msgs/msg/odometry.hpp>

class AUVControllerNode : public rclcpp::Node
{
public:
  AUVControllerNode() : Node("auv_controller")
  {
    cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/seeker/cmd_vel", 10);
    odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
      "/seeker/odometry", 10,
      std::bind(&AUVControllerNode::odom_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Underwater Navigation AUV Controller initialized.");
  }

private:
  void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
  {
    (void)msg;
  }

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<AUVControllerNode>());
  rclcpp::shutdown();
  return 0;
}

