// UnderwaterLevelSetup.cpp
// Spawns all underwater environment assets programmatically.

#include "Environment/UnderwaterLevelSetup.h"
#include "Environment/UnderwaterEnvironmentActor.h"
#include "Drone/UnderwaterDrone.h"

#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "UnderwaterSim.h"

// ─── Constructor ────────────────────────────────────────────────────────────

AUnderwaterLevelSetup::AUnderwaterLevelSetup()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // ── Load Static Mesh assets from Content paths ──

    // Terrain mesh (imported FBX → UAsset)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> TerrainFinder(
        TEXT("/Game/Environments/Terrain/model.model"));
    if (TerrainFinder.Succeeded()) TerrainMesh = TerrainFinder.Object;

    // Coral Reef mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CoralFinder(
        TEXT("/Game/Environments/Corals/CoralReef.CoralReef"));
    if (CoralFinder.Succeeded()) CoralMesh = CoralFinder.Object;

    // Coral block lighthouse (alternative coral)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CoralLighthouseFinder(
        TEXT("/Game/Environments/coral-block-lighthouse/source/sketchfabTemp_obj.sketchfabTemp_obj"));
    if (CoralLighthouseFinder.Succeeded()) CoralLighthouseMesh = CoralLighthouseFinder.Object;

    // Shipwreck
    static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipFinder(
        TEXT("/Game/Environments/Shipwreck/ship.ship"));
    if (ShipFinder.Succeeded()) ShipMesh = ShipFinder.Object;

    // Submarine (decorative prop)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SubFinder(
        TEXT("/Game/Drones/SubmarineModel/Object001.Object001"));
    if (SubFinder.Succeeded()) SubmarineMesh = SubFinder.Object;
}

// ─── BeginPlay ──────────────────────────────────────────────────────────────

void AUnderwaterLevelSetup::BeginPlay()
{
    Super::BeginPlay();

    if (bAutoPopulate)
    {
        PopulateLevel();
    }
}

// ─── Main Populate ──────────────────────────────────────────────────────────

void AUnderwaterLevelSetup::PopulateLevel()
{
    if (bLevelPopulated) return;
    bLevelPopulated = true;

    UE_LOG(LogUnderwaterSim, Log, TEXT("=== UnderwaterLevelSetup: Populating arena... ==="));

    SpawnTerrain();
    SpawnCorals();
    SpawnShipwreck();
    SpawnSubmarineProp();
    SpawnProceduralObstacles();
    SpawnEnvironmentActor();
    SpawnDrones();
    ConfigureAtmosphere();

    UE_LOG(LogUnderwaterSim, Log, TEXT("=== UnderwaterLevelSetup: Arena population complete! ==="));
}

// ─── Helper: Spawn a Static Mesh Actor ──────────────────────────────────────

AActor* AUnderwaterLevelSetup::SpawnStaticMeshActor(
    UStaticMesh* Mesh,
    const FVector& Location,
    const FRotator& Rotation,
    const FVector& Scale,
    const FString& Label)
{
    if (!Mesh)
    {
        UE_LOG(LogUnderwaterSim, Warning, TEXT("LevelSetup: Mesh is null for '%s' — skipping."), *Label);
        return nullptr;
    }

    UWorld* World = GetWorld();
    if (!World) return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = FName(*Label);
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>(
        AStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);

    if (MeshActor)
    {
        MeshActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
        MeshActor->SetActorScale3D(Scale);
        MeshActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
#if WITH_EDITOR
        MeshActor->SetActorLabel(Label);
#endif

        UE_LOG(LogUnderwaterSim, Log, TEXT("  Spawned: %s at (%.0f, %.0f, %.0f)"),
               *Label, Location.X, Location.Y, Location.Z);
    }

    return MeshActor;
}

