// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Yeast.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API AYeast : public AActor
{
	GENERATED_BODY()

public:
	AYeast();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
