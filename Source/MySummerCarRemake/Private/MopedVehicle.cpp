// 2026 sabaka-chabaka

#include "MySummerCarRemake/Public/MopedVehicle.h"
#include "MySummerCarRemake/Public/MSCCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogMoped, All, All);

AMopedVehicle::AMopedVehicle()
{
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	RootComponent = BodyMesh;
	BodyMesh->SetSimulatePhysics(true);
	BodyMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	BodyMesh->SetLinearDamping(0.05f);
	BodyMesh->SetAngularDamping(1.5f);

	FrontWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrontWheelMesh"));
	FrontWheelMesh->SetupAttachment(BodyMesh);
	FrontWheelMesh->SetSimulatePhysics(false);
	FrontWheelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RearWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RearWheelMesh"));
	RearWheelMesh->SetupAttachment(BodyMesh);
	RearWheelMesh->SetSimulatePhysics(false);
	RearWheelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SeatSocket = CreateDefaultSubobject<USceneComponent>(TEXT("SeatSocket"));
	SeatSocket->SetupAttachment(BodyMesh);
	SeatSocket->SetRelativeLocation(FVector(-10.f, 0.f, 30.f));
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BodyMesh);
	SpringArm->TargetArmLength = 280.f;
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll  = false;

	MopedCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MopedCamera"));
	MopedCamera->SetupAttachment(SpringArm);
	MopedCamera->SetAutoActivate(false);

	FrontWheel.LocalOffset    = FVector(65.f,  0.f, -20.f);
	FrontWheel.Radius         = 28.f;
	FrontWheel.SuspensionLength = 18.f;
	FrontWheel.SpringStiffness  = 16000.f;
	FrontWheel.DamperCoefficient= 1800.f;
	FrontWheel.FrictionCoeff    = 0.85f;
	FrontWheel.bDriven  = false;
	FrontWheel.bSteered = true;

	RearWheel.LocalOffset    = FVector(-60.f, 0.f, -20.f);
	RearWheel.Radius         = 28.f;
	RearWheel.SuspensionLength = 18.f;
	RearWheel.SpringStiffness  = 18000.f;
	RearWheel.DamperCoefficient= 2000.f;
	RearWheel.FrictionCoeff    = 0.85f;
	RearWheel.bDriven  = true;
	RearWheel.bSteered = false;

	Gears.SetNum(5);
	Gears[0].Ratio = -3.5f;
	Gears[1].Ratio =  3.0f;
	Gears[2].Ratio =  1.9f;
	Gears[3].Ratio =  1.3f;
	Gears[4].Ratio =  0.95f;

	CurrentGear = 0;
}

void AMopedVehicle::BeginPlay()
{
	Super::BeginPlay();

	BodyPrimitive = Cast<UPrimitiveComponent>(RootComponent);
	if (BodyPrimitive)
	{
		BodyPrimitive->SetMassOverrideInKg(NAME_None, VehicleMass);
		BodyPrimitive->SetCenterOfMass(COMOffset);
	}

	StarterState = EStarterState::Idle;
	EngineAngularVelocity = 0.f;
	EngineRPM = 0.f;
}

void AMopedVehicle::TryMountCharacter(AMSCCharacter* Character)
{
	if (!Character || RidingCharacter) return;

	const float Dist = FVector::Dist(Character->GetActorLocation(), GetActorLocation());
	if (Dist > MountDistance)
	{
		UE_LOG(LogMoped, Warning, TEXT("Too far to mount moped (%.0f cm)"), Dist);
		return;
	}

	RidingCharacter = Character;

	Character->GetCharacterMovement()->DisableMovement();
	Character->GetCharacterMovement()->StopMovementImmediately();
	Character->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	Character->AttachToComponent(SeatSocket, Rules);

	if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
	{
		MopedCamera->SetActive(true);
		PC->SetViewTargetWithBlend(this, 0.3f);
	}

	UE_LOG(LogMoped, Log, TEXT("Character mounted moped"));
}

