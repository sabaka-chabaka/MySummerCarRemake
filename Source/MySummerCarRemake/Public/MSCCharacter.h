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
	
	UPROPERTY(VisibleAnywhere, Category="Crouching")
	ECrouchState CrouchState;
	
protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUp(float Value);
	void Crouch();
};
