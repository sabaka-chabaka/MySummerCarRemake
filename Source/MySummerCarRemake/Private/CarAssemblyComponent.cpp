// 2026 sabaka-chabaka

#include "CarAssemblyComponent.h"
#include "CarBody.h"
#include "CarPart.h"

DEFINE_LOG_CATEGORY_STATIC(LogCarAssembly, All, All);

UCarAssemblyComponent::UCarAssemblyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAssembledEventFired = false;
}

void UCarAssemblyComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UCarAssemblyComponent::TryInstallPart(ACarPart* Part)
{
	if (!Part)
	{
		UE_LOG(LogCarAssembly, Warning, TEXT("TryInstallPart: null part"));
		return false;
	}

	if (!Part->CanBeInstalled())
	{
		UE_LOG(LogCarAssembly, Warning,
			TEXT("TryInstallPart: '%s' not in Held state (state=%d)"),
			*Part->PartDisplayName, (int32)Part->PartState);
		return false;
	}

	FCarPartSlot* Slot = FindSlot(Part->PartType);
	if (!Slot)
	{
		UE_LOG(LogCarAssembly, Warning,
			TEXT("TryInstallPart: no slot for part type %d"), (int32)Part->PartType);
		return false;
	}

	if (Slot->InstalledPart)
	{
		UE_LOG(LogCarAssembly, Warning,
			TEXT("TryInstallPart: slot '%s' already occupied"), *Slot->SlotDisplayName);
		return false;
	}

	ACarBody* CarBody = Cast<ACarBody>(GetOwner());
	if (!CarBody || !CarBody->BodyMesh)
	{
		UE_LOG(LogCarAssembly, Error, TEXT("TryInstallPart: owner is not ACarBody!"));
		return false;
	}

	Part->InstalledOn = CarBody;
	Part->AttachToCar(CarBody->BodyMesh, Slot->SocketName);

	Slot->InstalledPart = Part;

	UE_LOG(LogCarAssembly, Log,
		TEXT("Installed '%s' into slot '%s'"), *Part->PartDisplayName, *Slot->SlotDisplayName);

	OnPartInstalled.Broadcast(Part->PartType, Part);
	CheckFullyAssembled();

	return true;
}

ACarPart* UCarAssemblyComponent::UninstallPart(ECarPartType PartType)
{
	FCarPartSlot* Slot = FindSlot(PartType);
	if (!Slot || !Slot->InstalledPart)
	{
		return nullptr;
	}

	ACarPart* Part = Slot->InstalledPart;
	Slot->InstalledPart = nullptr;

	Part->DetachFromCar();
	Part->InstalledOn = nullptr;

	UE_LOG(LogCarAssembly, Log, TEXT("Uninstalled '%s'"), *Part->PartDisplayName);

	OnPartUninstalled.Broadcast(PartType, Part);
	bAssembledEventFired = false;

	return Part;
}

ACarPart* UCarAssemblyComponent::GetInstalledPart(ECarPartType PartType) const
{
	const FCarPartSlot* Slot = FindSlot(PartType);
	return Slot ? Slot->InstalledPart : nullptr;
}

bool UCarAssemblyComponent::IsPartInstalled(ECarPartType PartType) const
{
	const FCarPartSlot* Slot = FindSlot(PartType);
	return Slot && Slot->InstalledPart != nullptr;
}

bool UCarAssemblyComponent::CanStartEngine() const
{
	for (const FCarPartSlot& Slot : PartSlots)
	{
		if (!Slot.bRequiredForEngine)
		{
			continue;
		}
		if (!Slot.InstalledPart)
		{
			return false;
		}
		if (Slot.InstalledPart->bRequiresBolts && !Slot.InstalledPart->IsFullyBolted())
		{
			return false;
		}
	}
	return true;
}

int32 UCarAssemblyComponent::GetMissingRequiredPartsCount() const
{
	int32 Count = 0;
	for (const FCarPartSlot& Slot : PartSlots)
	{
		if (Slot.bRequiredForEngine && !Slot.InstalledPart)
		{
			Count++;
		}
	}
	return Count;
}

TArray<FString> UCarAssemblyComponent::GetMissingPartNames() const
{
	TArray<FString> Names;
	for (const FCarPartSlot& Slot : PartSlots)
	{
		if (!Slot.InstalledPart)
		{
			Names.Add(Slot.SlotDisplayName);
		}
	}
	return Names;
}

bool UCarAssemblyComponent::IsFullyAssembled() const
{
	for (const FCarPartSlot& Slot : PartSlots)
	{
		if (!Slot.InstalledPart)
		{
			return false;
		}
		if (Slot.InstalledPart->bRequiresBolts && !Slot.InstalledPart->IsFullyBolted())
		{
			return false;
		}
	}
	return true;
}

float UCarAssemblyComponent::GetAssemblyProgress() const
{
	if (PartSlots.IsEmpty())
	{
		return 0.f;
	}

	float Total = 0.f;
	float Max   = 0.f;

	for (const FCarPartSlot& Slot : PartSlots)
	{
		Max += 1.f;
		if (!Slot.InstalledPart)
		{
			continue;
		}

		if (!Slot.InstalledPart->bRequiresBolts)
		{
			Total += 1.f;
		}
		else
		{
			const float BoltRatio = FMath::Clamp(
				(float)Slot.InstalledPart->BoltsTightened / FMath::Max(1, Slot.InstalledPart->BoltsRequired),
				0.f, 1.f);
			Total += 0.5f + 0.5f * BoltRatio;
		}
	}

	return Total / Max;
}

FCarPartSlot* UCarAssemblyComponent::FindSlot(ECarPartType PartType)
{
	for (FCarPartSlot& Slot : PartSlots)
	{
		if (Slot.PartType == PartType)
		{
			return &Slot;
		}
	}
	return nullptr;
}

const FCarPartSlot* UCarAssemblyComponent::FindSlot(ECarPartType PartType) const
{
	for (const FCarPartSlot& Slot : PartSlots)
	{
		if (Slot.PartType == PartType)
		{
			return &Slot;
		}
	}
	return nullptr;
}

void UCarAssemblyComponent::CheckFullyAssembled()
{
	if (bAssembledEventFired)
	{
		return;
	}
	if (IsFullyAssembled())
	{
		bAssembledEventFired = true;
		UE_LOG(LogCarAssembly, Log, TEXT("Car fully assembled!"));
		OnCarFullyAssembled.Broadcast();
	}
}