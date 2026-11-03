// UnderwaterDrone.h
// Main Pawn class for the underwater drone (BlueROV2-inspired).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UnderwaterDrone.generated.h"

class UBuoyancyComponent;
class UThrusterComponent;
class UStaticMeshComponent;
class UCameraComponent;
class USpringArmComponent;

/**
 * Drone role in the hide-and-seek game.
 */
UENUM(BlueprintType)
enum class EDroneRole : uint8
{
    Seeker   UMETA(DisplayName = "Seeker"),
    Hider    UMETA(DisplayName = "Hider"),
    Neutral  UMETA(DisplayName = "Neutral")
};

/**
 * AUnderwaterDrone
 *
 * A physics-based underwater drone pawn with:
 * - 6-DOF movement via vectored thrusters (BlueROV2 layout)
 * - Buoyancy and hydrodynamic drag
 * - Onboard camera and sensor attachment points
 * - ROS 2 interface for autonomous control
 *
 * Thruster Layout (BlueROV2 Heavy):
 *   Top-down view:
 *     T1(FL)──────T2(FR)     Horizontal thrusters at 45° angles
 *         \      /           for omnidirectional XY movement
 *          [BODY]
 *         /      \
 *     T3(RL)──────T4(RR)
 *
 *     T5(VL)    T6(VR)       Vertical thrusters for depth control
 */
UCLASS()
class UNDERWATERSIM_API AUnderwaterDrone : public APawn
{
    GENERATED_BODY()

public:
    AUnderwaterDrone();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // ── Components ──

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone")
    UStaticMeshComponent* DroneBody;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone")
    UBuoyancyComponent* BuoyancyComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Camera")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Camera")
    UCameraComponent* FollowCamera;

    // ── Thrusters (BlueROV2 Heavy: 6 thrusters) ──

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Thrusters")
    UThrusterComponent* ThrusterFL; // Front-Left horizontal

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Thrusters")
    UThrusterComponent* ThrusterFR; // Front-Right horizontal

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Thrusters")
    UThrusterComponent* ThrusterRL; // Rear-Left horizontal

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Thrusters")
    UThrusterComponent* ThrusterRR; // Rear-Right horizontal

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Thrusters")
    UThrusterComponent* ThrusterVL; // Vertical Left

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Thrusters")
    UThrusterComponent* ThrusterVR; // Vertical Right

    // ── Game Properties ──

    /** Role of this drone in the hide-and-seek game */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    EDroneRole DroneRole = EDroneRole::Neutral;

    /** Unique drone ID for multi-agent identification */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    int32 DroneID = 0;

    // ── Movement API (called by AI or player input) ──

    /** Move forward/backward (-1 to 1) */
    UFUNCTION(BlueprintCallable, Category = "Drone|Movement")
    void SetSurge(float Value);

    /** Move left/right (-1 to 1) */
    UFUNCTION(BlueprintCallable, Category = "Drone|Movement")
    void SetSway(float Value);

    /** Move up/down (-1 to 1) */
    UFUNCTION(BlueprintCallable, Category = "Drone|Movement")
    void SetHeave(float Value);

    /** Rotate yaw (-1 to 1) */
    UFUNCTION(BlueprintCallable, Category = "Drone|Movement")
    void SetYaw(float Value);

    /** Stop all thrusters */
    UFUNCTION(BlueprintCallable, Category = "Drone|Movement")
    void StopAllThrusters();

    /** Get current velocity in m/s */
    UFUNCTION(BlueprintPure, Category = "Drone|State")
    FVector GetVelocityMetersPerSec() const;

    /** Get current depth below water surface in meters */
    UFUNCTION(BlueprintPure, Category = "Drone|State")
    float GetCurrentDepth() const;

protected:
    virtual void BeginPlay() override;

private:
    /** Create and configure all thruster components */
    void SetupThrusters();

    /** Input binding callbacks */
    void InputSurge(float Value);
    void InputSway(float Value);
    void InputHeave(float Value);
    void InputYaw(float Value);
};