void AMopedVehicle::DismountCharacter()
{
	if (!RidingCharacter) return;

	AMSCCharacter* Character = RidingCharacter;
	RidingCharacter = nullptr;

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	Character->DetachFromActor(DetachRules);

	const FVector DismountPos = GetActorLocation()
		+ GetActorRightVector() * 80.f
		+ FVector(0.f, 0.f, 10.f);
	Character->SetActorLocation(DismountPos);

	Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Character->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);

	ThrottleInput = 0.f;
	SteerInput    = 0.f;
	BrakeInput    = 0.f;

	if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
	{
		MopedCamera->SetActive(false);
		PC->SetViewTargetWithBlend(Character, 0.3f);
	}

	UE_LOG(LogMoped, Log, TEXT("Character dismounted moped"));
}

void AMopedVehicle::PressStarter()
{
	if (StarterState == EStarterState::Running) return;

	StarterState = EStarterState::Cranking;
	CrankTimer   = 0.f;
	UE_LOG(LogMoped, Log, TEXT("Starter: cranking..."));
}

void AMopedVehicle::ReleaseStarter()
{
	if (StarterState == EStarterState::Cranking)
	{
		StarterState = EStarterState::Idle;
		UE_LOG(LogMoped, Log, TEXT("Starter: released without start"));
	}
}

void AMopedVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!BodyPrimitive) return;

	UpdateStarterCrank(DeltaTime);
	UpdateSteering(DeltaTime);
	UpdateWheelPhysics(FrontWheel, DeltaTime);
	UpdateWheelPhysics(RearWheel,  DeltaTime);
	ApplyWheelForces(FrontWheel, DeltaTime);
	ApplyWheelForces(RearWheel,  DeltaTime);
	UpdateEngineAndDrivetrain(DeltaTime);
	UpdateWheelVisuals(DeltaTime);

	SpeedKmh = BodyPrimitive->GetPhysicsLinearVelocity().Size() * 0.036f;

#if WITH_EDITOR
	auto DrawWheel = [&](const FMopedWheel& W)
	{
		FVector Attach = GetActorTransform().TransformPosition(W.LocalOffset);
		FVector Down   = -GetActorUpVector() * (W.SuspensionLength + W.Radius);
		DrawDebugLine(GetWorld(), Attach, Attach + Down,
			W.bGrounded ? FColor::Green : FColor::Red, false, -1.f, 0, 2.f);
		if (W.bGrounded)
			DrawDebugSphere(GetWorld(), W.ContactPoint, 5.f, 6, FColor::Yellow, false, -1.f, 0, 1.f);
	};
	DrawWheel(FrontWheel);
	DrawWheel(RearWheel);
#endif
}

void AMopedVehicle::UpdateStarterCrank(float DeltaTime)
{
	if (StarterState != EStarterState::Cranking) return;

	const float CrankRad = 200.f * (PI / 30.f);
	EngineAngularVelocity = FMath::FInterpTo(EngineAngularVelocity, CrankRad, DeltaTime, 5.f);
	EngineRPM = EngineAngularVelocity * (30.f / PI);

	CrankTimer += DeltaTime;

	if (CrankTimer >= CrankDuration)
	{
		const float Roll = FMath::FRandRange(0.f, 1.f);
		const float Threshold = 1.f - FMath::Pow(1.f - StartProbabilityPerSec, CrankDuration);

		if (Roll < Threshold)
		{
			StarterState = EStarterState::Running;
			EngineAngularVelocity = IdleRPM * (PI / 30.f);
			UE_LOG(LogMoped, Log, TEXT("Engine started! RPM: %.0f"), IdleRPM);
		}
		else
		{
			CrankTimer = 0.f;
			EngineAngularVelocity = FMath::FRandRange(50.f, 150.f) * (PI / 30.f); // кашляет
			UE_LOG(LogMoped, Log, TEXT("Engine failed to start, retrying..."));
		}
	}
}

