// 2026 sabaka-chabaka

#include "CarPart.h"

ACarPart::ACarPart()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACarPart::BeginPlay()
{
	Super::BeginPlay();
}

void ACarPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}