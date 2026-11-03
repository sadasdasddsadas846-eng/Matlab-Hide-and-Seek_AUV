// UnderwaterEnvironmentActor.h
// Controls global underwater conditions, water currents, and optical/acoustic properties.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnderwaterEnvironmentActor.generated.h"

class UPostProcessComponent;
class UDirectionalLightComponent;

UCLASS()
class UNDERWATERSIM_API AUnderwaterEnvironmentActor : public AActor
{
    GENERATED_BODY()

public:
    AUnderwaterEnvironmentActor();

    virtual void Tick(float DeltaTime) override;

    // ── Water Properties ──

    /** Water surface elevation in world Z (cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean")
    float WaterSurfaceZ = 0.0f;

    /** Seawater density (kg/m^3) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean")
    float WaterDensity = 1025.0f;

    // ── Global Water Current ──

    /** Constant or baseline ocean current velocity (m/s in world space) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currents")
    FVector GlobalCurrentVelocity = FVector(0.2f, 0.1f, 0.0f); // m/s

    /** Current turbulence/fluctuation amplitude */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currents")
    float CurrentTurbulence = 0.05f;

    // ── Visual / Optical Properties ──

    /** Underwater visual turbidity (0 = crystal clear, 1 = murky harbor) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility", meta=(ClampMin="0.0", ClampMax="1.0"))
    float Turbidity = 0.35f;

    /** Maximum visual range in meters before complete extinction */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
    float MaxVisualRangeMeters = 15.0f;

    /** Post process component for underwater tint and fog */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
    UPostProcessComponent* UnderwaterPostProcess;

    // ── API ──

    /** Evaluates total fluid current vector at a given world position */
    UFUNCTION(BlueprintPure, Category = "Environment")
    FVector GetCurrentAtLocation(const FVector& WorldLocation) const;

protected:
    virtual void BeginPlay() override;

private:
    float SimulationTime = 0.0f;
    void UpdatePostProcessSettings();
};
