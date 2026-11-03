# Panduan Instalasi & Setup Environment

## 1. Persyaratan Sistem

- **Sistem Operasi:** Windows 10/11 (64-bit) atau Ubuntu 22.04 LTS
- **Game Engine:** Unreal Engine 5.3 atau lebih baru
- **Robot Operating System:** ROS 2 Humble Hawksbill / Iron Irwini
- **Visual Studio:** Visual Studio 2022 (dengan paket *Desktop Development with C++* dan *Game Development with C++*)
- **DDS Bridge:** [rclUE](https://github.com/rapyuta-robotics/rclUE) (Unreal Engine Plugin untuk ROS 2)

---

## 2. Setup Unreal Engine Project

1. Clone repository:
   ```bash
   git clone https://github.com/sadasdasddsadas846-eng/Matlab-Hide-and-Seek_AUV.git
   cd Matlab-Hide-and-Seek_AUV
   ```
2. Buka folder `UnrealProject`:
   - Klik kanan pada `UnderwaterSim.uproject` -> Pilih **Generate Visual Studio project files**.
   - Buka `UnderwaterSim.sln` di Visual Studio 2022.
   - Set build configuration ke `Development Editor` | `Win64`.
   - Build Solution (`Ctrl + Shift + B`).
3. Buka project di Unreal Engine Editor:
   - Double click `UnderwaterSim.uproject`.
   - Level utama berada di `Content/Maps/UnderwaterArena`.

---

## 3. Setup ROS 2 Workspace

Buka terminal ROS 2 di Linux (atau WSL2 pada Windows):

```bash
cd ROS2/underwater_ws
colcon build --packages-select underwater_interfaces
source install/setup.bash
```