// ─── Terrain / Seabed ───────────────────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnTerrain()
{
    // Main terrain: large seabed floor
    SpawnStaticMeshActor(
        TerrainMesh,
        FVector(0.0f, 0.0f, ArenaFloorZ),
        FRotator::ZeroRotator,
        FVector(TerrainScale),
        TEXT("Seabed_Terrain"));

    // If we also have the agisoft terrain, overlay it slightly offset for variety
    UStaticMesh* AgisoftTerrain = LoadObject<UStaticMesh>(nullptr,
        TEXT("/Game/Environments/underwater-terrain-agisoftnaturechallenge/textures/model.model"));
    if (AgisoftTerrain)
    {
        SpawnStaticMeshActor(
            AgisoftTerrain,
            FVector(4000.0f, 3000.0f, ArenaFloorZ + 200.0f),
            FRotator(0.0f, 45.0f, 0.0f),
            FVector(TerrainScale * 0.8f),
            TEXT("Seabed_Terrain_Agisoft"));
    }
}

// ─── Coral Clusters ─────────────────────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnCorals()
{
    // Predetermined coral placement positions for natural look
    struct FCoralPlacement
    {
        FVector Location;
        FRotator Rotation;
        float ScaleMult;
    };

    const TArray<FCoralPlacement> CoralPlacements = {
        // Main coral reef cluster — center-ish area
        { FVector(0.0f, 0.0f, ArenaFloorZ + 50.0f),        FRotator(0.0f, 0.0f, 0.0f),     1.0f },
        // Scattered coral formations around the arena
        { FVector(3000.0f, 2000.0f, ArenaFloorZ + 30.0f),   FRotator(0.0f, 72.0f, 0.0f),    0.8f },
        { FVector(-2500.0f, 3500.0f, ArenaFloorZ + 40.0f),  FRotator(0.0f, 144.0f, 0.0f),   0.9f },
        { FVector(4000.0f, -1500.0f, ArenaFloorZ + 20.0f),  FRotator(0.0f, 216.0f, 0.0f),   1.1f },
        { FVector(-3000.0f, -2000.0f, ArenaFloorZ + 60.0f), FRotator(0.0f, 288.0f, 0.0f),   0.7f },
        { FVector(1500.0f, -4000.0f, ArenaFloorZ + 35.0f),  FRotator(0.0f, 36.0f, 0.0f),    0.85f },
        { FVector(-5000.0f, 1000.0f, ArenaFloorZ + 45.0f),  FRotator(0.0f, 108.0f, 0.0f),   0.95f },
        { FVector(5500.0f, 4500.0f, ArenaFloorZ + 25.0f),   FRotator(0.0f, 180.0f, 0.0f),   0.75f },
    };

    int32 CoralIndex = 0;
    for (const auto& Placement : CoralPlacements)
    {
        // Alternate between CoralReef and CoralLighthouse for variety
        UStaticMesh* MeshToUse = (CoralIndex % 2 == 0) ? CoralMesh : CoralLighthouseMesh;
        if (!MeshToUse) MeshToUse = CoralMesh ? CoralMesh : CoralLighthouseMesh;

        FString Name = FString::Printf(TEXT("Coral_Cluster_%d"), CoralIndex);
        SpawnStaticMeshActor(
            MeshToUse,
            Placement.Location,
            Placement.Rotation,
            FVector(CoralScale * Placement.ScaleMult),
            Name);
        CoralIndex++;
    }
}

// ─── Shipwreck ──────────────────────────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnShipwreck()
{
    // Main shipwreck — tilted on its side, partially buried
    SpawnStaticMeshActor(
        ShipMesh,
        FVector(-4000.0f, -4000.0f, ArenaFloorZ + 300.0f),
        FRotator(15.0f, -30.0f, 25.0f),  // Tilted for dramatic effect
        FVector(ShipwreckScale),
        TEXT("Shipwreck_Main"));

    // Also load the cruise ship wreck (from wreck-of-cruise-ship folder)
    UStaticMesh* CruiseShipMesh = nullptr;

    // Try loading from wreck-of-cruise-ship folder (textures/ship is a texture, not a mesh)
    // The actual mesh should be in the source or root of the folder
    CruiseShipMesh = LoadObject<UStaticMesh>(nullptr,
        TEXT("/Game/Environments/wreck-of-cruise-ship-saudi-red-sea-coast/textures/ship.ship"));

    if (CruiseShipMesh)
    {
        SpawnStaticMeshActor(
            CruiseShipMesh,
            FVector(5000.0f, -3000.0f, ArenaFloorZ + 200.0f),
            FRotator(-10.0f, 60.0f, 20.0f),
            FVector(ShipwreckScale * 0.8f),
            TEXT("Shipwreck_CruiseShip"));
    }
}

