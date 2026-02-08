// 2026 sabaka-chabaka

#include "MySummerCarRemake/Public/MSCCharacter.h"

#include "InteractInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
DEFINE_LOG_CATEGORY_STATIC(LogMSCCharacter, All, All);

#include "Camera/CameraComponent.h"

AMSCCharacter::AMSCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	CrouchState = Stand;
	Thirst = 0;
	Hunger = 0;
	Stress = 0;
	Urine = 0;
	Fatigue = 0;
	Dirtiness = 0;
	Money = 3000;
	Alcohol = 0;
	Weight = 75;
	bPeeing = false;
	TargetRotation = FRotator::ZeroRotator;
}

void AMSCCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogMSCCharacter, Log, TEXT("Character loaded, welcome to My Summer Car!"));
	ChangedStress();
	ChangedAlcohol();
	ChangedDirtiness();
	ChangedFatigue();
	ChangedHunger();
	ChangedMoney();
	ChangedPlayerWeight();
	ChangedThirst();
	ChangedUrine();
}

void AMSCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Needs filling formulas based on real time
	
	// Thirst: 1.83% / 45 real seconds
	SetThirst(DeltaTime * (0.34f / 45.0f), true);
	
	// Hunger: 1.25% / 45 real seconds
	SetHunger(DeltaTime * (0.2f / 45.0f), true);
	
	// Stress: 1% / 45 real seconds
	SetStress(DeltaTime * (0.50f / 45.0f), true);
	
	// Urine: 1.56% / 45 real seconds
	SetUrine(DeltaTime * (0.56f / 45.0f), true);
	
	// Fatigue: 0.56% / 45 real seconds
	if (GetCharacterMovement()->IsMovementInProgress())
	{
		SetFatigue(DeltaTime * (1.21f / 45.0f), true);
	}
	else
	{
		SetFatigue(DeltaTime * (0.21f / 45.0f), true);
	}
	
	// Dirtiness: 1% / 45 real seconds
	SetDirtiness(DeltaTime * (0.50f / 45.0f), true);

	if (bPeeing == true && Urine > 0)
	{
		SetUrine(-DeltaTime * 5.0f, true); // Decrease urine by 10 per second
		if (Urine < 0) Urine = 0;
	}

	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		FVector TargetLocation = Camera->GetComponentLocation() + Camera->GetForwardVector() * 150.0f;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, TargetRotation);
	}
}

void AMSCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMSCCharacter::Crouch);
	PlayerInputComponent->BindAction("Pee", IE_Pressed, this, &AMSCCharacter::Pee);
	PlayerInputComponent->BindAction("Pee", IE_Released, this, &AMSCCharacter::StopPee);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMSCCharacter::Interact);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &AMSCCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMSCCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnAtRate", this, &AMSCCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMSCCharacter::LookUp);
	PlayerInputComponent->BindAxis("RotateActorWheel", this, &AMSCCharacter::RotateActorWheel);
}

void AMSCCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void AMSCCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void AMSCCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate);
}

void AMSCCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMSCCharacter::RotateActorWheel(float Value)
{
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent() && Value != 0.0f)
	{
		TargetRotation.Yaw += Value * 10.0f; // Multiplier for wheel rotation
	}
}

void AMSCCharacter::Crouch()
{
	switch (CrouchState)
	{
		default:
			break;
		case Stand:
			SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
			CrouchState = Crouched;
			GetCharacterMovement()->MaxWalkSpeed = 300;
			break;
		case Crouched:
			SetActorScale3D(FVector(0.1f, 0.1f, 0.1f));
			CrouchState = Land;
			GetCharacterMovement()->MaxWalkSpeed = 100;
			break;
		case Land:
			SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
			CrouchState = Stand;
			GetCharacterMovement()->MaxWalkSpeed = 600;
			break;
	}
}

void AMSCCharacter::Pee()
{
	UE_LOG(LogMSCCharacter, Display, TEXT("Peeing"));
	bPeeing = true;
}

void AMSCCharacter::StopPee()
{
	UE_LOG(LogMSCCharacter, Display, TEXT("Stopped peeing"));
	bPeeing = false;
}

void AMSCCharacter::Interact()
{
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		PhysicsHandle->ReleaseComponent();
		return;
	}

	FHitResult Hit;
	float TraceDistance = 1000.0f;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * TraceDistance;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) return;

		// 1. Try Interact
		if (HitActor->Implements<UInteractInterface>())
		{
			IInteractInterface::Execute_Interact(HitActor, this);
			return;
		}

		// 2. Try To Pick Up
		UPrimitiveComponent* HitComponent = Hit.GetComponent();
		if (HitComponent && HitComponent->IsSimulatingPhysics())
		{
			TargetRotation = HitComponent->GetComponentRotation();
			PhysicsHandle->GrabComponentAtLocationWithRotation(
				HitComponent,
				NAME_None,
				HitComponent->GetComponentLocation(),
				TargetRotation
			);
		}
	}
}

