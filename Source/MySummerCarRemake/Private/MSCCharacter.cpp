// 2026 sabaka-chabaka

#include "MySummerCarRemake/Public/MSCCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogMSCCharacter, All, All);

#include "Camera/CameraComponent.h"

AMSCCharacter::AMSCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
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
	
	// Thirst: 10 real minutes (600 seconds) to fill 100
	SetThirst(DeltaTime * (100.0f / 600.0f), true);
	
	// Hunger: 15 real minutes (900 seconds) to fill 100
	SetHunger(DeltaTime * (100.0f / 900.0f), true);
	
	// Stress: 20 real minutes (1200 seconds) to fill 100
	SetStress(DeltaTime * (100.0f / 1200.0f), true);
	
	// Urine: 10 real minutes (600 seconds) to fill 100
	SetUrine(DeltaTime * (100.0f / 600.0f), true);
	
	// Fatigue: 30 real minutes (1800 seconds) to fill 100
	SetFatigue(DeltaTime * (100.0f / 1800.0f), true);
	
	// Dirtiness: 20 real minutes (1200 seconds) to fill 100
	SetDirtiness(DeltaTime * (100.0f / 1200.0f), true);

	if (bPeeing == true && Urine > 0)
	{
		SetUrine(-DeltaTime * 10.0f, true); // Decrease urine by 10 per second
		if (Urine < 0) Urine = 0;
	}
}

void AMSCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMSCCharacter::Crouch);
	PlayerInputComponent->BindAction("Pee", IE_Pressed, this, &AMSCCharacter::Pee);
	PlayerInputComponent->BindAction("Pee", IE_Released, this, &AMSCCharacter::StopPee);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &AMSCCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMSCCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnAtRate", this, &AMSCCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMSCCharacter::LookUp);
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

void AMSCCharacter::Crouch()
{
	switch (CrouchState)
	{
		default:
			break;
		case Stand:
			SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
			CrouchState = Crouched;
			break;
		case Crouched:
			SetActorScale3D(FVector(0.1f, 0.1f, 0.1f));
			CrouchState = Land;
			break;
		case Land:
			SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
			CrouchState = Stand;
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
	UE_LOG(LogMSCCharacter, Warning , TEXT("Changed stress"));
}

void AMSCCharacter::ChangedUrine()
{
	UE_LOG(LogMSCCharacter, Warning , TEXT("Changed urine"));
}

void AMSCCharacter::ChangedFatigue()
{
	UE_LOG(LogMSCCharacter, Warning , TEXT("Changed fatigue"));
}

void AMSCCharacter::ChangedDirtiness()
{
	UE_LOG(LogMSCCharacter, Warning , TEXT("Changed dirtiness"));
}

void AMSCCharacter::ChangedMoney()
{
	UE_LOG(LogMSCCharacter, Warning , TEXT("Changed money"));
}

void AMSCCharacter::ChangedAlcohol()
{
	UE_LOG(LogMSCCharacter, Warning , TEXT("Changed alcohol"));
}

void AMSCCharacter::ChangedPlayerWeight()
{
	UE_LOG(LogMSCCharacter, Warning , TEXT("Changed weight"));
}