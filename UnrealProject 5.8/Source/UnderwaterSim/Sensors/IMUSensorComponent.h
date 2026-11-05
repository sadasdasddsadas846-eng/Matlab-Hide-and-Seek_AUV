// IMUSensorComponent.h
// Simulates 9-DOF Inertial Measurement Unit (Accelerometer, Gyroscope, Magnetometer/Orientation).

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IMUSensorComponent.generated.h"

USTRUCT(BlueprintType)
struct FIMUMeasurement
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "IMU")
    FVector LinearAcceleration = FVector::ZeroVector; // m/s^2 (in sensor frame, with gravity)

    UPROPERTY(BlueprintReadOnly, Category = "IMU")
    FVector AngularVelocity = FVector::ZeroVector; // rad/s (in sensor frame)

    UPROPERTY(BlueprintReadOnly, Category = "IMU")
    FQuat Orientation = FQuat::Identity; // Orientation quaternion (ENU/NED)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIMUMeasurementUpdated, const FIMUMeasurement&, IMUData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNDERWATERSIM_API UIMUSensorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIMUSensorComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IMU")
    float UpdateRateHz = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IMU|Noise")
    float AccelNoiseStdDev = 0.05f; // m/s^2

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IMU|Noise")
    float GyroNoiseStdDev = 0.01f; // rad/s

    UPROPERTY(BlueprintAssignable, Category = "IMU")
    FOnIMUMeasurementUpdated OnIMUMeasurementUpdated;

    UFUNCTION(BlueprintPure, Category = "IMU")
    FIMUMeasurement GetLatestMeasurement() const { return LatestData; }

protected:
    virtual void BeginPlay() override;

private:
    float ElapsedTime = 0.0f;
    FVector PreviousVelocity = FVector::ZeroVector;
    FIMUMeasurement LatestData;

    UPROPERTY()
    UPrimitiveComponent* PhysicsComponent = nullptr;
};

