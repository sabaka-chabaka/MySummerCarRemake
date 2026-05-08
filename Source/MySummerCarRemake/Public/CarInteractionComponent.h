// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarPart.h"
#include "CarInteractionComponent.generated.h"

class ACarBody;
class ACarPart;

UCLASS(ClassGroup = (Car), meta = (BlueprintSpawnableComponent))
class MYSUMMERCARREMAKE_API UCarInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCarInteractionComponent();

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarInteraction")
	float InteractDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarInteraction")
	FVector HoldOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarInteraction")
	float DropDistance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CarInteraction")
	ACarPart* HeldPart;

	UFUNCTION(BlueprintCallable, Category = "CarInteraction")
	void Interact();

	UFUNCTION(BlueprintCallable, Category = "CarInteraction")
	void DropHeldPart();

	UFUNCTION(BlueprintPure, Category = "CarInteraction")
	bool IsHoldingPart() const { return HeldPart != nullptr; }

private:
	bool DoLineTrace(FHitResult& OutHit) const;

	void PickupPart(ACarPart* Part);

	void TryInstall(ACarBody* CarBody);

	void UpdateHeldPartPosition(float DeltaTime) const;

	UPROPERTY()
	class UCameraComponent* OwnerCamera;

	virtual void BeginPlay() override;
};