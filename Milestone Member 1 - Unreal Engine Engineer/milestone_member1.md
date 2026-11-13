# Progress Report – Underwater Drone Hide and Seek
## Member 1: Unreal Engine Simulation Engineer

**Project:** Underwater Drone Hide and Seek  
**Role:** Member 1 – Unreal Engine Engineer  
**Development Environment:** Windows 11 (64-bit) + Visual Studio 2022  
**Game Engine:** Unreal Engine 5.8 (DirectX 12, Lumen GI & Reflections, TSR, Nanite)  
**Programming Language:** C++17 / C++20 & Unreal Python Editor Scripting  
**Module Name:** `UnderwaterSim`  
**Project Directory:** `UnrealProject 5.8/`  
**Main Level:** `/Game/Maps/UnderwaterArena`

---

## 1. Project Overview

Member 1 bertanggung jawab penuh atas pengembangan **Core Physics & Visual Simulator** di **Unreal Engine 5.8** untuk skenario taktis *Hide-and-Seek* antara dua Autonomous Underwater Vehicles (AUVs): **Seeker (Kuning)** dan **Hider (Biru)**.

Pengembangan difokuskan pada perancangan dunia bawah laut 3D berskala luas, pemodelan dinamika fluida dan fisika 6-DOF BlueROV2 Heavy, simulasi rangkaian sensor akustik/inersia bawah air, logika aturan permainan (*Game Mode*), serta komponen jembatan komunikasi (*ROS 2 Bridge*) untuk pertukaran data dengan **Member 2 (ROS 2 Navigation Engineer)** dan **Member 3 (Perception & AI Engineer)**.

Ruang lingkup pekerjaan meliputi:

- Setup project Unreal Engine 5.8 dan arsitektur modul C++ `UnderwaterSim`.
- Perancangan *Underwater World* berskala luas (`108m x 108m`) bertema kota bawah laut **Bikini Bottom Ultra-HD** + **Red Sea Cruise Shipwreck**.
- Efek atmosfer bawah laut: *Exponential Height Fog*, *Volumetric Scattering*, kekeruhan air (*Turbidity*), dan medan arus laut 3D (*Ocean Currents*).
- Pemodelan fisik 6-DOF AUV (BlueROV2 Heavy): 4 *Vectored Horizontal Thrusters* ($45^\circ$) + 2 *Vertical Heave Thrusters*.
- Implementasi gaya apung *Archimedes Buoyancy*, *Metacentric Restoring Torque*, serta *Linear & Quadratic Hydrodynamic Drag*.
- Pengembangan *Simulated Sensor Suite* di C++: Forward-Looking Acoustic Sonar, 9-DOF IMU, Hydrostatic Depth/Pressure Sensor, dan 4-Beam Janus DVL.
- Sistem aturan permainan otomatis (`AHideAndSeekGameMode`): *Hiding Phase*, *Seeking Phase*, *Capture Distance Check*, dan *Round Timer*.
- Pengembangan komponen `UROS2BridgeComponent` dan paket pesan standar `underwater_interfaces` (`DroneState.msg`, `SonarPing.msg`, `SonarScan.msg`, `ResetMatch.srv`).
- Integrasi *live socket/DDS* antara Unreal Engine 5.8 (Windows) dengan node ROS 2 Jazzy di WSL2 milik Member 2 & Member 3.

---

## 2. Development Environment & Rendering Configuration

Lingkungan pengembangan simulasi telah dikonfigurasi menggunakan Unreal Engine 5.8 dengan fitur rendering **Ultra-HD**.

| Komponen | Konfigurasi |
|---|---|
| Host Operating System | Windows 11 (64-bit) |
| Game Engine | Unreal Engine 5.8 |
| C++ Toolchain | MSVC v14.44 (Visual Studio 2022 Build Tools) & Windows 10/11 SDK |
| Graphics RHI | DirectX 12 (`PCD3D_SM6`, `PCD3D_SM5`) |
| Global Illumination & Reflections | Lumen GI & Lumen Reflections (`r.GenerateMeshDistanceFields=True`, `r.Lumen.TraceMeshSDFs=1`) |
| Anti-Aliasing & Upscaling | Temporal Super Resolution / TSR (`r.AntiAliasingMethod=4`, `r.ScreenPercentage=100`) |
| Texture Filtering & Sharpening | `16x Anisotropic Filtering` (`r.MaxAnisotropy=16`) + `r.Tonemapper.Sharpen=0.75` |
| Geometry Pipeline | Nanite Enabled (`bGenerateNaniteFallbackMeshes=True`) |

