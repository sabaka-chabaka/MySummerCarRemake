// 2026 sabaka-chabaka

#include "Beer.h"

#include "MSCCharacter.h"

ABeer::ABeer()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeerMesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetSimulatePhysics(true);
}

void ABeer::BeginPlay()
{
	Super::BeginPlay();
}

void ABeer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABeer::Drink_Implementation(AActor* Interactor)
{
	if (AMSCCharacter* Character = Cast<AMSCCharacter>(Interactor))
	{
		Character->SetAlcohol(0.1, true);
		Character->SetThirst(-40, true);
		this->Destroy();
	}
}
