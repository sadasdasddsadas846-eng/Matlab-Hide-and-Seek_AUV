# Panduan Instalasi

## Prerequisites

- Ubuntu 22.04 LTS
- ROS 2 Humble
- Gazebo Garden
- Python 3.10+

## Langkah Instalasi

### 1. Install ROS 2 Humble

```bash
sudo apt update && sudo apt install -y ros-humble-desktop
```

### 2. Install Gazebo Garden

```bash
sudo apt install -y gz-garden
```

### 3. Clone Repository

```bash
git clone https://github.com/sadasdasddsadas846-eng/Matlab-Hide-and-Seek_AUV.git
cd Matlab-Hide-and-Seek_AUV
```

### 4. Install Dependencies

```bash
pip install -r requirements.txt
```

### 5. Build Workspace

```bash
colcon build
source install/setup.bash
```

