# Progress Report – Underwater Drone Hide and Seek
## Member 2: ROS 2 Navigation Engineer

**Project:** Underwater Drone Hide and Seek  
**Role:** ROS 2 Navigation Engineer  
**Development Environment:** Windows 11 + WSL2 + Ubuntu 24.04 LTS  
**ROS Distribution:** ROS 2 Jazzy  
**Programming Language:** Python  
**Build System:** ament  
**Build Type:** ament_python  
**ROS 2 Package:** `underwater_navigation`  
**ROS 2 Workspace:** `~/ros2_ws`  
**MATLAB Version:** MATLAB R2026a  
**Simulation Environment:** Simulink R2026a  

---

## 1. Project Overview

Member 2 bertanggung jawab mengembangkan sistem navigasi dan kontrol gerakan Autonomous Underwater Vehicle (AUV) menggunakan ROS 2.

Pengembangan difokuskan pada komunikasi perintah gerakan, sistem kontrol berbasis feedback, pemrosesan informasi sensor, serta algoritma navigasi yang nantinya akan diintegrasikan dengan simulasi Unreal Engine 5.

Ruang lingkup pekerjaan meliputi:

- Setup ROS 2 workspace dan package.
- Pengembangan motion command untuk mengontrol gerakan AUV.
- Implementasi PID Controller dan sistem kontrol gerakan 6-DOF.
- Integrasi data odometry dan IMU.
- Waypoint Navigation.
- Path Planning menggunakan algoritma A* atau RRT*.
- Reactive Obstacle Avoidance.
- Integrasi target position dari Member 3.
- Integrasi sistem navigasi ROS 2 dengan Unreal Engine 5.
- Pengembangan ROS 2 launch dan bringup.

Pengembangan dilakukan secara bertahap, dimulai dari persiapan environment, implementasi motion command, pengembangan sistem kontrol, hingga integrasi dengan simulasi.

## 2. Development Environment Setup

Lingkungan pengembangan telah berhasil disiapkan menggunakan Windows Subsystem for Linux (WSL2).

| Komponen | Konfigurasi |
|---|---|
| Host Operating System | Windows 11 |
| Virtualization Environment | WSL2 |
| Linux Distribution | Ubuntu 24.04.4 LTS |
| WSL Version | WSL 2 |
| ROS Distribution | ROS 2 Jazzy |
| Build System | ament |
| Build Type | ament_python |
| Programming Language | Python |
| Workspace Location | `/home/akbar/ros2_ws` |
| MATLAB | MATLAB R2026a |
| Simulation Tool | Simulink R2026a |

### 2.1 ROS 2 Installation

Tahapan instalasi yang telah dilakukan:

1. Memastikan Ubuntu 24.04.4 LTS berjalan menggunakan WSL2.
2. Menambahkan ROS 2 APT Repository.
3. Menginstal ROS 2 Jazzy Desktop.
4. Menginstal ROS Development Tools.
5. Mengonfigurasi ROS 2 environment.
6. Mengaktifkan environment secara otomatis melalui `.bashrc`.

Perintah untuk mengaktifkan ROS 2 environment:

```bash
source /opt/ros/jazzy/setup.bash
```

Konfigurasi tersebut telah ditambahkan ke `.bashrc` agar ROS 2 aktif secara otomatis ketika terminal Ubuntu dibuka.

Verifikasi instalasi dilakukan menggunakan:

```bash
printenv ROS_DISTRO
```

Output:

```text
jazzy
```

ROS 2 CLI juga telah berhasil diuji menggunakan:

```bash
ros2 --help
```

### 2.2 ROS 2 Communication Test

Pengujian komunikasi dasar dilakukan menggunakan dua node bawaan ROS 2.

**Terminal 1 – Talker**

```bash
ros2 run demo_nodes_cpp talker
```

**Terminal 2 – Listener**

```bash
ros2 run demo_nodes_py listener
```

