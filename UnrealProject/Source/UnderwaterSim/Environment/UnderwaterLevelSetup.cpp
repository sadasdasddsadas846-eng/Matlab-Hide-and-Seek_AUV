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

    // Coral Reef mesh (122MB static mesh)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CoralFinder(
        TEXT("/Game/Environments/Corals/CoralReef.CoralReef"));
    if (CoralFinder.Succeeded()) CoralMesh = CoralFinder.Object;

    // Submarine mesh (_Submarine2.uasset is the 4MB static mesh)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SubFinder(
        TEXT("/Game/Drones/SubmarineModel/_Submarine2._Submarine2"));
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

    // Note: UnderwaterArena already has a complete Landscape floor at Z=-1250.
    // Do NOT spawn model1.model1 (SpawnTerrain) as it contains floating scan debris.
    SpawnCorals();
    SpawnSubmarineProp();
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

        UE_LOG(LogUnderwaterSim, Log, TEXT("  Spawned: %s at (%.0f, %.0f, %.0f) Rot=(P=%.0f, Y=%.0f, R=%.0f)"),
               *Label, Location.X, Location.Y, Location.Z, Rotation.Pitch, Rotation.Yaw, Rotation.Roll);
    }

    return MeshActor;
}

// ─── Terrain / Seabed ───────────────────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnTerrain()
{
    // Disabled: UnderwaterArena already uses the built-in Landscape floor.
}

// ─── Coral Clusters ─────────────────────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnCorals()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // Check if coral actors already exist in the level (placed in Editor)
    TArray<AActor*> StaticMeshActors;
    UGameplayStatics::GetAllActorsOfClass(World, AStaticMeshActor::StaticClass(), StaticMeshActors);

    int32 ExistingCoralCount = 0;
    for (AActor* Actor : StaticMeshActors)
    {
        AStaticMeshActor* SMA = Cast<AStaticMeshActor>(Actor);
        if (SMA && SMA->GetStaticMeshComponent() && SMA->GetStaticMeshComponent()->GetStaticMesh())
        {
            const FString MeshPath = SMA->GetStaticMeshComponent()->GetStaticMesh()->GetPathName();
            if (MeshPath.Contains(TEXT("CoralReef")))
            {
                // Ensure existing coral is right-side up and flat on seabed: Pitch=0, Roll=+90
                SMA->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
                FRotator CurRot = SMA->GetActorRotation();
                FVector CurLoc = SMA->GetActorLocation();
                SMA->SetActorLocation(FVector(CurLoc.X, CurLoc.Y, ArenaFloorZ + 30.0f));
                SMA->SetActorRotation(FRotator(0.0f, CurRot.Yaw, 90.0f));
                ExistingCoralCount++;
            }
            else if (MeshPath.Contains(TEXT("model1")) || MeshPath.Contains(TEXT("sketchfabTemp")))
            {
                // Destroy any broken floating photogrammetry scrap actors still in the map
                SMA->Destroy();
            }
        }
    }

    if (ExistingCoralCount > 0)
    {
        UE_LOG(LogUnderwaterSim, Log, TEXT("  Found %d existing CoralReef actors in level (straightened to Roll=+90)."), ExistingCoralCount);
        return;
    }

    // In CoralReef.uasset, the coral top surface faces -Y at Rot(0,0,0) due to FBX Y-inversion.
    // FRotator(Pitch=0, Yaw=..., Roll=+90) rotates -Y directly into +Z (World Up), laying it 100% flat and right-side up.
    struct FCoralPlacement
    {
        FVector Location;
        FRotator Rotation;
        float ScaleMult;
    };

    const float CoralZ = ArenaFloorZ + 30.0f; // Sits flush on the Landscape floor
    const TArray<FCoralPlacement> CoralPlacements = {
        { FVector(0.0f, 0.0f, CoralZ),             FRotator(0.0f, 0.0f, 90.0f),   1.0f },
        { FVector(2800.0f, 2000.0f, CoralZ),       FRotator(0.0f, 60.0f, 90.0f),  0.85f },
        { FVector(-2600.0f, -1800.0f, CoralZ),     FRotator(0.0f, 150.0f, 90.0f), 0.85f },
    };

    int32 CoralIndex = 0;
    for (const auto& Placement : CoralPlacements)
    {
        if (!CoralMesh) continue;

        FString Name = FString::Printf(TEXT("Coral_Cluster_%d"), CoralIndex);
        SpawnStaticMeshActor(
            CoralMesh,
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
    // Shipwreck static mesh optional
}

// ─── Submarine Prop ─────────────────────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnSubmarineProp()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // Check if Submarine decoration already exists in the map
    TArray<AActor*> StaticMeshActors;
    UGameplayStatics::GetAllActorsOfClass(World, AStaticMeshActor::StaticClass(), StaticMeshActors);

    bool bFoundFirstSub = false;
    for (AActor* Actor : StaticMeshActors)
    {
        AStaticMeshActor* SMA = Cast<AStaticMeshActor>(Actor);
        if (SMA && SMA->GetStaticMeshComponent() && SMA->GetStaticMeshComponent()->GetStaticMesh())
        {
            const FString MeshPath = SMA->GetStaticMeshComponent()->GetStaticMesh()->GetPathName();
            if (MeshPath.Contains(TEXT("_Submarine2")))
            {
                if (!bFoundFirstSub)
                {
                    bFoundFirstSub = true;
                    SMA->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
                    // In _Submarine2, local UP is -Y; Roll=+90 rotates -Y to +Z (upright!)
                    SMA->SetActorLocation(FVector(1200.0f, 800.0f, ArenaFloorZ + 70.0f));
                    SMA->SetActorRotation(FRotator(0.0f, -45.0f, 90.0f));
                    SMA->SetActorScale3D(FVector(SubmarineScale));
                }
                else
                {
                    // Remove duplicate submarine static mesh actor
                    SMA->Destroy();
                }
            }
        }
    }

    if (bFoundFirstSub)
    {
        UE_LOG(LogUnderwaterSim, Log, TEXT("  Straightened existing Submarine prop to Roll=+90 (upright)."));
        return;
    }

    if (SubmarineMesh)
    {
        SpawnStaticMeshActor(
            SubmarineMesh,
            FVector(1200.0f, 800.0f, ArenaFloorZ + 70.0f),
            FRotator(0.0f, -45.0f, 90.0f), // Roll=+90 rotates local -Y up axis to +Z (upright)
            FVector(SubmarineScale),
            TEXT("Submarine_Decoration"));
    }
}

