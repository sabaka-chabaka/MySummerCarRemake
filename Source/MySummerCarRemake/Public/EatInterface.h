// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EatInterface.generated.h"

UINTERFACE()
class UEatInterface : public UInterface
{
	GENERATED_BODY()
};

class MYSUMMERCARREMAKE_API IEatInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Hunger")
	void Hunger(AActor* Interactor);
};