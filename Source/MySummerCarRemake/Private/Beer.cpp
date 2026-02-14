// 2026 sabaka-chabaka

#include "Beer.h"

#include "MSCCharacter.h"

ABeer::ABeer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABeer::BeginPlay()
{
	Super::BeginPlay();
}

void ABeer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABeer::Interact_Implementation(AActor* Interactor)
{
	IInteractInterface::Interact_Implementation(Interactor);
	Drink(Interactor);
}

void ABeer::Drink(AActor* Interactor)
{
	if (AMSCCharacter* Character = Cast<AMSCCharacter>(Interactor))
	{
		Character->SetAlcohol(0.2, true);
		this->Destroy();
	}
}
