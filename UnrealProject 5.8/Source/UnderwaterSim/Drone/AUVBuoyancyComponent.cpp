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
    // If gravity is disabled on the physics body, it is already neutrally buoyant in water
    if (!PhysicsBody->IsGravityEnabled())
    {
        return;
    }

    // Gravity is already in cm/s^2 (981.0f), so rho * g_cm * V is directly in kg*cm/s^2 (Unreal Force units)
    const float BuoyancyForceMagnitude = WaterDensity * Gravity * DisplacedVolume;
    const float ScaledForce = BuoyancyForceMagnitude * SubmersionRatio;
    const FVector BuoyancyForce = FVector(0.0f, 0.0f, ScaledForce);
    PhysicsBody->AddForce(BuoyancyForce, NAME_None, false);
}

void UAUVBuoyancyComponent::ApplyHydrodynamicDrag(float DeltaTime)
{
    const FVector VelocityCm = PhysicsBody->GetComponentVelocity();
    const float SpeedCm = VelocityCm.Size();

    if (SpeedCm < KINDA_SMALL_NUMBER)
    {
        return;
    }

    const FVector VelocityDir = VelocityCm / SpeedCm;
    const float SpeedMeters = SpeedCm / 100.0f;

    // Drag in Newtons: F_d = 0.5 * rho * A * (Cd_lin * v + Cd_quad * v^2)
    const float LinearDrag = LinearDragCoefficient * SpeedMeters;
    const float QuadraticDrag = QuadraticDragCoefficient * SpeedMeters * SpeedMeters;
    const float DragNewtons = (LinearDrag + QuadraticDrag) * WaterDensity * CrossSectionalArea * 0.5f;

    // Convert Newtons (kg*m/s^2) to Unreal force units (kg*cm/s^2) by multiplying by 100
    const FVector DragForce = -VelocityDir * (DragNewtons * 100.0f) * SubmersionRatio;
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
    const float BuoyancyForceNewtons = WaterDensity * 9.81f * DisplacedVolume;
    const float GravityForceNewtons = DroneMass * 9.81f;
    return (BuoyancyForceNewtons * SubmersionRatio) - GravityForceNewtons;
}

float UAUVBuoyancyComponent::GetDepth() const
{
    if (!GetOwner()) return 0.0f;
    const float DepthCm = WaterSurfaceZ - GetOwner()->GetActorLocation().Z;
    return FMath::Max(0.0f, DepthCm / 100.0f);
}
