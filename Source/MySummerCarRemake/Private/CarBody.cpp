// 2026 sabaka-chabaka

#include "CarBody.h"
#include "CarPart.h"
#include "CarAssemblyComponent.h"
#include "Components/StaticMeshComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCarBody, All, All);

ACarBody::ACarBody()
{
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	RootComponent = BodyMesh;

	BodyMesh->SetSimulatePhysics(true);
	BodyMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	BodyMesh->SetEnableGravity(true);

	AssemblyComponent = CreateDefaultSubobject<UCarAssemblyComponent>(TEXT("AssemblyComponent"));

	BodyMass       = 600.f;
	LinearDamping  = 0.5f;
	AngularDamping = 1.0f;
}

void ACarBody::BeginPlay()
{
	Super::BeginPlay();

	BodyMesh->SetMassOverrideInKg(NAME_None, BodyMass, true);
	BodyMesh->SetLinearDamping(LinearDamping);
	BodyMesh->SetAngularDamping(AngularDamping);

	UE_LOG(LogCarBody, Log, TEXT("CarBody '%s' ready. Mass=%.0fkg"), *GetName(), BodyMass);
}

void ACarBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ACarBody::TryInstallPart(ACarPart* Part)
{
	if (!AssemblyComponent)
	{
		return false;
	}
	return AssemblyComponent->TryInstallPart(Part);
}

ACarPart* ACarBody::UninstallPart(ECarPartType PartType)
{
	if (!AssemblyComponent)
	{
		return nullptr;
	}
	return AssemblyComponent->UninstallPart(PartType);
}

bool ACarBody::IsPartInstalled(ECarPartType PartType) const
{
	if (!AssemblyComponent)
	{
		return false;
	}
	return AssemblyComponent->IsPartInstalled(PartType);
}

float ACarBody::GetAssemblyProgress() const
{
	if (!AssemblyComponent)
	{
		return 0.f;
	}
	return AssemblyComponent->GetAssemblyProgress();
}

bool ACarBody::CanStartEngine() const
{
	if (!AssemblyComponent)
	{
		return false;
	}
	return AssemblyComponent->CanStartEngine();
}