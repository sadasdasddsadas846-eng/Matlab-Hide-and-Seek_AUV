#include <rclcpp/rclcpp.hpp>
#include <underwater_interfaces/msg/sonar_scan.hpp>

class SonarTargetTrackerNode : public rclcpp::Node
{
public:
  SonarTargetTrackerNode() : Node("sonar_target_tracker")
  {
    sonar_sub_ = this->create_subscription<underwater_interfaces::msg::SonarScan>(
      "/seeker/sonar/scan", 10,
      std::bind(&SonarTargetTrackerNode::sonar_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Underwater Perception Sonar Target Tracker initialized.");
  }

private:
  void sonar_callback(const underwater_interfaces::msg::SonarScan::SharedPtr msg)
  {
    (void)msg;
  }

  rclcpp::Subscription<underwater_interfaces::msg::SonarScan>::SharedPtr sonar_sub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SonarTargetTrackerNode>());
  rclcpp::shutdown();
  return 0;
}

