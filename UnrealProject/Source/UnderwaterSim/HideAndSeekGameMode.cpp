// HideAndSeekGameMode.cpp
#include "HideAndSeekGameMode.h"
#include "Environment/UnderwaterLevelSetup.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "UnderwaterSim.h"

AHideAndSeekGameMode::AHideAndSeekGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    DefaultPawnClass = AUnderwaterDrone::StaticClass();
}

void AHideAndSeekGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Ensure the underwater world is populated immediately before finding drones
    TArray<AActor*> FoundSetups;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnderwaterLevelSetup::StaticClass(), FoundSetups);
    AUnderwaterLevelSetup* LevelSetup = nullptr;
    if (FoundSetups.Num() == 0)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        LevelSetup = GetWorld()->SpawnActor<AUnderwaterLevelSetup>(
            AUnderwaterLevelSetup::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    }
    else
    {
        LevelSetup = Cast<AUnderwaterLevelSetup>(FoundSetups[0]);
    }

    if (LevelSetup)
    {
        LevelSetup->PopulateLevel();
    }

    FindDrones();

    // Auto-possess the Seeker drone so the player controls it directly
    if (SeekerDrone)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            APawn* OldPawn = PC->GetPawn();
            if (OldPawn && OldPawn != SeekerDrone)
            {
                PC->UnPossess();
                OldPawn->Destroy();
            }
            PC->Possess(SeekerDrone);
            PC->SetControlRotation(FRotator(-15.0f, SeekerDrone->GetActorRotation().Yaw, 0.0f));
            UE_LOG(LogUnderwaterSim, Log, TEXT("Player now possessing SEEKER drone (%s)!"), *SeekerDrone->GetName());
        }
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
        if (!Drone) continue;

        const FString Name = Drone->GetName();
        if ((Drone->DroneRole == EDroneRole::Seeker || Name.Contains(TEXT("Seeker"))) && !SeekerDrone)
        {
            SeekerDrone = Drone;
            SeekerDrone->DroneRole = EDroneRole::Seeker;
        }
        else if ((Drone->DroneRole == EDroneRole::Hider || Name.Contains(TEXT("Hider"))) && !HiderDrone)
        {
            HiderDrone = Drone;
            HiderDrone->DroneRole = EDroneRole::Hider;
        }
    }

    // Fallback: if still no SeekerDrone, pick the first available drone
    if (!SeekerDrone && FoundDrones.Num() > 0)
    {
        SeekerDrone = Cast<AUnderwaterDrone>(FoundDrones[0]);
        if (SeekerDrone) SeekerDrone->DroneRole = EDroneRole::Seeker;
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

    if (CurrentPhase == EGameStatePhase::HidingPhase && RoundTimer >= HidingHeadstartSeconds)
    {
        CurrentPhase = EGameStatePhase::SeekingPhase;
        OnGamePhaseChanged.Broadcast(CurrentPhase);
        UE_LOG(LogUnderwaterSim, Log, TEXT("Phase Changed: ACTIVE SEEKING!"));
    }

    if (RoundTimer >= MaxRoundDurationSeconds)
    {
        CurrentPhase = EGameStatePhase::HiderWon;
        OnGamePhaseChanged.Broadcast(CurrentPhase);
        UE_LOG(LogUnderwaterSim, Log, TEXT("Match Over: HIDER SURVIVED!"));
        return;
    }

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
    CurrentDistanceBetweenDrones = DistanceCm / 100.0f;

    if (CurrentDistanceBetweenDrones <= CaptureDistanceThresholdMeters)
    {
        CurrentPhase = EGameStatePhase::SeekerWon;
        OnGamePhaseChanged.Broadcast(CurrentPhase);
        OnTargetCaptured.Broadcast(SeekerDrone, HiderDrone);
        UE_LOG(LogUnderwaterSim, Log, TEXT("Match Over: SEEKER CAPTURED HIDER at distance %.2fm!"), CurrentDistanceBetweenDrones);
    }
}