---

## 3. Current Development Progress (Completed vs. Not Started)

Berikut adalah status penyelesaian seluruh *task* pada *jobdesk* **Member 1 (Unreal Engine Engineer)**:

| No. | Task / Milestone | Kategori | Status | File / Asset Terkait |
|---:|---|---|---|---|
| 1 | Setup Project Unreal Engine 5.8 & C++ Module `UnderwaterSim` | Core Engine | **Completed** ✅ | `UnderwaterSim.uproject`, `UnderwaterSim.Build.cs` |
| 2 | Desain Dunia Bawah Laut Ultra-HD (`108m x 108m` Bikini Bottom + Cruise Shipwreck) | Environment 3D | **Completed** ✅ | `Content/Maps/UnderwaterArena.umap`, `SM_Shipwreck` |
| 3 | Pembuatan 29 High-Poly 3D Mesh & 12 Tekstur PBR `1024x1024` + Normal Maps | Environment 3D | **Completed** ✅ | `Content/Environments/MarineLife/*` |
| 4 | Solid Collision (`26-DOP` + `Box` + `DoubleSidedGeometry`) pada Seluruh Bangunan & Seabed | Physics & Collision | **Completed** ✅ | `SM_PineappleHouse_*`, `SM_SquidwardMoai`, `SM_KrustyKrab_*`, `SM_SandySeabed` |
| 5 | Animasi Biota Laut Otomatis (53 Ikan, Hiu, Penyu, Pari Manta, & Ubur-Ubur) | Environment AI | **Completed** ✅ | `AUnderwaterLevelSetup::GatherMarineLifeForAnimation()` |
| 6 | Simulasi Atmosfer Air, Volumetric Fog, Turbidity, & 3D Ocean Currents | Environment Physics | **Completed** ✅ | `UnderwaterEnvironmentActor.h/.cpp`, `UnderwaterLevelSetup.cpp` |
| 7 | Model 3D & Blueprint Kapal Selam **Seeker (Kuning)** & **Hider (Biru)** | Drone Asset | **Completed** ✅ | `BP_SeekerDrone.uasset`, `BP_HiderDrone.uasset`, `M_HiderSub_Blue.uasset` |
| 8 | Fisika 6-DOF Vectored Thruster Layout (BlueROV2 Heavy: 4 Horizontal + 2 Vertical) | Drone Physics | **Completed** ✅ | `UnderwaterDrone.h/.cpp`, `ThrusterComponent.h/.cpp` |
| 9 | Gaya Apung Archimedes (`UAUVBuoyancyComponent`) & Quadratic Hydrodynamic Drag | Drone Physics | **Completed** ✅ | `AUVBuoyancyComponent.h/.cpp` |
| 10 | Kontrol Manual 6-DOF (Surge, Sway, Heave Dive/Ascend, Yaw, & Mouse Camera Orbit) | Drone Control | **Completed** ✅ | `AUnderwaterDrone::Tick()` |
| 11 | Simulasi Sensor Forward-Looking Acoustic Sonar (2D Raycast Sweep & Echo Intensity) | Sensor Suite | **Completed** ✅ | `SonarSensorComponent.h/.cpp` |
| 12 | Simulasi Sensor 9-DOF IMU (Accelerometer, Gyroscope, Orientation + Gaussian Noise) | Sensor Suite | **Completed** ✅ | `IMUSensorComponent.h/.cpp` |
| 13 | Simulasi Sensor Kedalaman Hidrostatik (`UDepthSensorComponent`) | Sensor Suite | **Completed** ✅ | `DepthSensorComponent.h/.cpp` |
| 14 | Simulasi Sensor 4-Beam Janus Doppler Velocity Log (`UDVLSensorComponent`) | Sensor Suite | **Completed** ✅ | `DVLSensorComponent.h/.cpp` |
| 15 | Logika Permainan Hide-and-Seek (`HidingPhase`, `SeekingPhase`, Capture Detection) | Game Mode | **Completed** ✅ | `HideAndSeekGameMode.h/.cpp` |
| 16 | Definisi Paket Pesan & Service ROS 2 (`underwater_interfaces`) | ROS 2 Interface | **Completed** ✅ | `DroneState.msg`, `SonarPing.msg`, `SonarScan.msg`, `ResetMatch.srv` |
| 17 | Komponen C++ `UROS2BridgeComponent` (Mapping `/cmd_vel` & Konversi Koordinat REP-103) | ROS 2 Bridge | **Completed (Core API)** ✅ | `ROS2BridgeComponent.h/.cpp` |
| 18 | Koneksi Live Transport (UDP/TCP Bridge atau `rclUE` DDS) dari UE5 Windows ke WSL2 ROS 2 Jazzy | Live Integration | **In Progress / Not Started** ⏳ | Integrasi bersama Member 2 (`/cmd_vel`) & Member 3 (`/sonar/scan`) |
| 19 | Underwater Camera RenderTarget Capture & Streaming ke Topik `/camera/image_raw` | Sensor Streaming | **Not Started** ⏳ | Untuk kebutuhan *Visual Object Detection* Member 3 |
| 20 | In-Game HUD / UI Overlay (Indikator Fase Game, Kedalaman, Jarak Seeker–Hider, & Radar Sonar) | UI / Visualization | **Not Started** ⏳ | Widget Blueprint / C++ HUD |
| 21 | Implementasi Service Handler `/game/reset_match` dari ROS 2 ke `AHideAndSeekGameMode` | Live Integration | **Not Started** ⏳ | `ROS2BridgeComponent.cpp` + `HideAndSeekGameMode.cpp` |

