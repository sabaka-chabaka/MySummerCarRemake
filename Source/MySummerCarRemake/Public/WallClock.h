// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallClock.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API AWallClock : public AActor
{
	GENERATED_BODY()

public:
	AWallClock();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
protected:
	UPROPERTY(EditAnywhere, Category="Mesh")
	class UStaticMeshComponent* ClockMesh;
	
	UPROPERTY(EditAnywhere, Category="Mesh")
	class UStaticMeshComponent* HourHand;
	
	UPROPERTY(EditAnywhere, Category="Mesh")
	class UStaticMeshComponent* MinuteHand;
};