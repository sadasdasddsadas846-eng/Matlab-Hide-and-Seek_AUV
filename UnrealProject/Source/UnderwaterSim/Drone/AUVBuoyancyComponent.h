// AUVBuoyancyComponent.h
// Simulates buoyancy and hydrodynamic drag for underwater actors.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AUVBuoyancyComponent.generated.h"

/**
 * UAUVBuoyancyComponent
 *
 * Applies buoyancy force, gravity compensation, and hydrodynamic drag
 * to any actor with a UPrimitiveComponent that simulates physics.
 *
 * Buoyancy force:  F_b = rho * g * V_submerged  (Archimedes' principle)
 * Drag force:      F_d = -0.5 * rho * Cd * A * |v| * v
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNDERWATERSIM_API UAUVBuoyancyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAUVBuoyancyComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    // ── Buoyancy Parameters ──

    /** Water density in kg/m^3 (seawater ~1025) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
    float WaterDensity = 1025.0f;

    /** Displaced volume of the body in m^3 (0.0118 gives slight positive buoyancy for 11.5kg drone) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
    float DisplacedVolume = 0.0118f;

    /** Mass of the drone in kg */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
    float DroneMass = 11.5f;

    /** Water surface Z coordinate in world space (cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
    float WaterSurfaceZ = 0.0f;

    // ── Drag Parameters ──

    /** Linear drag coefficient (unitless, typical ROV: 0.4–1.2) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag")
    float LinearDragCoefficient = 0.8f;

    /** Quadratic drag coefficient */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag")
    float QuadraticDragCoefficient = 0.5f;

    /** Cross-sectional area for drag calculation in m^2 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag")
    float CrossSectionalArea = 0.04f;

    /** Angular drag coefficient for rotational damping */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag")
    float AngularDragCoefficient = 1.5f;

    // ── State ──

    /** Current submersion ratio (0 = above water, 1 = fully submerged) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buoyancy|State")
    float SubmersionRatio = 0.0f;

    /** Whether the actor is currently underwater */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buoyancy|State")
    bool bIsSubmerged = false;

    // ── API ──

    /** Get the net vertical force (buoyancy - gravity) in Newtons */
    UFUNCTION(BlueprintCallable, Category = "Buoyancy")
    float GetNetBuoyancyForce() const;

    /** Get the current depth below water surface in meters */
    UFUNCTION(BlueprintCallable, Category = "Buoyancy")
    float GetDepth() const;

protected:
    virtual void BeginPlay() override;

private:
    /** Cached reference to the physics body */
    UPROPERTY()
    UPrimitiveComponent* PhysicsBody;

    /** Apply buoyancy force to the physics body */
    void ApplyBuoyancy(float DeltaTime);

    /** Apply hydrodynamic drag (linear + quadratic) */
    void ApplyHydrodynamicDrag(float DeltaTime);

    /** Apply angular damping to simulate water resistance to rotation */
    void ApplyAngularDrag(float DeltaTime);

    /** Calculate submersion ratio based on actor position */
    float CalculateSubmersionRatio() const;

    /** Gravity constant cm/s^2 (UE uses cm) */
    static constexpr float Gravity = 981.0f;
};