---

## 4. Detailed Breakdown of Completed Modules

### 4.1 Underwater World & Environment (`UnderwaterArena`)
Level utama `/Game/Maps/UnderwaterArena` telah dibangun dan divalidasi dengan `0 Error(s), 0 Warning(s)` pada Unreal Map Check:
- **Arena Size:** Lembah pasir bawah laut berukuran `108m x 108m` (`SM_SandySeabed`, `39.200` triangles) pada kedalaman $Z = -1240\text{ cm}$ dengan permukaan air di $Z = +810\text{ cm}$ (kedalaman kolom air $\approx 20.5\text{ meter}$).
- **Landmark & Obstacles (Solid Collision):**
  - Conch Street Trio: Rumah Nanas SpongeBob (`SM_PineappleHouse_*`), Rumah Moai Squidward (`SM_SquidwardMoai`), dan Rumah Batu Patrick (`SM_PatrickRock`).
  - Restoran: The Krusty Krab (`SM_KrustyKrab_*`) dan The Chum Bucket (`SM_ChumBucket_*`).
  - 16 Gedung Baja Kota Bikini Bottom (`SM_BikiniBuilding_A/B/C`) sebagai rintangan (*obstacles*) tempat bersembunyi bagi Hider.
  - 2 Bangkai Kapal Pesiar Laut Merah (`SM_Shipwreck` dari dataset fotogrametri `wreck-of-cruise-ship-saudi-red-sea-coast`).
  - 4 Lengkungan Batu (*Swim-Through Rock Arches*), Reruntuhan Kuno (*Sunken Ruins*), Hutan Kelp, Karang Otak (*Brain Coral*), Karang Tanduk (*Staghorn Coral*), dan Ranjau Laut (*Naval Mines*).
- **Dynamic Marine Life:** 53 makhluk laut (hiu karang, penyu laut, pari manta, kawanan ikan tropis, dan ubur-ubur merah muda) yang bergerak dan berenang secara prosedural setiap frame di `AUnderwaterLevelSetup::Tick()`.
- **3D Ocean Currents & Turbidity (`AUnderwaterEnvironmentActor`):** Menghitung vektor kecepatan arus laut 3D berbasis ruang dan waktu (kombinasi arus dasar + pusaran sinusoidal/vortex) serta visibilitas optik berdasarkan kekeruhan air (`Turbidity`).

