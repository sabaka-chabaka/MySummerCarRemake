// 2026 sabaka-chabaka

#include "EngineComponent.h"

#include "CarHealthTools.h"
#include "ClutchComponent.h"
#include "Components/AudioComponent.h"

UEngineComponent::UEngineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	TorqueCurve.Add(FVector2D(0.0f, 0.3f));
	TorqueCurve.Add(FVector2D(0.3f, 0.9f));
	TorqueCurve.Add(FVector2D(0.6f, 1.0f));
	TorqueCurve.Add(FVector2D(0.8f, 0.95f));
	TorqueCurve.Add(FVector2D(1.0f, 0.7f));
}

void UEngineComponent::BeginPlay()
{
	Super::BeginPlay();
	if (AActor* Owner = GetOwner())
	{
		ClutchComponent = Owner->FindComponentByClass<UClutchComponent>();
		
		EngineAudioComponent = NewObject<UAudioComponent>(Owner);
		if (EngineAudioComponent)
		{
			EngineAudioComponent->SetupAttachment(Owner->GetRootComponent());
			EngineAudioComponent->RegisterComponent();
			EngineAudioComponent->SetSound(SoundSettings.EngineSound);
			EngineAudioComponent->bAutoActivate = false;
		}
		
		StarterAudioComponent = NewObject<UAudioComponent>(Owner);
		if (StarterAudioComponent)
		{
			StarterAudioComponent->SetupAttachment(Owner->GetRootComponent());
			StarterAudioComponent->RegisterComponent();
			StarterAudioComponent->SetSound(StarterSound);
			StarterAudioComponent->bAutoActivate = false;
		}
	}
	
	EngineState = Disabled;
	CurrentRPM = 0.0f;
}

void UEngineComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopEngineSound();
	if (StarterAudioComponent)
	{
		EngineAudioComponent->Stop();
		EngineAudioComponent->DestroyComponent();
	}
	if (EngineAudioComponent)
	{
		EngineAudioComponent->DestroyComponent();
	}
	
	Super::EndPlay(EndPlayReason);
}

void UEngineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	switch (EngineState)
	{
	case Disabled:
		CurrentRPM = FMath::FInterpTo(CurrentRPM, 0.0f, DeltaTime, 5.0f);
		CurrentTorque = 0.0f;
		break;
		
	case Starting:
		UpdateStartingSequence(DeltaTime);
		break;
		
	case Running:
		UpdateEngine(DeltaTime);

		if (CurrentRPM < StallRPM)
		{
			StallTimer += DeltaTime;
			if (StallTimer >= StallTime)
			{
				EngineState = Stalling;
				StallTimer = 0.0f;
			}
		}
		else
		{
			StallTimer = 0.0f;
		}
		break;
		
	case Stalling:
		UpdateStalling(DeltaTime);
		break;
	}
	
	SoundUpdateTimer += DeltaTime;
	if (SoundUpdateTimer >= SoundUpdateRate)
	{
		UpdateEngineSound();
		SoundUpdateTimer = 0.0f;
	}
}

bool UEngineComponent::StartEngine()
{
	if (EngineState == Disabled && bIgnitionOn)
	{
		EngineState = Starting;
		StartTime = 0;

		if (StarterAudioComponent && StarterSound)
		{
			StarterAudioComponent->Play();
		}
		
		PlayEngineSound();
		
		return true;
	}
	
	return false;
}

void UEngineComponent::StopEngine()
{
	if (EngineState != Disabled)
	{
		EngineState = Disabled;
		
		if (StarterAudioComponent)
		{
			StarterAudioComponent->Stop();
		}
		
		StopEngineSound();
	}
}

void UEngineComponent::SetThrottle(float NewThrottle)
{
	ThrottleInput = FMath::Clamp(NewThrottle, 0.0f, 1.0f);
}

void UEngineComponent::SetIgnition(bool bIgnitionOn)
{
	this->bIgnitionOn = bIgnitionOn;

	if (!bIgnitionOn && EngineState != Disabled)
	{
		StopEngine();
	}
}

void UEngineComponent::UpdateStartingSequence(float DeltaTime)
{
	StartTimer += DeltaTime;
	
	float StarterRPM = StartRPM * FMath::Sin(StartTimer * 10.0f) * 0.5f + StartRPM * 0.5f;
	CurrentRPM = FMath::FInterpTo(CurrentRPM, StarterRPM, DeltaTime, 2.0f);
	CurrentTorque = StartTorque;
	
	if (StartTimer >= StartTime)
	{
		if (UCarHealthTools::CanEngineStart(CurrentRPM, MinRunningRPM))
		{
			EngineState = EEngineState::Running;
			CurrentRPM = IdleRPM;
            
			if (StarterAudioComponent)
			{
				StarterAudioComponent->Stop();
			}
            
			UpdateEngineSound();
		}
		else
		{
			EngineState = EEngineState::Disabled;
			StartTimer = 0.0f;
            
			if (StarterAudioComponent)
			{
				StarterAudioComponent->Stop();
			}
			StopEngineSound();
		}
	}
}

void UEngineComponent::UpdateStalling(float DeltaTime)
{
	CurrentRPM = FMath::FInterpTo(CurrentRPM, 0.0f, DeltaTime, 3.0f);
	CurrentTorque = 0.0f;
    
	UpdateEngineSound();
    
	if (CurrentRPM < 50.0f)
	{
		EngineState = EEngineState::Disabled;
        
		StopEngineSound();
	}
}

