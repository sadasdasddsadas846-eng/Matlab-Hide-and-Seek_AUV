// ThrusterComponent.cpp
#include "Drone/ThrusterComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "UnderwaterSim.h"

UThrusterComponent::UThrusterComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UThrusterComponent::BeginPlay()
{
    Super::BeginPlay();

    // Find the physics body on the parent actor
    ParentPhysicsBody = GetOwner()->FindComponentByClass<UPrimitiveComponent>();
    if (!ParentPhysicsBody)
    {
        UE_LOG(LogUnderwaterSim, Error,
               TEXT("ThrusterComponent on %s: No physics body found!"),
               *GetOwner()->GetName());
    }
}

void UThrusterComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ── Apply deadband ──
    float EffectiveTarget = TargetInput;
    if (FMath::Abs(EffectiveTarget) < Deadband)
    {
        EffectiveTarget = 0.0f;
    }

    // ── First-order response (exponential ramp) ──
    // Simulates motor spin-up/spin-down delay
    const float Alpha = FMath::Clamp(DeltaTime / ResponseTime, 0.0f, 1.0f);
    CurrentInput = FMath::Lerp(CurrentInput, EffectiveTarget, Alpha);

    // ── Compute thrust force ──
    if (CurrentInput >= 0.0f)
    {
        CurrentThrust = CurrentInput * MaxThrustForward;
    }
    else
    {
        CurrentThrust = CurrentInput * MaxThrustReverse;
    }

    // ── Apply force ──
    ApplyThrustForce();
}

void UThrusterComponent::SetThrustInput(float Input)
{
    TargetInput = FMath::Clamp(Input, -1.0f, 1.0f);
}

FVector UThrusterComponent::GetThrustDirection() const
{
    // Thrust is applied along the component's forward (X) axis
    return GetForwardVector();
}

void UThrusterComponent::ApplyThrustForce()
{
    if (!ParentPhysicsBody || !ParentPhysicsBody->IsSimulatingPhysics())
    {
        return;
    }

    if (FMath::IsNearlyZero(CurrentThrust))
    {
        return;
    }

    // Force in Newtons -> UE uses kg*cm/s^2, so multiply by 100
    const FVector Force = GetThrustDirection() * CurrentThrust * 100.0f;
    const FVector Location = GetComponentLocation();

    // Apply force at thruster location (creates torque if off-center)
    ParentPhysicsBody->AddForceAtLocation(Force, Location);
}

