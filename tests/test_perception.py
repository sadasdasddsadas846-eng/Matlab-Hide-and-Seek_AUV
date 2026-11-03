"""Tests for perception module."""

import pytest
import numpy as np
from perception.perception.target_tracking import TargetTracker
from perception.perception.image_processing import ImageProcessor


class TestTargetTracker:
    """Test cases for target tracking."""

    def test_update_returns_dict(self):
        tracker = TargetTracker()
        detections = [(1.0, 2.0, -3.0), (4.0, 5.0, -6.0)]
        tracks = tracker.update(detections)
        assert isinstance(tracks, dict)
        assert len(tracks) == 2

    def test_get_active_tracks_empty(self):
        tracker = TargetTracker()
        assert len(tracker.get_active_tracks()) == 0


class TestImageProcessor:
    """Test cases for image processing."""

    def test_preprocess_preserves_shape(self):
        processor = ImageProcessor()
        dummy_image = np.zeros((480, 640, 3), dtype=np.uint8)
        result = processor.preprocess(dummy_image)
        assert result.shape == dummy_image.shape

    def test_preprocess_returns_copy(self):
        processor = ImageProcessor()
        dummy_image = np.zeros((480, 640, 3), dtype=np.uint8)
        result = processor.preprocess(dummy_image)
        assert result is not dummy_image

