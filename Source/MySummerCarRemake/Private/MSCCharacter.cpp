// 2026 sabaka-chabaka

#include "MySummerCarRemake/Public/MSCCharacter.h"
#include "InteractInterface.h"
#include "MSCSaveGameComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "NiagaraComponent.h"
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
	Cigarettes = 0;
	bPeeing = false;
	TargetRotation = FRotator::ZeroRotator;
	AlcoholKillLocation = FVector::ZeroVector;
	RespawnLocation = FVector::ZeroVector;
	
	PeeRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PeeRoot"));
	PeeRoot->SetupAttachment(GetCapsuleComponent());

	PeeRoot->SetRelativeLocation(PeeOffset);

	PeeComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PeeComponent"));
	PeeComponent->SetupAttachment(PeeRoot);
	PeeComponent->bAutoActivate = false;
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
	ChangedCigarettes();
	if (UGameplayStatics::DoesSaveGameExist(TEXT("ManualSave"), 0))
	{
		if (UMSCSaveGameComponent* Save = Cast<UMSCSaveGameComponent>(UGameplayStatics::LoadGameFromSlot(TEXT("ManualSave"), 0)))
		{
			SetActorRotation(Save->PlayerRotation);
			SetActorLocation(Save->PlayerLocation);
			SetThirst(Save->Thirst, false);
			SetHunger(Save->Hunger, false);
			SetStress(Save->Stress, false);
			SetUrine(Save->Urine, false);
			SetFatigue(Save->Fatigue, false);
			SetDirtiness(Save->Dirtiness, false);
			SetMoney(Save->Money, false);
			SetAlcohol(Save->Alcohol, false);
			SetPlayerWeight(Save->Weight, false);
			SetCigarettes(Save->Cigarettes, false);
			if (AWeatherController* WC = Cast<AWeatherController>(UGameplayStatics::GetActorOfClass(GetWorld(), WeatherController)))
			{
				WC->SetWeekDay(Save->WeekDay);
				WC->SetWeatherState(Save->WeatherState);
				WC->SetHours(Save->Hours, false);
				WC->SetMinutes(Save->Minutes, false);
				WC->SetSeconds(Save->Seconds, false);
			}
		}
	}
}

void AMSCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Needs filling formulas based on real time
	
	// Thirst: 1.83% / 45 real seconds
	SetThirst(DeltaTime * (1.2f / 45.0f), true);
	if (Alcohol >= 2.0f)
	{
		SetThirst(DeltaTime * (0.8f / 45.0f), true);
	}
	
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
		float Mass = PhysicsHandle->GrabbedComponent->GetMass();
		
		float ThirstRate = FMath::Clamp(
			Mass / 90.0f,
			0.1f,
			3.0f
		);

		SetThirst(DeltaTime * ThirstRate, true);
		if (Alcohol >= 5.0f)
		{
			float Chance = FMath::RandRange(1, 10000);
			if (Chance == 1)
			{
				PhysicsHandle->ReleaseComponent();
			}
		}
	}
	
	if (PeeComponent && PeeComponent->IsActive())
	{
		FVector Dir = GetControlRotation().Vector();

		PeeComponent->SetVectorParameter(
			FName("User.Direction"),
			Dir
		);
	}

	if (Alcohol >= 0.0f)
	{
		ApplyAlcoholSway(DeltaTime);
		SoberUp(DeltaTime);
	}

	if (Urine <= 0.0f)
	{
		bPeeing = false;
		StopPee();
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
	FVector Drift = FVector::ZeroVector;
	if (Alcohol > 0.f)
	{
		float Alpha = Alcohol / 10.0f;

		Drift.X = FMath::FRandRange(-1.f, 1.f) * Alpha * 0.25f;
		Drift.Y = FMath::FRandRange(-1.f, 1.f) * Alpha * 02.5f;
	}
	
	AddMovementInput(GetActorForwardVector() + Drift, Value);
}

void AMSCCharacter::MoveRight(float Value)
{
	FVector Drift = FVector::ZeroVector;
	if (Alcohol > 0.f)
	{
		float Alpha = Alcohol / 10.0f;

		Drift.X = FMath::FRandRange(-1.f, 1.f) * Alpha * 0.25f;
		Drift.Y = FMath::FRandRange(-1.f, 1.f) * Alpha * 02.5f;
	}
	
	AddMovementInput(GetActorRightVector() + Drift, Value);
}

void AMSCCharacter::TurnAtRate(float Rate)
{
	float Alpha = Alcohol / 10.0f;

	float Noise = FMath::FRandRange(-0.5f, 0.5f) * Alpha;
	AddControllerYawInput(Rate + Noise);
}

void AMSCCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMSCCharacter::RotateActorWheel(float Value)
{
	if (Value == 0.0f) return;

	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		TargetRotation.Yaw += Value * 10.0f;
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
	if (Urine <= 0.0f)
		return;
	bPeeing = true;
	if (PeeComponent)
	{
		PeeComponent->Activate(true);
	}
}

void AMSCCharacter::StopPee()
{
	UE_LOG(LogMSCCharacter, Display, TEXT("Stopped peeing"));
	bPeeing = false;
	if (PeeComponent)
	{
		PeeComponent->Deactivate();
	}
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

		if (HitActor->Implements<UInteractInterface>())
		{
			IInteractInterface::Execute_Interact(HitActor, this);
			return;
		}

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

void AMSCCharacter::SetCigarettes(int ToSet, bool bAdd)
{
	if (bAdd)
	{
		Cigarettes += ToSet;
		ChangedCigarettes();
		return;
	}
	Cigarettes = ToSet;
	ChangedCigarettes();
}

void AMSCCharacter::KillPlayer(EKillType KillType)
{
	UE_LOG(LogMSCCharacter, Warning , TEXT("Player killed"));
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, [this, KillType]()
	{
		RespawnPlayer(KillType);
	}, 5, false);
}

void AMSCCharacter::RespawnPlayer(EKillType KillType)
{
	//some logic
	if (KillType == EKillType::Alcohol)
	{
		SetActorLocation(AlcoholKillLocation);
	}
	else
	{
		SetActorLocation(RespawnLocation);
	}
}

void AMSCCharacter::ApplyAlcoholSway(float DeltaTime)
{
	if (Alcohol <= 0.f || !Controller)
		return;

	float Alpha = FMath::Clamp(Alcohol / 10.0f, 0.f, 1.f);
	float Time = GetWorld()->GetTimeSeconds();

	// Медленные, не синхронные колебания
	float YawOffset =
		FMath::Sin(Time * 0.6f) * 0.8f * Alpha;

	float PitchOffset =
		FMath::Sin(Time * 0.9f + 1.3f) * 0.8f * Alpha;

	FRotator ControlRot = Controller->GetControlRotation();
	ControlRot.Yaw   += YawOffset;
	ControlRot.Pitch += PitchOffset;

	Controller->SetControlRotation(ControlRot);
}

void AMSCCharacter::SoberUp(float DeltaTime)
{
	Alcohol = FMath::Max(0.f, Alcohol - DeltaTime * 0.005f);
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
	if (Alcohol >= 10)
	{
		KillPlayer(EKillType::Alcohol);
	}
}

void AMSCCharacter::ChangedPlayerWeight()
{
	UE_LOG(LogMSCCharacter, Display , TEXT("Changed weight"));
}

void AMSCCharacter::ChangedCigarettes()
{
	UE_LOG(LogMSCCharacter, Display , TEXT("Changed cigarettes"));
}

void AMSCCharacter::RespawnedPlayer()
{
	UE_LOG(LogMSCCharacter, Display , TEXT("Respawned player"));
}