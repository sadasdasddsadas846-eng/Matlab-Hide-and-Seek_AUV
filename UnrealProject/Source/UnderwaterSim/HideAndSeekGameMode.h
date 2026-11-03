// HideAndSeekGameMode.h
// Manages the rules, round phases, capture conditions, and scoring for Underwater Hide-and-Seek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Drone/UnderwaterDrone.h"
#include "HideAndSeekGameMode.generated.h"

UENUM(BlueprintType)
enum class EGameStatePhase : uint8
{
    Preparation     UMETA(DisplayName = "Preparation"),
    HidingPhase     UMETA(DisplayName = "Hiding Phase (Hider Headstart)"),
    SeekingPhase    UMETA(DisplayName = "Seeking Phase (Active Search)"),
    SeekerWon       UMETA(DisplayName = "Seeker Won (Target Captured)"),
    HiderWon        UMETA(DisplayName = "Hider Won (Time Expired)")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EGameStatePhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTargetCaptured, AUnderwaterDrone*, Seeker, AUnderwaterDrone*, Hider);

UCLASS()
class UNDERWATERSIM_API AHideAndSeekGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AHideAndSeekGameMode();

    virtual void Tick(float DeltaTime) override;

    // ── Game Parameters ──

    /** Duration of the hiding headstart in seconds */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rules")
    float HidingHeadstartSeconds = 15.0f;

    /** Total round duration in seconds */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rules")
    float MaxRoundDurationSeconds = 180.0f;

    /** Distance threshold in meters for successful capture */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rules")
    float CaptureDistanceThresholdMeters = 2.5f;

    // ── Current State ──

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rules|State")
    EGameStatePhase CurrentPhase = EGameStatePhase::Preparation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rules|State")
    float RoundTimer = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rules|State")
    float CurrentDistanceBetweenDrones = 0.0f;

    // ── Events ──

    UPROPERTY(BlueprintAssignable, Category = "Rules|Events")
    FOnGamePhaseChanged OnGamePhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Rules|Events")
    FOnTargetCaptured OnTargetCaptured;

    UFUNCTION(BlueprintCallable, Category = "Rules")
    void StartNewMatch();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    AUnderwaterDrone* SeekerDrone = nullptr;

    UPROPERTY()
    AUnderwaterDrone* HiderDrone = nullptr;

    void FindDrones();
    void CheckCaptureCondition();
};

