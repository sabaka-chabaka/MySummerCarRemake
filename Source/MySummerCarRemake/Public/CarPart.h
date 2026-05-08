// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarPart.generated.h"

UENUM(BlueprintType)
enum class ECarPartType : uint8
{
	None			UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class ECarPartState : uint8
{
	Lying		UMETA(DisplayName = "Lying on ground"),
	Held		UMETA(DisplayName = "Held by player"),
	Installed	UMETA(DisplayName = "Installed"),
};

UCLASS()
class MYSUMMERCARREMAKE_API ACarPart : public AActor
{
	GENERATED_BODY()

public:
	ACarPart();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CarPart|Components")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CarPart")
	ECarPartType PartType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarPart")
	FString PartDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarPart")
	float PartMass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarPart")
	bool bRequiresBolts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarPart", meta = (EditCondition = "bRequiresBolts"))
	int32 BoltsRequired;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CarPart")
	ECarPartState PartState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CarPart")
	int32 BoltsTightened;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CarPart")
	class ACarBody* InstalledOn;

	UFUNCTION(BlueprintCallable, Category = "CarPart")
	void AttachToCar(USceneComponent* TheParentComponent, FName SocketName);
	
	UFUNCTION(BlueprintCallable, Category = "CarPart")
	void DetachFromCar();

	UFUNCTION(BlueprintCallable, Category = "CarPart")
	void OnPickedUp(AActor* ByActor);

	UFUNCTION(BlueprintCallable, Category = "CarPart")
	void OnDropped(FVector DropLocation);

	UFUNCTION(BlueprintCallable, Category = "CarPart")
	bool TightenBolt();

	UFUNCTION(BlueprintPure, Category = "CarPart")
	bool CanBeInstalled() const;

	UFUNCTION(BlueprintPure, Category = "CarPart")
	bool IsFullyBolted() const;

	void SetPhysicsSimulation(bool bSimulate);

private:
	UPROPERTY()
	AActor* HoldingActor;
};