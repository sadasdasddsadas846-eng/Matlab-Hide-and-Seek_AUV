// UnderwaterLevelSetup.h
// Programmatically populates the underwater arena with environment meshes, drones,
// lighting, fog, and obstacles so the level is ready-to-play without manual editor placement.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnderwaterLevelSetup.generated.h"

class UStaticMesh;

/**
 * AUnderwaterLevelSetup
 *
 * Drop ONE instance of this actor into UnderwaterArena (or let GameMode spawn it).
 * On BeginPlay it spawns and positions every asset needed for the underwater world:
 *  - Terrain / seabed
 *  - Coral reef clusters
 *  - Shipwreck
 *  - Submarine prop
 *  - Two drones (Seeker + Hider) from Blueprints
 *  - Underwater environment actor (currents, post-process)
 *  - Exponential Height Fog for underwater atmosphere
 *  - Rock/box obstacles for hiding spots
 *  - Directional light dimming for deep-water feel
 */
UCLASS()
class UNDERWATERSIM_API AUnderwaterLevelSetup : public AActor
{
    GENERATED_BODY()

public:
    AUnderwaterLevelSetup();

    /** If true, this actor auto-populates the level on BeginPlay. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
    bool bAutoPopulate = true;

    /** Depth (negative Z in cm) of the arena floor center (matches Landscape at Z=-1250). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
    float ArenaFloorZ = -1250.0f;

    /** Scale multiplier for the terrain mesh. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Terrain")
    float TerrainScale = 10.0f;

    /** Scale multiplier for coral reef meshes. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Corals")
    float CoralScale = 3.5f;

    /** Scale multiplier for the shipwreck. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Shipwreck")
    float ShipwreckScale = 5.0f;

    /** Scale for the decorative submarine prop. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Submarine")
    float SubmarineScale = 3.0f;

    /** Number of procedural rock clusters to spawn for extra hiding spots. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Obstacles")
    int32 NumRockClusters = 12;

    /** Arena half-extent in cm (obstacles spread within this radius). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
    float ArenaRadius = 8000.0f;

    UFUNCTION(BlueprintCallable, Category = "Setup")
    void PopulateLevel();

protected:
    virtual void BeginPlay() override;

private:
    // Helpers
    AActor* SpawnStaticMeshActor(UStaticMesh* Mesh, const FVector& Location,
                                 const FRotator& Rotation, const FVector& Scale,
                                 const FString& Label);
    void SpawnTerrain();
    void SpawnCorals();
    void SpawnShipwreck();
    void SpawnSubmarineProp();
    void SpawnDrones();
    void SpawnEnvironmentActor();
    void SpawnProceduralObstacles();
    void ConfigureAtmosphere();

    // Cached mesh references
    UPROPERTY() UStaticMesh* TerrainMesh = nullptr;
    UPROPERTY() UStaticMesh* CoralMesh = nullptr;
    UPROPERTY() UStaticMesh* ShipMesh = nullptr;
    UPROPERTY() UStaticMesh* SubmarineMesh = nullptr;
    UPROPERTY() UStaticMesh* CoralLighthouseMesh = nullptr;

    // Track if already spawned (guards against double-fire)
    bool bLevelPopulated = false;
};

