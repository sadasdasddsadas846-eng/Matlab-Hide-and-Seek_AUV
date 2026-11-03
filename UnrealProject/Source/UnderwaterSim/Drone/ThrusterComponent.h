// ThrusterComponent.h
// Simulates an individual thruster on an underwater drone (ROV/AUV).

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ThrusterComponent.generated.h"

/**
 * Thruster configuration preset matching common ROV setups.
 */
UENUM(BlueprintType)
enum class EThrusterType : uint8
{
    /** Horizontal thruster for forward/backward/strafe motion */
    Horizontal    UMETA(DisplayName = "Horizontal"),
    /** Vertical thruster for depth control */
    Vertical      UMETA(DisplayName = "Vertical"),
    /** Lateral thruster for yaw rotation */
    Lateral       UMETA(DisplayName = "Lateral")
};

/**
 * UThrusterComponent
 *
 * Represents a single thruster mounted on the drone body.
 * Applies force along its forward axis, with realistic response
 * characteristics (ramp-up time, max thrust, deadband).
 *
 * Typical BlueROV2 configuration: 6 thrusters
 *   - 4x Horizontal (vectored, 45° angles for omnidirectional XY)
 *   - 2x Vertical (for depth control)
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNDERWATERSIM_API UThrusterComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UThrusterComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    // ── Configuration ──

    /** Type of thruster */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thruster")
    EThrusterType ThrusterType = EThrusterType::Horizontal;

    /** Maximum thrust force in Newtons (T200 thruster: ~51N forward, ~40N reverse) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thruster", meta=(ClampMin="0"))
    float MaxThrustForward = 51.0f;

    /** Maximum reverse thrust in Newtons */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thruster", meta=(ClampMin="0"))
    float MaxThrustReverse = 40.0f;

    /** Time constant for thruster ramp-up in seconds */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thruster", meta=(ClampMin="0.01"))
    float ResponseTime = 0.15f;

    /** Deadband: minimum input below which thruster produces no force */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thruster", meta=(ClampMin="0", ClampMax="0.5"))
    float Deadband = 0.05f;

    // ── Input ──

    /**
     * Set thruster input (-1 to +1).
     * Positive = thrust along forward vector, Negative = reverse.
     */
    UFUNCTION(BlueprintCallable, Category = "Thruster")
    void SetThrustInput(float Input);

    /** Get current thrust input */
    UFUNCTION(BlueprintPure, Category = "Thruster")
    float GetThrustInput() const { return TargetInput; }

    /** Get current actual thrust force in Newtons */
    UFUNCTION(BlueprintPure, Category = "Thruster")
    float GetCurrentThrust() const { return CurrentThrust; }

    /** Get thrust direction in world space */
    UFUNCTION(BlueprintPure, Category = "Thruster")
    FVector GetThrustDirection() const;

    // ── State (visible for debugging) ──

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Thruster|Debug")
    float CurrentThrust = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Thruster|Debug")
    float CurrentInput = 0.0f;

protected:
    virtual void BeginPlay() override;

private:
    float TargetInput = 0.0f;

    UPROPERTY()
    UPrimitiveComponent* ParentPhysicsBody;

    /** Apply the computed thrust force to the parent physics body */
    void ApplyThrustForce();
};