void AMopedVehicle::UpdateWheelPhysics(FMopedWheel& Wheel, float DeltaTime)
{
	const FVector AttachWorld = GetActorTransform().TransformPosition(Wheel.LocalOffset);
	const FVector DownDir     = -GetActorUpVector();
	const float   RayLength   = Wheel.SuspensionLength + Wheel.Radius;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (RidingCharacter) Params.AddIgnoredActor(RidingCharacter);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit, AttachWorld, AttachWorld + DownDir * RayLength,
		ECC_Visibility, Params
	);

	const float PrevCompression = Wheel.SuspensionCompression;

	if (bHit)
	{
		Wheel.bGrounded       = true;
		Wheel.ContactPoint    = Hit.ImpactPoint;
		Wheel.ContactNormal   = Hit.ImpactNormal;
		const float Dist      = (AttachWorld - Hit.ImpactPoint).Size();
		Wheel.SuspensionCompression = FMath::Max(0.f, RayLength - Dist);
		Wheel.SuspensionVelocity    = (Wheel.SuspensionCompression - PrevCompression) / FMath::Max(DeltaTime, SMALL_NUMBER);
	}
	else
	{
		Wheel.bGrounded             = false;
		Wheel.SuspensionCompression = 0.f;
		Wheel.SuspensionVelocity    = 0.f;
		Wheel.ContactNormal         = FVector::UpVector;
		Wheel.NormalForce           = 0.f;
	}
}

void AMopedVehicle::ApplyWheelForces(FMopedWheel& Wheel, float DeltaTime)
{
	if (!Wheel.bGrounded) return;

	const FVector AttachWorld = GetActorTransform().TransformPosition(Wheel.LocalOffset);

	const float SpringF  = Wheel.SpringStiffness  * Wheel.SuspensionCompression;
	const float DamperF  = Wheel.DamperCoefficient * Wheel.SuspensionVelocity;
	Wheel.NormalForce    = FMath::Max(0.f, SpringF + DamperF);
	BodyPrimitive->AddForceAtLocation(Wheel.ContactNormal * Wheel.NormalForce, AttachWorld);

	FRotator WheelRot = GetActorRotation();
	if (Wheel.bSteered) WheelRot.Yaw += CurrentSteerAngle;

	const FVector WheelFwd   = WheelRot.Vector();
	const FVector WheelRight = FRotationMatrix(WheelRot).GetScaledAxis(EAxis::Y);

	const FVector BodyVel = BodyPrimitive->GetPhysicsLinearVelocityAtPoint(AttachWorld);
	const float   LongVel = FVector::DotProduct(BodyVel, WheelFwd);
	const float   LatVel  = FVector::DotProduct(BodyVel, WheelRight);

	const float MaxForce = Wheel.NormalForce * Wheel.FrictionCoeff;

	const float LatForce = FMath::Clamp(-LatVel * VehicleMass * 0.5f, -MaxForce, MaxForce);
	BodyPrimitive->AddForceAtLocation(WheelRight * LatForce, AttachWorld);

	const float WheelMOI   = 0.5f * 3.5f * Wheel.Radius * Wheel.Radius;
	const float WheelLinVel = Wheel.AngularVelocity * Wheel.Radius;
	const float LongSlip    = WheelLinVel - LongVel;
	const float SlipStiff   = 90000.f;
	const float LongForce   = FMath::Clamp(LongSlip * SlipStiff, -MaxForce, MaxForce);

	Wheel.AngularVelocity += (-LongForce * Wheel.Radius / WheelMOI) * DeltaTime;
	BodyPrimitive->AddForceAtLocation(WheelFwd * LongForce, AttachWorld);

	if (BrakeInput > 0.01f)
	{
		const float BTorque = Wheel.bSteered ? FrontBrakeTorque : RearBrakeTorque;
		const float BSign   = (Wheel.AngularVelocity > 0.f) ? -1.f : 1.f;
		Wheel.AngularVelocity += BSign * BTorque * BrakeInput / WheelMOI * DeltaTime;
		if (FMath::Abs(Wheel.AngularVelocity) < 0.5f) Wheel.AngularVelocity = 0.f;
	}

	if (bHandbrake && !Wheel.bSteered)
		Wheel.AngularVelocity = FMath::FInterpTo(Wheel.AngularVelocity, 0.f, DeltaTime, 10.f);

	const float RollingF = -FMath::Sign(LongVel) * Wheel.NormalForce * 0.015f;
	BodyPrimitive->AddForceAtLocation(WheelFwd * RollingF, AttachWorld);
}

