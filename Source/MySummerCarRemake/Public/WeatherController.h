// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeatherController.generated.h"

UENUM(BlueprintType)
enum EWeatherState : uint8
{
	Clear,
	Cloudy,
	Rain,
	Fog
};

UENUM(BlueprintType)
enum EWeekDay : uint8
{
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday
};

UCLASS()
class MYSUMMERCARREMAKE_API AWeatherController : public AActor
{
	GENERATED_BODY()

public:
	AWeatherController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category="Weather")
	TEnumAsByte<EWeatherState> WeatherState;
	
	UPROPERTY(EditAnywhere, Category="Weather")
	TEnumAsByte<EWeekDay> WeekDay;
	
	UPROPERTY(EditAnywhere, Category="Weather")
	float Hours;
	
	UPROPERTY(EditAnywhere, Category="Weather")
	float Minutes;
	
	UPROPERTY(EditAnywhere, Category="Weather")
	float Seconds;
	
	UFUNCTION(BlueprintCallable, Category="WeatherController")
	void SetWeatherState(EWeatherState NewWeatherState);
	
	UFUNCTION(BlueprintCallable, Category="WeatherController")
	void SetWeekDay(EWeekDay NewWeekDay);
	
	UFUNCTION(BlueprintCallable, Category="WeatherController")
	void SetHours(float NewHours);
	
	UFUNCTION(BlueprintCallable, Category="WeatherController")
	void SetMinutes(float NewMinutes);
	
	UFUNCTION(BlueprintCallable, Category="WeatherController")
	void SetSeconds(float NewSeconds);
	
	UFUNCTION(BlueprintCallable, Category="WeatherController")
	void ChangedWeatherState();
	
	UFUNCTION(BlueprintCallable, Category="WeatherController")
	void ChangedWeekDay();
	
	UFUNCTION(BlueprintCallable, Category="WeatherController")
	void ChangedHours();
	
	UFUNCTION(BlueprintCallable, Category="WeatherController")
	void ChangedMinutes();
	
	UFUNCTION(BlueprintCallable, Category="WeatherController")
	void ChangedSeconds();
	
	UFUNCTION(BlueprintCallable, Category="WeatherController")
	void UpdateSunRotation();
	
protected:
	UPROPERTY(EditAnywhere, Category="Weather")
	class UVolumetricCloudComponent* CloudyComponent;
	
	UPROPERTY(EditAnywhere, Category="Weather")
	class UExponentialHeightFogComponent* FogComponent;

	UPROPERTY(EditAnywhere, Category="Weather")
	class UDirectionalLightComponent* SunLight;
};