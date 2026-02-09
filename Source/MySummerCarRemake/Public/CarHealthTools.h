// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CarHealthTools.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API UCarHealthTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="CarHealthTools")
	static bool CanEngineStart(float CurrentRPM, float MinRunningRPM);
	
	UFUNCTION(BlueprintCallable, Category="CarHealthTools")
	static float GetMassEngineHealth();
};
