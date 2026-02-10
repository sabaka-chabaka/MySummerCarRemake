// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MSCCharacter.generated.h"

UENUM(BlueprintType)
enum ECrouchState : uint8
{
	Stand,
	Crouched,
	Land
};

UENUM(BlueprintType)
enum EKillType : uint8
{
	Thirst,
	Hunger
};

UCLASS()
class MYSUMMERCARREMAKE_API AMSCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMSCCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, Category="Camera")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
	class UPhysicsHandleComponent* PhysicsHandle;
	
	UPROPERTY(VisibleAnywhere, Category="Crouching")
	TEnumAsByte<ECrouchState> CrouchState;

	UPROPERTY(VisibleAnywhere, Category="Peeing")
	bool bPeeing;

	UPROPERTY(VisibleAnywhere, Category="Physics")
	FRotator TargetRotation;
	
protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUp(float Value);
	void RotateActorWheel(float Value);
	void Crouch();
	void Pee();
	void StopPee();
	void Interact();
	
	FTimerHandle ThirstKillHandle;
	FTimerHandle HungerKillHandle;
	
public:
	UPROPERTY(EditAnywhere, Category="Needs")
	float Thirst;
	
	UPROPERTY(EditAnywhere, Category="Needs")
	float Hunger;
	
	UPROPERTY(EditAnywhere, Category="Needs")
	float Stress;
	
	UPROPERTY(EditAnywhere, Category="Needs")
	float Urine;
	
	UPROPERTY(EditAnywhere, Category="Needs")
	float Fatigue;
	
	UPROPERTY(EditAnywhere, Category="Needs")
	float Dirtiness;
	
	UPROPERTY(EditAnywhere, Category="Needs")
	float Money;
	
	UPROPERTY(EditAnywhere, Category="Needs")
	float Alcohol;
	
	UPROPERTY(EditAnywhere, Category="Needs")
	float Weight;
	
	UPROPERTY(EditAnywhere, Category="References")
	TSubclassOf<AActor> WeatherController;
	
	UFUNCTION(BlueprintCallable, Category="Needs")
	void SetThirst(float ToSet, bool bAdd);
	
	UFUNCTION(BlueprintCallable, Category="Needs")
	void SetHunger(float ToSet, bool bAdd);
	
	UFUNCTION(BlueprintCallable, Category="Needs")
	void SetStress(float ToSet, bool bAdd);
	
	UFUNCTION(BlueprintCallable, Category="Needs")
	void SetUrine(float ToSet, bool bAdd);
	
	UFUNCTION(BlueprintCallable, Category="Needs")
	void SetFatigue(float ToSet, bool bAdd);
	
	UFUNCTION(BlueprintCallable, Category="Needs")
	void SetDirtiness(float ToSet, bool bAdd);
	
	UFUNCTION(BlueprintCallable, Category="Needs")
	void SetMoney(float ToSet, bool bAdd);
	
	UFUNCTION(BlueprintCallable, Category="Needs")
	void SetAlcohol(float ToSet, bool bAdd);
	
	UFUNCTION(BlueprintCallable, Category="Needs")
	void SetPlayerWeight(float ToSet, bool bAdd);
	
	UFUNCTION(BlueprintCallable, Category="Needs")
	void KillPlayer(EKillType KillType);
	
protected:
	void ChangedThirst();
	
	void ChangedHunger();
	
	void ChangedStress();
	
	void ChangedUrine();
	
	void ChangedFatigue();
	
	void ChangedDirtiness();
	
	void ChangedMoney();
	
	void ChangedAlcohol();
	
	void ChangedPlayerWeight();
};