// ─── Submarine Prop ─────────────────────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnSubmarineProp()
{
    // Decorative submarine sitting on the seabed
    SpawnStaticMeshActor(
        SubmarineMesh,
        FVector(2000.0f, 5000.0f, ArenaFloorZ + 400.0f),
        FRotator(0.0f, -45.0f, 5.0f),
        FVector(SubmarineScale),
        TEXT("Submarine_Decoration"));

    // Try loading additional submarine parts for a more complete model
    UStaticMesh* SubPart2 = LoadObject<UStaticMesh>(nullptr,
        TEXT("/Game/Drones/SubmarineModel/Object002.Object002"));
    UStaticMesh* SubPart3 = LoadObject<UStaticMesh>(nullptr,
        TEXT("/Game/Drones/SubmarineModel/Object003.Object003"));
    UStaticMesh* SubBody = LoadObject<UStaticMesh>(nullptr,
        TEXT("/Game/Drones/SubmarineModel/_Submarine2._Submarine2"));

    const FVector SubLoc(2000.0f, 5000.0f, ArenaFloorZ + 400.0f);
    const FRotator SubRot(0.0f, -45.0f, 5.0f);

    if (SubPart2) SpawnStaticMeshActor(SubPart2, SubLoc, SubRot, FVector(SubmarineScale), TEXT("Submarine_Part2"));
    if (SubPart3) SpawnStaticMeshActor(SubPart3, SubLoc, SubRot, FVector(SubmarineScale), TEXT("Submarine_Part3"));
    if (SubBody) SpawnStaticMeshActor(SubBody, SubLoc, SubRot, FVector(SubmarineScale), TEXT("Submarine_Body"));
}

// ─── Procedural Obstacles (Rocks/Boxes for hiding spots) ────────────────────

void AUnderwaterLevelSetup::SpawnProceduralObstacles()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // Use the engine's built-in primitive shapes as obstacles
    UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr,
        TEXT("/Engine/BasicShapes/Cube.Cube"));
    UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr,
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(nullptr,
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

    // Create deterministic but varied obstacle placement
    FRandomStream RNG(42); // Fixed seed for reproducibility

    for (int32 i = 0; i < NumRockClusters; i++)
    {
        // Random position within arena bounds
        float X = RNG.FRandRange(-ArenaRadius, ArenaRadius);
        float Y = RNG.FRandRange(-ArenaRadius, ArenaRadius);
        float Z = ArenaFloorZ + RNG.FRandRange(50.0f, 600.0f);

        // Random rotation
        FRotator Rot(RNG.FRandRange(-20.0f, 20.0f), RNG.FRandRange(0.0f, 360.0f), RNG.FRandRange(-15.0f, 15.0f));

        // Random scale (large boulders and smaller rocks)
        float BaseScale = RNG.FRandRange(1.5f, 5.0f);

        // Slightly non-uniform scale for rock-like shapes
        FVector Scale(
            BaseScale * RNG.FRandRange(0.6f, 1.4f),
            BaseScale * RNG.FRandRange(0.6f, 1.4f),
            BaseScale * RNG.FRandRange(0.4f, 1.2f));

        // Choose mesh shape
        UStaticMesh* ChosenMesh;
        int32 ShapeChoice = i % 3;
        if (ShapeChoice == 0)      ChosenMesh = SphereMesh;   // Boulder
        else if (ShapeChoice == 1) ChosenMesh = CubeMesh;     // Crate/Box
        else                       ChosenMesh = CylinderMesh; // Pillar/Column

        FString Name = FString::Printf(TEXT("Rock_Obstacle_%d"), i);
        SpawnStaticMeshActor(ChosenMesh, FVector(X, Y, Z), Rot, Scale, Name);
    }

    // Add some larger structures as major hiding spots
    // Big rock arch
    SpawnStaticMeshActor(CubeMesh,
        FVector(2500.0f, -2500.0f, ArenaFloorZ + 200.0f),
        FRotator(35.0f, 20.0f, 0.0f),
        FVector(8.0f, 3.0f, 6.0f),
        TEXT("RockArch_1"));

    SpawnStaticMeshActor(CubeMesh,
        FVector(-3500.0f, 3500.0f, ArenaFloorZ + 150.0f),
        FRotator(-20.0f, 60.0f, 10.0f),
        FVector(6.0f, 4.0f, 7.0f),
        TEXT("RockArch_2"));

    // Tall rock pillars
    SpawnStaticMeshActor(CylinderMesh,
        FVector(0.0f, -5000.0f, ArenaFloorZ + 100.0f),
        FRotator(5.0f, 0.0f, 0.0f),
        FVector(2.0f, 2.0f, 12.0f),
        TEXT("RockPillar_1"));

    SpawnStaticMeshActor(CylinderMesh,
        FVector(-6000.0f, 0.0f, ArenaFloorZ + 80.0f),
        FRotator(-3.0f, 45.0f, 0.0f),
        FVector(2.5f, 2.5f, 10.0f),
        TEXT("RockPillar_2"));
}

