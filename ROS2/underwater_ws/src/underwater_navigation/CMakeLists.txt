cmake_minimum_required(VERSION 3.8)
project(underwater_navigation)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(underwater_interfaces REQUIRED)

add_executable(auv_controller src/auv_controller.cpp)
ament_target_dependencies(auv_controller
  rclcpp
  std_msgs
  geometry_msgs
  nav_msgs
  underwater_interfaces
)

install(TARGETS
  auv_controller
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
