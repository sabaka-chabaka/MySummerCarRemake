// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sugar.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API ASugar : public AActor
{
	GENERATED_BODY()

public:
	ASugar();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};