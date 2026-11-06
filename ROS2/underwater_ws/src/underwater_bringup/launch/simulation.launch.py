from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='underwater_perception',
            executable='sonar_target_tracker',
            name='sonar_target_tracker',
            output='screen',
        ),
        Node(
            package='underwater_navigation',
            executable='auv_controller',
            name='auv_controller',
            output='screen',
        ),
    ])
