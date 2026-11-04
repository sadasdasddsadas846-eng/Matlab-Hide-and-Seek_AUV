# save_fixed_level.py
# Modifies World Partition actors and explicitly saves each actor's package to disk.

import unreal

print(">>> Loading UnderwaterArena map...")
unreal.EditorLoadingAndSavingUtils.load_map("/Game/Maps/UnderwaterArena")
subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

all_actors = subsystem.get_all_level_actors()
packages_to_save = set()

FLOOR_Z = -1700.0

for actor in all_actors:
    label = actor.get_actor_label()

    # 1. Delete all duplicates / scrap meshes
    if any(k in label for k in ["_Submarine3", "BP_HiderDrone2", "BP_HiderDrone3", "BP_SeekerDrone2", "SM_CoralBlock"]):
        print(f"Deleting scrap/duplicate: {label}")
        actor.destroy_actor()
        continue

    # 2. Lay Coral Reef flat on seabed (Roll = -90 lays the reef flat)
    if "SM_CoralReef_Main" in label:
        print(f"Flattening {label} on seabed...")
        actor.set_actor_location(unreal.Vector(0.0, 0.0, FLOOR_Z), False, False)
        actor.set_actor_rotation(unreal.Rotator(0.0, 0.0, -90.0), False)
        actor.set_actor_scale3d(unreal.Vector(4.5, 4.5, 4.5))
        packages_to_save.add(actor.get_package())
        print(f"  Added {label} package to save list: {actor.get_package().get_name()}")

    # 3. Straighten Submarine (standing upright on its skids)
    if "Submarine" in label:
        print(f"Straightening {label}...")
        actor.set_actor_location(unreal.Vector(1800.0, 1500.0, FLOOR_Z + 150.0), False, False)
        # Roll = 0, Pitch = 0 makes it upright
        actor.set_actor_rotation(unreal.Rotator(0.0, 45.0, 0.0), False)
        actor.set_actor_scale3d(unreal.Vector(2.0, 2.0, 2.0))
        packages_to_save.add(actor.get_package())
        print(f"  Added {label} package to save list: {actor.get_package().get_name()}")

    # 4. Straighten and position Drones in the water column
    if "BP_SeekerDrone" in label:
        actor.set_actor_location(unreal.Vector(1000.0, 1000.0, -600.0), False, False)
        actor.set_actor_rotation(unreal.Rotator(0.0, 180.0, 0.0), False)
        packages_to_save.add(actor.get_package())

    if "BP_HiderDrone" in label:
        actor.set_actor_location(unreal.Vector(-1000.0, -1000.0, -800.0), False, False)
        actor.set_actor_rotation(unreal.Rotator(0.0, 0.0, 0.0), False)
        packages_to_save.add(actor.get_package())

# Also add dirty packages
dirty = unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
for p in dirty:
    packages_to_save.add(p)

print(f">>> Total packages to save: {len(packages_to_save)}")
for p in packages_to_save:
    print(f"  Saving: {p.get_name()}")

# Save packages to disk
unreal.EditorLoadingAndSavingUtils.save_packages(list(packages_to_save), False)
unreal.EditorLoadingAndSavingUtils.save_current_level()
print(">>> ALL PACKAGES WRITTEN TO DISK SUCCESSFULLY!")

