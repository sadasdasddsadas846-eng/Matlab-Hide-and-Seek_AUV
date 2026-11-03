// UnderwaterEnvironmentActor.cpp
#include "Environment/UnderwaterEnvironmentActor.h"
#include "Components/PostProcessComponent.h"
#include "Engine/World.h"

AUnderwaterEnvironmentActor::AUnderwaterEnvironmentActor()
{
    PrimaryActorTick.bCanEverTick = true;

    UnderwaterPostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("UnderwaterPostProcess"));
    RootComponent = UnderwaterPostProcess;
    UnderwaterPostProcess->bUnbound = true; // Applies across entire map
}

void AUnderwaterEnvironmentActor::BeginPlay()
{
    Super::BeginPlay();
    UpdatePostProcessSettings();
}

void AUnderwaterEnvironmentActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    SimulationTime += DeltaTime;
}

FVector AUnderwaterEnvironmentActor::GetCurrentAtLocation(const FVector& WorldLocation) const
{
    // If above surface, no water current
    if (WorldLocation.Z > WaterSurfaceZ)
    {
        return FVector::ZeroVector;
    }

    // Add spatiotemporal turbulence using Perlin/trigonometric variations
    const float DepthScale = FMath::Clamp((WaterSurfaceZ - WorldLocation.Z) / 3000.0f, 0.2f, 1.0f);
    const float WaveOffset = FMath::Sin((WorldLocation.X * 0.001f) + (SimulationTime * 0.5f)) *
                             FMath::Cos((WorldLocation.Y * 0.001f) + (SimulationTime * 0.4f));

    FVector Current = GlobalCurrentVelocity * DepthScale;
    Current.X += WaveOffset * CurrentTurbulence;
    Current.Y += WaveOffset * CurrentTurbulence * 0.5f;

    return Current;
}

void AUnderwaterEnvironmentActor::UpdatePostProcessSettings()
{
    if (!UnderwaterPostProcess) return;

    FPostProcessSettings& Settings = UnderwaterPostProcess->Settings;
    
    // Underwater color grading / blue-green absorption tint
    Settings.bOverride_ColorSaturation = true;
    Settings.ColorSaturation = FVector4(0.7f, 0.9f, 1.2f, 1.0f);

    Settings.bOverride_ColorContrast = true;
    Settings.ColorContrast = FVector4(0.95f, 1.0f, 1.05f, 1.0f);

    Settings.bOverride_DepthOfFieldFocalDistance = true;
    Settings.DepthOfFieldFocalDistance = MaxVisualRangeMeters * 100.0f;
}
