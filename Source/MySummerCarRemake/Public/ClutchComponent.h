#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClutchComponent.generated.h"

/**
 * A component that simulates a clutch mechanism between two rotating systems.
 * It calculates torque transmission based on engagement and friction.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYSUMMERCARREMAKE_API UClutchComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UClutchComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Maximum torque the clutch can transmit when fully engaged (Nm) at unit friction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clutch")
	float MaxTorqueCapacity;

	/** How quickly the clutch engagement moves towards the target value (0 to 1 range per second) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clutch")
	float EngagementRate;

	/** Friction coefficient when there is no slippage (static) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clutch")
	float StaticFrictionCoefficient;

	/** Friction coefficient when there is slippage (dynamic) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clutch")
	float DynamicFrictionCoefficient;

	/** Moment of inertia of the output system (kg*m^2). Used to calculate speed changes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clutch")
	float OutputInertia;

	/** Current rotation speed of the input side (e.g., Engine) in rad/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clutch")
	float InputRotationSpeed;

	/** Current rotation speed of the output side (e.g., Gearbox input) in rad/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clutch")
	float OutputRotationSpeed;

	/** Enable or disable debug visualization on screen */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clutch|Debug")
	bool bShowDebug;

	/** 
	 * Sets the desired engagement percentage.
	 * @param Percent Target engagement from 0.0 (fully disengaged) to 1.0 (fully engaged)
	 */
	UFUNCTION(BlueprintCallable, Category = "Clutch")
	void SetEngagement(float Percent);

	/** 
	 * Returns the current slippage ratio.
	 * 0.0 means perfectly synchronized, 1.0 means full speed difference relative to a reference.
	 * Here we return the absolute speed difference.
	 */
	UFUNCTION(BlueprintPure, Category = "Clutch")
	float GetSlippageRatio() const;

	/** Returns true if the clutch is currently slipping beyond a small threshold */
	UFUNCTION(BlueprintPure, Category = "Clutch")
	bool IsSlipping() const { return bIsSlipping; }

	/** Returns the current transmitted torque in Nm */
	UFUNCTION(BlueprintPure, Category = "Clutch")
	float GetTransmittedTorque() const { return TransmittedTorque; }

private:
	/** Current actual engagement value (interpolated) */
	float CurrentEngagement;
	
	/** Desired engagement value */
	float TargetEngagement;
	
	/** Last calculated transmitted torque */
	float TransmittedTorque;
	
	/** Internal state of slippage */
	bool bIsSlipping;

	/** Updates the current engagement value based on the rate and DeltaTime */
	void UpdateEngagement(float DeltaTime);
	
	/** Calculates the torque transmission and updates the output speed */
	void CalculateTorque(float DeltaTime);
	
	/** Renders debug information on the screen */
	void DrawDebugInfo();
};
