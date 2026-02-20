// 2026 sabaka-chabaka

#include "Door.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	bOpened = false;
	ID = 0;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoor::Interact_Implementation(AActor* Interactor)
{
	IInteractInterface::Interact_Implementation(Interactor);
	if (bOpened)
	{
		Close();
	}
	else
	{
		Open();
	}
}

void ADoor::Open()
{
	StaticMeshComponent->SetRelativeRotation(OpenedRot);
}

void ADoor::Close()
{
	StaticMeshComponent->SetRelativeRotation(ClosedRot);
}
