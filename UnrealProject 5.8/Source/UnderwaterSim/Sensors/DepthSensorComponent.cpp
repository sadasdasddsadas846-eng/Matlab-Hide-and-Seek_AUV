// DepthSensorComponent.cpp
#include "Sensors/DepthSensorComponent.h"
#include "GameFramework/Actor.h"

UDepthSensorComponent::UDepthSensorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDepthSensorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    ElapsedTime += DeltaTime;
    const float Interval = 1.0f / FMath::Max(1.0f, SampleRateHz);

    if (ElapsedTime >= Interval)
    {
        ElapsedTime = 0.0f;
        AActor* Owner = GetOwner();
        if (!Owner) return;

        // Depth = Distance below water surface
        const float ActorZ = Owner->GetActorLocation().Z;
        float RawDepthMeters = FMath::Max(0.0f, (WaterSurfaceZ - ActorZ) / 100.0f);

        // Add sensor Gaussian noise
        if (NoiseStdDev > 0.0f)
        {
            RawDepthMeters += FMath::FRandRange(-NoiseStdDev, NoiseStdDev);
            RawDepthMeters = FMath::Max(0.0f, RawDepthMeters);
        }

        CurrentDepth = RawDepthMeters;

        // Hydrostatic pressure equation: P = P_atm + (rho * g * h)
        const float Gravity = 9.80665f;
        CurrentPressure = AtmosphericPressure + (FluidDensity * Gravity * CurrentDepth);

        // Simulated water temperature drop with depth
        Temperature = FMath::Clamp(24.0f - (CurrentDepth * 0.25f), 4.0f, 28.0f);

        OnDepthUpdated.Broadcast(CurrentDepth, CurrentPressure, Temperature);
    }
}