void AMopedVehicle::UpdateEngineAndDrivetrain(float DeltaTime)
{
	const float MaxRad  = MaxRPM  * (PI / 30.f);
	const float IdleRad = IdleRPM * (PI / 30.f);

	if (StarterState != EStarterState::Running)
	{
		EngineAngularVelocity = FMath::FInterpTo(EngineAngularVelocity, 0.f, DeltaTime, 3.f);
		EngineRPM = EngineAngularVelocity * (30.f / PI);
		return;
	}

	const float NormRPM    = FMath::Clamp(EngineRPM / MaxRPM, 0.f, 1.f);
	const float TorqueFact = GetEngineTorqueFactor(NormRPM);
	const float EngineTorque = ThrottleInput * MaxTorque * TorqueFact;

	const float EngineFriction = -EngineAngularVelocity * EngineFrictionCoeff * MaxTorque;

	ClutchEngagement = bClutchPressed
		? FMath::FInterpTo(ClutchEngagement, 0.f, DeltaTime, 8.f)
		: FMath::FInterpTo(ClutchEngagement, 1.f, DeltaTime, ClutchEngageSpeed);

	const float GearRatio = GetCurrentGearRatio();
	const float WheelMOI  = 0.5f * 3.5f * RearWheel.Radius * RearWheel.Radius;

	if (CurrentGear != 0 && !FMath::IsNearlyZero(GearRatio))
	{
		const float EngineToWheel    = GearRatio * FinalDriveRatio;
		const float WheelOmegaTarget = EngineAngularVelocity / EngineToWheel;
		const float OmegaDiff        = WheelOmegaTarget - RearWheel.AngularVelocity;

		const float ClutchTorqueAtWheel = FMath::Clamp(
			OmegaDiff * ClutchMaxTorque * ClutchEngagement,
			-ClutchMaxTorque, ClutchMaxTorque
		);
		const float ClutchTorqueAtEngine = ClutchTorqueAtWheel * EngineToWheel;

		const float dOmegaEngine = (EngineTorque + EngineFriction - ClutchTorqueAtEngine) / EngineInertia * DeltaTime;
		EngineAngularVelocity    = FMath::Clamp(EngineAngularVelocity + dOmegaEngine, 0.f, MaxRad);

		RearWheel.AngularVelocity += (ClutchTorqueAtWheel / WheelMOI) * DeltaTime;
	}
	else
	{
		const float dOmegaEngine = (EngineTorque + EngineFriction) / EngineInertia * DeltaTime;
		EngineAngularVelocity    = FMath::Clamp(EngineAngularVelocity + dOmegaEngine, 0.f, MaxRad);
	}

	if (ThrottleInput < 0.01f)
		EngineAngularVelocity = FMath::Max(EngineAngularVelocity, IdleRad);

	if (EngineAngularVelocity < IdleRad * 0.3f && ThrottleInput < 0.01f)
	{
		StarterState = EStarterState::Idle;
		EngineAngularVelocity = 0.f;
		UE_LOG(LogMoped, Log, TEXT("Engine stalled!"));
	}

	EngineRPM = EngineAngularVelocity * (30.f / PI);
}

