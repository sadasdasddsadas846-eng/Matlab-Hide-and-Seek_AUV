# Member 2 — ROS 2 & MATLAB Command Guide

Panduan perintah untuk menjalankan ROS 2 workspace, package `underwater_navigation`, motion command node, dan integrasi MATLAB/Simulink.

## 1. Membuka ROS 2 Environment

Buka terminal Ubuntu (WSL2), kemudian jalankan:

```bash
source /opt/ros/jazzy/setup.bash
source ~/ros2_ws/install/setup.bash
```

Perintah pertama mengaktifkan ROS 2 Jazzy. Perintah kedua mengaktifkan workspace yang sudah dibangun.

## 2. Masuk ke ROS 2 Workspace

```bash
cd ~/ros2_ws
```

## 3. Build Package

```bash
colcon build --packages-select underwater_navigation
```

Jika build berhasil, aktifkan kembali workspace:

```bash
source ~/ros2_ws/install/setup.bash
```

Catatan: Jalankan build kembali setiap kali ada perubahan kode package.

## 4. Menjalankan Motion Command Node

```bash
ros2 run underwater_navigation motion_command
```

Node ini membaca input keyboard dan mengirim perintah gerakan ke topic `/cmd_vel`.

## 5. Daftar Keyboard Command

| Tombol | Gerakan | Sumbu |
|---|---|---|
| W | Maju | Linear X (+) |
| S | Mundur | Linear X (-) |
| A | Kiri | Linear Y (+) |
| D | Kanan | Linear Y (-) |
| Q | Naik | Linear Z (+) |
| E | Turun | Linear Z (-) |
| J | Roll positif | Angular X (+) |
| L | Roll negatif | Angular X (-) |
| I | Pitch positif | Angular Y (+) |
| K | Pitch negatif | Angular Y (-) |
| U | Yaw positif | Angular Z (+) |
| O | Yaw negatif | Angular Z (-) |
| X | Berhenti | Semua sumbu 0 |

Kecepatan perintah saat ini adalah ±0.5.

Catatan: Program menggunakan input terminal berbasis baris, sehingga tekan Enter setelah memasukkan tombol.

## 6. Memeriksa ROS 2 Node

```bash
ros2 node list
```

## 7. Memeriksa Topic

```bash
ros2 topic list
```

## 8. Memeriksa Informasi Topic /cmd_vel

```bash
ros2 topic info /cmd_vel
```

## 9. Melihat Pesan /cmd_vel

```bash
ros2 topic echo /cmd_vel
```

Gunakan terminal Ubuntu kedua untuk memantau pesan saat motion command node berjalan.

## 10. MATLAB sebagai ROS 2 Subscriber

Buka MATLAB R2026a di Windows.

Buat subscriber:

```matlab
sub = ros2subscriber("/cmd_vel", "geometry_msgs/Twist");
```

Terima pesan:

```matlab
msg = receive(sub, 10);
```

Periksa komponen pesan:

```matlab
msg.linear.x
msg.linear.y
msg.linear.z

msg.angular.x
msg.angular.y
msg.angular.z
```

Catatan: Ini adalah contoh pengujian subscriber menggunakan MATLAB command window. Model Simulink menggunakan blok ROS 2 Subscribe.

## 11. Konfigurasi Simulink ROS 2 Subscribe

- Topic Source: Select from ROS network
- Topic: `/cmd_vel`
- Message Type: `geometry_msgs/Twist`
- Sample Time: `-1`

Hubungkan output Subscribe ke Bus Selector untuk mengambil:

- `linear.x`, `linear.y`, `linear.z`
- `angular.x`, `angular.y`, `angular.z`

Kemudian hubungkan masing-masing sinyal ke target input controller yang sesuai.

## 12. Memeriksa Status Simulink

```matlab
get_param('auv_6dof_control', 'SimulationStatus')
```

## 13. Menjalankan Simulink secara Continuous

```matlab
set_param('auv_6dof_control', 'StopTime', 'inf');
set_param('auv_6dof_control', 'SimulationCommand', 'start');
```

## 14. Menghentikan Simulink

```matlab
set_param('auv_6dof_control', 'SimulationCommand', 'stop');
```

## 15. Lokasi Project

**ROS 2 Workspace (Ubuntu):**

```text
/home/akbar/ros2_ws
```

**MATLAB Project (Windows):**

```text
D:\Documents\MatlabHide&Seek
```

**Simulink Model:**

```text
auv_6dof_control.slx
```

---

**Status:** Panduan awal berdasarkan implementasi dan pengujian ROS 2–Simulink yang sudah dilakukan. Perbarui jika ada perubahan package, node, topic, atau model.