Hasil pengujian menunjukkan bahwa node Listener berhasil menerima pesan yang diterbitkan oleh node Talker melalui topic `/chatter`.

Pengujian ini memvalidasi komunikasi dasar Publisher dan Subscriber pada ROS 2.

## 3. Workspace and Package Creation

Workspace ROS 2 berhasil dibuat pada filesystem Linux WSL.

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

Package utama untuk pengembangan navigasi AUV diberi nama:

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

Dependensi package:

| Dependency | Fungsi |
|---|---|
| `rclpy` | ROS 2 Python Client Library |
| `geometry_msgs` | Pesan geometri, termasuk `Twist` |
| `nav_msgs` | Pesan navigasi, termasuk `Odometry` |
| `sensor_msgs` | Pesan sensor, termasuk `Imu` |

### 3.2 Current Directory Structure

Struktur package setelah implementasi Motion Command:

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
            ├── __init__.py
            └── motion_command.py
```

Package berhasil dibuat dan dibangun menggunakan `colcon`.

Perintah build:

```bash
cd ~/ros2_ws

colcon build --packages-select underwater_navigation

source install/setup.bash
```

## 4. Workspace Access and Development Workflow

Workspace disimpan di filesystem Linux WSL, bukan langsung di direktori Windows `/mnt/c`.

Lokasi workspace:

```text
/home/akbar/ros2_ws
```

Akses melalui Windows File Explorer:

```text
\\wsl$\Ubuntu\home\akbar\ros2_ws
```

Perintah untuk memasuki workspace:

```bash
cd ~/ros2_ws
```

Perintah untuk masuk ke package:

```bash
cd ~/ros2_ws/src/underwater_navigation
```

Perintah untuk membuka folder aktif menggunakan VS Code:

```bash
code .
```

Workflow dasar setelah mengubah kode:

```bash
cd ~/ros2_ws

colcon build --packages-select underwater_navigation

