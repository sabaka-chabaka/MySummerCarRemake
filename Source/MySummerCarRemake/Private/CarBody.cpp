// 2026 sabaka-chabaka

#include "CarBody.h"

ACarBody::ACarBody()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACarBody::BeginPlay()
{
	Super::BeginPlay();
}

void ACarBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}