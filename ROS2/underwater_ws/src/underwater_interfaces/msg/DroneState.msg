std_msgs/Header header
string drone_id
string role                  # "SEEKER", "HIDER", "NEUTRAL"
float32 depth                # Depth below surface in meters
float32 battery_percentage
bool is_submerged
geometry_msgs/Vector3 thruster_forces

