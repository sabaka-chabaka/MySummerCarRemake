// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "WeatherController.h"
#include "GameFramework/SaveGame.h"
#include "MSCSaveGameComponent.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API UMSCSaveGameComponent : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	FVector PlayerLocation;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	FRotator PlayerRotation;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Thirst;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Hunger;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Stress;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Urine;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Fatigue;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Dirtiness;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Money;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Alcohol;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Weight;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Cigarettes;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	TEnumAsByte<EWeatherState> WeatherState;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	TEnumAsByte<EWeekDay> WeekDay;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Hours;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Minutes;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	float Seconds;
	
	UPROPERTY(EditAnywhere, Category = "Doors")
	TMap<int, bool> Doors;
};
