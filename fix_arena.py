# fix_arena.py
# Fixes orientations, removes floating artifacts, and straightens the underwater assets.

import unreal

print(">>> Loading UnderwaterArena to fix orientations...")
unreal.EditorLoadingAndSavingUtils.load_map("/Game/Maps/UnderwaterArena")
world = unreal.EditorLevelLibrary.get_editor_world()

actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

for actor in all_actors:
    label = actor.get_actor_label()

    # 1. Remove fragmented floating coral blocks
    if "SM_CoralBlock" in label:
        print(f"Removing floating/fragmented asset: {label}")
        actor.destroy_actor()
        continue

    # 2. Fix Coral Reef rotation and seat it cleanly on the seafloor
    if "SM_CoralReef_Main" in label:
        print(f"Straightening {label}...")
        # Level out the rotation and sink slightly into floor so no floating edges
        actor.set_actor_location(unreal.Vector(0.0, 0.0, -1800.0), False, False)
        actor.set_actor_rotation(unreal.Rotator(0.0, 15.0, 0.0), False)
        actor.set_actor_scale3d(unreal.Vector(4.5, 4.5, 4.5))
        print("Coral Reef straightened and seated into seabed.")

    # 3. Fix Submarine rotation (straight and upright)
    if "SM_Submarine" in label:
        print(f"Straightening {label}...")
        actor.set_actor_location(unreal.Vector(1500.0, -1800.0, -1200.0), False, False)
        # Completely horizontal/upright (Pitch=0, Roll=0, Yaw=-45)
        actor.set_actor_rotation(unreal.Rotator(0.0, -45.0, 0.0), False)
        actor.set_actor_scale3d(unreal.Vector(2.5, 2.5, 2.5))
        print("Submarine straightened horizontally on the seabed.")

    # 4. Seabed terrain position
    if "SM_Seabed" in label:
        actor.set_actor_location(unreal.Vector(0.0, 0.0, -1850.0), False, False)
        actor.set_actor_rotation(unreal.Rotator(0.0, 0.0, 0.0), False)
        actor.set_actor_scale3d(unreal.Vector(12.0, 12.0, 10.0))

    # 5. Position Drones cleanly in the water column
    if "BP_SeekerDrone" in label:
        actor.set_actor_location(unreal.Vector(800.0, 1200.0, -600.0), False, False)
        actor.set_actor_rotation(unreal.Rotator(0.0, 180.0, 0.0), False)

    if "BP_HiderDrone" in label:
        actor.set_actor_location(unreal.Vector(-1000.0, -800.0, -800.0), False, False)
        actor.set_actor_rotation(unreal.Rotator(0.0, 0.0, 0.0), False)

print(">>> Saving updated map...")
unreal.EditorLoadingAndSavingUtils.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
print(">>> Map successfully fixed and saved!")

