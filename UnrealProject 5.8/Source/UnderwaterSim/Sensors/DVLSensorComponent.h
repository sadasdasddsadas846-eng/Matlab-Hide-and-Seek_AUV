// DVLSensorComponent.h
// Doppler Velocity Log (DVL) sensor simulating 4-beam acoustic Janus configuration.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "DVLSensorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDVLUpdated, FVector, VelocityBody, float, AltitudeMeters, bool, bBottomLock, float, Correlation);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNDERWATERSIM_API UDVLSensorComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UDVLSensorComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DVL")
    float MaxAltitudeMeters = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DVL")
    float UpdateRateHz = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DVL|Noise")
    float VelocityNoiseStdDev = 0.02f; // m/s

    UPROPERTY(BlueprintAssignable, Category = "DVL")
    FOnDVLUpdated OnDVLUpdated;

    UFUNCTION(BlueprintPure, Category = "DVL")
    FVector GetVelocityBody() const { return MeasuredVelocityBody; }

    UFUNCTION(BlueprintPure, Category = "DVL")
    float GetAltitude() const { return MeasuredAltitude; }

    UFUNCTION(BlueprintPure, Category = "DVL")
    bool HasBottomLock() const { return bHasBottomLock; }

protected:
    virtual void BeginPlay() override;

private:
    float Elapsed = 0.0f;
    FVector MeasuredVelocityBody = FVector::ZeroVector;
    float MeasuredAltitude = 0.0f;
    bool bHasBottomLock = false;

    UPROPERTY()
    UPrimitiveComponent* PhysicsComponent = nullptr;
};

