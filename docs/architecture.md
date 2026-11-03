# Arsitektur Sistem

## Overview

Sistem ini terdiri dari tiga modul utama yang terintegrasi melalui ROS 2:

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Simulation │◄───►│  Navigation │◄───►│  Perception │
│  (Gazebo)   │     │  (ROS 2)    │     │  (ROS 2)    │
└─────────────┘     └─────────────┘     └─────────────┘
```

## Modul

### 1. Simulation
- Lingkungan bawah air 3D (Gazebo)
- Model BlueROV2
- Obstacles dan target objects

### 2. Navigation
- Controller untuk pergerakan drone
- Path planning algorithm
- Obstacle avoidance

### 3. Perception
- Object detection (YOLO/SSD)
- Target tracking
- Image processing pipeline

