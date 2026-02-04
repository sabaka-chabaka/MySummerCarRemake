// 2026 sabaka-chabaka

#include "MySummerCarRemake/Public/WeatherController.h"
#include "Components/VolumetricCloudComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/KismetMathLibrary.h"

AWeatherController::AWeatherController()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	CloudyComponent = CreateDefaultSubobject<UVolumetricCloudComponent>(TEXT("CloudyComponent"));
	CloudyComponent->SetupAttachment(Root);

	FogComponent = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("FogComponent"));
	FogComponent->SetupAttachment(Root);

	SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
	SunLight->SetupAttachment(Root);

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
	case Monday: DayName = "Понедельник"; break;
	case Tuesday: DayName = "Вторник"; break;
	case Wednesday: DayName = "Среда"; break;
	case Thursday: DayName = "Четверг"; break;
	case Friday: DayName = "Пятница"; break;
	case Saturday: DayName = "Суббота"; break;
	case Sunday: DayName = "Воскресенье"; break;
	}
	UE_LOG(LogTemp, Log, TEXT("Today is %s"), *DayName);
}

void AWeatherController::ChangedHours()
{
	UpdateSunRotation();
	WeatherState = static_cast<EWeatherState>(UKismetMathLibrary::RandomIntegerInRange(0, 3));
	if (Hours >= 24)
	{
		Hours = 0;
		switch (WeekDay)
		{
		default:
			ChangedWeekDay();
			break;
		case Sunday:
			WeekDay = Monday;
			ChangedWeekDay();
			break;
		}
		
	}
}

void AWeatherController::ChangedMinutes()
{
	UpdateSunRotation();
	if (Minutes >= 60)
	{
		Minutes = 0;
		SetHours(Hours + 1);
	}
}

void AWeatherController::ChangedSeconds()
{
	UpdateSunRotation();
	if (Seconds >= 60)
	{
		Seconds = 0;
		SetMinutes(Minutes + 1);
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
	// At 12:00 (midday), sun should be at the top (usually Pitch = -90).
	// At 0:00, sun should be at the bottom (Pitch = 90).
	// Pitch: -90 (12:00), 0 (18:00), 90 (0:00), 270 (6:00)
	// Simple mapping: Rotation = (Time/MaxTime) * 360 - 90
	
	float DayRatio = TotalSeconds / (24.0f * 3600.0f);
	float SunPitch = (DayRatio * 360.0f) - 90.0f;

	FRotator NewRotation = FRotator(SunPitch, 0.0f, 0.0f);
	SunLight->SetRelativeRotation(NewRotation);
}