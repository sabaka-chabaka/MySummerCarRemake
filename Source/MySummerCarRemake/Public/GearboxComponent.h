//2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GearboxComponent.generated.h"

UENUM(BlueprintType)
enum class EGear : int8
{
    Reverse = -1,
    Neutral = 0,
    First = 1,
    Second = 2,
    Third = 3,
    Fourth = 4
};

USTRUCT(BlueprintType)
struct FGearData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gear")
    float Ratio = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gear")
    FVector2D SpeedRange = FVector2D(0, 100);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gear")
    float MaxRPM = 6000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gear")
    float MinRPM = 800.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYSUMMERCARREMAKE_API UGearboxComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGearboxComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Gearbox|Control")
    bool ShiftUp();
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Control")
    bool ShiftDown();
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Control")
    bool ShiftToNeutral();
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Control")
    bool ShiftToReverse();
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Control")
    bool ShiftToGear(EGear Gear);
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Control")
    void ForceShiftToGear(EGear Gear);
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    EGear GetCurrentGear() const { return CurrentGear; }
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    FString GetCurrentGearString() const;
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    float GetCurrentRatio() const;
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    float GetOutputTorque(float InputTorque) const;
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    float GetOutputRPM(float InputRPM) const;
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    float GetInputRPM(float WheelRPM) const;
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    bool IsInNeutral() const { return CurrentGear == EGear::Neutral; }
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    bool IsInReverse() const { return CurrentGear == EGear::Reverse; }
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    bool IsShifting() const { return bIsShifting; }
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    float GetShiftProgress() const { return ShiftTimer / ShiftTime; }
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    bool CanShiftTo(EGear Gear) const;
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    FVector2D GetRecommendedShiftRange() const;
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    bool ShouldShiftUp() const;
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Data")
    bool ShouldShiftDown() const;

    UFUNCTION(BlueprintCallable, Category = "Gearbox|Settings")
    void SetShiftTime(float NewShiftTime) { ShiftTime = NewShiftTime; }
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Settings")
    void EnableSyncromesh(bool bEnable) { bUseSyncromesh = bEnable; }
    
    UFUNCTION(BlueprintCallable, Category = "Gearbox|Settings")
    void SetGearData(EGear Gear, const FGearData& Data);

    UFUNCTION(BlueprintImplementableEvent, Category = "Gearbox|Events")
    void OnGearChanged(EGear OldGear, EGear NewGear);
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Gearbox|Events")
    void OnShiftStarted();
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Gearbox|Events")
    void OnShiftCompleted();
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Gearbox|Events")
    void OnGrindSound();

private:
    void UpdateShift(float DeltaTime);
    bool AttemptShift(EGear NewGear);
    void CompleteShift();
    float CalculateSyncromeshTime(EGear NewGear) const;
    void GrindGears();
    
    bool CheckRPMForShift(EGear NewGear) const;
    bool CheckSpeedForShift(EGear NewGear) const;
    bool CheckClutchForShift() const;
    float GetCurrentSpeedKMH() const;

    UPROPERTY(EditAnywhere, Category = "Gearbox Settings")
    float FinalDriveRatio = 4.1f;
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Settings")
    float ShiftTime = 0.3f; 
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Settings")
    float MinShiftRPM = 1000.0f; 
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Settings")
    float MaxShiftRPM = 5500.0f; 
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Settings")
    float SpeedTolerance = 5.0f; 
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Settings")
    bool bUseSyncromesh = true; 
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Settings")
    float SyncromeshTimeMultiplier = 1.0f; 
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Settings")
    bool bAllowSkipShift = false; 
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Settings")
    bool bPreventMoneyShift = true;

    UPROPERTY(EditAnywhere, Category = "Gearbox Gears")
    FGearData ReverseGear;
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Gears")
    FGearData FirstGear;
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Gears")
    FGearData SecondGear;
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Gears")
    FGearData ThirdGear;
    
    UPROPERTY(EditAnywhere, Category = "Gearbox Gears")
    FGearData FourthGear;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gearbox State")
    EGear CurrentGear = EGear::Neutral;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gearbox State")
    EGear TargetGear = EGear::Neutral;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gearbox State")
    bool bIsShifting = false;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gearbox State")
    float ShiftTimer = 0.0f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gearbox State")
    float CurrentShiftTime = 0.3f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gearbox State")
    int32 GrindCount = 0; 
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gearbox State")
    float GearboxHealth = 100.0f; 

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gearbox Stats")
    int32 TotalShifts = 0;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gearbox Stats")
    int32 FailedShifts = 0;

    UPROPERTY()
    class UEngineComponent* EngineComponent;
    
    UPROPERTY()
    class UClutchComponent* ClutchComponent;
};
