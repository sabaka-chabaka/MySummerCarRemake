// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

UINTERFACE()
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class MYSUMMERCARREMAKE_API IInteractInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* Interactor);
};