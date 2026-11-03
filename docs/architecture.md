# Arsitektur Sistem: Unreal Engine 5 + ROS 2

Proyek ini adalah implementasi dari [MathWorks Underwater Drone Hide-and-Seek Challenge](https://github.com/mathworks/MATLAB-Simulink-Challenge-Project-Hub/tree/main/projects/Underwater%20Drone%20Hide%20and%20Seek), yang diimplementasikan menggunakan arsitektur modern **Unreal Engine 5** untuk simulasi fisik/visual dan **ROS 2** untuk kecerdasan otonom, kontrol navigasi, dan persepsi.

```
┌────────────────────────────────────────────────────────────────────────┐
│                   UNREAL ENGINE 5 (SIMULATOR CORE)                    │
│                                                                        │
│  ┌─────────────────────────┐         ┌───────────────────────────────┐ │
│  │   Underwater World      │         │     BlueROV2 Drone Actor      │ │
│  │  - Water Surface & Fog  │         │  - 6-DOF Vectored Thrusters   │ │
│  │  - Ocean Currents Field │◄────────┤  - Buoyancy & Quadratic Drag  │ │
│  │  - Obstacles & Terrain  │         │  - Rigid Body Physics Core    │ │
│  └─────────────────────────┘         └──────────────┬────────────────┘ │
│                                                     │                  │
│  ┌──────────────────────────────────────────────────┴────────────────┐ │
│  │                    SIMULATED SENSOR SUITE                         │ │
│  │  - Forward-Looking Sonar (Acoustic sweep & intensity)             │ │
│  │  - Depth/Pressure Sensor (Hydrostatic P = P_atm + rho*g*h)        │ │
│  │  - 9-DOF IMU (Accelerometer, Gyroscope, Orientation)              │ │
│  │  - DVL (4-Beam Janus Doppler Velocity Log & Altitude)             │ │
│  │  - HD Underwater Camera (Color absorption & turbidity)            │ │
│  └──────────────────────────────────┬────────────────────────────────┘ │
│                                     │                                  │
│  ┌──────────────────────────────────┴────────────────────────────────┐ │
│  │                  UNREAL ROS 2 BRIDGE (rclUE)                      │ │
│  │        Coordinate Transform: UE (cm, LH) <-> ROS (m, RH)         │ │
│  └──────────────────────────────────┬────────────────────────────────┘ │
└─────────────────────────────────────┼──────────────────────────────────┘
                                      │ DDS / ROS 2 Network
┌─────────────────────────────────────┴──────────────────────────────────┐
│                         ROS 2 WORKSPACE (STACK)                        │
│                                                                        │
│  ┌───────────────────────┐ ┌───────────────────────┐ ┌───────────────┐ │
│  │ underwater_navigation │ │ underwater_perception │ │ Game Master   │ │
│  │ - 6-DOF Controller    │ │ - Sonar Target Filter │ │ - Rules Engine│ │
│  │ - A* / 3D RRT Planner │ │ - Vision Detection    │ │ - Round Timer │ │
│  │ - Obstacle Avoidance  │ │ - EKF Track Filter    │ │ - Match Stats │ │
│  └───────────────────────┘ └───────────────────────┘ └───────────────┘ │
└────────────────────────────────────────────────────────────────────────┘
```

---

## 1. Fisika dan Model Hidrodinamika

Model fisik drone didasarkan pada persamaan marine craft 6-DOF standard (Fossen):

$$M \dot{\nu} + C(\nu)\nu + D(\nu)\nu + g(\eta) = \tau + \tau_{current}$$

- **Buoyancy ($g(\eta)$):** Gaya apung Archimedes $F_b = \rho_{water} \cdot g \cdot V_{submerged}$, dikurangi gaya gravitasi drone $F_g = m \cdot g$.
- **Hydrodynamic Drag ($D(\nu)\nu$):** Kombinasi redaman linier (skin friction) dan kuadratik (form drag):
  $$F_{drag} = -\frac{1}{2} \rho C_d A |v| v$$
- **Thruster Layout (BlueROV2 Heavy - 6 Thruster):**
  - **4 Thruster Horizontal:** Terpasang pada sudut 45 derajat di 4 sudut drone, memungkinkan pergerakan bebas (*holonomic omnidirectional*) pada sumbu Surge ($X$), Sway ($Y$), dan rotasi Yaw.
  - **2 Thruster Vertikal:** Terpasang sejajar sumbu vertikal untuk kontrol kedalaman (Heave / $Z$).