source install/setup.bash
```

Kemudian node dapat dijalankan menggunakan:

```bash
ros2 run underwater_navigation motion_command
```

## 5. Current Development Progress

| No. | Task | Status |
|---:|---|---|
| 1 | WSL2 and Ubuntu Setup | Completed |
| 2 | ROS 2 Jazzy Installation | Completed |
| 3 | ROS 2 Environment Configuration | Completed |
| 4 | ROS 2 Talker-Listener Test | Completed |
| 5 | ROS 2 Workspace Creation | Completed |
| 6 | `underwater_navigation` Package Creation | Completed |
| 7 | Motion Command Node | Completed |
| 8 | PID Controller / 6-DOF Control | Completed (Initial Implementation and ROS 2–Simulink Integration) |
| 9 | Odometry and IMU Integration | Not Started |
| 10 | Waypoint Navigation | Not Started |
| 11 | Path Planning | Not Started |
| 12 | Obstacle Avoidance | Not Started |
| 13 | Unreal Engine 5 Integration | Not Started |
| 14 | ROS 2 Launch and Bringup | Not Started |

**Catatan:** Status Task 8 menunjukkan bahwa implementasi awal enam kanal kontrol independen dan integrasi ROS 2–Simulink telah selesai diuji. Pengembangan dinamika AUV 6-DOF yang terkopel secara fisik masih menjadi pekerjaan lanjutan.

### 5.1 Motion Command Implementation

Node pertama dalam package `underwater_navigation` telah berhasil dikembangkan menggunakan Python dan `rclpy`.

File:

```text
underwater_navigation/motion_command.py
```

Node name:

```text
motion_command
```

Node ini menggunakan Publisher ROS 2 dengan konfigurasi:

| Parameter | Value |
|---|---|
| Topic | `/cmd_vel` |
| Message Type | `geometry_msgs/msg/Twist` |
| Publisher Queue Depth | 10 |
| Input Method | Keyboard |
| Node Name | `motion_command` |

Publisher dibuat menggunakan:

```python
self.publisher_ = self.create_publisher(
    Twist,
    '/cmd_vel',
    10
)
```

### 5.2 Keyboard Motion Commands

Sistem perintah gerakan telah diimplementasikan menggunakan input keyboard pada terminal.

Mapping perintah mencakup enam sumbu gerakan AUV:

| Key | Motion Command | Twist Component |
|---|---|---|
| `w` | Forward | `linear.x = +0.5` |
| `s` | Backward | `linear.x = -0.5` |
| `a` | Left | `linear.y = +0.5` |
| `d` | Right | `linear.y = -0.5` |
| `q` | Up | `linear.z = +0.5` |
| `e` | Down | `linear.z = -0.5` |
| `j` | Roll Positive | `angular.x = +0.5` |
| `l` | Roll Negative | `angular.x = -0.5` |
| `i` | Pitch Positive | `angular.y = +0.5` |
| `k` | Pitch Negative | `angular.y = -0.5` |
| `u` | Yaw Positive | `angular.z = +0.5` |
| `o` | Yaw Negative | `angular.z = -0.5` |
| `x` | Stop | All components = 0.0 |

Nilai linear menggunakan satuan m/s, sedangkan nilai angular menggunakan rad/s.

Setiap perintah akan mengubah satu komponen gerakan, sementara komponen lainnya tetap bernilai nol.

Saat ini, input keyboard masih menggunakan metode terminal line input, sehingga pengguna perlu menekan Enter setelah memasukkan perintah.

### 5.3 Motion Command Testing

Pengujian dilakukan dengan menjalankan node:

```bash
ros2 run underwater_navigation motion_command
```

Kemudian memantau pesan yang diterbitkan menggunakan:

```bash
ros2 topic echo /cmd_vel
```

Node juga telah diverifikasi menggunakan:

```bash
ros2 node list
```

dan:

```bash
ros2 node info /motion_command
```

Hasil pengujian menunjukkan bahwa:

- Node `/motion_command` berhasil terdaftar di ROS 2.
- Publisher `/cmd_vel` berhasil dibuat.
- Pesan `geometry_msgs/msg/Twist` berhasil diterbitkan.
- Seluruh 12 perintah gerakan berhasil menghasilkan nilai komponen Twist yang sesuai.
- Perintah `x` menghasilkan seluruh komponen gerakan bernilai `0.0`.
- Pengiriman perintah dapat diterima oleh subscriber MATLAB/Simulink.

Dengan demikian, komunikasi motion command melalui ROS 2 telah berhasil diimplementasikan dan diuji untuk keenam sumbu gerakan.

### 5.4 PID Controller and 6-DOF Control Development

Pengembangan sistem kontrol gerakan AUV dilakukan menggunakan MATLAB dan Simulink R2026a.

Model awal dibuat untuk menguji sistem kontrol kecepatan pada satu sumbu gerakan, yaitu Surge.

File model awal:

```text
auv_surge_dynamics.slx
```

Model tersebut kemudian dikembangkan menjadi model dengan enam kanal kontrol independen.

File model pengembangan:

```text
auv_6dof_control.slx
```

Lokasi proyek MATLAB:

```text
D:\Documents\MatlabHide&Seek
```

#### 5.4.1 Initial Plant Model

Pengembangan dimulai menggunakan model plant sederhana orde satu:

```text
G(s) = 1 / (m*s + d)
```

Dengan parameter ilustratif:

```text
m = 1
d = 1
```

Sehingga diperoleh:

```text
G(s) = 1 / (s + 1)
```

Model ini digunakan untuk menguji respons sistem kontrol kecepatan dengan feedback.

Model awal terdiri dari:

1. Step sebagai target velocity.
2. Sum sebagai pembanding target dan actual velocity.
3. PID Controller sebagai pengendali.
4. Transfer Function sebagai plant.
5. Feedback dari output plant menuju input pembanding.
6. Scope dan To Workspace untuk pengamatan serta penyimpanan data simulasi.

Pengujian awal menggunakan konfigurasi PI:

| Parameter | Value |
|---|---:|
| Kp | 1 |
| Ki | 1 |
| Kd | 0 |

Konfigurasi ini digunakan sebagai baseline untuk pengembangan enam kanal kontrol.

**Catatan:** Parameter plant di atas merupakan parameter ilustratif untuk pengujian awal, bukan parameter hidrodinamika AUV yang telah diidentifikasi.

#### 5.4.2 Six Independent Control Channels

Model kemudian dikembangkan menjadi enam kanal kontrol yang mewakili enam derajat kebebasan gerakan AUV.

| Channel | Motion | Target Input | Output |
|---|---|---|---|
| 1 | Surge | `linear.x` | `v_out` |
| 2 | Sway | `linear.y` | `vy_out` |
| 3 | Heave | `linear.z` | `vz_out` |
| 4 | Roll | `angular.x` | `roll_out` |
| 5 | Pitch | `angular.y` | `pitch_out` |
| 6 | Yaw | `angular.z` | `yaw_out` |

Setiap kanal menggunakan subsystem controller tersendiri yang terdiri dari:

- Input target velocity.
- Input actual velocity.
- Sum untuk menghitung error.
- PI/PID Controller.
- Output force atau torque command.

Masing-masing kanal juga memiliki plant Transfer Function tersendiri.

Dengan struktur ini, keenam kanal dapat berjalan dalam satu simulasi Simulink.

#### 5.4.3 ROS 2 Integration with Simulink

Integrasi antara ROS 2 dan Simulink dilakukan menggunakan ROS Toolbox pada MATLAB.

Node ROS 2 mengirimkan perintah melalui:

```text
/cmd_vel
```

Message type:

```text
geometry_msgs/msg/Twist
```

Pada Simulink digunakan block:

```text
ROS 2 Subscribe
```

Konfigurasi block:

| Parameter | Configuration |
|---|---|
| Topic Source | Select from ROS network |
| Topic | `/cmd_vel` |
| Message Type | `geometry_msgs/Twist` |
| Sample Time | `-1` |

Pesan yang diterima kemudian diproses menggunakan Bus Selector.

Struktur pemrosesan:

```text
ROS 2 Subscribe
       |
       v
   Bus Selector
       |
       +---- linear
       |       |
       |       +---- x -> Surge Controller
       |       +---- y -> Sway Controller
       |       +---- z -> Heave Controller
       |
       +---- angular
               |
               +---- x -> Roll Controller
               +---- y -> Pitch Controller
               +---- z -> Yaw Controller
