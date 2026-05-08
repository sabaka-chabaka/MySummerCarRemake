// 2026 sabaka-chabaka

#include "CarAssemblyComponent.h"

UCarAssemblyComponent::UCarAssemblyComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCarAssemblyComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCarAssemblyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}