// DepthSensorComponent.h
// Simulates a Bar30/Keller high-resolution underwater depth and pressure sensor.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DepthSensorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDepthUpdated, float, DepthMeters, float, PressurePascal, float, TemperatureCelsius);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNDERWATERSIM_API UDepthSensorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDepthSensorComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Water surface Z coordinate in world space (cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Depth Sensor")
    float WaterSurfaceZ = 0.0f;

    /** Atmospheric pressure in Pascal (standard: 101325 Pa) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Depth Sensor")
    float AtmosphericPressure = 101325.0f;

    /** Fluid density in kg/m^3 (seawater ~1025) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Depth Sensor")
    float FluidDensity = 1025.0f;

    /** Depth measurement noise std dev in meters */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Depth Sensor|Noise")
    float NoiseStdDev = 0.01f;

    /** Sensor update frequency (Hz) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Depth Sensor")
    float SampleRateHz = 20.0f;

    UPROPERTY(BlueprintAssignable, Category = "Depth Sensor")
    FOnDepthUpdated OnDepthUpdated;

    UFUNCTION(BlueprintPure, Category = "Depth Sensor")
    float GetDepth() const { return CurrentDepth; }

    UFUNCTION(BlueprintPure, Category = "Depth Sensor")
    float GetPressure() const { return CurrentPressure; }

private:
    float CurrentDepth = 0.0f;
    float CurrentPressure = 101325.0f;
    float Temperature = 18.0f;
    float ElapsedTime = 0.0f;
};
