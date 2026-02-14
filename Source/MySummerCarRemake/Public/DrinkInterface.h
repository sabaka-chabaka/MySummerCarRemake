// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DrinkInterface.generated.h"

UINTERFACE()
class UDrinkInterface : public UInterface
{
	GENERATED_BODY()
};

class MYSUMMERCARREMAKE_API IDrinkInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drink")
	void Drink(AActor* Interactor);
};
