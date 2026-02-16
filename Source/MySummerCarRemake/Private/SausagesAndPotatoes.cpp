// 2026 sabaka-chabaka

#include "SausagesAndPotatoes.h"

#include "MSCCharacter.h"

ASausagesAndPotatoes::ASausagesAndPotatoes()
{
	PrimaryActorTick.bCanEverTick = true;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeginMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetSimulatePhysics(true);
	AfterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AfterMesh"));
	AfterMesh->SetupAttachment(RootComponent);
	AfterMesh->SetSimulatePhysics(true);
	AfterMesh->SetVisibility(false);
}

void ASausagesAndPotatoes::BeginPlay()
{
	Super::BeginPlay();
}

void ASausagesAndPotatoes::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASausagesAndPotatoes::Hunger_Implementation(AActor* Interactor)
{
	IEatInterface::Hunger_Implementation(Interactor);
	if (AMSCCharacter* Character = Cast<AMSCCharacter>(Interactor))
	{
		Character->SetThirst(20, true);
		Character->SetHunger(0, false);
		StaticMesh->SetVisibility(false);
		AfterMesh->SetVisibility(true);
	}
}
