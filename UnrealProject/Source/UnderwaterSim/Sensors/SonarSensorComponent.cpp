// SonarSensorComponent.cpp
#include "Sensors/SonarSensorComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

USonarSensorComponent::USonarSensorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USonarSensorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    TimeSinceLastScan += DeltaTime;
    const float Interval = 1.0f / FMath::Max(1.0f, UpdateFrequency);

    if (TimeSinceLastScan >= Interval)
    {
        PerformScan();
        TimeSinceLastScan = 0.0f;
    }
}

void USonarSensorComponent::PerformScan()
{
    UWorld* World = GetWorld();
    if (!World) return;

    LatestDetections.Empty(BeamCount);

    const FVector SensorOrigin = GetComponentLocation();
    const FVector ForwardVector = GetForwardVector();
    const FVector RightVector = GetRightVector();
    const FVector UpVector = GetUpVector();

    const float HalfHorizontalFOV = HorizontalFOV * 0.5f;
    const float AngleStep = (BeamCount > 1) ? (HorizontalFOV / (BeamCount - 1)) : 0.0f;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bTraceComplex = true;

    for (int32 i = 0; i < BeamCount; ++i)
    {
        const float Azimuth = -HalfHorizontalFOV + (i * AngleStep);
        const FRotator BeamRotation = FRotator(0.0f, Azimuth, 0.0f);
        
        // Compute direction in sensor frame
        FVector BeamDir = ForwardVector.RotateAngleAxis(Azimuth, UpVector);
        FVector TraceEnd = SensorOrigin + (BeamDir * (MaxRangeMeters * 100.0f)); // cm

        FHitResult HitResult;
        bool bHit = World->LineTraceSingleByChannel(
            HitResult,
            SensorOrigin + (BeamDir * (MinRangeMeters * 100.0f)),
            TraceEnd,
            ECC_Visibility,
            QueryParams
        );

        if (bHit)
        {
            float MeasuredDistanceMeters = HitResult.Distance / 100.0f; // cm to m
            // Add Gaussian range noise
            if (RangeNoiseStdDev > 0.0f)
            {
                MeasuredDistanceMeters += FMath::FRandRange(-RangeNoiseStdDev, RangeNoiseStdDev);
                MeasuredDistanceMeters = FMath::Clamp(MeasuredDistanceMeters, MinRangeMeters, MaxRangeMeters);
            }

            FSonarDetection Detection;
            Detection.Range = MeasuredDistanceMeters;
            Detection.AzimuthAngle = Azimuth;
            Detection.ElevationAngle = 0.0f;
            Detection.HitLocation = HitResult.ImpactPoint;
            Detection.DetectedActor = HitResult.GetActor();

            // Calculate acoustic intensity based on distance attenuation and incident angle
            float CosIncident = FMath::Abs(FVector::DotProduct(HitResult.ImpactNormal, -BeamDir));
            float Attenuation = 1.0f - (MeasuredDistanceMeters / MaxRangeMeters);
            Detection.Intensity = FMath::Clamp(CosIncident * Attenuation, 0.1f, 1.0f);

            LatestDetections.Add(Detection);

            if (bShowDebugBeams)
            {
                DrawDebugLine(World, SensorOrigin, HitResult.ImpactPoint, FColor::Cyan, false, 0.1f, 0, 1.0f);
                DrawDebugPoint(World, HitResult.ImpactPoint, 5.0f, FColor::Orange, false, 0.1f);
            }
        }
        else if (bShowDebugBeams && (i % 8 == 0))
        {
            DrawDebugLine(World, SensorOrigin, TraceEnd, FColor(0, 50, 100), false, 0.05f, 0, 0.5f);
        }
    }

    OnSonarScanComplete.Broadcast(LatestDetections);
}

