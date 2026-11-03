# Prosedur Pengujian (Testing Guide)

Dokumen ini menjelaskan langkah-langkah verifikasi dan validasi untuk modul simulasi fisik, sensor, dan integrasi ROS 2.

---

## 1. Verifikasi Fisika Drone (Buoyancy & Hydrodynamics)

1. **Uji Gaya Apung Netral (Neutral Buoyancy):**
   - Letakkan `AUnderwaterDrone` pada kedalaman $Z = -500\text{ cm}$.
   - Pastikan thruster dimatikan (`StopAllThrusters()`).
   - Amati pergerakan vertikal: Drone seharusnya mempertahankan kedalamannya dengan deviasi $< 0.05\text{ m/s}$ karena gaya apung seimbang dengan gravitasi.
2. **Uji Thruster Step Response:**
   - Berikan input `SetSurge(1.0f)`.
   - Pastikan drone berakselerasi hingga mencapai kecepatan terminal konstan ($\approx 1.2\text{ - }1.5\text{ m/s}$) saat gaya dorong thruster seimbang dengan hambatan air (*hydrodynamic drag*).

---

## 2. Pengujian Sensor di Unreal Engine

1. **Sonar Raycast Sweep:**
   - Aktifkan flag `bShowDebugBeams = true` pada `SonarSensorComponent`.
   - Letakkan rintangan (*rock/obstacle*) di depan drone pada jarak 10 meter.
   - Amati visualisasi raycast di Editor: Garis cyan harus berhenti tepat pada permukaan rintangan dan menghasilkan titik impact oranye.
2. **Depth & Pressure Sensor:**
   - Turunkan drone ke kedalaman $10\text{ meter}$.
   - Verifikasi output `sensor_msgs/FluidPressure`: Tekanan harus bernilai sekitar $201.8\text{ kPa}$ ($1\text{ atm} + \approx 1\text{ bar}$).

---

## 3. Pengujian Game Loop Hide-and-Seek

1. Masuk ke mode Play In Editor (PIE).
2. Verifikasi state awal: Ronde berada pada fase `HidingPhase` selama 15 detik.
3. Setelah 15 detik, fase otomatis berganti menjadi `SeekingPhase`.
4. Jika jarak Seeker ke Hider kurang dari $2.5\text{ meter}$, game mengumumkan `SeekerWon` dan menghentikan timer.
