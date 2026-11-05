// UnderwaterDrone.cpp
#include "Drone/UnderwaterDrone.h"
#include "Drone/AUVBuoyancyComponent.h"
#include "Drone/ThrusterComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "InputCoreTypes.h"
#include "UnderwaterSim.h"

AUnderwaterDrone::AUnderwaterDrone()
{
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    // ── Drone Body (root collision sphere for smooth physics) ──
    DroneBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DroneBody"));
    RootComponent = DroneBody;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereFinder.Succeeded())
    {
        DroneBody->SetStaticMesh(SphereFinder.Object);
    }

    DroneBody->SetSimulatePhysics(true);
    DroneBody->SetEnableGravity(false); // Neutrally buoyant in water so it never floats up uncontrollably
    DroneBody->SetCollisionProfileName(TEXT("Pawn"));
    DroneBody->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    DroneBody->SetMassOverrideInKg(NAME_None, 11.5f, true); // BlueROV2 ~11.5 kg
    DroneBody->SetLinearDamping(2.0f);
    DroneBody->SetAngularDamping(4.0f);
    DroneBody->SetVisibility(false, false);
    DroneBody->SetHiddenInGame(true, false);

    // Auto-stabilization / Metacentric lock: keep drone permanently upright (Roll = 0, Pitch = 0)
    DroneBody->BodyInstance.bLockXRotation = true;
    DroneBody->BodyInstance.bLockYRotation = true;

    // ── Visual Mesh (Submarine 3D model, upright with Roll = +90) ──
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(DroneBody);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SubMeshFinder(
        TEXT("/Game/Drones/SubmarineModel/_Submarine2._Submarine2"));
    if (SubMeshFinder.Succeeded())
    {
        VisualMesh->SetStaticMesh(SubMeshFinder.Object);
    }
    // _Submarine2 has local UP along -Y; Roll=+90 rotates -Y to +Z (World Up)
    VisualMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
    VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -15.0f));
    VisualMesh->SetRelativeScale3D(FVector(1.5f));
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetVisibility(true, false);
    VisualMesh->SetHiddenInGame(false, false);

    // ── Buoyancy ──
    BuoyancyComp = CreateDefaultSubobject<UAUVBuoyancyComponent>(TEXT("Buoyancy"));

    // ── Camera (third-person follow for debugging/viewing) ──
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 350.0f;
    CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
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

    // Force upright rotation (Pitch=0, Roll=0) in case map actor had stale rotation
    FRotator CurrentRot = GetActorRotation();
    SetActorRotation(FRotator(0.0f, CurrentRot.Yaw, 0.0f));

    if (DroneBody)
    {
        DroneBody->SetEnableGravity(false);
        DroneBody->BodyInstance.bLockXRotation = true;
        DroneBody->BodyInstance.bLockYRotation = true;
        DroneBody->SetVisibility(false, false);
        DroneBody->SetHiddenInGame(true, false);
    }

    if (VisualMesh)
    {
        VisualMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
        VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -15.0f));
        VisualMesh->SetVisibility(true, false);
        VisualMesh->SetHiddenInGame(false, false);

        if (DroneRole == EDroneRole::Hider || GetName().Contains(TEXT("Hider")))
        {
            UMaterialInterface* BlueMat = LoadObject<UMaterialInterface>(nullptr,
                TEXT("/Game/Environments/MarineLife/M_HiderSub_Blue.M_HiderSub_Blue"));
            if (BlueMat)
            {
                const int32 NumMats = VisualMesh->GetNumMaterials();
                for (int32 i = 0; i < NumMats; ++i)
                {
                    UMaterialInterface* CurMat = VisualMesh->GetMaterial(i);
                    const FString MatName = CurMat ? CurMat->GetName() : TEXT("");
                    if (MatName != TEXT("Glass") && MatName != TEXT("Black_Metal") && MatName != TEXT("Light"))
                    {
                        VisualMesh->SetMaterial(i, BlueMat);
                    }
                }
            }
        }
    }

    UE_LOG(LogUnderwaterSim, Log, TEXT("Drone %d spawned as %s at Z=%.1f"),
           DroneID,
           DroneRole == EDroneRole::Seeker ? TEXT("SEEKER") :
           DroneRole == EDroneRole::Hider ? TEXT("HIDER") : TEXT("NEUTRAL"),
           GetActorLocation().Z);
}

