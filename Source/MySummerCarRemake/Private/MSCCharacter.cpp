// 2026 sabaka-chabaka

#include "MySummerCarRemake/Public/MSCCharacter.h"

#include "Camera/CameraComponent.h"

AMSCCharacter::AMSCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	CrouchState = Stand;
}

void AMSCCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMSCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMSCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMSCCharacter::Crouch);
	
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
