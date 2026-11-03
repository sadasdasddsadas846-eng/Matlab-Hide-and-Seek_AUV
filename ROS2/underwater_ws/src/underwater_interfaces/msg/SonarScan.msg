# Complete horizontal sonar sweep scan
std_msgs/Header header
float32 horizontal_fov      # Total FOV in degrees
float32 max_range           # Maximum range in meters
SonarPing[] detections      # Array of detected return echoes