float AMopedVehicle::GetEngineTorqueFactor(float NormRPM) const
{
	if (NormRPM < 0.10f) return NormRPM * 5.f;
	if (NormRPM < 0.55f) return FMath::Lerp(0.5f, 1.0f, (NormRPM - 0.1f) / 0.45f);
	if (NormRPM < 0.75f) return 1.0f;
	return FMath::Lerp(1.0f, 0.3f, (NormRPM - 0.75f) / 0.25f);
}

float AMopedVehicle::GetCurrentGearRatio() const
{
	if (CurrentGear == 0) return 0.f;
	const int32 Idx = CurrentGear - 1;
	return Gears.IsValidIndex(Idx) ? Gears[Idx].Ratio : 0.f;
}

void AMopedVehicle::UpdateSteering(float DeltaTime)
{
	if (FMath::Abs(SteerInput) > 0.01f)
	{
		CurrentSteerAngle = FMath::Clamp(
			CurrentSteerAngle + SteerInput * SteerSpeed * DeltaTime,
			-MaxSteerAngle, MaxSteerAngle
		);
	}
	else
	{
		const float ReturnDir = -FMath::Sign(CurrentSteerAngle);
		CurrentSteerAngle += ReturnDir * SteerReturnSpeed * DeltaTime;
		if (FMath::Abs(CurrentSteerAngle) < 1.f) CurrentSteerAngle = 0.f;
	}

	const float SpeedMS = BodyPrimitive->GetPhysicsLinearVelocity().Size();
	if (SpeedMS > 2.f && FMath::Abs(CurrentSteerAngle) > 0.5f)
	{
		const float LeanTorque = (CurrentSteerAngle / MaxSteerAngle) * SpeedMS * 0.8f;
		BodyPrimitive->AddTorqueInDegrees(GetActorForwardVector() * LeanTorque, NAME_None, true);
	}

	FRotator ForkRot = FrontWheelMesh->GetRelativeRotation();
	ForkRot.Yaw = CurrentSteerAngle;
	FrontWheelMesh->SetRelativeRotation(ForkRot);
}

void AMopedVehicle::UpdateWheelVisuals(float DeltaTime)
{
	auto RotateWheel = [&](UStaticMeshComponent* Mesh, const FMopedWheel& Wheel)
	{
		if (!Mesh) return;
		FRotator Rot = Mesh->GetRelativeRotation();
		Rot.Pitch += Wheel.AngularVelocity * (180.f / PI) * DeltaTime;
		Mesh->SetRelativeRotation(Rot);
	};
	RotateWheel(FrontWheelMesh, FrontWheel);
	RotateWheel(RearWheelMesh,  RearWheel);
}

void AMopedVehicle::MopedThrottle(float Value)    { ThrottleInput = FMath::Clamp(Value, 0.f,  1.f); }
void AMopedVehicle::MopedBrake(float Value)       { BrakeInput    = FMath::Clamp(Value, 0.f,  1.f); }
void AMopedVehicle::MopedSteer(float Value)       { SteerInput    = FMath::Clamp(Value, -1.f, 1.f); }
void AMopedVehicle::MopedClutchPress()            { bClutchPressed = true; }
void AMopedVehicle::MopedClutchRelease()          { bClutchPressed = false; }
void AMopedVehicle::MopedHandbrakePress()         { bHandbrake = true; }
void AMopedVehicle::MopedHandbrakeRelease()       { bHandbrake = false; }

void AMopedVehicle::MopedGearUp()
{
	if (CurrentGear < Gears.Num())
	{
		CurrentGear++;
		UE_LOG(LogMoped, Log, TEXT("Gear -> %d"), CurrentGear);
	}
}

void AMopedVehicle::MopedGearDown()
{
	if (CurrentGear > 0)
	{
		CurrentGear--;
		UE_LOG(LogMoped, Log, TEXT("Gear -> %d"), CurrentGear);
	}
}