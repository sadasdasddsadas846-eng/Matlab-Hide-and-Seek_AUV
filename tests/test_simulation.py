"""Tests for simulation module."""

import pytest


class TestSimulation:
    """Test cases for the simulation environment."""

    def test_world_file_exists(self):
        """Test that the world file is present."""
        import os
        assert os.path.exists('simulation/worlds/underwater_world.sdf')

    def test_world_file_valid_xml(self):
        """Test that the world file is valid XML."""
        import xml.etree.ElementTree as ET
        tree = ET.parse('simulation/worlds/underwater_world.sdf')
        root = tree.getroot()
        assert root.tag == 'sdf'

