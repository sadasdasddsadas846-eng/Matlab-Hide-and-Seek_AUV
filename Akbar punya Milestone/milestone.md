# Progress Report – Underwater Drone Hide and Seek
## Member 2: ROS 2 Navigation Engineer

**Project:** Underwater Drone Hide and Seek  
**Role:** ROS 2 Navigation Engineer  
**Development Environment:** Windows 11 + WSL2 + Ubuntu 24.04 LTS  
**ROS Distribution:** ROS 2 Jazzy  
**Programming Language:** Python  
**Package:** `underwater_navigation`

---

## 1. Project Overview

Tugas Member 2 berfokus pada pengembangan sistem navigasi dan kontrol gerakan Autonomous Underwater Vehicle (AUV) menggunakan ROS 2.

Ruang lingkup pekerjaan meliputi:

- Pembuatan ROS 2 workspace dan package.
- Pengembangan motion command untuk mengontrol gerakan AUV.
- Implementasi PID Controller dan sistem kontrol gerakan 6-DOF.
- Integrasi data odometry dan IMU.
- Waypoint Navigation.
- Path Planning menggunakan algoritma A* atau RRT*.
- Obstacle Avoidance.
- Integrasi sistem navigasi dengan Unreal Engine 5.

## 2. Development Environment Setup

Lingkungan pengembangan telah berhasil disiapkan menggunakan Windows Subsystem for Linux (WSL2).

| Komponen | Konfigurasi |
|---|---|
| Operating System | Ubuntu 24.04.4 LTS |
| WSL Version | WSL 2 |
| ROS Distribution | ROS 2 Jazzy |
| Build System | ament |
| Build Type | ament_python |
| Programming Language | Python |
| Workspace Location | `/home/akbar/ros2_ws` |

### 2.1 ROS 2 Installation

Tahapan instalasi yang telah dilakukan:

1. Memastikan Ubuntu 24.04 berjalan pada WSL2.
2. Mengaktifkan Ubuntu Universe repository.
3. Menambahkan ROS 2 APT Repository.
4. Menginstal ROS 2 Jazzy Desktop.
5. Menginstal ROS Development Tools.
6. Mengaktifkan ROS 2 environment.

Perintah untuk mengaktifkan ROS 2:

```bash
source /opt/ros/jazzy/setup.bash
```

Environment juga telah ditambahkan ke `.bashrc` agar aktif otomatis ketika terminal Ubuntu dibuka.

Verifikasi instalasi:

```bash
printenv ROS_DISTRO
```

Output:

```text
jazzy
```

ROS 2 CLI juga telah diuji menggunakan:

```bash
ros2 --help
```

### 2.2 ROS 2 Communication Test

Pengujian komunikasi dasar dilakukan menggunakan dua node bawaan ROS 2:

**Terminal 1 – Talker**

```bash
ros2 run demo_nodes_cpp talker
```

**Terminal 2 – Listener**

```bash
ros2 run demo_nodes_py listener
```

Hasil pengujian menunjukkan bahwa Listener berhasil menerima pesan yang diterbitkan oleh Talker melalui topic `/chatter`.

Pengujian ini memvalidasi komunikasi dasar Publisher dan Subscriber pada ROS 2.

## 3. Workspace and Package Creation

Workspace ROS 2 telah berhasil dibuat.

Lokasi workspace:

```text
/home/akbar/ros2_ws
```

Perintah pembuatan workspace:

```bash
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws
```

### 3.1 Package Creation

Package utama untuk pengembangan navigasi AUV:

```text
underwater_navigation
```

Package dibuat menggunakan perintah:

```bash
cd ~/ros2_ws/src

ros2 pkg create underwater_navigation \
--build-type ament_python \
--dependencies rclpy geometry_msgs nav_msgs sensor_msgs
```

Dependensi yang digunakan:

| Dependency | Fungsi |
|---|---|
| `rclpy` | ROS 2 Python Client Library |
| `geometry_msgs` | Pesan geometri, termasuk Twist |
| `nav_msgs` | Pesan navigasi, termasuk Odometry |
| `sensor_msgs` | Pesan sensor, termasuk IMU |

### 3.2 Current Directory Structure

Struktur package saat ini:

```text
ros2_ws/
└── src/
    └── underwater_navigation/
        ├── package.xml
        ├── resource/
        │   └── underwater_navigation
        ├── setup.cfg
        ├── setup.py
        ├── test/
        │   ├── test_copyright.py
        │   ├── test_flake8.py
        │   └── test_pep257.py
        └── underwater_navigation/
            └── __init__.py
```

Package telah berhasil dibuat tanpa error.

## 4. ROS 2 Workspace Access

Workspace disimpan di filesystem Linux WSL, bukan di direktori Windows `/mnt/c`.

Lokasi workspace:

```text
/home/akbar/ros2_ws
```

Akses melalui Windows File Explorer:

```text
\\wsl$\Ubuntu\home\akbar\ros2_ws
```

Perintah untuk kembali ke workspace setelah membuka terminal Ubuntu:

```bash
cd ~/ros2_ws
```

Perintah untuk masuk langsung ke package:

```bash
cd ~/ros2_ws/src/underwater_navigation
```

Perintah untuk membuka folder aktif menggunakan VS Code:

```bash
code .
```

## 5. Current Progress

| No. | Task | Status |
|---|---|---|
| 1 | WSL2 and Ubuntu Setup | Completed |
| 2 | ROS 2 Jazzy Installation | Completed |
| 3 | ROS 2 Environment Configuration | Completed |
| 4 | ROS 2 Talker-Listener Test | Completed |
| 5 | ROS 2 Workspace Creation | Completed |
| 6 | `underwater_navigation` Package Creation | Completed |
| 7 | Motion Command Node | Not Started |
| 8 | PID Controller / 6-DOF Control | Not Started |
| 9 | Odometry and IMU Integration | Not Started |
| 10 | Waypoint Navigation | Not Started |
| 11 | Path Planning | Not Started |
| 12 | Obstacle Avoidance | Not Started |
| 13 | Unreal Engine 5 Integration | Not Started |
| 14 | ROS 2 Launch and Bringup | Not Started |

## 6. Next Development Plan

Pengembangan selanjutnya akan dilakukan secara bertahap.

### Phase 1 – Motion Command

Membuat node `motion_command.py` untuk menerbitkan perintah gerakan AUV melalui topic:

```text
/cmd_vel
```

Gerakan yang direncanakan:

- Forward
- Backward
- Left
- Right
- Up
- Down
- Stop

Menggunakan message type:

```text
geometry_msgs/msg/Twist
```

### Phase 2 – Motion Control

Mengembangkan sistem kontrol gerakan AUV, meliputi:

- PID Controller.
- Kontrol gerakan 6-DOF.
- Pengaturan kecepatan linear dan angular.
- Pengujian respons terhadap target gerakan.

### Phase 3 – Sensor Integration

Mengintegrasikan data sensor melalui topic:

```text
/odom
/imu/data
```

Data tersebut digunakan sebagai masukan sistem navigasi dan kontrol.

### Phase 4 – Navigation and Planning

Mengembangkan:

- Waypoint Navigation.
- A* atau RRT* Path Planning.
- Reactive Obstacle Avoidance.
- Integrasi target position dari Member 3.

### Phase 5 – Simulation Integration

Menghubungkan sistem navigasi ROS 2 dengan Unreal Engine 5.

Integrasi mencakup komunikasi perintah gerakan, data sensor, odometry, dan informasi target.

---

## 7. Important Notes

- Seluruh pengembangan ROS 2 dilakukan melalui Ubuntu WSL2.
- Jangan menjalankan perintah `ros2` langsung dari PowerShell Windows kecuali sudah dikonfigurasi secara khusus.
- Workspace disimpan di filesystem Linux untuk mendukung performa build ROS 2.
- ROS 2 Jazzy harus aktif sebelum menjalankan node. Aktivasi otomatis sudah dikonfigurasi melalui `.bashrc`.
- Pengembangan algoritma navigasi belum dimulai. Tahap yang telah diselesaikan masih berupa persiapan environment, pengujian komunikasi, dan pembuatan package.
- Integrasi Unreal Engine 5 dan sistem kontrol AUV akan dilakukan setelah komponen navigasi dasar dikembangkan.

---

**Last Updated:** 8 November 2026  
**Current Milestone:** ROS 2 Environment Setup and Package Initialization  
**Next Milestone:** Motion Command Node Development