```

Dengan konfigurasi tersebut, pesan `Twist` dari ROS 2 dapat digunakan sebagai target input bagi masing-masing controller.

Integrasi ini berhasil diuji menggunakan komunikasi ROS 2 antara Ubuntu WSL2 dan MATLAB yang berjalan pada Windows.

#### 5.4.4 Six-Axis Control Output Testing

Pengujian dilakukan dengan menjalankan node `motion_command` pada Ubuntu dan model `auv_6dof_control.slx` pada Simulink.

Model dijalankan secara kontinu agar dapat menerima perintah ROS 2 selama simulasi berlangsung.

Setiap perintah dikirim secara terpisah untuk memastikan bahwa output plant mengikuti target yang diberikan.

Pengujian dilakukan terhadap 12 arah gerakan.

| No. | Axis | Command | Target | Output Verification | Result |
|---:|---|---|---:|---|---|
| 1 | Surge | `w` | +0.5 m/s | Maximum and final output = +0.5 | Pass |
| 2 | Surge | `s` | -0.5 m/s | Minimum and final output = -0.5 | Pass |
| 3 | Sway | `a` | +0.5 m/s | Maximum and final output = +0.5 | Pass |
| 4 | Sway | `d` | -0.5 m/s | Minimum and final output = -0.5 | Pass |
| 5 | Heave | `q` | +0.5 m/s | Maximum and final output = +0.5 | Pass |
| 6 | Heave | `e` | -0.5 m/s | Minimum and final output = -0.5 | Pass |
| 7 | Roll | `j` | +0.5 rad/s | Maximum and final output = +0.5 | Pass |
| 8 | Roll | `l` | -0.5 rad/s | Minimum and final output = -0.5 | Pass |
| 9 | Pitch | `i` | +0.5 rad/s | Maximum and final output = +0.5 | Pass |
| 10 | Pitch | `k` | -0.5 rad/s | Minimum and final output = -0.5 | Pass |
| 11 | Yaw | `u` | +0.5 rad/s | Maximum and final output = +0.5 | Pass |
| 12 | Yaw | `o` | -0.5 rad/s | Minimum and final output = -0.5 | Pass |

**Testing Result: 12/12 directional tests passed.**

Hasil pengujian menunjukkan bahwa:

- Semua perintah translasi berhasil diterima oleh Simulink.
- Semua perintah rotasi berhasil diterima oleh Simulink.
- Target velocity pada masing-masing controller sesuai dengan komponen pesan `Twist`.
- Output plant berhasil mengikuti target positif maupun negatif.
- Keenam kanal kontrol dapat dijalankan dalam satu model Simulink.

Pengujian ini memvalidasi alur komunikasi:

```text
ROS 2 Publisher
       |
       v
    /cmd_vel
       |
       v