void UEngineComponent::UpdateEngine(float DeltaTime)
{
	float TargetRPM = IdleRPM + (MaxRPM - IdleRPM) * ThrottleInput;
    
	float LoadFactor = 1.0f;
	if (ClutchComponent)
	{
		float ClutchEngagement = ClutchComponent->EngagementRate;
		LoadFactor = 1.0f - ClutchEngagement * 0.7f;
	}
    
	float RPMChangeRate = (CurrentRPM < TargetRPM) ? RPMIncreaseRate : RPMDecreaseRate;
	RPMChangeRate *= FMath::Clamp(ThrottleInput, 0.3f, 1.0f);
    
	CurrentRPM = FMath::FInterpTo(CurrentRPM, TargetRPM, DeltaTime, RPMChangeRate * 0.001f);
    
	CurrentRPM = FMath::Clamp(CurrentRPM, 0.0f, MaxRPM * 1.1f);
    
	float TorqueMultiplier = CalculateTorqueCurve(CurrentRPM / MaxRPM);
	CurrentTorque = TorqueMultiplier * MaxTorque * ThrottleInput * LoadFactor;
    
	if (CurrentRPM > MaxRPM * 0.9f)
	{
		float DamageRate = (CurrentRPM - MaxRPM * 0.9f) / (MaxRPM * 0.1f);
	}
}

void UEngineComponent::UpdateEngineSound()
{
    if (!EngineAudioComponent || !EngineAudioComponent->IsPlaying())
        return;
    
    float Pitch = CalculateSoundPitch();
    float Volume = CalculateSoundVolume();
    
    EngineAudioComponent->SetPitchMultiplier(Pitch);
    EngineAudioComponent->SetVolumeMultiplier(Volume);
    
    EngineAudioComponent->SetFloatParameter(FName("RPM"), CurrentRPM);
    EngineAudioComponent->SetFloatParameter(FName("Load"), ThrottleInput);
}

void UEngineComponent::PlayEngineSound()
{
    if (EngineAudioComponent && SoundSettings.EngineSound)
    {
        if (!EngineAudioComponent->IsPlaying())
        {
            EngineAudioComponent->SetSound(SoundSettings.EngineSound);
            EngineAudioComponent->Play();
            
            float InitialPitch = (EngineState == EEngineState::Starting) ? 
                SoundSettings.MinPitch : CalculateSoundPitch();
            float InitialVolume = (EngineState == EEngineState::Starting) ? 
                SoundSettings.MinVolume * 0.5f : CalculateSoundVolume();
                
            EngineAudioComponent->SetPitchMultiplier(InitialPitch);
            EngineAudioComponent->SetVolumeMultiplier(InitialVolume);
        }
    }
}

void UEngineComponent::StopEngineSound()
{
    if (EngineAudioComponent && EngineAudioComponent->IsPlaying())
    {
        EngineAudioComponent->FadeOut(0.5f, 0.0f);
    }
}

void UEngineComponent::SetEngineSound(USoundBase* NewSound)
{
    SoundSettings.EngineSound = NewSound;
    
    if (EngineAudioComponent)
    {
        bool bWasPlaying = EngineAudioComponent->IsPlaying();
        float CurrentPitch = EngineAudioComponent->PitchMultiplier;
        float CurrentVolume = EngineAudioComponent->VolumeMultiplier;
        
        EngineAudioComponent->SetSound(NewSound);
        
        if (bWasPlaying)
        {
            EngineAudioComponent->Play();
            EngineAudioComponent->SetPitchMultiplier(CurrentPitch);
            EngineAudioComponent->SetVolumeMultiplier(CurrentVolume);
        }
    }
}

float UEngineComponent::CalculateTorqueCurve(float NormalizedRPM) const
{
    if (TorqueCurve.Num() < 2)
        return 0.0f;
    
    for (int32 i = 0; i < TorqueCurve.Num() - 1; i++)
    {
        if (NormalizedRPM >= TorqueCurve[i].X && NormalizedRPM <= TorqueCurve[i + 1].X)
        {
            float Alpha = (NormalizedRPM - TorqueCurve[i].X) / 
                         (TorqueCurve[i + 1].X - TorqueCurve[i].X);
            return FMath::Lerp(TorqueCurve[i].Y, TorqueCurve[i + 1].Y, Alpha);
        }
    }
    
    return TorqueCurve.Last().Y;
}

float UEngineComponent::CalculateSoundPitch() const
{
    if (CurrentRPM <= 0.0f)
        return SoundSettings.MinPitch;
    
    float NormalizedRPM = FMath::Clamp(CurrentRPM / MaxRPM, 0.0f, 1.0f);
    
    float Pitch = FMath::Lerp(
        SoundSettings.MinPitch,
        SoundSettings.MaxPitch,
        FMath::Pow(NormalizedRPM, SoundSettings.PitchCurveExponent)
    );
    
    float Variation = FMath::Sin(GetWorld()->GetTimeSeconds() * CurrentRPM * 0.001f) * 0.02f;
    
    return Pitch + Variation;
}

float UEngineComponent::CalculateSoundVolume() const
{
    if (CurrentRPM <= 0.0f)
        return 0.0f;
    
    float NormalizedRPM = FMath::Clamp(CurrentRPM / MaxRPM, 0.0f, 1.0f);
    float LoadFactor = ThrottleInput;
    
    float BaseVolume = FMath::Lerp(SoundSettings.MinVolume, SoundSettings.MaxVolume, NormalizedRPM);
    
    if (NormalizedRPM > 0.7f && LoadFactor < 0.3f)
    {
        BaseVolume *= 0.7f;
    }
    
    if (EngineState == EEngineState::Starting)
    {
        BaseVolume *= 0.3f;
    }
    else if (EngineState == EEngineState::Stalling)
    {
        BaseVolume *= 0.5f;
    }
    
    return BaseVolume;
}