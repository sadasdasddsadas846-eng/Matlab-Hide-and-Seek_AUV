// UnderwaterDrone.cpp
#include "Drone/UnderwaterDrone.h"
#include "Drone/AUVBuoyancyComponent.h"
#include "Drone/ThrusterComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UnderwaterSim.h"

AUnderwaterDrone::AUnderwaterDrone()
{
    PrimaryActorTick.bCanEverTick = true;

    // ── Drone Body (root) ──
    DroneBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DroneBody"));
    RootComponent = DroneBody;
    DroneBody->SetSimulatePhysics(true);
    DroneBody->SetCollisionProfileName(TEXT("PhysicsActor"));
    DroneBody->SetMassOverrideInKg(NAME_None, 11.5f, true); // BlueROV2 ~11.5 kg
    DroneBody->SetLinearDamping(0.1f);
    DroneBody->SetAngularDamping(0.5f);

    // ── Buoyancy ──
    BuoyancyComp = CreateDefaultSubobject<UAUVBuoyancyComponent>(TEXT("Buoyancy"));

    // ── Camera (third-person follow for debugging/viewing) ──
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bDoCollisionTest = false;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // ── Thrusters (BlueROV2 Heavy vectored layout) ──
    SetupThrusters();
}

void AUnderwaterDrone::SetupThrusters()
{
    // Front-Left: mounted at (+20, -15, 0) cm, angled 45° inward
    ThrusterFL = CreateDefaultSubobject<UThrusterComponent>(TEXT("ThrusterFL"));
    ThrusterFL->SetupAttachment(DroneBody);
    ThrusterFL->SetRelativeLocation(FVector(20.0f, -15.0f, 0.0f));
    ThrusterFL->SetRelativeRotation(FRotator(0.0f, 45.0f, 0.0f));
    ThrusterFL->ThrusterType = EThrusterType::Horizontal;

    // Front-Right: mounted at (+20, +15, 0) cm, angled -45°
    ThrusterFR = CreateDefaultSubobject<UThrusterComponent>(TEXT("ThrusterFR"));
    ThrusterFR->SetupAttachment(DroneBody);
    ThrusterFR->SetRelativeLocation(FVector(20.0f, 15.0f, 0.0f));
    ThrusterFR->SetRelativeRotation(FRotator(0.0f, -45.0f, 0.0f));
    ThrusterFR->ThrusterType = EThrusterType::Horizontal;

    // Rear-Left: mounted at (-20, -15, 0) cm, angled -45°
    ThrusterRL = CreateDefaultSubobject<UThrusterComponent>(TEXT("ThrusterRL"));
    ThrusterRL->SetupAttachment(DroneBody);
    ThrusterRL->SetRelativeLocation(FVector(-20.0f, -15.0f, 0.0f));
    ThrusterRL->SetRelativeRotation(FRotator(0.0f, -45.0f, 0.0f));
    ThrusterRL->ThrusterType = EThrusterType::Horizontal;

    // Rear-Right: mounted at (-20, +15, 0) cm, angled 45°
    ThrusterRR = CreateDefaultSubobject<UThrusterComponent>(TEXT("ThrusterRR"));
    ThrusterRR->SetupAttachment(DroneBody);
    ThrusterRR->SetRelativeLocation(FVector(-20.0f, 15.0f, 0.0f));
    ThrusterRR->SetRelativeRotation(FRotator(0.0f, 45.0f, 0.0f));
    ThrusterRR->ThrusterType = EThrusterType::Horizontal;

    // ── 2 Vertical Thrusters ──
    ThrusterVL = CreateDefaultSubobject<UThrusterComponent>(TEXT("ThrusterVL"));
    ThrusterVL->SetupAttachment(DroneBody);
    ThrusterVL->SetRelativeLocation(FVector(0.0f, -12.0f, 5.0f));
    ThrusterVL->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    ThrusterVL->ThrusterType = EThrusterType::Vertical;

    ThrusterVR = CreateDefaultSubobject<UThrusterComponent>(TEXT("ThrusterVR"));
    ThrusterVR->SetupAttachment(DroneBody);
    ThrusterVR->SetRelativeLocation(FVector(0.0f, 12.0f, 5.0f));
    ThrusterVR->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    ThrusterVR->ThrusterType = EThrusterType::Vertical;
}

void AUnderwaterDrone::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogUnderwaterSim, Log, TEXT("Drone %d spawned as %s"),
           DroneID,
           DroneRole == EDroneRole::Seeker ? TEXT("SEEKER") :
           DroneRole == EDroneRole::Hider ? TEXT("HIDER") : TEXT("NEUTRAL"));
}

void AUnderwaterDrone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AUnderwaterDrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AUnderwaterDrone::InputSurge);
    PlayerInputComponent->BindAxis("MoveRight", this, &AUnderwaterDrone::InputSway);
    PlayerInputComponent->BindAxis("MoveUp", this, &AUnderwaterDrone::InputHeave);
    PlayerInputComponent->BindAxis("Turn", this, &AUnderwaterDrone::InputYaw);
}

void AUnderwaterDrone::SetSurge(float Value)
{
    Value = FMath::Clamp(Value, -1.0f, 1.0f);
    ThrusterFL->SetThrustInput(Value);
    ThrusterFR->SetThrustInput(Value);
    ThrusterRL->SetThrustInput(Value);
    ThrusterRR->SetThrustInput(Value);
}

void AUnderwaterDrone::SetSway(float Value)
{
    Value = FMath::Clamp(Value, -1.0f, 1.0f);
    ThrusterFL->SetThrustInput(-Value);
    ThrusterFR->SetThrustInput(Value);
    ThrusterRL->SetThrustInput(Value);
    ThrusterRR->SetThrustInput(-Value);
}

void AUnderwaterDrone::SetHeave(float Value)
{
    Value = FMath::Clamp(Value, -1.0f, 1.0f);
    ThrusterVL->SetThrustInput(Value);
    ThrusterVR->SetThrustInput(Value);
}

void AUnderwaterDrone::SetYaw(float Value)
{
    Value = FMath::Clamp(Value, -1.0f, 1.0f);
    ThrusterFL->SetThrustInput(Value);
    ThrusterFR->SetThrustInput(-Value);
    ThrusterRL->SetThrustInput(-Value);
    ThrusterRR->SetThrustInput(Value);
}

void AUnderwaterDrone::StopAllThrusters()
{
    ThrusterFL->SetThrustInput(0.0f);
    ThrusterFR->SetThrustInput(0.0f);
    ThrusterRL->SetThrustInput(0.0f);
    ThrusterRR->SetThrustInput(0.0f);
    ThrusterVL->SetThrustInput(0.0f);
    ThrusterVR->SetThrustInput(0.0f);
}

void AUnderwaterDrone::InputSurge(float Value) { SetSurge(Value); }
void AUnderwaterDrone::InputSway(float Value) { SetSway(Value); }
void AUnderwaterDrone::InputHeave(float Value) { SetHeave(Value); }
void AUnderwaterDrone::InputYaw(float Value) { SetYaw(Value); }

FVector AUnderwaterDrone::GetVelocityMetersPerSec() const
{
    return DroneBody->GetComponentVelocity() / 100.0f;
}

float AUnderwaterDrone::GetCurrentDepth() const
{
    if (BuoyancyComp)
    {
        return BuoyancyComp->GetDepth();
    }
    return 0.0f;
}