### 4.2 6-DOF AUV Physics & Control (`AUnderwaterDrone`)
Kedua drone menggunakan arsitektur fisik **BlueROV2 Heavy**:
- **Seeker Drone (`BP_SeekerDrone`):** Berwarna **Kuning**, dikendalikan oleh pemain atau algoritma *Pursuit* ROS 2 (`DroneID = 1`, `DroneRole = EDroneRole::Seeker`).
- **Hider Drone (`BP_HiderDrone`):** Berwarna **Biru Metalik** (`M_HiderSub_Blue`), bertindak sebagai target yang bersembunyi (`DroneID = 2`, `DroneRole = EDroneRole::Hider`).
- **6 Vectored Thrusters (`UThrusterComponent`):**
  - 4 Thruster Horizontal (`ThrusterFL`, `ThrusterFR`, `ThrusterRL`, `ThrusterRR`) dipasang menyudut $\pm 45^\circ$ untuk pergerakan omnidirectional (Surge $X$, Sway $Y$, dan Yaw $\psi$).
  - 2 Thruster Vertikal (`ThrusterVL`, `ThrusterVR`) untuk pergerakan menyelam dan naik (Heave $Z$).
  - Mendukung kurva dorong non-linier, *first-order motor time constant* ($\tau = 0.05\text{ s}$), dan kehilangan efisiensi saat bergerak mundur (`ReverseEfficiency = 0.75`).
- **Hydrodynamics (`UAUVBuoyancyComponent`):**
  - Gaya apung Archimedes proporsional terhadap fraksi volume yang terendam di bawah `WaterSurfaceZ`.
  - Redaman hidrodinamik linier (*skin friction*) dan kuadratik (*form drag*):
    $$F_{\text{drag}} = -\frac{1}{2} \rho C_d A |v_{\text{rel}}| v_{\text{rel}}$$
  - Penguncian stabilitas *Roll* dan *Pitch* (*metacentric lock*) sehingga drone selalu tegak lurus dan stabil saat bermanuver.

### 4.3 Simulated Underwater Sensor Suite (`Source/UnderwaterSim/Sensors/`)
Seluruh komponen sensor telah ditulis dalam C++ dan siap dipasang pada Seeker maupun Hider:
1. **Forward-Looking Sonar (`USonarSensorComponent`):**
   - Melakukan pemindaian akustik 2D multi-ray (`NumRays = 64`, `HorizontalFOV = 120°`, `VerticalFOV = 20°`, `MaxRange = 50m`, `ScanRate = 10 Hz`).
   - Menghitung intensitas gema akustik (*echo intensity*) berdasarkan jarak, sudut datang permukaan (*incidence angle*), reflektivitas material (logam/kapal selam memantulkan sinyal lebih kuat), serta derau Gaussian.
   - Mendeteksi secara otomatis apakah pantulan berasal dari kapal selam lawan (`bDetectedDrone`).
2. **9-DOF IMU (`UIMUSensorComponent`):**
   - Mengukur percepatan spesifik 3-sumbu (termasuk kompensasi gravitasi dalam kerangka bodi), kecepatan sudut giroskop (`rad/s`), dan orientasi pada frekuensi `100 Hz` lengkap dengan *random walk bias* dan derau Gaussian.
3. **Hydrostatic Depth Sensor (`UDepthSensorComponent`):**
   - Menghitung tekanan hidrostatik $P = P_{\text{atm}} + \rho g h$ (dalam kPa) dan kedalaman (meter) pada frekuensi `20 Hz`.
4. **Doppler Velocity Log (`UDVLSensorComponent`):**
   - Mensimulasikan konfigurasi 4-beam Janus (menyudut $30^\circ$ dari vertikal) yang melakukan *raycast* ke dasar laut untuk mengukur kecepatan relatif terhadap dasar laut (*bottom-track velocity*) dan ketinggian dari dasar laut (*altitude*) hingga jarak `30m`.

