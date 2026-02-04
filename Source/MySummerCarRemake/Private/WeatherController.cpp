// 2026 sabaka-chabaka

#include "MySummerCarRemake/Public/WeatherController.h"
#include "Components/VolumetricCloudComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/DirectionalLightComponent.h"

AWeatherController::AWeatherController()
{
	PrimaryActorTick.bCanEverTick = true;
	WeatherState = Clear;
	WeekDay = Monday;
	Hours = 0;
	Minutes = 0;
	Seconds = 0;
}

void AWeatherController::BeginPlay()
{
	Super::BeginPlay();
	ChangedWeatherState();
	ChangedWeekDay();
	UpdateSunRotation();
}

void AWeatherController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeatherController::SetWeatherState(EWeatherState NewWeatherState)
{
	WeatherState = NewWeatherState;
	ChangedWeatherState();
}

void AWeatherController::SetWeekDay(EWeekDay NewWeekDay)
{
	WeekDay = NewWeekDay;
	ChangedWeekDay();
}

void AWeatherController::SetHours(float NewHours)
{
	Hours = NewHours;
	ChangedHours();
}

void AWeatherController::SetMinutes(float NewMinutes)
{
	Minutes = NewMinutes;
	ChangedMinutes();
}

void AWeatherController::SetSeconds(float NewSeconds)
{
	Seconds = NewSeconds;
	ChangedSeconds();
}

void AWeatherController::ChangedWeatherState()
{
	if (!CloudyComponent || !FogComponent)
	{
		return;
	}

	switch (WeatherState)
	{
	case Clear:
		CloudyComponent->SetVisibility(false);
		FogComponent->SetVisibility(false);
		break;
	case Cloudy:
		CloudyComponent->SetVisibility(true);
		FogComponent->SetVisibility(false);
		break;
	case Rain:
		CloudyComponent->SetVisibility(true);
		FogComponent->SetVisibility(true);
		FogComponent->SetFogDensity(0.02f);
		break;
	case Fog:
		CloudyComponent->SetVisibility(false);
		FogComponent->SetVisibility(true);
		FogComponent->SetFogDensity(0.05f);
		break;
	default:
		break;
	}
}

void AWeatherController::ChangedWeekDay()
{
	FString DayName;
	switch (WeekDay)
	{
	case Monday: DayName = "Monday"; break;
	case Tuesday: DayName = "Tuesday"; break;
	case Wednesday: DayName = "Wednesday"; break;
	case Thursday: DayName = "Thursday"; break;
	case Friday: DayName = "Friday"; break;
	case Saturday: DayName = "Saturday"; break;
	case Sunday: DayName = "Sunday"; break;
	}
	UE_LOG(LogTemp, Log, TEXT("Today is %s"), *DayName);
}

void AWeatherController::ChangedHours()
{
	UpdateSunRotation();
}

void AWeatherController::ChangedMinutes()
{
	UpdateSunRotation();
}

void AWeatherController::ChangedSeconds()
{
	UpdateSunRotation();
}

void AWeatherController::UpdateSunRotation()
{
	if (!SunLight)
	{
		return;
	}

	// Calculate total time in seconds (max 24 * 3600)
	float TotalSeconds = (Hours * 3600.0f) + (Minutes * 60.0f) + Seconds;
	
	// Map 0-24h to 0-360 degrees. 
	// At 12:00 (midday), sun should be at the top (usually Pitch = -90).
	// At 0:00, sun should be at the bottom (Pitch = 90).
	// Pitch: -90 (12:00), 0 (18:00), 90 (0:00), 270 (6:00)
	// Simple mapping: Rotation = (Time/MaxTime) * 360 - 90
	
	float DayRatio = TotalSeconds / (24.0f * 3600.0f);
	float SunPitch = (DayRatio * 360.0f) - 90.0f;

	FRotator NewRotation = FRotator(SunPitch, 0.0f, 0.0f);
	SunLight->SetRelativeRotation(NewRotation);
}