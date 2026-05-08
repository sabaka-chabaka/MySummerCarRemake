// 2026 sabaka-chabaka

#include "CarPart.h"

DEFINE_LOG_CATEGORY_STATIC(LogCarPart, All, All);
 
ACarPart::ACarPart()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
 
	PartType          = ECarPartType::None;
	PartDisplayName   = TEXT("Car Part");
	PartMass          = 10.f;
	bRequiresBolts    = true;
	BoltsRequired     = 4;
	BoltsTightened    = 0;
	PartState         = ECarPartState::Lying;
	InstalledOn       = nullptr;
	HoldingActor      = nullptr;
}

void ACarPart::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetMassOverrideInKg(NAME_None, PartMass, true);
}

void ACarPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACarPart::SetPhysicsSimulation(bool bSimulate)
{
	if (Mesh)
	{
		Mesh->SetSimulatePhysics(bSimulate);
		Mesh->SetEnableGravity(bSimulate);
		if (bSimulate)
		{
			Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
		}
		else
		{
			Mesh->SetCollisionProfileName(TEXT("NoCollision"));
		}
	}
}

void ACarPart::AttachToCar(USceneComponent* ParentComponent, FName SocketName)
{
	if (!ParentComponent)
	{
		UE_LOG(LogCarPart, Warning, TEXT("AttachToCar: ParentComponent is null for %s"), *GetName());
		return;
	}
	
	SetPhysicsSimulation(false);
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	
	AttachToComponent(ParentComponent, Rules, SocketName);
	PartState = ECarPartState::Installed;
	
	UE_LOG(LogCarPart, Log, TEXT("Part '%s' attached to socket '%s'"), *PartDisplayName, *SocketName.ToString());
}

void ACarPart::DetachFromCar()
{
	if (PartState != ECarPartState::Installed) return;
	
	FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(Rules);
	
	SetPhysicsSimulation(true);
	
	BoltsTightened = 0;
	PartState = ECarPartState::Lying;
	InstalledOn = nullptr;
	
	UE_LOG(LogCarPart, Log, TEXT("Part '%s' detached from car"), *PartDisplayName);
}

void ACarPart::OnPickedUp(AActor* ByActor)
{
	if (PartState == ECarPartState::Installed) return;
	
	HoldingActor = ByActor;
	SetPhysicsSimulation(false);
	PartState = ECarPartState::Held;
	
	UE_LOG(LogCarPart, Log, TEXT("Part '%s' picked up"), *PartDisplayName);
}

void ACarPart::OnDropped(FVector DropLocation)
{
	HoldingActor = nullptr;
	SetActorLocation(DropLocation);
	SetPhysicsSimulation(true);
	PartState = ECarPartState::Lying;
 
	UE_LOG(LogCarPart, Log, TEXT("Part '%s' dropped at %s"), *PartDisplayName, *DropLocation.ToString());
}

bool ACarPart::TightenBolt()
{
	if (PartState != ECarPartState::Installed)
	{
		UE_LOG(LogCarPart, Warning, TEXT("TightenBolt: part '%s' is not installed!"), *PartDisplayName);
		return false;
	}
 
	if (IsFullyBolted())
	{
		return true;
	}
 
	BoltsTightened++;
	UE_LOG(LogCarPart, Log, TEXT("Part '%s': bolt %d/%d tightened"), *PartDisplayName, BoltsTightened, BoltsRequired);
	return IsFullyBolted();
}

bool ACarPart::CanBeInstalled() const
{
	return PartState == ECarPartState::Held;
}

bool ACarPart::IsFullyBolted() const
{
	return bRequiresBolts ? BoltsTightened >= BoltsRequired : true;
}