// HideAndSeekGameMode.cpp
#include "HideAndSeekGameMode.h"
#include "Environment/UnderwaterLevelSetup.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "UnderwaterSim.h"

AHideAndSeekGameMode::AHideAndSeekGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set the drone as the default pawn so player spawns AS the drone
    DefaultPawnClass = AUnderwaterDrone::StaticClass();
}

void AHideAndSeekGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Ensure the underwater world is populated
    TArray<AActor*> FoundSetups;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnderwaterLevelSetup::StaticClass(), FoundSetups);
    if (FoundSetups.Num() == 0)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        AUnderwaterLevelSetup* LevelSetup = GetWorld()->SpawnActor<AUnderwaterLevelSetup>(
            AUnderwaterLevelSetup::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        if (LevelSetup)
        {
            UE_LOG(LogUnderwaterSim, Log, TEXT("GameMode spawned AUnderwaterLevelSetup to populate arena."));
        }
    }

    FindDrones();

    // Auto-possess the Seeker drone so the player controls it directly
    if (SeekerDrone)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            PC->Possess(SeekerDrone);
            UE_LOG(LogUnderwaterSim, Log, TEXT("Player now possessing SEEKER drone!"));
        }
    }
    else
    {
        UE_LOG(LogUnderwaterSim, Warning, TEXT("No Seeker drone found to possess! Player uses default pawn."));
    }

    StartNewMatch();
}

void AHideAndSeekGameMode::FindDrones()
{
    TArray<AActor*> FoundDrones;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnderwaterDrone::StaticClass(), FoundDrones);

    for (AActor* Actor : FoundDrones)
    {
        AUnderwaterDrone* Drone = Cast<AUnderwaterDrone>(Actor);
        if (Drone)
        {
            if (Drone->DroneRole == EDroneRole::Seeker && !SeekerDrone)
            {
                SeekerDrone = Drone;
            }
            else if (Drone->DroneRole == EDroneRole::Hider && !HiderDrone)
            {
                HiderDrone = Drone;
            }
        }
    }
}

void AHideAndSeekGameMode::StartNewMatch()
{
    RoundTimer = 0.0f;
    CurrentPhase = EGameStatePhase::HidingPhase;
    OnGamePhaseChanged.Broadcast(CurrentPhase);

    UE_LOG(LogUnderwaterSim, Log, TEXT("Game started! Phase: HIDING (Headstart: %.1fs)"), HidingHeadstartSeconds);
}

void AHideAndSeekGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentPhase == EGameStatePhase::SeekerWon || CurrentPhase == EGameStatePhase::HiderWon)
    {
        return;
    }

    RoundTimer += DeltaTime;

    // Transition from Hiding Phase to Active Seeking Phase
    if (CurrentPhase == EGameStatePhase::HidingPhase && RoundTimer >= HidingHeadstartSeconds)
    {
        CurrentPhase = EGameStatePhase::SeekingPhase;
        OnGamePhaseChanged.Broadcast(CurrentPhase);
        UE_LOG(LogUnderwaterSim, Log, TEXT("Phase Changed: ACTIVE SEEKING!"));
    }

    // Check round timeout
    if (RoundTimer >= MaxRoundDurationSeconds)
    {
        CurrentPhase = EGameStatePhase::HiderWon;
        OnGamePhaseChanged.Broadcast(CurrentPhase);
        UE_LOG(LogUnderwaterSim, Log, TEXT("Match Over: HIDER SURVIVED!"));
        return;
    }

    // Check capture condition if seeking is active
    if (CurrentPhase == EGameStatePhase::SeekingPhase)
    {
        CheckCaptureCondition();
    }
}

void AHideAndSeekGameMode::CheckCaptureCondition()
{
    if (!SeekerDrone || !HiderDrone)
    {
        FindDrones();
        return;
    }

    const float DistanceCm = FVector::Dist(SeekerDrone->GetActorLocation(), HiderDrone->GetActorLocation());
    CurrentDistanceBetweenDrones = DistanceCm / 100.0f; // meters

    if (CurrentDistanceBetweenDrones <= CaptureDistanceThresholdMeters)
    {
        CurrentPhase = EGameStatePhase::SeekerWon;
        OnGamePhaseChanged.Broadcast(CurrentPhase);
        OnTargetCaptured.Broadcast(SeekerDrone, HiderDrone);
        UE_LOG(LogUnderwaterSim, Log, TEXT("Match Over: SEEKER CAPTURED HIDER at distance %.2fm!"), CurrentDistanceBetweenDrones);
    }
}

