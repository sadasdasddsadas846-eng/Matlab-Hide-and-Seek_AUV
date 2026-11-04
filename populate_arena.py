# populate_arena.py
# Places underwater environment assets permanently into UnderwaterArena.umap

import unreal

print(">>> Starting automated underwater scene population...")

# 1. Load the level
map_path = "/Game/Maps/UnderwaterArena"
success = unreal.EditorLoadingAndSavingUtils.load_map(map_path)
print(f">>> Level loaded: {success}")

actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

# Helper function to spawn static mesh
def spawn_mesh(asset_path, location, rotation, scale, label):
    mesh_asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not mesh_asset:
        print(f"FAILED to load asset: {asset_path}")
        return None
    
    actor = actor_subsystem.spawn_actor_from_object(mesh_asset, location, rotation)
    if actor:
        actor.set_actor_scale3d(scale)
        actor.set_actor_label(label)
        print(f"Spawned {label} at {location}")
        return actor
    return None

# Helper function to spawn blueprint class
def spawn_blueprint(bp_path, location, rotation, label):
    bp_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
    if not bp_class:
        print(f"FAILED to load blueprint: {bp_path}")
        return None
    
    actor = actor_subsystem.spawn_actor_from_class(bp_class, location, rotation)
    if actor:
        actor.set_actor_label(label)
        print(f"Spawned BP {label} at {location}")
        return actor
    return None

# Center floor Z in UnderwaterArena
FLOOR_Z = -1500.0

# 1. Large Coral Reef (Main Centerpiece)
spawn_mesh(
    "/Game/Environments/Corals/CoralReef",
    unreal.Vector(0.0, 0.0, FLOOR_Z + 100.0),
    unreal.Rotator(0.0, 0.0, 0.0),
    unreal.Vector(5.0, 5.0, 5.0),
    "SM_CoralReef_Main"
)

# 2. Additional Coral Block Lighthouse (Flanking)
spawn_mesh(
    "/Game/Environments/coral-block-lighthouse/source/sketchfabTemp_obj",
    unreal.Vector(2500.0, 2000.0, FLOOR_Z + 80.0),
    unreal.Rotator(0.0, 45.0, 0.0),
    unreal.Vector(4.0, 4.0, 4.0),
    "SM_CoralBlock_1"
)

spawn_mesh(
    "/Game/Environments/coral-block-lighthouse/source/sketchfabTemp_obj",
    unreal.Vector(-2800.0, 2200.0, FLOOR_Z + 80.0),
    unreal.Rotator(0.0, -60.0, 0.0),
    unreal.Vector(3.5, 3.5, 3.5),
    "SM_CoralBlock_2"
)

# 3. Seabed Terrain mesh
spawn_mesh(
    "/Game/Environments/Terrain/model1",
    unreal.Vector(0.0, 0.0, FLOOR_Z),
    unreal.Rotator(0.0, 0.0, 0.0),
    unreal.Vector(10.0, 10.0, 10.0),
    "SM_Seabed_Terrain"
)

# 4. Submarine Decoration
spawn_mesh(
    "/Game/Drones/SubmarineModel/_Submarine2",
    unreal.Vector(1800.0, -2200.0, FLOOR_Z + 350.0),
    unreal.Rotator(10.0, -35.0, 5.0),
    unreal.Vector(2.5, 2.5, 2.5),
    "SM_Submarine_Decoration"
)

# 5. Seeker Drone (BlueROV2 BP)
spawn_blueprint(
    "/Game/Blueprints/BP_SeekerDrone",
    unreal.Vector(1200.0, 1500.0, FLOOR_Z + 1200.0),
    unreal.Rotator(0.0, 180.0, 0.0),
    "BP_SeekerDrone_Active"
)

# 6. Hider Drone (BlueROV2 BP)
spawn_blueprint(
    "/Game/Blueprints/BP_HiderDrone",
    unreal.Vector(-1500.0, -1200.0, FLOOR_Z + 800.0),
    unreal.Rotator(0.0, 0.0, 0.0),
    "BP_HiderDrone_Active"
)

# 7. Underwater Atmosphere (Exponential Height Fog)
fog_actors = unreal.GameplayStatics.get_all_actors_of_class(unreal.EditorLevelLibrary.get_editor_world(), unreal.ExponentialHeightFog)
if len(fog_actors) > 0:
    fog = fog_actors[0]
    fog_comp = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
    if fog_comp:
        fog_comp.set_fog_density(0.04)
        fog_comp.set_fog_inscattering_color(unreal.LinearColor(0.02, 0.08, 0.2, 1.0))
        fog_comp.set_volumetric_fog(True)
        print("Updated Exponential Height Fog for underwater atmosphere.")

# 8. Save the map and dirty packages
print(">>> Saving map...")
unreal.EditorLoadingAndSavingUtils.save_current_level()
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
print(">>> All assets successfully placed and map saved!")

