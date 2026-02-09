// 2026 sabaka-chabaka

#include "MSCCarBody.h"
#include "MSCBasePart.h"
#include "Components/StaticMeshComponent.h"

AMSCCarBody::AMSCCarBody()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

void AMSCCarBody::BeginPlay()
{
	Super::BeginPlay();
}

void AMSCCarBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AMSCCarBody::TryAttachPart(AMSCBasePart* Part)
{
	if (!Part) return false;

	for (FPartMountInfo& Info : MountPoints)
	{
		if (Info.PartID == Part->PartID && Info.CurrentPart == nullptr)
		{
			FVector SocketLocation = MeshComponent->GetSocketLocation(Info.SocketName);
			float Distance = FVector::Dist(Part->GetActorLocation(), SocketLocation);

			if (Distance < 50.0f) 
			{
				Part->AttachToCar(this, Info.SocketName);
				Info.CurrentPart = Part;
				OnPartAttached(Part);
				return true;
			}
		}
	}

	return false;
}

void AMSCCarBody::RemovePart(AMSCBasePart* Part)
{
	if (!Part) return;

	for (FPartMountInfo& Info : MountPoints)
	{
		if (Info.CurrentPart == Part)
		{
			Info.CurrentPart = nullptr;
			Part->DetachFromCar();
			OnPartRemoved(Part);
			break;
		}
	}
}
