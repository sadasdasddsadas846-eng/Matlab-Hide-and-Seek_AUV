"""
Integration Launch File
Launches the full underwater hide-and-seek simulation.
"""

from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        # TODO: Add Gazebo simulation launch
        # TODO: Add navigation node
        # TODO: Add perception node

        Node(
            package='navigation',
            executable='controller',
            name='auv_controller',
            output='screen',
        ),
        Node(
            package='perception',
            executable='object_detector',
            name='object_detector',
            output='screen',
        ),
    ])

