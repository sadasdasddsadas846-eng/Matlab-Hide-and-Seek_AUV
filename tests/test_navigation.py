"""Tests for navigation module."""

import pytest
import numpy as np
from navigation.navigation.path_planning import PathPlanner
from navigation.navigation.obstacle_avoidance import ObstacleAvoider


class TestPathPlanner:
    """Test cases for path planning."""

    def test_plan_path_returns_list(self):
        planner = PathPlanner()
        path = planner.plan_path((0, 0, 0), (10, 10, -5))
        assert isinstance(path, list)
        assert len(path) >= 2

    def test_collision_free_no_obstacles(self):
        planner = PathPlanner()
        assert planner.is_collision_free((5, 5, -3))

    def test_collision_detected(self):
        planner = PathPlanner()
        planner.set_obstacles([(5, 5, -3, 2.0)])
        assert not planner.is_collision_free((5, 5, -3))


class TestObstacleAvoider:
    """Test cases for obstacle avoidance."""

    def test_no_obstacles(self):
        avoider = ObstacleAvoider()
        vel = avoider.compute_avoidance_velocity(
            (0, 0, 0), (10, 0, 0), []
        )
        assert vel[0] > 0  # Should move toward goal

    def test_max_speed_limit(self):
        avoider = ObstacleAvoider(max_speed=1.0)
        vel = avoider.compute_avoidance_velocity(
            (0, 0, 0), (100, 0, 0), []
        )
        speed = np.linalg.norm(vel)
        assert speed <= 1.0 + 1e-6

