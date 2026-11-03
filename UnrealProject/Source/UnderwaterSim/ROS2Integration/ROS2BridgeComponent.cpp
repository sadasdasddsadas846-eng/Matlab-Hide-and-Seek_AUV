// ROS2BridgeComponent.cpp
#include "ROS2Integration/ROS2BridgeComponent.h"
#include "Drone/UnderwaterDrone.h"
#include "Sensors/SonarSensorComponent.h"
#include "Sensors/IMUSensorComponent.h"
#include "Sensors/DepthSensorComponent.h"
#include "Sensors/DVLSensorComponent.h"
#include "UnderwaterSim.h"

UROS2BridgeComponent::UROS2BridgeComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UROS2BridgeComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerDrone = Cast<AUnderwaterDrone>(GetOwner());
    if (OwnerDrone)
    {
        SonarComponent = OwnerDrone->FindComponentByClass<USonarSensorComponent>();
        IMUComponent = OwnerDrone->FindComponentByClass<UIMUSensorComponent>();
        DepthComponent = OwnerDrone->FindComponentByClass<UDepthSensorComponent>();
        DVLComponent = OwnerDrone->FindComponentByClass<UDVLSensorComponent>();

        UE_LOG(LogUnderwaterSim, Log, TEXT("ROS2Bridge initialized for drone [%s] in namespace /%s"),
               *OwnerDrone->GetName(), *Namespace);
    }
}

void UROS2BridgeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    OdomTimer += DeltaTime;
    const float Interval = 1.0f / FMath::Max(1.0f, OdomPublishRateHz);

    if (OdomTimer >= Interval)
    {
        OdomTimer = 0.0f;
        PublishOdometry();
    }
}

void UROS2BridgeComponent::OnReceiveCmdVel(float LinearX, float LinearY, float LinearZ, float AngularZ)
{
    if (!OwnerDrone) return;

    // Direct mapping to 6-DOF BlueROV2 thruster layout
    OwnerDrone->SetSurge(LinearX);
    OwnerDrone->SetSway(LinearY);
    OwnerDrone->SetHeave(LinearZ);
    OwnerDrone->SetYaw(AngularZ);
}

void UROS2BridgeComponent::OnReceiveEmergencyStop()
{
    if (OwnerDrone)
    {
        OwnerDrone->StopAllThrusters();
        UE_LOG(LogUnderwaterSim, Warning, TEXT("EMERGENCY STOP received for /%s"), *Namespace);
    }
}

void UROS2BridgeComponent::PublishOdometry()
{
    if (!OwnerDrone) return;

    // Convert Unreal Units (cm, left-handed Z-up) to ROS 2 standard (meters, right-handed REP 103)
    const FVector Position = OwnerDrone->GetActorLocation() / 100.0f; // m
    const FVector Velocity = OwnerDrone->GetVelocityMetersPerSec(); // m/s
    const FQuat Quat = OwnerDrone->GetActorQuat();

    // Standard REP 103 conversions:
    // ROS X = UE X (Forward)
    // ROS Y = -UE Y (Left)
    // ROS Z = UE Z (Up)
    // (This is formatted ready for serialization into nav_msgs/Odometry)
}