// ─── Underwater Environment Actor ───────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnEnvironmentActor()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // Check if one already exists in the level
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(World, AUnderwaterEnvironmentActor::StaticClass(), Found);
    if (Found.Num() > 0)
    {
        UE_LOG(LogUnderwaterSim, Log, TEXT("  UnderwaterEnvironmentActor already exists — skipping."));
        return;
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AUnderwaterEnvironmentActor* EnvActor = World->SpawnActor<AUnderwaterEnvironmentActor>(
        AUnderwaterEnvironmentActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);

    if (EnvActor)
    {
        EnvActor->WaterSurfaceZ = 0.0f;
        EnvActor->WaterDensity = 1025.0f;
        EnvActor->Turbidity = 0.4f;
        EnvActor->MaxVisualRangeMeters = 20.0f;
        EnvActor->GlobalCurrentVelocity = FVector(0.15f, 0.08f, 0.0f);
        UE_LOG(LogUnderwaterSim, Log, TEXT("  Spawned UnderwaterEnvironmentActor."));
    }
}

// ─── Drones ─────────────────────────────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnDrones()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // Check if drones already exist (placed manually or by Blueprint)
    TArray<AActor*> ExistingDrones;
    UGameplayStatics::GetAllActorsOfClass(World, AUnderwaterDrone::StaticClass(), ExistingDrones);
    if (ExistingDrones.Num() >= 2)
    {
        UE_LOG(LogUnderwaterSim, Log, TEXT("  Drones already exist (%d found) — skipping."), ExistingDrones.Num());
        return;
    }

    // Try to spawn from Blueprints first (they have the visual mesh assigned)
    UClass* SeekerBP = LoadObject<UClass>(nullptr,
        TEXT("/Game/Blueprints/BP_SeekerDrone.BP_SeekerDrone_C"));
    UClass* HiderBP = LoadObject<UClass>(nullptr,
        TEXT("/Game/Blueprints/BP_HiderDrone.BP_HiderDrone_C"));

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Seeker Drone — near one edge of the arena
    AUnderwaterDrone* Seeker = nullptr;
    FVector SeekerLoc(3000.0f, 3000.0f, ArenaFloorZ + 1500.0f);

    if (SeekerBP)
    {
        Seeker = World->SpawnActor<AUnderwaterDrone>(SeekerBP, SeekerLoc, FRotator::ZeroRotator, Params);
    }
    else
    {
        Seeker = World->SpawnActor<AUnderwaterDrone>(AUnderwaterDrone::StaticClass(), SeekerLoc, FRotator::ZeroRotator, Params);
    }

    if (Seeker)
    {
        Seeker->DroneRole = EDroneRole::Seeker;
        Seeker->DroneID = 1;
        UE_LOG(LogUnderwaterSim, Log, TEXT("  Spawned SEEKER drone at (%.0f, %.0f, %.0f)"), SeekerLoc.X, SeekerLoc.Y, SeekerLoc.Z);
    }

    // Hider Drone — opposite side of arena
    AUnderwaterDrone* Hider = nullptr;
    FVector HiderLoc(-3000.0f, -3000.0f, ArenaFloorZ + 1200.0f);

    if (HiderBP)
    {
        Hider = World->SpawnActor<AUnderwaterDrone>(HiderBP, HiderLoc, FRotator(0.0f, 180.0f, 0.0f), Params);
    }
    else
    {
        Hider = World->SpawnActor<AUnderwaterDrone>(AUnderwaterDrone::StaticClass(), HiderLoc, FRotator(0.0f, 180.0f, 0.0f), Params);
    }

    if (Hider)
    {
        Hider->DroneRole = EDroneRole::Hider;
        Hider->DroneID = 2;
        UE_LOG(LogUnderwaterSim, Log, TEXT("  Spawned HIDER drone at (%.0f, %.0f, %.0f)"), HiderLoc.X, HiderLoc.Y, HiderLoc.Z);
    }
}