// ─── Procedural Obstacles ───────────────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnProceduralObstacles()
{
    // Disabled
}

// ─── Underwater Environment Actor ───────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnEnvironmentActor()
{
    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(World, AUnderwaterEnvironmentActor::StaticClass(), Found);
    if (Found.Num() > 0)
    {
        for (int32 i = 1; i < Found.Num(); ++i)
        {
            Found[i]->Destroy();
        }
        return;
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AUnderwaterEnvironmentActor* EnvActor = World->SpawnActor<AUnderwaterEnvironmentActor>(
        AUnderwaterEnvironmentActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);

    if (EnvActor)
    {
        EnvActor->WaterSurfaceZ = 810.0f;
        EnvActor->WaterDensity = 1025.0f;
        EnvActor->Turbidity = 0.4f;
        EnvActor->MaxVisualRangeMeters = 20.0f;
        EnvActor->GlobalCurrentVelocity = FVector::ZeroVector;
    }
}

// ─── Drones ─────────────────────────────────────────────────────────────────

void AUnderwaterLevelSetup::SpawnDrones()
{
    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> ExistingDrones;
    UGameplayStatics::GetAllActorsOfClass(World, AUnderwaterDrone::StaticClass(), ExistingDrones);

    AUnderwaterDrone* FoundSeeker = nullptr;
    AUnderwaterDrone* FoundHider = nullptr;

    for (AActor* Actor : ExistingDrones)
    {
        AUnderwaterDrone* Drone = Cast<AUnderwaterDrone>(Actor);
        if (!Drone) continue;

        const FString Name = Drone->GetName();
        if ((Drone->DroneRole == EDroneRole::Seeker || Name.Contains(TEXT("Seeker"))) && !FoundSeeker)
        {
            FoundSeeker = Drone;
            FoundSeeker->DroneRole = EDroneRole::Seeker;
            FoundSeeker->DroneID = 1;
            FoundSeeker->SetActorLocation(FVector(800.0f, 800.0f, -500.0f));
            FoundSeeker->SetActorRotation(FRotator(0.0f, -135.0f, 0.0f));
        }
        else if ((Drone->DroneRole == EDroneRole::Hider || Name.Contains(TEXT("Hider"))) && !FoundHider)
        {
            FoundHider = Drone;
            FoundHider->DroneRole = EDroneRole::Hider;
            FoundHider->DroneID = 2;
            FoundHider->SetActorLocation(FVector(-800.0f, -800.0f, -700.0f));
            FoundHider->SetActorRotation(FRotator(0.0f, 45.0f, 0.0f));
        }
        else
        {
            // Destroy duplicate drones
            Drone->Destroy();
        }
    }

    UClass* SeekerBP = LoadObject<UClass>(nullptr,
        TEXT("/Game/Blueprints/BP_SeekerDrone.BP_SeekerDrone_C"));
    UClass* HiderBP = LoadObject<UClass>(nullptr,
        TEXT("/Game/Blueprints/BP_HiderDrone.BP_HiderDrone_C"));

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (!FoundSeeker)
    {
        FVector SeekerLoc(800.0f, 800.0f, -500.0f);
        FRotator SeekerRot(0.0f, -135.0f, 0.0f);
        UClass* ClassToSpawn = SeekerBP ? SeekerBP : AUnderwaterDrone::StaticClass();
        FoundSeeker = World->SpawnActor<AUnderwaterDrone>(ClassToSpawn, SeekerLoc, SeekerRot, Params);
        if (FoundSeeker)
        {
            FoundSeeker->DroneRole = EDroneRole::Seeker;
            FoundSeeker->DroneID = 1;
        }
    }

    if (!FoundHider)
    {
        FVector HiderLoc(-800.0f, -800.0f, -700.0f);
        FRotator HiderRot(0.0f, 45.0f, 0.0f);
        UClass* ClassToSpawn = HiderBP ? HiderBP : AUnderwaterDrone::StaticClass();
        FoundHider = World->SpawnActor<AUnderwaterDrone>(ClassToSpawn, HiderLoc, HiderRot, Params);
        if (FoundHider)
        {
            FoundHider->DroneRole = EDroneRole::Hider;
            FoundHider->DroneID = 2;
        }
    }
}

// ─── Atmosphere (Fog + Light) ───────────────────────────────────────────────

void AUnderwaterLevelSetup::ConfigureAtmosphere()
{
    UWorld* World = GetWorld();
    if (!World) return;

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
            FogComp->SetFogDensity(0.02f);
            FogComp->SetFogHeightFalloff(0.001f);
            FogComp->SetFogInscatteringColor(FLinearColor(0.01f, 0.06f, 0.14f));
            FogComp->SetDirectionalInscatteringColor(FLinearColor(0.0f, 0.08f, 0.15f));
            FogComp->SetFogMaxOpacity(0.9f);
            FogComp->SetStartDistance(200.0f);
            FogComp->SetFogCutoffDistance(30000.0f);
            FogComp->SetVolumetricFog(true);
        }
    }
}
