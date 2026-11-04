import unreal

unreal.EditorLoadingAndSavingUtils.load_map("/Game/Maps/UnderwaterArena")
subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

for a in subsystem.get_all_level_actors():
    label = a.get_actor_label()
    
    # Submarine: completely flat/level on seabed
    if "Submarine" in label or "_Submarine" in label:
        a.set_actor_location(unreal.Vector(1800.0, 1500.0, -1650.0), False, False)
        a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=45.0, roll=0.0), False)
        print(f"Fixed Submarine: {label} -> Rot=(0, 45, 0)")
        
    # Seeker Drone: upright (Pitch=0, Yaw=180, Roll=0)
    if "BP_SeekerDrone" in label:
        a.set_actor_location(unreal.Vector(1000.0, 1000.0, -600.0), False, False)
        a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=180.0, roll=0.0), False)
        print(f"Fixed Seeker Drone: {label} -> Rot=(0, 180, 0)")

    # Hider Drone: upright (Pitch=0, Yaw=0, Roll=0)
    if "BP_HiderDrone" in label:
        a.set_actor_location(unreal.Vector(-1000.0, -1000.0, -800.0), False, False)
        a.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=0.0, roll=0.0), False)
        print(f"Fixed Hider Drone: {label} -> Rot=(0, 0, 0)")

dirty = unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
unreal.EditorLoadingAndSavingUtils.save_packages(dirty, False)
unreal.EditorLoadingAndSavingUtils.save_current_level()
print("All rotations explicitly locked to pitch=0, roll=0!")

