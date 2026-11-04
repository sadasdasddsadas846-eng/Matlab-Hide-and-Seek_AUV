# clean_and_save.py
import unreal

unreal.EditorLoadingAndSavingUtils.load_map("/Game/Maps/UnderwaterArena")
subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

actors_to_delete = []
for actor in subsystem.get_all_level_actors():
    label = actor.get_actor_label()
    # Delete scrap, broken meshes, or procedural cubes
    if any(k in label for k in ["Rock_Obstacle", "CoralBlock", "sketchfabTemp", "_Submarine3", "BP_HiderDrone2", "BP_HiderDrone3", "BP_SeekerDrone2", "SM_CoralReef_Main"]):
        actors_to_delete.append(actor)

for a in actors_to_delete:
    print(f"Deleting stale actor: {a.get_actor_label()}")
    a.destroy_actor()

dirty = unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
print(f"Saving {len(dirty)} dirty packages...")
unreal.EditorLoadingAndSavingUtils.save_packages(dirty, False)
unreal.EditorLoadingAndSavingUtils.save_current_level()
print("Clean and save complete!")

