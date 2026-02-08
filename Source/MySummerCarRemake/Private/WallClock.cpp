// 2026 sabaka-chabaka

#include "WallClock.h"

#include "MSCCharacter.h"
#include "WeatherController.h"
#include "Kismet/GameplayStatics.h"

AWallClock::AWallClock()
{
	PrimaryActorTick.bCanEverTick = true;
	ClockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClockMesh"));
	ClockMesh->SetupAttachment(RootComponent);
	HourHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HourHand"));
	HourHand->SetupAttachment(ClockMesh);
	MinuteHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MinuteHand"));
	MinuteHand->SetupAttachment(ClockMesh);
}

void AWallClock::BeginPlay()
{
	Super::BeginPlay();
}

void AWallClock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HourHand && MinuteHand)
	{
		if (AMSCCharacter* PlayerCharacter = Cast<AMSCCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			if (AWeatherController* WeatherController = Cast<AWeatherController>(UGameplayStatics::GetActorOfClass(GetWorld(), PlayerCharacter->WeatherController)))
			{
				float TotalSeconds = (WeatherController->Hours * 3600.0f) + (WeatherController->Minutes * 60.0f) + WeatherController->Seconds;

				float HourRatio = fmodf(TotalSeconds, 12.0f * 3600.0f) / (12.0f * 3600.0f);
				float MinuteRatio = fmodf(TotalSeconds, 3600.0f) / 3600.0f;
			
				float HourRotation = (HourRatio * 360.0f) + 90.0f;
				float MinuteRotation = (MinuteRatio * 360.0f) + 90.0f;

				HourHand->SetRelativeRotation(FRotator(0.0f, 0.0f, HourRotation));
				MinuteHand->SetRelativeRotation(FRotator(0.0f, 0.0f, MinuteRotation));
			}
		}
	}
}