// ─── Atmosphere (Fog + Light) ───────────────────────────────────────────────

void AUnderwaterLevelSetup::ConfigureAtmosphere()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // ── Configure existing Exponential Height Fog (or spawn one) ──
    TArray<AActor*> FogActors;
    UGameplayStatics::GetAllActorsOfClass(World, AExponentialHeightFog::StaticClass(), FogActors);

    AExponentialHeightFog* Fog = nullptr;
    if (FogActors.Num() > 0)
    {
        Fog = Cast<AExponentialHeightFog>(FogActors[0]);
    }
    else
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        Fog = World->SpawnActor<AExponentialHeightFog>(AExponentialHeightFog::StaticClass(),
            FVector(0.0f, 0.0f, ArenaFloorZ + 500.0f), FRotator::ZeroRotator, Params);
    }

    if (Fog)
    {
        UExponentialHeightFogComponent* FogComp = Fog->FindComponentByClass<UExponentialHeightFogComponent>();
        if (FogComp)
        {
            // Deep blue-green underwater fog
            FogComp->SetFogDensity(0.05f);
            FogComp->SetFogHeightFalloff(0.001f);
            FogComp->SetFogInscatteringColor(FLinearColor(0.01f, 0.05f, 0.12f)); // Deep blue
            FogComp->SetDirectionalInscatteringColor(FLinearColor(0.0f, 0.08f, 0.15f));
            FogComp->SetFogMaxOpacity(0.95f);
            FogComp->SetStartDistance(100.0f);
            FogComp->SetFogCutoffDistance(25000.0f);
            FogComp->SetVolumetricFog(true);

            UE_LOG(LogUnderwaterSim, Log, TEXT("  Configured underwater fog atmosphere."));
        }
    }

    // ── Dim the Directional Light for deep-water feel ──
    TArray<AActor*> LightActors;
    UGameplayStatics::GetAllActorsOfClass(World, ADirectionalLight::StaticClass(), LightActors);

    for (AActor* LightActor : LightActors)
    {
        ADirectionalLight* DirLight = Cast<ADirectionalLight>(LightActor);
        if (DirLight)
        {
            UDirectionalLightComponent* LightComp = DirLight->FindComponentByClass<UDirectionalLightComponent>();
            if (LightComp)
            {
                // Reduce intensity and tint blue for underwater feel
                LightComp->SetIntensity(2.0f); // Dimmer than default
                LightComp->SetLightColor(FLinearColor(0.4f, 0.6f, 0.9f)); // Blue-ish
                UE_LOG(LogUnderwaterSim, Log, TEXT("  Dimmed directional light for deep-water atmosphere."));
            }
        }
    }
}

