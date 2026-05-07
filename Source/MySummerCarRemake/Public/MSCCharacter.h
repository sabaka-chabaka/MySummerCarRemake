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
	Hunger,
	Alcohol
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
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* PeeRoot;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* PeeComponent;

	UPROPERTY(EditAnywhere, Category="Pee")
	FVector PeeOffset = FVector(20.f, 0.f, -90.f);

	UPROPERTY(EditAnywhere, Category="Alcohol")
	FVector AlcoholKillLocation;
	
	UPROPERTY(EditAnywhere, Category="Spawning")
	FVector RespawnLocation;
	
	UPROPERTY(EditAnywhere, Category="Satana")
	TArray<FString> FuckStrings;
	
protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUp(float Value);
	void RotateActorWheel(float Value);
	void ToggleCrouch();
	void Pee();
	void StopPee();
	void Interact();
	void DrinkableInteract();
	void Fuck();

	// ─── Мопед — перенаправление ввода ───
	void InputMopedThrottle(float Value);
	void InputMopedBrake(float Value);
	void InputMopedSteer(float Value);
	void InputMopedClutchPress();
	void InputMopedClutchRelease();
	void InputMopedHandbrakePress();
	void InputMopedHandbrakeRelease();
	void InputMopedGearUp();
	void InputMopedGearDown();
	void InputMopedStarterPress();
	void InputMopedStarterRelease();
	
	FTimerHandle ThirstKillHandle;
	FTimerHandle HungerKillHandle;
	FTimerHandle RespawnTimerHandle;
	
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
	
	UPROPERTY(EditAnywhere, Category="Some")
	int Cigarettes;
	
	UPROPERTY(EditAnywhere, Category="References")
	TSubclassOf<AActor> WeatherController;
	
	UPROPERTY(EditAnywhere, Category="References")
	TSubclassOf<AActor> DoorClass;

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
	
	UFUNCTION(BlueprintCallable, Category="Some")
	void SetCigarettes(int ToSet, bool bAdd);
	
	UFUNCTION(BlueprintCallable, Category="Needs")
	void KillPlayer(EKillType KillType);
	
	UFUNCTION(BlueprintCallable, Category="Spawning")
	void RespawnPlayer(EKillType KillType);
	
	UFUNCTION(BlueprintCallable, Category="Alcohol")
	void ApplyAlcoholSway(float DeltaTime);
	
	UFUNCTION(BlueprintCallable, Category="Alcohol")
	void SoberUp(float DeltaTime);
	
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
	void ChangedCigarettes();
	void RespawnedPlayer();
};