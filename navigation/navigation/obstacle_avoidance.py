"""
Obstacle Avoidance Module
Real-time obstacle avoidance for underwater drone.
"""

import numpy as np


class ObstacleAvoider:
    """Reactive obstacle avoidance using potential field method."""

    def __init__(self, safe_distance=2.0, max_speed=1.0):
        self.safe_distance = safe_distance
        self.max_speed = max_speed

    def compute_avoidance_velocity(self, current_pos, goal_pos, obstacles):
        """
        Compute velocity vector that avoids obstacles while moving toward goal.

        Args:
            current_pos: (x, y, z) current position
            goal_pos: (x, y, z) goal position
            obstacles: list of (x, y, z, radius) obstacle positions

        Returns:
            (vx, vy, vz) velocity command
        """
        # Attractive force toward goal
        attract = np.array(goal_pos) - np.array(current_pos)
        attract_norm = np.linalg.norm(attract)
        if attract_norm > 0:
            attract = attract / attract_norm

        # Repulsive force from obstacles
        repulse = np.zeros(3)
        for obs in obstacles:
            diff = np.array(current_pos) - np.array(obs[:3])
            dist = np.linalg.norm(diff)
            if dist < self.safe_distance and dist > 0:
                repulse += (diff / dist) * (1.0 / dist - 1.0 / self.safe_distance)

        # Combine forces
        velocity = attract + repulse
        speed = np.linalg.norm(velocity)
        if speed > self.max_speed:
            velocity = velocity / speed * self.max_speed

        return tuple(velocity)

