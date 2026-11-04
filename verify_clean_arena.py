import unreal

unreal.EditorLoadingAndSavingUtils.load_map("/Game/Maps/UnderwaterArena")
subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

actors = subsystem.get_all_level_actors()
print(f"=== VERIFICATION REPORT: {len(actors)} total actors ===")
for a in actors:
    name = a.get_actor_label()
    if any(k in name for k in ["Coral", "Submarine", "Ship", "Drone", "Seabed", "Obstacle", "sketchfab"]):
        loc = a.get_actor_location()
        rot = a.get_actor_rotation()
        print(f"  FOUND: {name} | Loc: ({loc.x:.0f}, {loc.y:.0f}, {loc.z:.0f}) | Rot: (P={rot.pitch:.0f}, Y={rot.yaw:.0f}, R={rot.roll:.0f})")
print("=== END REPORT ===")

