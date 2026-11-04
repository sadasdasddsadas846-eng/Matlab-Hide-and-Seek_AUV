// AUVBuoyancyComponent.cpp
#include "Drone/AUVBuoyancyComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "UnderwaterSim.h"

UAUVBuoyancyComponent::UAUVBuoyancyComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UAUVBuoyancyComponent::BeginPlay()
{
    Super::BeginPlay();

    PhysicsBody = GetOwner()->FindComponentByClass<UPrimitiveComponent>();
    if (PhysicsBody && !PhysicsBody->IsSimulatingPhysics())
    {
        UE_LOG(LogUnderwaterSim, Warning,
               TEXT("AUVBuoyancyComponent: %s physics body is not simulating physics!"),
               *GetOwner()->GetName());
    }
}

void UAUVBuoyancyComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!PhysicsBody || !PhysicsBody->IsSimulatingPhysics())
    {
        return;
    }

    SubmersionRatio = CalculateSubmersionRatio();
    bIsSubmerged = SubmersionRatio > 0.01f;

    if (bIsSubmerged)
    {
        ApplyBuoyancy(DeltaTime);
        ApplyHydrodynamicDrag(DeltaTime);
        ApplyAngularDrag(DeltaTime);
    }
}

void UAUVBuoyancyComponent::ApplyBuoyancy(float DeltaTime)
{
    const float BuoyancyForceMagnitude =
        WaterDensity * Gravity * DisplacedVolume * 100.0f * 100.0f * 100.0f;

    const float ScaledForce = BuoyancyForceMagnitude * SubmersionRatio;
    const FVector BuoyancyForce = FVector(0.0f, 0.0f, ScaledForce);
    PhysicsBody->AddForce(BuoyancyForce, NAME_None, false);
}

void UAUVBuoyancyComponent::ApplyHydrodynamicDrag(float DeltaTime)
{
    const FVector Velocity = PhysicsBody->GetComponentVelocity();
    const float Speed = Velocity.Size();

    if (Speed < KINDA_SMALL_NUMBER)
    {
        return;
    }

    const FVector VelocityDir = Velocity.GetSafeNormal();

    const float LinearDrag = LinearDragCoefficient * Speed;
    const float QuadraticDrag = QuadraticDragCoefficient * Speed * Speed;
    const float TotalDragMagnitude = (LinearDrag + QuadraticDrag) *
                                      WaterDensity * CrossSectionalArea * 0.5f;

    const FVector DragForce = -VelocityDir * TotalDragMagnitude * SubmersionRatio;
    PhysicsBody->AddForce(DragForce, NAME_None, false);
}

void UAUVBuoyancyComponent::ApplyAngularDrag(float DeltaTime)
{
    const FVector AngularVelocity = PhysicsBody->GetPhysicsAngularVelocityInRadians();
    const float AngularSpeed = AngularVelocity.Size();

    if (AngularSpeed < KINDA_SMALL_NUMBER)
    {
        return;
    }

    const FVector DampingTorque = -AngularVelocity * AngularDragCoefficient * SubmersionRatio;
    PhysicsBody->AddTorqueInRadians(DampingTorque, NAME_None, false);
}

float UAUVBuoyancyComponent::CalculateSubmersionRatio() const
{
    if (!GetOwner()) return 0.0f;

    const float ActorZ = GetOwner()->GetActorLocation().Z;
    const float HalfHeight = 30.0f;

    if (ActorZ + HalfHeight <= WaterSurfaceZ)
    {
        return 1.0f;
    }
    else if (ActorZ - HalfHeight >= WaterSurfaceZ)
    {
        return 0.0f;
    }
    else
    {
        return (WaterSurfaceZ - (ActorZ - HalfHeight)) / (2.0f * HalfHeight);
    }
}

float UAUVBuoyancyComponent::GetNetBuoyancyForce() const
{
    const float BuoyancyForce = WaterDensity * Gravity * DisplacedVolume * 100.0f * 100.0f * 100.0f;
    const float GravityForce = DroneMass * Gravity;
    return (BuoyancyForce * SubmersionRatio) - GravityForce;
}

float UAUVBuoyancyComponent::GetDepth() const
{
    if (!GetOwner()) return 0.0f;
    const float DepthCm = WaterSurfaceZ - GetOwner()->GetActorLocation().Z;
    return FMath::Max(0.0f, DepthCm / 100.0f);
}

