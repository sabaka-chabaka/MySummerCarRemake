// 2026 sabaka-chabaka

#include "SaveGameToilet.h"
#include "Kismet/GameplayStatics.h"
#include "MSCSaveGameComponent.h"
#include "MSCCharacter.h"
#include "WeatherController.h"
#include "Components/SphereComponent.h"

ASaveGameToilet::ASaveGameToilet()
{
	PrimaryActorTick.bCanEverTick = true;
	ToiletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	ToiletMesh->SetupAttachment(RootComponent);
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
}

void ASaveGameToilet::BeginPlay()
{
	Super::BeginPlay();
}

void ASaveGameToilet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASaveGameToilet::Save()
{
	if (AMSCCharacter* PlayerCharacter = Cast<AMSCCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (UMSCSaveGameComponent* SaveGameInstance = Cast<UMSCSaveGameComponent>(UGameplayStatics::CreateSaveGameObject(UMSCSaveGameComponent::StaticClass())))
		{
			SaveGameInstance->PlayerLocation = PlayerCharacter->GetActorLocation();
			SaveGameInstance->PlayerRotation = PlayerCharacter->GetActorRotation();
			SaveGameInstance->Thirst = PlayerCharacter->Thirst;
			SaveGameInstance->Hunger = PlayerCharacter->Hunger;
			SaveGameInstance->Stress = PlayerCharacter->Stress;
			SaveGameInstance->Urine = PlayerCharacter->Urine;
			SaveGameInstance->Fatigue = PlayerCharacter->Fatigue;
			SaveGameInstance->Dirtiness = PlayerCharacter->Dirtiness;
			SaveGameInstance->Money = PlayerCharacter->Money;
			SaveGameInstance->Alcohol = PlayerCharacter->Alcohol;
			SaveGameInstance->Weight = PlayerCharacter->Weight;
			if (AWeatherController* WeatherController = Cast<AWeatherController>(UGameplayStatics::GetActorOfClass(GetWorld(), PlayerCharacter->WeatherController)))
			{
				SaveGameInstance->WeekDay = WeatherController->WeekDay;
				SaveGameInstance->WeatherState = WeatherController->WeatherState;
				SaveGameInstance->Hours = WeatherController->Hours;
				SaveGameInstance->Minutes = WeatherController->Minutes;
				SaveGameInstance->Seconds = WeatherController->Seconds;
			}

			if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("ManualSave"), 0))
			{
				UE_LOG(LogTemp, Warning, TEXT("Game Saved successfully!"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Game Save failed!"));
			}
		}
	}
}

void ASaveGameToilet::Interact_Implementation(AActor* Interactor)
{
	Save();
}