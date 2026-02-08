#include "ClutchComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

UClutchComponent::UClutchComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	PrimaryComponentTick.bCanEverTick = true;

	// Default values
	MaxTorqueCapacity = 500.0f;
	EngagementRate = 2.0f; // Takes 0.5s to go from 0 to 1
	StaticFrictionCoefficient = 1.2f;
	DynamicFrictionCoefficient = 1.0f;
	OutputInertia = 0.5f;
	
	CurrentEngagement = 0.0f;
	TargetEngagement = 0.0f;
	TransmittedTorque = 0.0f;
	InputRotationSpeed = 0.0f;
	OutputRotationSpeed = 0.0f;
	bIsSlipping = false;
	bShowDebug = false;
}

void UClutchComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UClutchComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateEngagement(DeltaTime);
	CalculateTorque(DeltaTime);

	if (bShowDebug)
	{
		DrawDebugInfo();
	}
}

void UClutchComponent::SetEngagement(float Percent)
{
	TargetEngagement = FMath::Clamp(Percent, 0.0f, 1.0f);
}

float UClutchComponent::GetSlippageRatio() const
{
	// Absolute difference in rotation speed
	return FMath::Abs(InputRotationSpeed - OutputRotationSpeed);
}

void UClutchComponent::UpdateEngagement(float DeltaTime)
{
	// Smoothly interpolate current engagement towards target
	if (!FMath::IsNearlyEqual(CurrentEngagement, TargetEngagement))
	{
		float Step = EngagementRate * DeltaTime;
		if (CurrentEngagement < TargetEngagement)
		{
			CurrentEngagement = FMath::Min(CurrentEngagement + Step, TargetEngagement);
		}
		else
		{
			CurrentEngagement = FMath::Max(CurrentEngagement - Step, TargetEngagement);
		}
	}
}

void UClutchComponent::CalculateTorque(float DeltaTime)
{
	float SlipSpeed = InputRotationSpeed - OutputRotationSpeed;
	float AbsSlipSpeed = FMath::Abs(SlipSpeed);
	
	// 1. Friction Transition Logic
	// We transition from static to dynamic friction based on slip speed.
	const float StaticToDynamicThreshold = 1.0f; // rad/s
	float FrictionLerp = FMath::Clamp(AbsSlipSpeed / StaticToDynamicThreshold, 0.0f, 1.0f);
	float CurrentFrictionCoeff = FMath::Lerp(StaticFrictionCoefficient, DynamicFrictionCoefficient, FrictionLerp);
	
	// Determine slipping state for API
	bIsSlipping = AbsSlipSpeed > 0.1f; 
	
	// 2. Torque Capacity
	// Maximum torque transmittable at current engagement
	float TorqueCapacity = MaxTorqueCapacity * CurrentEngagement * CurrentFrictionCoeff;
	
	// 3. Transmitted Torque Calculation
	// We use a small window for "soft locking" to prevent numerical oscillations (chatter)
	const float SoftLockWindow = 0.5f; // rad/s
	float TorqueDirectionMultiplier = FMath::Clamp(SlipSpeed / SoftLockWindow, -1.0f, 1.0f);
	
	TransmittedTorque = TorqueCapacity * TorqueDirectionMultiplier;
	
	// 4. Update Output Rotation Speed
	// Physics update: dw = (Torque / Inertia) * dt
	if (OutputInertia > KINDA_SMALL_NUMBER)
	{
		float Acceleration = TransmittedTorque / OutputInertia;
		float SpeedChange = Acceleration * DeltaTime;
		
		// If the speed change would cause us to cross the input speed while not slipping much,
		// we just lock the speeds.
		if (AbsSlipSpeed < SoftLockWindow && FMath::Abs(SpeedChange) > AbsSlipSpeed)
		{
			OutputRotationSpeed = InputRotationSpeed;
			TransmittedTorque = 0.0f; // Relative acceleration torque becomes zero
		}
		else
		{
			OutputRotationSpeed += SpeedChange;
		}
	}
}

void UClutchComponent::DrawDebugInfo()
{
	if (!GEngine) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	FString SlippingStr = bIsSlipping ? TEXT("SLIPPING") : TEXT("LOCKED");
	FColor DebugColor = bIsSlipping ? FColor::Orange : FColor::Green;

	// Recalculate capacity for debug display
	float AbsSlipSpeed = FMath::Abs(InputRotationSpeed - OutputRotationSpeed);
	float FrictionLerp = FMath::Clamp(AbsSlipSpeed / 1.0f, 0.0f, 1.0f);
	float CurrentFrictionCoeff = FMath::Lerp(StaticFrictionCoefficient, DynamicFrictionCoefficient, FrictionLerp);
	float CurrentCapacity = MaxTorqueCapacity * CurrentEngagement * CurrentFrictionCoeff;

	FString DebugText = FString::Printf(TEXT("--- Clutch Debug [%s] ---\nEngagement: %.1f%%\nState: %s\nTorque: %.2f / %.2f Nm\nSlip Speed: %.2f rad/s\nIn Speed: %.2f\nOut Speed: %.2f"),
		*Owner->GetName(),
		CurrentEngagement * 100.0f,
		*SlippingStr,
		TransmittedTorque,
		CurrentCapacity,
		AbsSlipSpeed,
		InputRotationSpeed,
		OutputRotationSpeed);

	// Use a unique key for this component to prevent flickering/spam
	int32 DebugKey = GetTypeHash(this);
	GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, DebugColor, DebugText);
}
