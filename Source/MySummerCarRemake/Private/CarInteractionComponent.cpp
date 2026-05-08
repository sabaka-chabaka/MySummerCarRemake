// 2026 sabaka-chabaka

#include "CarInteractionComponent.h"
#include "CarBody.h"
#include "CarPart.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogCarInteract, All, All);

UCarInteractionComponent::UCarInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	InteractDistance = 250.f;
	HoldOffset       = FVector(120.f, 0.f, -20.f);
	DropDistance     = 150.f;
	HeldPart         = nullptr;
	OwnerCamera      = nullptr;
}

void UCarInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		OwnerCamera = Owner->FindComponentByClass<UCameraComponent>();
	}

	if (!OwnerCamera)
	{
		UE_LOG(LogCarInteract, Warning,
			TEXT("CarInteractionComponent: no UCameraComponent on owner '%s'"),
			*GetNameSafe(GetOwner()));
	}
}

void UCarInteractionComponent::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HeldPart)
	{
		UpdateHeldPartPosition(DeltaTime);
	}
}

void UCarInteractionComponent::Interact()
{
	FHitResult Hit;
	if (!DoLineTrace(Hit))
	{
		return;
	}

	AActor* HitActor = Hit.GetActor();

	if (!HeldPart)
	{
		if (ACarPart* Part = Cast<ACarPart>(HitActor))
		{
			if (Part->PartState == ECarPartState::Lying)
			{
				PickupPart(Part);
				return;
			}

			if (Part->PartState == ECarPartState::Installed)
			{
				const bool bDone = Part->TightenBolt();
				UE_LOG(LogCarInteract, Log,
					TEXT("Bolt tightened on '%s'. Done=%d"), *Part->PartDisplayName, bDone);
				return;
			}
		}
		return;
	}

	if (ACarBody* CarBody = Cast<ACarBody>(HitActor))
	{
		TryInstall(CarBody);
		return;
	}

	if (ACarPart* HitPart = Cast<ACarPart>(HitActor))
	{
		if (HitPart->InstalledOn)
		{
			TryInstall(HitPart->InstalledOn);
			return;
		}
	}
}

void UCarInteractionComponent::DropHeldPart()
{
	if (!HeldPart)
	{
		return;
	}

	FVector DropLoc = FVector::ZeroVector;
	if (OwnerCamera)
	{
		DropLoc = OwnerCamera->GetComponentLocation()
		        + OwnerCamera->GetForwardVector() * DropDistance;
	}
	else if (AActor* Owner = GetOwner())
	{
		DropLoc = Owner->GetActorLocation() + Owner->GetActorForwardVector() * DropDistance;
	}

	HeldPart->OnDropped(DropLoc);
	HeldPart = nullptr;

	UE_LOG(LogCarInteract, Log, TEXT("Part dropped"));
}

bool UCarInteractionComponent::DoLineTrace(FHitResult& OutHit) const
{
	if (!OwnerCamera)
	{
		return false;
	}

	const FVector Start = OwnerCamera->GetComponentLocation();
	const FVector End   = Start + OwnerCamera->GetForwardVector() * InteractDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	if (HeldPart)
	{
		Params.AddIgnoredActor(HeldPart);
	}

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		OutHit, Start, End, ECC_Visibility, Params);

#if WITH_EDITOR
	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.1f, 0, 1.f);
#endif

	return bHit;
}

void UCarInteractionComponent::PickupPart(ACarPart* Part)
{
	HeldPart = Part;
	Part->OnPickedUp(GetOwner());

	UE_LOG(LogCarInteract, Log, TEXT("Picked up '%s'"), *Part->PartDisplayName);
}

void UCarInteractionComponent::TryInstall(ACarBody* CarBody)
{
	if (!CarBody || !HeldPart)
	{
		return;
	}

	const bool bOk = CarBody->TryInstallPart(HeldPart);
	if (bOk)
	{
		UE_LOG(LogCarInteract, Log,
			TEXT("Installed '%s' into car '%s'"), *HeldPart->PartDisplayName, *CarBody->GetName());
		HeldPart = nullptr;
	}
	else
	{
		UE_LOG(LogCarInteract, Warning,
			TEXT("Failed to install '%s'"), *HeldPart->PartDisplayName);
	}
}

void UCarInteractionComponent::UpdateHeldPartPosition(float DeltaTime) const
{
	if (!HeldPart || !OwnerCamera)
	{
		return;
	}

	const FTransform CamTransform = OwnerCamera->GetComponentTransform();
	const FVector TargetLocation  = CamTransform.TransformPosition(HoldOffset);

	const FVector Current = HeldPart->GetActorLocation();
	const FVector NewLoc  = FMath::VInterpTo(Current, TargetLocation, DeltaTime, 12.f);

	HeldPart->SetActorLocation(NewLoc, false, nullptr, ETeleportType::TeleportPhysics);
}