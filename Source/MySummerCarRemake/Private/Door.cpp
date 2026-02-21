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
	TargetRot = ClosedRot;
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator CurrentRot = StaticMeshComponent->GetRelativeRotation();
	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 5.0f);
	StaticMeshComponent->SetRelativeRotation(NewRot);
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
	bOpened = true;
	TargetRot = OpenedRot;
}

void ADoor::Close()
{
	bOpened = false;
	TargetRot = ClosedRot;
}
