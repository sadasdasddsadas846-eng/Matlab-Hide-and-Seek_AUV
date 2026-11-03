// SonarSensorComponent.h
// Simulates a Forward-Looking Sonar (FLS) / Acoustic Profiler for underwater detection.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SonarSensorComponent.generated.h"

USTRUCT(BlueprintType)
struct FSonarDetection
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Sonar")
    float Range = 0.0f; // in meters

    UPROPERTY(BlueprintReadOnly, Category = "Sonar")
    float AzimuthAngle = 0.0f; // in degrees (-FOV/2 to +FOV/2)

    UPROPERTY(BlueprintReadOnly, Category = "Sonar")
    float ElevationAngle = 0.0f; // in degrees

    UPROPERTY(BlueprintReadOnly, Category = "Sonar")
    float Intensity = 1.0f; // Echo intensity (0.0 to 1.0)

    UPROPERTY(BlueprintReadOnly, Category = "Sonar")
    FVector HitLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Sonar")
    AActor* DetectedActor = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSonarScanComplete, const TArray<FSonarDetection>&, Detections);

/**
 * USonarSensorComponent
 * 
 * Performs acoustic beam sweeps across a horizontal FOV.
 * Simulates acoustic transmission, reflective targets, and noise.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNDERWATERSIM_API USonarSensorComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    USonarSensorComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ── Sonar Settings ──

    /** Maximum detection range in meters (typical: 30 - 60m) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sonar|Config", meta=(ClampMin="1.0", ClampMax="150.0"))
    float MaxRangeMeters = 40.0f;

    /** Minimum detection blind range in meters */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sonar|Config", meta=(ClampMin="0.1", ClampMax="5.0"))
    float MinRangeMeters = 0.5f;

    /** Horizontal field of view in degrees (e.g. 90 deg or 120 deg) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sonar|Config", meta=(ClampMin="10.0", ClampMax="180.0"))
    float HorizontalFOV = 90.0f;

    /** Vertical beam spread in degrees (typically narrow, e.g. 15-20 deg) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sonar|Config", meta=(ClampMin="1.0", ClampMax="45.0"))
    float VerticalBeamSpread = 15.0f;

    /** Number of horizontal acoustic beams per scan ping */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sonar|Config", meta=(ClampMin="8", ClampMax="256"))
    int32 BeamCount = 64;

    /** Scan rate in Hertz (pings per second) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sonar|Config", meta=(ClampMin="1.0", ClampMax="30.0"))
    float UpdateFrequency = 10.0f;

    /** Acoustic noise standard deviation (meters) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sonar|Noise")
    float RangeNoiseStdDev = 0.05f;

    /** Draw debug rays in viewport */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sonar|Debug")
    bool bShowDebugBeams = true;

    // ── Outputs ──

    UPROPERTY(BlueprintAssignable, Category = "Sonar|Events")
    FOnSonarScanComplete OnSonarScanComplete;

    UFUNCTION(BlueprintPure, Category = "Sonar")
    const TArray<FSonarDetection>& GetLatestDetections() const { return LatestDetections; }

    UFUNCTION(BlueprintCallable, Category = "Sonar")
    void PerformScan();

private:
    float TimeSinceLastScan = 0.0f;
    TArray<FSonarDetection> LatestDetections;
};

