// BuoyancyComponent.cpp
#include "Drone/BuoyancyComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "UnderwaterSim.h"

UBuoyancyComponent::UBuoyancyComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UBuoyancyComponent::BeginPlay()
{
    Super::BeginPlay();

    // Find the first primitive component that simulates physics
    PhysicsBody = GetOwner()->FindComponentByClass<UPrimitiveComponent>();
    if (PhysicsBody && !PhysicsBody->IsSimulatingPhysics())
    {
        UE_LOG(LogUnderwaterSim, Warning,
               TEXT("BuoyancyComponent: %s physics body is not simulating physics!"),
               *GetOwner()->GetName());
    }
}

void UBuoyancyComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!PhysicsBody || !PhysicsBody->IsSimulatingPhysics())
    {
        return;
    }

    // Update submersion state
    SubmersionRatio = CalculateSubmersionRatio();
    bIsSubmerged = SubmersionRatio > 0.01f;

    if (bIsSubmerged)
    {
        ApplyBuoyancy(DeltaTime);
        ApplyHydrodynamicDrag(DeltaTime);
        ApplyAngularDrag(DeltaTime);
    }
}

void UBuoyancyComponent::ApplyBuoyancy(float DeltaTime)
{
    // Archimedes' principle: F_buoyancy = rho * g * V_submerged
    // In UE units: force in kg·cm/s² (since UE uses cm)
    const float BuoyancyForceMagnitude =
        WaterDensity * Gravity * DisplacedVolume * 100.0f * 100.0f * 100.0f; // Convert m³ to cm³ density

    // Scale by submersion ratio for partial submersion near the surface
    const float ScaledForce = BuoyancyForceMagnitude * SubmersionRatio;

    // Apply upward force at center of mass
    const FVector BuoyancyForce = FVector(0.0f, 0.0f, ScaledForce);
    PhysicsBody->AddForce(BuoyancyForce, NAME_None, false);
}

void UBuoyancyComponent::ApplyHydrodynamicDrag(float DeltaTime)
{
    const FVector Velocity = PhysicsBody->GetComponentVelocity();
    const float Speed = Velocity.Size();

    if (Speed < KINDA_SMALL_NUMBER)
    {
        return;
    }

    const FVector VelocityDir = Velocity.GetSafeNormal();

    // F_drag = -(C_linear * v + C_quadratic * |v| * v) * rho * A * 0.5
    // Simplified for game physics:
    const float LinearDrag = LinearDragCoefficient * Speed;
    const float QuadraticDrag = QuadraticDragCoefficient * Speed * Speed;
    const float TotalDragMagnitude = (LinearDrag + QuadraticDrag) *
                                      WaterDensity * CrossSectionalArea * 0.5f;

    // Scale by submersion ratio
    const FVector DragForce = -VelocityDir * TotalDragMagnitude * SubmersionRatio;

    PhysicsBody->AddForce(DragForce, NAME_None, false);
}

void UBuoyancyComponent::ApplyAngularDrag(float DeltaTime)
{
    // Apply rotational damping to simulate water resistance
    const FVector AngularVelocity = PhysicsBody->GetPhysicsAngularVelocityInRadians();
    const float AngularSpeed = AngularVelocity.Size();

    if (AngularSpeed < KINDA_SMALL_NUMBER)
    {
        return;
    }

    const FVector DampingTorque = -AngularVelocity * AngularDragCoefficient * SubmersionRatio;
    PhysicsBody->AddTorqueInRadians(DampingTorque, NAME_None, false);
}

float UBuoyancyComponent::CalculateSubmersionRatio() const
{
    if (!GetOwner()) return 0.0f;

    const float ActorZ = GetOwner()->GetActorLocation().Z;
    // Assume a bounding extent for smooth transition at surface
    const float HalfHeight = 30.0f; // cm - approximate half-height of drone

    if (ActorZ + HalfHeight <= WaterSurfaceZ)
    {
        return 1.0f; // Fully submerged
    }
    else if (ActorZ - HalfHeight >= WaterSurfaceZ)
    {
        return 0.0f; // Fully above water
    }
    else
    {
        // Partial submersion - linear interpolation
        return (WaterSurfaceZ - (ActorZ - HalfHeight)) / (2.0f * HalfHeight);
    }
}

float UBuoyancyComponent::GetNetBuoyancyForce() const
{
    const float BuoyancyForce = WaterDensity * Gravity * DisplacedVolume * 100.0f * 100.0f * 100.0f;
    const float GravityForce = DroneMass * Gravity;
    return (BuoyancyForce * SubmersionRatio) - GravityForce;
}

float UBuoyancyComponent::GetDepth() const
{
    if (!GetOwner()) return 0.0f;
    const float DepthCm = WaterSurfaceZ - GetOwner()->GetActorLocation().Z;
    return FMath::Max(0.0f, DepthCm / 100.0f); // Return in meters
}

