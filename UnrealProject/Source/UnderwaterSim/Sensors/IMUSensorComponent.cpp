// IMUSensorComponent.cpp
#include "Sensors/IMUSensorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

UIMUSensorComponent::UIMUSensorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UIMUSensorComponent::BeginPlay()
{
    Super::BeginPlay();
    if (GetOwner())
    {
        PhysicsComponent = GetOwner()->FindComponentByClass<UPrimitiveComponent>();
        if (PhysicsComponent)
        {
            PreviousVelocity = PhysicsComponent->GetComponentVelocity() / 100.0f; // m/s
        }
    }
}

void UIMUSensorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!PhysicsComponent || DeltaTime <= 0.0f) return;

    ElapsedTime += DeltaTime;
    const float Interval = 1.0f / FMath::Max(1.0f, UpdateRateHz);

    if (ElapsedTime >= Interval)
    {
        // 1. Current velocity in m/s
        const FVector CurrentVelocity = PhysicsComponent->GetComponentVelocity() / 100.0f;
        
        // 2. Linear acceleration in world frame: a = (v_now - v_prev) / dt
        FVector WorldLinearAccel = (CurrentVelocity - PreviousVelocity) / ElapsedTime;
        PreviousVelocity = CurrentVelocity;
        ElapsedTime = 0.0f;

        // 3. Add gravity reaction: a_s = a_world - g (where g is -9.81 in Z)
        WorldLinearAccel.Z += 9.80665f;

        // Transform acceleration to Body Frame
        const FTransform ActorTransform = GetOwner()->GetActorTransform();
        FVector BodyLinearAccel = ActorTransform.InverseTransformVector(WorldLinearAccel);

        // Add accelerometer noise
        if (AccelNoiseStdDev > 0.0f)
        {
            BodyLinearAccel.X += FMath::FRandRange(-AccelNoiseStdDev, AccelNoiseStdDev);
            BodyLinearAccel.Y += FMath::FRandRange(-AccelNoiseStdDev, AccelNoiseStdDev);
            BodyLinearAccel.Z += FMath::FRandRange(-AccelNoiseStdDev, AccelNoiseStdDev);
        }

        // 4. Angular velocity (rad/s in body frame)
        FVector AngularVelRad = PhysicsComponent->GetPhysicsAngularVelocityInRadians();
        // Transform to body frame
        FVector BodyAngularVel = ActorTransform.InverseTransformVector(AngularVelRad);

        if (GyroNoiseStdDev > 0.0f)
        {
            BodyAngularVel.X += FMath::FRandRange(-GyroNoiseStdDev, GyroNoiseStdDev);
            BodyAngularVel.Y += FMath::FRandRange(-GyroNoiseStdDev, GyroNoiseStdDev);
            BodyAngularVel.Z += FMath::FRandRange(-GyroNoiseStdDev, GyroNoiseStdDev);
        }

        LatestData.LinearAcceleration = BodyLinearAccel;
        LatestData.AngularVelocity = BodyAngularVel;
        LatestData.Orientation = GetOwner()->GetActorQuat();

        OnIMUMeasurementUpdated.Broadcast(LatestData);
    }
}