### 4.4 Hide-and-Seek Game Mode (`AHideAndSeekGameMode`)
- **Hiding Phase:** Memberikan waktu *headstart* (`HidingHeadstartSeconds = 15.0s`) bagi Hider untuk mencari tempat persembunyian sebelum Seeker mulai mengejar.
- **Seeking Phase:** Menghitung jarak Euclidean 3D antara Seeker dan Hider setiap frame.
- **Win Conditions:**
  - Jika jarak Seeker dan Hider $\le$ `CaptureDistanceThresholdMeters` (`3.0m`), status berubah menjadi `EGameStatePhase::SeekerWon` dan memicu event `OnTargetCaptured`.
  - Jika waktu habis (`MaxRoundDurationSeconds = 180.0s`) tanpa tertangkap, status berubah menjadi `EGameStatePhase::HiderWon`.

### 4.5 ROS 2 Interfaces & Bridge Component (`UROS2BridgeComponent`)
- Telah menyediakan fungsi penerima perintah kecepatan `OnReceiveCmdVel(LinearX, LinearY, LinearZ, AngularZ)` yang memetakan pesan `geometry_msgs/msg/Twist` langsung ke thruster Surge, Sway, Heave, dan Yaw.
- Telah mengimplementasikan konversi koordinat standar antara Unreal Engine (cm, *Left-Handed* Z-Up) dan ROS 2 **REP-103** (meter, *Right-Handed* Z-Up):
  $$X_{\text{ros}} = \frac{X_{\text{ue}}}{100}, \quad Y_{\text{ros}} = -\frac{Y_{\text{ue}}}{100}, \quad Z_{\text{ros}} = \frac{Z_{\text{ue}}}{100}$$

---

## 5. Next Development Plan (Remaining Tasks for Member 1)

Untuk menyelesaikan integrasi penuh dengan **Member 2 (Akbar – Navigation)** dan **Member 3 (Perception)**, tahapan pengembangan berikutnya adalah:

### Phase 1 – Live Socket / ROS 2 Bridge Transport (UE5 Windows $\leftrightarrow$ WSL2 Ubuntu)
**Status: In Progress / Next Priority**
- Karena Unreal Engine 5.8 berjalan di Windows 11 sedangkan ROS 2 Jazzy berjalan di dalam Ubuntu 24.04 WSL2, diperlukan jembatan komunikasi lintas-OS (misalnya menggunakan **UDP/JSON Socket Bridge Node** di ROS 2 atau `rosbridge_suite`) yang meneruskan:
  - Topik `/cmd_vel` (`geometry_msgs/msg/Twist`) dari node `motion_command` / `auv_controller` milik Member 2 langsung ke `UROS2BridgeComponent::OnReceiveCmdVel()` di Unreal Engine.
  - Data `/odom` (`nav_msgs/msg/Odometry`), `/imu/data` (`sensor_msgs/msg/Imu`), dan `/sonar/scan` (`underwater_interfaces/msg/SonarScan`) dari Unreal Engine ke workspace ROS 2 di WSL2.

### Phase 2 – In-Game HUD & Sonar Visualization
**Status: Not Started**
- Menampilkan informasi pertandingan secara *real-time* di layar Unreal Engine:
  - Status fase permainan (`HIDING PHASE` / `ACTIVE SEEKING` / `SEEKER WON` / `HIDER WON`).
  - Countdown timer ronde dan jarak ke Hider (dalam meter).
  - Kedalaman saat ini (meter) dan kecepatan AUV (`m/s`).
  - Visualisasi sapuan *Forward-Looking Sonar* untuk memudahkan debugging deteksi target bersama Member 3.

### Phase 3 – Underwater Camera Streaming & Match Reset Service
**Status: Not Started**
- Menambahkan `USceneCaptureComponent2D` pada moncong Seeker Drone untuk menangkap gambar RGB bawah air dan mengirimkannya ke topik `/seeker/camera/image_raw` bagi algoritma deteksi visual Member 3.
- Menghubungkan service ROS 2 `/game/reset_match` (`underwater_interfaces/srv/ResetMatch`) agar ronde permainan dapat di-reset secara otomatis dari terminal ROS 2.

---

**Last Updated:** 10 November 2026  
**Current Milestone:** Ultra-HD Bikini Bottom Arena, 6-DOF BlueROV2 Physics, Simulated Sensor Suite, and C++ ROS 2 Bridge Core Completed  
**Next Milestone:** Live Windows-to-WSL2 ROS 2 Socket Bridge Integration with Member 2 (`/cmd_vel` & `/odom`) and In-Game HUD