void AMSCCharacter::SetThirst(float ToSet, bool bAdd)
{
	if (bAdd)
	{
		Thirst += ToSet;
		ChangedThirst();
		return;
	}
	Thirst = ToSet;
	ChangedThirst();
}

void AMSCCharacter::SetHunger(float ToSet, bool bAdd)
{
	if (bAdd)
	{
		Hunger += ToSet;
		ChangedHunger();
		return;
	}
	Hunger = ToSet;
	ChangedHunger();
}

void AMSCCharacter::SetStress(float ToSet, bool bAdd)
{
	if (bAdd)
	{
		Stress += ToSet;
		ChangedStress();
		return;
	}
	Stress = ToSet;
	ChangedStress();
}

void AMSCCharacter::SetUrine(float ToSet, bool bAdd)
{
	if (bAdd)
	{
		Urine += ToSet;
		ChangedUrine();
		return;
	}
	Urine = ToSet;
	ChangedUrine();
}

void AMSCCharacter::SetFatigue(float ToSet, bool bAdd)
{
	if (bAdd)
	{
		Fatigue += ToSet;
		ChangedFatigue();
		return;
	}
	Fatigue = ToSet;
	ChangedFatigue();
}

void AMSCCharacter::SetDirtiness(float ToSet, bool bAdd)
{
	if (bAdd)
	{
		Dirtiness += ToSet;
		ChangedDirtiness();
		return;
	}
	Dirtiness = ToSet;
	ChangedDirtiness();
}

void AMSCCharacter::SetMoney(float ToSet, bool bAdd)
{
	if (bAdd)
	{
		Money += ToSet;
		ChangedMoney();
		return;
	}
	Money = ToSet;
	ChangedMoney();
}

void AMSCCharacter::SetAlcohol(float ToSet, bool bAdd)
{
	if (bAdd)
	{
		Alcohol += ToSet;
		ChangedAlcohol();
		return;
	}
	Alcohol = ToSet;
	ChangedAlcohol();
}

void AMSCCharacter::SetPlayerWeight(float ToSet, bool bAdd)
{
	if (bAdd)
	{
		Weight += ToSet;
		ChangedPlayerWeight();
		return;
	}
	Weight = ToSet;
	ChangedPlayerWeight();
}

void AMSCCharacter::KillPlayer(EKillType KillType)
{
	UE_LOG(LogMSCCharacter, Warning , TEXT("Player killed"));
}

void AMSCCharacter::ChangedThirst()
{
	if (Thirst >= 100)
	{
		GetWorld()->GetTimerManager().SetTimer(ThirstKillHandle, [this]()
		{
			KillPlayer(EKillType::Thirst);
		}, 60, false);
	}
	if (Thirst < 100)
	{
		if (GetWorld()->GetTimerManager().TimerExists(ThirstKillHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(ThirstKillHandle);
		}
	}
}

void AMSCCharacter::ChangedHunger()
{
	if (Hunger >= 100)
	{
		GetWorld()->GetTimerManager().SetTimer(HungerKillHandle, [this]()
		{
			KillPlayer(EKillType::Hunger);
		}, 60, false);
	}
	if (Hunger < 100)
	{
		if (GetWorld()->GetTimerManager().TimerExists(HungerKillHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(HungerKillHandle);
		}
	}
}

void AMSCCharacter::ChangedStress()
{
	UE_LOG(LogMSCCharacter, Display , TEXT("Changed stress"));
}

void AMSCCharacter::ChangedUrine()
{
	UE_LOG(LogMSCCharacter, Display , TEXT("Changed urine"));
}

void AMSCCharacter::ChangedFatigue()
{
	UE_LOG(LogMSCCharacter, Display , TEXT("Changed fatigue"));
}

void AMSCCharacter::ChangedDirtiness()
{
	UE_LOG(LogMSCCharacter, Display , TEXT("Changed dirtiness"));
}

void AMSCCharacter::ChangedMoney()
{
	UE_LOG(LogMSCCharacter, Display , TEXT("Changed money"));
}

void AMSCCharacter::ChangedAlcohol()
{
	UE_LOG(LogMSCCharacter, Display , TEXT("Changed alcohol"));
}

void AMSCCharacter::ChangedPlayerWeight()
{
	UE_LOG(LogMSCCharacter, Display , TEXT("Changed weight"));
}