Simulink ROS 2 Subscribe
       |
       v
   Bus Selector
       |
       v
Six Independent Controllers
       |
       v
Six Plant Outputs
```

#### 5.4.5 Current Model Limitations

Walaupun enam kanal kontrol telah berhasil dibuat dan diuji, model saat ini masih menggunakan enam sistem SISO independen.

Artinya, setiap sumbu dikendalikan secara terpisah dan belum memperhitungkan interaksi dinamika antar-sumbu.

Beberapa aspek yang belum dimodelkan secara lengkap:

- Coupled 6-DOF equations of motion.
- Rigid-body mass and inertia matrix.
- Added mass.
- Hydrodynamic damping.
- Buoyancy and restoring forces.
- Thruster configuration and allocation.
- Nonlinear hydrodynamic effects.

Dengan demikian, model yang dikembangkan saat ini merupakan implementasi awal enam kanal kontrol independen, bukan model dinamika AUV 6-DOF yang sepenuhnya realistis.

Pengembangan model dinamika yang lebih lengkap dapat dilakukan pada tahap selanjutnya setelah struktur kontrol dasar dan komunikasi ROS 2 berhasil divalidasi.

## 6. Next Development Plan

Pengembangan selanjutnya dibagi menjadi beberapa tahap.

### Phase 1 – Motion Command Development

**Status: Completed**

Pekerjaan yang telah dilakukan:

- Membuat `motion_command.py`.
- Membuat Publisher `/cmd_vel`.
- Menggunakan `geometry_msgs/msg/Twist`.
- Mengimplementasikan keyboard command untuk enam sumbu gerakan.
- Mengimplementasikan perintah stop.
- Menguji seluruh 12 arah gerakan.
- Memverifikasi komunikasi dengan subscriber MATLAB/Simulink.

Pengembangan lanjutan:

- Menambahkan mekanisme command timeout atau watchdog.
- Menyesuaikan konvensi koordinat gerakan dengan sistem koordinat AUV dan Unreal Engine.

### Phase 2 – Motion Control

**Status: Initial Implementation and Integration Testing Completed**

Pekerjaan yang telah dilakukan:

- Membuat model plant awal untuk pengujian kontrol kecepatan.
- Mengimplementasikan PI/PID Controller.
- Mengembangkan enam kanal kontrol independen.
- Mengintegrasikan ROS 2 Subscribe dengan Simulink.
- Menghubungkan enam komponen pesan `Twist` ke masing-masing controller.
- Melakukan pengujian output untuk 12 arah gerakan.
- Memverifikasi respons output setiap plant terhadap target positif dan negatif.

Pengembangan lanjutan:

- Mengembangkan model dinamika AUV 6-DOF yang terkopel.
- Menambahkan parameter massa, inersia, dan hidrodinamika AUV.
- Mengembangkan thruster allocation.
- Mengevaluasi sistem kontrol menggunakan model dinamika AUV yang lebih realistis.
- Melakukan pengujian robustness terhadap gangguan dan perubahan beban.

### Phase 3 – Sensor Integration

**Status: Not Started**

Mengintegrasikan data sensor sebagai feedback untuk sistem kontrol dan navigasi.

Topic yang direncanakan:

```text
/odom
/imu/data
```

Data yang digunakan meliputi:

- Posisi dan orientasi AUV.
- Kecepatan linear dan angular.
- Data percepatan dan kecepatan sudut dari IMU.

Data sensor akan digunakan sebagai masukan feedback bagi sistem kontrol dan navigasi.

### Phase 4 – Navigation and Planning

**Status: Not Started**

Mengembangkan sistem navigasi AUV yang meliputi:

- Waypoint Navigation.
- Path Planning menggunakan A* atau RRT*.
- Reactive Obstacle Avoidance menggunakan Artificial Potential Field.
- Integrasi target position dari Member 3.
- Pengembangan trajectory untuk kebutuhan navigasi dan hide-and-seek.

### Phase 5 – Simulation Integration

**Status: Not Started**

Mengintegrasikan sistem navigasi ROS 2 dengan Unreal Engine 5.

Lingkup integrasi:

- Pengiriman perintah gerakan dari ROS 2 ke simulasi.
- Pertukaran data posisi dan orientasi AUV.
- Integrasi data sensor dan odometry.
- Penyesuaian koordinat antara Unreal Engine dan ROS 2.
- Integrasi informasi target dari komponen sistem lainnya.

### Phase 6 – Launch and Bringup

**Status: Not Started**

Membuat konfigurasi ROS 2 launch untuk menjalankan komponen navigasi secara terintegrasi.

Lingkup pekerjaan:

- Membuat package `underwater_bringup`.
- Menyusun launch file.
- Menjalankan node motion control dan navigation secara bersamaan.
- Mengatur parameter sistem.
- Melakukan pengujian integrasi keseluruhan.

---

## 7. Important Notes

- Seluruh pengembangan ROS 2 dilakukan melalui Ubuntu WSL2.
- Perintah ROS 2 dijalankan melalui terminal Ubuntu.
- Workspace disimpan di filesystem Linux WSL untuk mendukung workflow pengembangan ROS 2.
- ROS 2 Jazzy telah dikonfigurasi agar aktif otomatis melalui `.bashrc`.
- Setiap perubahan kode perlu diikuti proses build ulang menggunakan `colcon`.
- Motion command menggunakan `geometry_msgs/msg/Twist` dengan enam komponen gerakan.
- Integrasi ROS 2 dengan Simulink telah berhasil diuji menggunakan ROS Toolbox.
- Enam kanal kontrol independen telah berhasil dibuat dan dijalankan dalam satu model Simulink.
- Seluruh 12 pengujian arah gerakan telah berhasil dilakukan.
- Model kontrol saat ini masih menggunakan plant orde satu yang identik dan belum memperhitungkan coupling dinamika AUV.
- Integrasi Odometry, IMU, waypoint navigation, path planning, dan obstacle avoidance masih menjadi pekerjaan berikutnya.
- Integrasi Unreal Engine 5 masih menjadi pekerjaan lanjutan.
- Konvensi koordinat dan antarmuka komunikasi antarkomponen perlu disepakati bersama anggota tim sebelum integrasi sistem dilakukan.

---

**Last Updated:** 10 November 2026  
**Current Milestone:** ROS 2 Motion Command, Six-Channel PID Control, and ROS 2–Simulink Integration Testing  
**Motion Control Test Result:** 12/12 Directional Tests Passed  
**Next Milestone:** Odometry and IMU Integration

