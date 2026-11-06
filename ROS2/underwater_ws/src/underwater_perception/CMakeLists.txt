cmake_minimum_required(VERSION 3.8)
project(underwater_perception)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(underwater_interfaces REQUIRED)

add_executable(sonar_target_tracker src/sonar_target_tracker.cpp)
ament_target_dependencies(sonar_target_tracker
  rclcpp
  std_msgs
  sensor_msgs
  geometry_msgs
  underwater_interfaces
)

install(TARGETS
  sonar_target_tracker
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
