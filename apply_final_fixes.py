# apply_final_fixes.py
# Clean up duplicate actors, flatten CoralReef onto seabed, and straighten submarine.

import unreal

print(">>> Starting Final Cleanup & Orientation Fixes...")
unreal.EditorLoadingAndSavingUtils.load_map("/Game/Maps/UnderwaterArena")
subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

all_actors = subsystem.get_all_level_actors()

# Floor Z level in the arena
FLOOR_Z = -1700.0

for actor in all_actors:
    label = actor.get_actor_label()

    # 1. Delete all duplicates / scrap meshes
    if any(k in label for k in ["_Submarine3", "BP_HiderDrone2", "BP_HiderDrone3", "BP_SeekerDrone2", "SM_CoralBlock"]):
        print(f"Deleting scrap/duplicate: {label}")
        actor.destroy_actor()
        continue

    # 2. Flatten Coral Reef on the seabed
    if "SM_CoralReef_Main" in label:
        print(f"Flattening {label} on seabed...")
        actor.set_actor_location(unreal.Vector(0.0, 0.0, FLOOR_Z), False, False)
        # Roll = -90 lays the reef flat with textured coral surface facing UP
        actor.set_actor_rotation(unreal.Rotator(0.0, 0.0, -90.0), False)
        actor.set_actor_scale3d(unreal.Vector(4.5, 4.5, 4.5))
        print("Coral Reef is now completely flat on the seafloor!")

    # 3. Straighten Submarine (standing upright on its skids)
    if "Submarine" in label:
        print(f"Straightening {label}...")
        actor.set_actor_location(unreal.Vector(1800.0, 1500.0, FLOOR_Z + 150.0), False, False)
        # Roll = 0, Pitch = 0 makes it upright
        actor.set_actor_rotation(unreal.Rotator(0.0, 45.0, 0.0), False)
        actor.set_actor_scale3d(unreal.Vector(2.0, 2.0, 2.0))
        print("Submarine is now upright and resting on the seabed!")

    # 4. Straighten and position Drones in the water column
    if "BP_SeekerDrone" in label:
        actor.set_actor_location(unreal.Vector(1000.0, 1000.0, -600.0), False, False)
        actor.set_actor_rotation(unreal.Rotator(0.0, 180.0, 0.0), False)

    if "BP_HiderDrone" in label:
        actor.set_actor_location(unreal.Vector(-1000.0, -1000.0, -800.0), False, False)
        actor.set_actor_rotation(unreal.Rotator(0.0, 0.0, 0.0), False)

# Save the map cleanly
print(">>> Saving clean level...")
unreal.EditorLoadingAndSavingUtils.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
print(">>> ALL FIXES APPLIED & MAP SAVED SUCCESSFULLY!")

