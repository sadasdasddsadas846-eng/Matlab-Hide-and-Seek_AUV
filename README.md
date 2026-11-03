# DENGAN ATAS NAMA ALLAH YANG MAHA PENGASIH LAGI MAHA PENYAYANG
# KITA SELESAIKAN MODEL INI
## GOKIL LU NDRONG

# Underwater Drone Hide and Seek Simulation (Unreal Engine 5 + ROS 2)

Proyek simulasi skenario taktis **Hide-and-Seek** antara dua Autonomous Underwater Vehicles (AUVs) menggunakan **Unreal Engine 5** untuk simulasi fisik/visual dan **ROS 2** untuk algoritma navigasi, persepsi, dan kontrol otonom.

Diadaptasi dari [MathWorks MATLAB/Simulink Challenge Project Hub: Underwater Drone Hide and Seek](https://github.com/mathworks/MATLAB-Simulink-Challenge-Project-Hub/tree/main/projects/Underwater%20Drone%20Hide%20and%20Seek).

---

## Pembagian Peran Tim (3 Anggota)

| Peran | Penanggung Jawab | Fokus & Tanggung Jawab |
|---|---|---|
| **Unreal Engine Engineer** | **Kamu** | - Environment bawah laut & visual effects (ocean currents, turbidity, fog)<br>- Model 3D & fisika drone (BlueROV2 6-DOF vectored thruster)<br>- Buoyancy (Archimedes) & hydrodynamic drag<br>- Sensor simulasi (Sonar, Camera, IMU, Depth, DVL)<br>- Integrasi bridge Unreal Engine ke ROS 2 |
| **Navigation & Control Engineer** | Anggota 2 | - 6-DOF PID/SMC Motion Controller<br>- 3D Path Planning (A* / RRT*)<br>- Reactive Obstacle Avoidance (Artificial Potential Field)<br>- Evasion trajectory generator untuk Hider |
| **Perception & AI Engineer** | Anggota 3 | - Sonar pointcloud & acoustic echo filtering<br>- Visual object tracking (YOLO / Kalman Filter)<br>- Search pattern strategy (Lawnmower, spiral, LOS pursuit)<br>- Decision state machine (Search -> Track -> Intercept) |

---

## Struktur Folder Repositori

```text
UNDERWATER-HIDE-AND-SEEK/
│
├── UnrealProject/                      # [TANGGUNG JAWAB: UNREAL ENGINE ENGINEER]
│   ├── UnderwaterSim.uproject          # File project Unreal Engine 5
│   ├── Content/                        # Asset & Blueprint
│   │   ├── Maps/                       # Map underwater arena & test tracks
│   │   ├── Blueprints/                 # BP_UnderwaterDrone, BP_GameMode
│   │   ├── Drones/                     # Mesh & asset drone BlueROV2
│   │   ├── Environments/               # Karang, bebatuan, dasar laut, shipwrecks
│   │   ├── Sensors/                    # Blueprint sensor presets
│   │   └── Materials/                  # Material air, caustics, post-process
│   ├── Plugins/                        # rclUE (ROS 2 plugin)
│   └── Source/UnderwaterSim/           # C++ Core Simulation Source
│       ├── Drone/                      # Buoyancy, Thrusters, Pawn Core
│       ├── Sensors/                    # Sonar, IMU, Depth, DVL
│       ├── Environment/                # Arus laut (currents) & efek optik
│       ├── ROS2Integration/            # Bridge subscriber/publisher
│       └── HideAndSeekGameMode.*       # Logika aturan hide & seek
│
├── ROS2/
│   └── underwater_ws/
│       └── src/
│           ├── underwater_interfaces/  # Custom msg (SonarScan, DroneState) & srv
│           ├── underwater_navigation/  # [ANGGOTA 2] Path planning & control
│           ├── underwater_perception/  # [ANGGOTA 3] Object detection & tracking
│           └── underwater_bringup/     # Launch files & parameter configs
│
├── docs/
│   ├── architecture.md                 # Detail sistem, gaya fisika, dan sensor
│   ├── installation.md                 # Panduan instalasi UE5, ROS 2, dan rclUE
│   ├── communication.md                # Mapping topik ROS 2 dan sistem koordinat
│   └── testing.md                      # Panduan pengujian fisika dan game loop
│
└── README.md
```

---

## Cara Menjalankan

1. **Buka Project Unreal Engine:**
   - Double-click `UnrealProject/UnderwaterSim.uproject` di Unreal Engine 5.3+.
   - Buka Level `Content/Maps/UnderwaterArena`.
2. **Build ROS 2 Interface:**
   ```bash
   cd ROS2/underwater_ws
   colcon build
   source install/setup.bash
   ```
3. Detail lengkap silakan cek folder [docs/](docs/).
