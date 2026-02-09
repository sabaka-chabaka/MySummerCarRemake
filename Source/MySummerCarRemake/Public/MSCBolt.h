// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MSCBolt.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API AMSCBolt : public AActor
{
	GENERATED_BODY()
	
public:	
	AMSCBolt();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bolt Settings", meta = (ClampMin = "5", ClampMax = "20"))
	int32 BoltSize; // 5 to 20 mm

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bolt Status")
	float Tightness; // 0.0 to 1.0

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bolt Settings")
	int32 MaxSteps;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bolt Status")
	int32 CurrentStep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bolt Settings")
	float BoltTravelDistance;

	UFUNCTION(BlueprintCallable, Category = "Bolt Logic")
	void SetTightness(float NewTightness);

	UFUNCTION(BlueprintCallable, Category = "Bolt Logic")
	void AdjustStep(int32 Delta);

	UFUNCTION(BlueprintPure, Category = "Bolt Logic")
	bool IsFullyTightened() const { return CurrentStep >= MaxSteps; }

	UFUNCTION(BlueprintPure, Category = "Bolt Logic")
	bool IsLoose() const { return CurrentStep <= 0; }

private:
	void UpdateVisualPosition();
	
	FVector InitialLocalLocation;
};
