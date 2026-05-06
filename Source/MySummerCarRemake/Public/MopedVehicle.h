// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MopedVehicle.generated.h"

USTRUCT(BlueprintType)
struct FMopedWheel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Wheel")
	FVector LocalOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category="Wheel")
	float Radius = 28.f;

	UPROPERTY(EditAnywhere, Category="Wheel")
	float SuspensionLength = 20.f;

	UPROPERTY(EditAnywhere, Category="Wheel")
	float SpringStiffness = 18000.f;

	UPROPERTY(EditAnywhere, Category="Wheel")
	float DamperCoefficient = 2000.f;

	UPROPERTY(EditAnywhere, Category="Wheel")
	float FrictionCoeff = 0.85f;

	UPROPERTY(EditAnywhere, Category="Wheel")
	bool bDriven = false;

	UPROPERTY(EditAnywhere, Category="Wheel")
	bool bSteered = false;

	float SuspensionVelocity = 0.f;
	float SuspensionCompression = 0.f;
	float NormalForce = 0.f;
	float AngularVelocity = 0.f;
	FVector ContactPoint = FVector::ZeroVector;
	FVector ContactNormal = FVector::UpVector;
	bool bGrounded = false;
	float VisualRotationAngle = 0.f;
};

USTRUCT(BlueprintType)
struct FGearRatio
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Gearbox")
	float Ratio = 1.f;
};

UCLASS()
class MYSUMMERCARREMAKE_API AMopedVehicle : public APawn
{
	GENERATED_BODY()

public:
	AMopedVehicle();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category="Moped")
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, Category="Moped")
	UStaticMeshComponent* FrontWheelMesh;

	UPROPERTY(VisibleAnywhere, Category="Moped")
	UStaticMeshComponent* RearWheelMesh;

	UPROPERTY(VisibleAnywhere, Category="Moped")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category="Moped")
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category="Wheels")
	FMopedWheel FrontWheel;

	UPROPERTY(EditAnywhere, Category="Wheels")
	FMopedWheel RearWheel;

	UPROPERTY(EditAnywhere, Category="Engine")
	float IdleRPM = 900.f;

	UPROPERTY(EditAnywhere, Category="Engine")
	float MaxRPM = 7500.f;

	UPROPERTY(EditAnywhere, Category="Engine")
	float MaxTorque = 12.f;

	UPROPERTY(EditAnywhere, Category="Engine")
	float EngineInertia = 0.15f;

	UPROPERTY(EditAnywhere, Category="Engine")
	float EngineFrictionCoeff = 0.04f;

	UPROPERTY(EditAnywhere, Category="Gearbox")
	float FinalDriveRatio = 4.5f;

	UPROPERTY(EditAnywhere, Category="Gearbox")
	TArray<FGearRatio> Gears;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gearbox")
	int32 CurrentGear = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Clutch")
	float ClutchEngagement = 1.f;

	UPROPERTY(EditAnywhere, Category="Clutch")
	float ClutchEngageSpeed = 2.f;

	UPROPERTY(EditAnywhere, Category="Clutch")
	float ClutchMaxTorque = 25.f;

	UPROPERTY(EditAnywhere, Category="Brakes")
	float FrontBrakeTorque = 150.f;

	UPROPERTY(EditAnywhere, Category="Brakes")
	float RearBrakeTorque = 100.f;

	UPROPERTY(EditAnywhere, Category="Physics")
	float VehicleMass = 90.f;  // kg

	UPROPERTY(EditAnywhere, Category="Physics")
	FVector COMOffset = FVector(0.f, 0.f, -10.f);

	UPROPERTY(EditAnywhere, Category="Steering")
	float MaxSteerAngle = 35.f;

	UPROPERTY(EditAnywhere, Category="Steering")
	float SteerSpeed = 120.f;

	UPROPERTY(EditAnywhere, Category="Steering")
	float SteerReturnSpeed = 80.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Engine")
	float EngineRPM = 0.f;

	float EngineAngularVelocity = 0.f;

	float CurrentSteerAngle = 0.f;

	float ThrottleInput = 0.f;
	float BrakeInput = 0.f;
	float SteerInput = 0.f;
	bool bClutchPressed = false;
	bool bHandbrake = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Engine")
	bool bEngineRunning = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Physics")
	float SpeedKmh = 0.f;

private:
	void InputThrottle(float Value);
	void InputBrake(float Value);
	void InputSteer(float Value);
	void InputClutch();
	void InputClutchRelease();
	void InputHandbrake();
	void InputHandbrakeRelease();
	void InputGearUp();
	void InputGearDown();
	void InputStartEngine();
	
	void UpdateWheelPhysics(FMopedWheel& Wheel, float DeltaTime);
	void ApplyWheelForces(FMopedWheel& Wheel, float DeltaTime);
	void UpdateEngineAndDrivetrain(float DeltaTime);
	void UpdateSteering(float DeltaTime);
	void UpdateWheelVisuals();

	float GetEngineTorqueFactor(float NormRPM) const;

	float GetCurrentGearRatio() const;
	
	UPrimitiveComponent* BodyPrimitive = nullptr;
};