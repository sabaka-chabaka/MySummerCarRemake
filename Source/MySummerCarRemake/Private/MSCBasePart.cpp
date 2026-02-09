// 2026 sabaka-chabaka

#include "MSCBasePart.h"
#include "MSCBolt.h"
#include "MSCCarBody.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"

AMSCBasePart::AMSCBasePart()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->SetSimulatePhysics(true);
	
	bIsAttached = false;
	AttachedToActor = nullptr;
}

void AMSCBasePart::BeginPlay()
{
	Super::BeginPlay();
}

void AMSCBasePart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMSCBasePart::Interact_Implementation(AActor* Interactor)
{
	if (bIsAttached && !IsSecurelyBolted())
	{
		AMSCCarBody* Car = Cast<AMSCCarBody>(AttachedToActor);
		if (Car)
		{
			Car->RemovePart(this);
		}
		else
		{
			DetachFromCar();
		}
	}
}

bool AMSCBasePart::IsSecurelyBolted() const
{
	for (AMSCBolt* Bolt : AttachedBolts)
	{
		if (Bolt && Bolt->Tightness > 0.1f)
		{
			return true;
		}
	}
	return false;
}

void AMSCBasePart::RegisterBolt(AMSCBolt* Bolt)
{
	if (Bolt)
	{
		AttachedBolts.AddUnique(Bolt);
	}
}

void AMSCBasePart::UnregisterBolt(AMSCBolt* Bolt)
{
	if (Bolt)
	{
		AttachedBolts.Remove(Bolt);
	}
}

void AMSCBasePart::AttachToCar(AActor* CarActor, FName SocketName)
{
	if (!CarActor) return;

	bIsAttached = true;
	AttachedToActor = CarActor;
	
	MeshComponent->SetSimulatePhysics(false);
	AttachToComponent(CarActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	
	SetActorEnableCollision(true); 
	OnAttached();
}

void AMSCBasePart::DetachFromCar()
{
	bIsAttached = false;
	AttachedToActor = nullptr;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	MeshComponent->SetSimulatePhysics(true);
	OnDetached();
}
