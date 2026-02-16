// 2026 sabaka-chabaka

#include "Pizza.h"

#include "MSCCharacter.h"

APizza::APizza()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PizzaMesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetSimulatePhysics(true);
}

void APizza::BeginPlay()
{
	Super::BeginPlay();
}

void APizza::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APizza::Hunger_Implementation(AActor* Interactor)
{
	if (AMSCCharacter* Character = Cast<AMSCCharacter>(Interactor))
	{
		Character->SetThirst(10, true);
		Character->SetHunger(-80, true);
		this->Destroy();
	}
}
