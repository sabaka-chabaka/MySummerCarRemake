// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EngineComponent.generated.h"

UENUM(BlueprintType)
enum EEngineState : uint8
{
	Disabled,
	Starting,
	Running,
	Stalling
};

USTRUCT(BlueprintType)
struct FEngineSoundSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* EngineSound = nullptr;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float MinPitch = 0.5f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float MaxPitch = 2.0f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float MinVolume = 0.3f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float MaxVolume = 1.0f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float PitchCurveExponent = 1.5f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYSUMMERCARREMAKE_API UEngineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEngineComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category = "Engine|Ignition")
    bool StartEngine();
    
    UFUNCTION(BlueprintCallable, Category = "Engine|Ignition")
    void StopEngine();
    
    UFUNCTION(BlueprintCallable, Category = "Engine|Ignition")
    bool IsEngineRunning() const { return EngineState == EEngineState::Running; }
    
    UFUNCTION(BlueprintCallable, Category = "Engine|Ignition")
    bool IsEngineStarting() const { return EngineState == EEngineState::Starting; }
    
    UFUNCTION(BlueprintCallable, Category = "Engine|Control")
    void SetThrottle(float NewThrottle);
    
    UFUNCTION(BlueprintCallable, Category = "Engine|Control")
    void SetIgnition(bool bIgnitionOn);
    
    UFUNCTION(BlueprintCallable, Category = "Engine|Sound")
    void UpdateEngineSound();
    
    UFUNCTION(BlueprintCallable, Category = "Engine|Sound")
    void SetEngineSound(USoundBase* NewSound);
    
    UFUNCTION(BlueprintCallable, Category = "Engine|Data")
    float GetCurrentTorque() const { return CurrentTorque; }
    
    UFUNCTION(BlueprintCallable, Category = "Engine|Data")
    float GetCurrentRPM() const { return CurrentRPM; }
    
    UFUNCTION(BlueprintCallable, Category = "Engine|Data")
    EEngineState GetEngineState() const { return EngineState; }

private:
    void UpdateEngine(float DeltaTime);
    void UpdateStartingSequence(float DeltaTime);
    void UpdateStalling(float DeltaTime);
    float CalculateTorqueCurve(float RPM) const;
    float CalculateSoundPitch() const;
    float CalculateSoundVolume() const;
    
    void PlayEngineSound();
    void StopEngineSound();

    UPROPERTY(EditAnywhere, Category = "Engine Settings")
    float MaxTorque = 500.0f;
    
    UPROPERTY(EditAnywhere, Category = "Engine Settings")
    float MaxRPM = 6000.0f;
    
    UPROPERTY(EditAnywhere, Category = "Engine Settings")
    float IdleRPM = 800.0f;
    
    UPROPERTY(EditAnywhere, Category = "Engine Settings")
    float RPMIncreaseRate = 1000.0f;
    
    UPROPERTY(EditAnywhere, Category = "Engine Settings")
    float RPMDecreaseRate = 500.0f;
    
    UPROPERTY(EditAnywhere, Category = "Engine Settings")
    TArray<FVector2D> TorqueCurve;

    UPROPERTY(EditAnywhere, Category = "Ignition Settings")
    float StartRPM = 200.0f;
    
    UPROPERTY(EditAnywhere, Category = "Ignition Settings")
    float StartTorque = 100.0f;
    
    UPROPERTY(EditAnywhere, Category = "Ignition Settings")
    float StartTime = 2.0f;
    
    UPROPERTY(EditAnywhere, Category = "Ignition Settings")
    float MinRunningRPM = 400.0f;
    
    UPROPERTY(EditAnywhere, Category = "Ignition Settings")
    float StallRPM = 300.0f;
    
    UPROPERTY(EditAnywhere, Category = "Ignition Settings")
    float StallTime = 3.0f;

    UPROPERTY(EditAnywhere, Category = "Sound Settings")
    FEngineSoundSettings SoundSettings;
    
    UPROPERTY(EditAnywhere, Category = "Sound Settings")
    USoundBase* StarterSound = nullptr;
    
    UPROPERTY(EditAnywhere, Category = "Sound Settings")
    float SoundUpdateRate = 0.05f;
	
    UPROPERTY(VisibleAnywhere, Category = "Engine State")
    EEngineState EngineState = EEngineState::Disabled;
    
    UPROPERTY(VisibleAnywhere, Category = "Engine State")
    float CurrentRPM = 0.0f;
    
    UPROPERTY(VisibleAnywhere, Category = "Engine State")
    float CurrentTorque = 0.0f;
    
    UPROPERTY(VisibleAnywhere, Category = "Engine State")
    float ThrottleInput = 0.0f;
    
    UPROPERTY(VisibleAnywhere, Category = "Engine State")
    float StartTimer = 0.0f;
    
    UPROPERTY(VisibleAnywhere, Category = "Engine State")
    float StallTimer = 0.0f;
    
    UPROPERTY(VisibleAnywhere, Category = "Engine State")
    bool bIgnitionOn = false;
    
    UPROPERTY(VisibleAnywhere, Category = "Engine State")
    float SoundUpdateTimer = 0.0f;

    UPROPERTY(Transient)
    UAudioComponent* EngineAudioComponent = nullptr;
    
    UPROPERTY(Transient)
    UAudioComponent* StarterAudioComponent = nullptr;

	UPROPERTY()
    class UClutchComponent* ClutchComponent;
    
    FTimerHandle SoundUpdateTimerHandle;
};