# Komunikasi ROS 2 & Protokol Interface

Dokumen ini mendefinisikan topik, tipe pesan, dan konvensi transformasi koordinat antara **Unreal Engine 5** dan **ROS 2**.

---

## 1. Konversi Sistem Koordinat

Unreal Engine menggunakan sistem tangan kiri (*left-handed*, Z-up dalam satuan centimeter), sedangkan ROS 2 menggunakan standar **REP 103** (*right-handed*, Z-up dalam satuan meter).

| Parameter | Unreal Engine | ROS 2 (REP 103) | Rumus Konversi |
|---|---|---|---|
| Sumbu $X$ | Depan (+X) | Depan (+X) | $X_{ros} = X_{ue} / 100$ |
| Sumbu $Y$ | Kanan (+Y) | Kiri (+Y) | $Y_{ros} = -Y_{ue} / 100$ |
| Sumbu $Z$ | Atas (+Z) | Atas (+Z) | $Z_{ros} = Z_{ue} / 100$ |
| Roll | Putar sumbu X | Putar sumbu X | $\phi_{ros} = \phi_{ue}$ |
| Pitch | Putar sumbu Y | Putar sumbu Y | $\theta_{ros} = -\theta_{ue}$ |
| Yaw | Putar sumbu Z | Putar sumbu Z | $\psi_{ros} = -\psi_{ue}$ |

---

## 2. Daftar Topik ROS 2

Setiap drone memiliki namespace tersendiri (misal: `/seeker` dan `/hider`).

### A. Kontrol & Input (ROS 2 -> Unreal Engine)

| Topik | Tipe Pesan | Frekuensi | Deskripsi |
|---|---|---|---|
| `/<drone_id>/cmd_vel` | `geometry_msgs/msg/Twist` | 20-50 Hz | Perintah kecepatan (linear.x = surge, linear.y = sway, linear.z = heave, angular.z = yaw) |
| `/game/reset_match` | `underwater_interfaces/srv/ResetMatch` | On-demand | Mereset posisi drone dan memulai ronde baru |

### B. Sensor Telemetri (Unreal Engine -> ROS 2)

| Topik | Tipe Pesan | Frekuensi | Deskripsi |
|---|---|---|---|
| `/<drone_id>/odometry` | `nav_msgs/msg/Odometry` | 30 Hz | Posisi dan orientasi absolut ground-truth |
| `/<drone_id>/imu/data` | `sensor_msgs/msg/Imu` | 50 Hz | Akselerometer, Giroskop, Orientasi dengan noise sensor |
| `/<drone_id>/depth` | `sensor_msgs/msg/FluidPressure` | 20 Hz | Tekanan hidrostatis dan estimasi kedalaman |
| `/<drone_id>/dvl` | `geometry_msgs/msg/TwistWithCovarianceStamped` | 10 Hz | Kecepatan relatif terhadap dasar laut |
| `/<drone_id>/sonar/scan`| `underwater_interfaces/msg/SonarScan` | 10 Hz | Hasil pemindaian acoustic sonar (jarak & sudut) |
| `/<drone_id>/camera/image_raw`| `sensor_msgs/msg/Image` | 15-30 Hz | Frame kamera bawah laut (RGB) |