void AUnderwaterDrone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        // Direct keyboard polling fallback so diving & moving ALWAYS work regardless of EnhancedInput settings
        float KeySurge = 0.0f;
        if (PC->IsInputKeyDown(EKeys::W) || PC->IsInputKeyDown(EKeys::Up))   KeySurge += 1.0f;
        if (PC->IsInputKeyDown(EKeys::S))                                   KeySurge -= 1.0f;

        float KeySway = 0.0f;
        if (PC->IsInputKeyDown(EKeys::D) || PC->IsInputKeyDown(EKeys::Right)) KeySway += 1.0f;
        if (PC->IsInputKeyDown(EKeys::A) || PC->IsInputKeyDown(EKeys::Left))  KeySway -= 1.0f;

        float KeyHeave = 0.0f;
        // Ascend keys: E, SpaceBar
        if (PC->IsInputKeyDown(EKeys::E) || PC->IsInputKeyDown(EKeys::SpaceBar))
        {
            KeyHeave += 1.0f;
        }
        // Dive keys: Q, LeftShift, RightShift, C, LeftControl, Down Arrow
        if (PC->IsInputKeyDown(EKeys::Q) ||
            PC->IsInputKeyDown(EKeys::LeftShift) ||
            PC->IsInputKeyDown(EKeys::RightShift) ||
            PC->IsInputKeyDown(EKeys::C) ||
            PC->IsInputKeyDown(EKeys::LeftControl) ||
            PC->IsInputKeyDown(EKeys::Down))
        {
            KeyHeave -= 1.0f;
        }

        if (FMath::Abs(KeySurge) > FMath::Abs(PendingSurge)) PendingSurge = KeySurge;
        if (FMath::Abs(KeySway) > FMath::Abs(PendingSway))   PendingSway = KeySway;
        if (FMath::Abs(KeyHeave) > FMath::Abs(PendingHeave)) PendingHeave = KeyHeave;

        // Mouse look for camera orbit & pitch
        float MouseX = 0.0f, MouseY = 0.0f;
        PC->GetInputMouseDelta(MouseX, MouseY);
        if (!FMath::IsNearlyZero(MouseX))
        {
            AddControllerYawInput(MouseX * 0.6f);
        }
        if (!FMath::IsNearlyZero(MouseY))
        {
            AddControllerPitchInput(-MouseY * 0.6f);
        }

        // Compute desired 3D movement velocity
        const FRotator ControlRot = PC->GetControlRotation();
        const FRotator YawOnlyRot(0.0f, ControlRot.Yaw, 0.0f);

        // Keep drone body level (Pitch=0, Roll=0) and aligned with camera Yaw
        SetActorRotation(YawOnlyRot);

        // Forward vector follows camera pitch so looking down + pressing W also dives!
        const FVector CamForward = ControlRot.Vector();
        const FVector RightDir = FRotationMatrix(YawOnlyRot).GetUnitAxis(EAxis::Y);
        const FVector UpDir = FVector::UpVector;

        const float MaxHorizontalSpeed = 650.0f; // 6.5 m/s
        const float MaxVerticalSpeed = 550.0f;   // 5.5 m/s diving/ascending speed

        FVector DesiredVelocity =
            CamForward * (PendingSurge * MaxHorizontalSpeed) +
            RightDir * (PendingSway * MaxHorizontalSpeed) +
            UpDir * (PendingHeave * MaxVerticalSpeed);

        if (DroneBody && DroneBody->IsSimulatingPhysics())
        {
            const FVector CurrentVel = DroneBody->GetPhysicsLinearVelocity();
            const FVector NewVel = FMath::VInterpTo(CurrentVel, DesiredVelocity, DeltaTime, 6.0f);
            DroneBody->SetPhysicsLinearVelocity(NewVel);
            DroneBody->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
        }
        else
        {
            AddActorWorldOffset(DesiredVelocity * DeltaTime, true);
        }
    }

    ApplyMixedThrusterInputs();

    // Reset per-frame input accumulators
    PendingSurge = 0.0f;
    PendingSway = 0.0f;
    PendingHeave = 0.0f;
    PendingYaw = 0.0f;
}

void AUnderwaterDrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AUnderwaterDrone::InputSurge);
    PlayerInputComponent->BindAxis("MoveRight", this, &AUnderwaterDrone::InputSway);
    PlayerInputComponent->BindAxis("MoveUp", this, &AUnderwaterDrone::InputHeave);
    PlayerInputComponent->BindAxis("Turn", this, &AUnderwaterDrone::InputYaw);
    PlayerInputComponent->BindAxis("LookUp", this, &AUnderwaterDrone::InputLookUp);
}

void AUnderwaterDrone::ApplyMixedThrusterInputs()
{
    const float FL = FMath::Clamp(PendingSurge - PendingSway + PendingYaw, -1.0f, 1.0f);
    const float FR = FMath::Clamp(PendingSurge + PendingSway - PendingYaw, -1.0f, 1.0f);
    const float RL = FMath::Clamp(PendingSurge + PendingSway - PendingYaw, -1.0f, 1.0f);
    const float RR = FMath::Clamp(PendingSurge - PendingSway + PendingYaw, -1.0f, 1.0f);
    const float V  = FMath::Clamp(PendingHeave, -1.0f, 1.0f);

    if (ThrusterFL) ThrusterFL->SetThrustInput(FL);
    if (ThrusterFR) ThrusterFR->SetThrustInput(FR);
    if (ThrusterRL) ThrusterRL->SetThrustInput(RL);
    if (ThrusterRR) ThrusterRR->SetThrustInput(RR);
    if (ThrusterVL) ThrusterVL->SetThrustInput(V);
    if (ThrusterVR) ThrusterVR->SetThrustInput(V);
}

void AUnderwaterDrone::SetSurge(float Value)
{
    PendingSurge = FMath::Clamp(Value, -1.0f, 1.0f);
    ApplyMixedThrusterInputs();
}

void AUnderwaterDrone::SetSway(float Value)
{
    PendingSway = FMath::Clamp(Value, -1.0f, 1.0f);
    ApplyMixedThrusterInputs();
}

void AUnderwaterDrone::SetHeave(float Value)
{
    PendingHeave = FMath::Clamp(Value, -1.0f, 1.0f);
    ApplyMixedThrusterInputs();
}

void AUnderwaterDrone::SetYaw(float Value)
{
    PendingYaw = FMath::Clamp(Value, -1.0f, 1.0f);
    ApplyMixedThrusterInputs();
}

void AUnderwaterDrone::StopAllThrusters()
{
    PendingSurge = 0.0f;
    PendingSway = 0.0f;
    PendingHeave = 0.0f;
    PendingYaw = 0.0f;
    ApplyMixedThrusterInputs();
}

void AUnderwaterDrone::InputSurge(float Value)
{
    if (!FMath::IsNearlyZero(Value)) PendingSurge = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AUnderwaterDrone::InputSway(float Value)
{
    if (!FMath::IsNearlyZero(Value)) PendingSway = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AUnderwaterDrone::InputHeave(float Value)
{
    if (!FMath::IsNearlyZero(Value)) PendingHeave = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AUnderwaterDrone::InputYaw(float Value)
{
    if (!FMath::IsNearlyZero(Value))
    {
        PendingYaw = FMath::Clamp(Value, -1.0f, 1.0f);
        AddControllerYawInput(Value);
    }
}

void AUnderwaterDrone::InputLookUp(float Value)
{
    if (!FMath::IsNearlyZero(Value))
    {
        AddControllerPitchInput(Value);
    }
}

FVector AUnderwaterDrone::GetVelocityMetersPerSec() const
{
    return DroneBody ? (DroneBody->GetComponentVelocity() / 100.0f) : FVector::ZeroVector;
}

float AUnderwaterDrone::GetCurrentDepth() const
{
    if (BuoyancyComp)
    {
        return BuoyancyComp->GetDepth();
    }
    return 0.0f;
}
