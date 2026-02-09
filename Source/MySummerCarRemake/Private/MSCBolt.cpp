// 2026 sabaka-chabaka

#include "MSCBolt.h"
#include "MSCBasePart.h"
#include "Components/StaticMeshComponent.h"

AMSCBolt::AMSCBolt()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	
	BoltSize = 10;
	Tightness = 0.0f;
	MaxSteps = 8;
	CurrentStep = 0;
	BoltTravelDistance = 2.0f;
}

void AMSCBolt::BeginPlay()
{
	Super::BeginPlay();
	InitialLocalLocation = GetRootComponent()->GetRelativeLocation();
	UpdateVisualPosition();

	AActor* Parent = GetAttachParentActor();
	if (Parent)
	{
		AMSCBasePart* ParentPart = Cast<AMSCBasePart>(Parent);
		if (ParentPart)
		{
			ParentPart->RegisterBolt(this);
		}
	}
}

void AMSCBolt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMSCBolt::SetTightness(float NewTightness)
{
	Tightness = FMath::Clamp(NewTightness, 0.0f, 1.0f);
	CurrentStep = FMath::RoundToInt(Tightness * MaxSteps);
	UpdateVisualPosition();
}

void AMSCBolt::AdjustStep(int32 Delta)
{
	CurrentStep = FMath::Clamp(CurrentStep + Delta, 0, MaxSteps);
	Tightness = (MaxSteps > 0) ? (float)CurrentStep / (float)MaxSteps : 1.0f;
	UpdateVisualPosition();
	
	if (Delta != 0)
	{
		AddActorLocalRotation(FRotator(0, Delta * (360.0f / (float)MaxSteps), 0));
	}
}

void AMSCBolt::UpdateVisualPosition()
{
	float Offset = (1.0f - Tightness) * BoltTravelDistance;
	SetActorRelativeLocation(InitialLocalLocation + FVector(0, 0, Offset), false, nullptr, ETeleportType::TeleportPhysics);
}

//TODO: GET HOW WORK ITS AND MAKE A SOME COMPONENTS AND A MOPED