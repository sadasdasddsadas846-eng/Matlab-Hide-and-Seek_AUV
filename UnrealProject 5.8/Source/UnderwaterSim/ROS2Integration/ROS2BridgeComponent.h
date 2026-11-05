// ROS2BridgeComponent.h
// Provides bi-directional ROS 2 interface for the Underwater Drone.
// Integrates with rclUE plugin or acts as standard message dispatcher.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ROS2BridgeComponent.generated.h"

class AUnderwaterDrone;
class USonarSensorComponent;
class UIMUSensorComponent;
class UDepthSensorComponent;
class UDVLSensorComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNDERWATERSIM_API UROS2BridgeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UROS2BridgeComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ── ROS 2 Configuration ──

    /** Robot namespace (e.g. "seeker_drone" or "hider_drone") */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ROS2")
    FString Namespace = TEXT("drone0");

    /** Odometry publish frequency in Hz */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ROS2")
    float OdomPublishRateHz = 30.0f;

    // ── Incoming ROS 2 Commands ──

    /**
     * Callback for geometry_msgs/Twist command velocity.
     * linear.x  = surge (-1 to 1)
     * linear.y  = sway  (-1 to 1)
     * linear.z  = heave (-1 to 1)
     * angular.z = yaw   (-1 to 1)
     */
    UFUNCTION(BlueprintCallable, Category = "ROS2|Input")
    void OnReceiveCmdVel(float LinearX, float LinearY, float LinearZ, float AngularZ);

    /** Emergency stop command */
    UFUNCTION(BlueprintCallable, Category = "ROS2|Input")
    void OnReceiveEmergencyStop();

    // ── Status ──

    UFUNCTION(BlueprintPure, Category = "ROS2")
    bool IsConnected() const { return bIsConnected; }

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    AUnderwaterDrone* OwnerDrone = nullptr;

    UPROPERTY()
    USonarSensorComponent* SonarComponent = nullptr;

    UPROPERTY()
    UIMUSensorComponent* IMUComponent = nullptr;

    UPROPERTY()
    UDepthSensorComponent* DepthComponent = nullptr;

    UPROPERTY()
    UDVLSensorComponent* DVLComponent = nullptr;

    bool bIsConnected = true;
    float OdomTimer = 0.0f;

    void PublishOdometry();
};

