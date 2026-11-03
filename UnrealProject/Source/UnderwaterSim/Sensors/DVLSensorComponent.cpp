// DVLSensorComponent.cpp
#include "Sensors/DVLSensorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

UDVLSensorComponent::UDVLSensorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDVLSensorComponent::BeginPlay()
{
    Super::BeginPlay();
    if (GetOwner())
    {
        PhysicsComponent = GetOwner()->FindComponentByClass<UPrimitiveComponent>();
    }
}

void UDVLSensorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    Elapsed += DeltaTime;
    const float Interval = 1.0f / FMath::Max(1.0f, UpdateRateHz);

    if (Elapsed >= Interval)
    {
        Elapsed = 0.0f;
        UWorld* World = GetWorld();
        if (!World || !PhysicsComponent) return;

        // Trace downwards to measure seabed distance (altitude)
        const FVector Start = GetComponentLocation();
        const FVector DownVector = -GetUpVector();
        const FVector End = Start + (DownVector * (MaxAltitudeMeters * 100.0f));

        FCollisionQueryParams Params;
        Params.AddIgnoredActor(GetOwner());

        FHitResult Hit;
        bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

        if (bHit)
        {
            MeasuredAltitude = Hit.Distance / 100.0f;
            bHasBottomLock = true;
        }
        else
        {
            MeasuredAltitude = MaxAltitudeMeters;
            bHasBottomLock = false;
        }

        // True velocity in body coordinates (m/s)
        const FVector WorldVel = PhysicsComponent->GetComponentVelocity() / 100.0f;
        FVector BodyVel = GetComponentTransform().InverseTransformVector(WorldVel);

        if (VelocityNoiseStdDev > 0.0f)
        {
            BodyVel.X += FMath::FRandRange(-VelocityNoiseStdDev, VelocityNoiseStdDev);
            BodyVel.Y += FMath::FRandRange(-VelocityNoiseStdDev, VelocityNoiseStdDev);
            BodyVel.Z += FMath::FRandRange(-VelocityNoiseStdDev, VelocityNoiseStdDev);
        }

        MeasuredVelocityBody = BodyVel;
        float Correlation = bHasBottomLock ? 0.98f : 0.2f;

        OnDVLUpdated.Broadcast(MeasuredVelocityBody, MeasuredAltitude, bHasBottomLock, Correlation);
    }
}
