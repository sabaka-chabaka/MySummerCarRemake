// 2026 sabaka-chabaka

#include "MySummerCarRemake/Public/WeatherController.h"
#include "Components/VolumetricCloudComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeather, All, All);

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

	// 1 game hour = 5 real minutes (300 real seconds)
	// 3600 game seconds = 300 real seconds
	// 1 real second = 12 game seconds
	float GameSecondsToAdd = DeltaTime * 12.0f;
	SetSeconds(GameSecondsToAdd, true);
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

void AWeatherController::SetHours(float NewHours, bool bAdd)
{
	if (bAdd)
	{
		Hours += NewHours;
		ChangedHours();
		return;
	}
	Hours = NewHours;
	ChangedHours();
}

void AWeatherController::SetMinutes(float NewMinutes, bool bAdd)
{
	if (bAdd)
	{
		Minutes += NewMinutes;
		ChangedMinutes();
		return;
	}
	Minutes = NewMinutes;
	ChangedMinutes();
}

void AWeatherController::SetSeconds(float NewSeconds, bool bAdd)
{
	if (bAdd)
	{
		Seconds += NewSeconds;
		ChangedSeconds();
		return;
	}
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
		FogComponent->SetVisibility(true);
		FogComponent->SetFogDensity(0.05f);
		break;
	case Cloudy:
		CloudyComponent->SetVisibility(true);
		FogComponent->SetVisibility(true);
		FogComponent->SetFogDensity(0.05f);
		break;
	case Rain:
		CloudyComponent->SetVisibility(true);
		FogComponent->SetVisibility(true);
		FogComponent->SetFogDensity(1.0f);
		break;
	case Fog:
		CloudyComponent->SetVisibility(false);
		FogComponent->SetVisibility(true);
		FogComponent->SetFogDensity(1.0f);
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
	case Monday: DayName = "Понедельник"; break;
	case Tuesday: DayName = "Вторник"; break;
	case Wednesday: DayName = "Среда"; break;
	case Thursday: DayName = "Четверг"; break;
	case Friday: DayName = "Пятница"; break;
	case Saturday: DayName = "Суббота"; break;
	case Sunday: DayName = "Воскресенье"; break;
	}
	UE_LOG(LogWeather, Log, TEXT("Today is %s"), *DayName);
}

void AWeatherController::ChangedHours()
{
	UpdateSunRotation();
	WeatherState = static_cast<EWeatherState>(UKismetMathLibrary::RandomIntegerInRange(0, 3));
	if (Hours >= 24)
	{
		Hours = 0;
		SetWeekDay(static_cast<EWeekDay>((static_cast<uint8>(WeekDay.GetValue()) + 1) % 7));
	}
}

void AWeatherController::ChangedMinutes()
{
	UpdateSunRotation();
	if (Minutes >= 60)
	{
		Minutes = 0;
		SetHours(1, true);
	}
}

void AWeatherController::ChangedSeconds()
{
	UpdateSunRotation();
	if (Seconds >= 60)
	{
		Seconds = 0;
		SetMinutes(1, true);
	}
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
	// 0:00 -> Ratio 0.0 -> SunPitch 90 (Bottom)
	// 6:00 -> Ratio 0.25 -> SunPitch 180 or 0? 
	// 12:00 -> Ratio 0.5 -> SunPitch 270 or -90 (Top)
	// 18:00 -> Ratio 0.75 -> SunPitch 360 or 0?
	
	float DayRatio = TotalSeconds / (24.0f * 3600.0f);
	
	// Shift by 0.5 so that 12:00 is at the start of the mapping if we want
	// Or just: SunPitch = (DayRatio * 360.0f) + 90.0f;
	// 0:00 -> 90 (Down)
	// 12:00 -> 180 + 90 = 270 (which is -90, Up)
	
	float SunPitch = (DayRatio * 360.0f) + 90.0f;

	FRotator NewRotation = FRotator(SunPitch, 0.0f, 0.0f);
	SunLight->SetRelativeRotation(NewRotation);
}