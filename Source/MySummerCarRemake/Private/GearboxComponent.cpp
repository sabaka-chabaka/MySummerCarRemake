// 2026 sabaka-chabaka

#include "GearboxComponent.h"
#include "EngineComponent.h"
#include "ClutchComponent.h"

UGearboxComponent::UGearboxComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    ReverseGear.Ratio = -3.5f;
    ReverseGear.SpeedRange = FVector2D(0, 20);
    ReverseGear.MaxRPM = 3000.0f;
    ReverseGear.MinRPM = 700.0f;
    
    FirstGear.Ratio = 3.8f;
    FirstGear.SpeedRange = FVector2D(0, 40);
    FirstGear.MaxRPM = 6500.0f;
    FirstGear.MinRPM = 800.0f;
    
    SecondGear.Ratio = 2.2f;
    SecondGear.SpeedRange = FVector2D(20, 70);
    SecondGear.MaxRPM = 6500.0f;
    SecondGear.MinRPM = 1200.0f;
    
    ThirdGear.Ratio = 1.5f;
    ThirdGear.SpeedRange = FVector2D(40, 110);
    ThirdGear.MaxRPM = 6500.0f;
    ThirdGear.MinRPM = 1500.0f;
    
    FourthGear.Ratio = 1.0f;
    FourthGear.SpeedRange = FVector2D(60, 180);
    FourthGear.MaxRPM = 6500.0f;
    FourthGear.MinRPM = 1800.0f;
}

void UGearboxComponent::BeginPlay()
{
    Super::BeginPlay();
    
    AActor* Owner = GetOwner();
    if (Owner)
    {
        EngineComponent = Owner->FindComponentByClass<UEngineComponent>();
        ClutchComponent = Owner->FindComponentByClass<UClutchComponent>();
    }
    
    CurrentGear = EGear::Neutral;
    TargetGear = EGear::Neutral;
}

void UGearboxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (bIsShifting)
    {
        UpdateShift(DeltaTime);
    }
    
    if (EngineComponent && EngineComponent->IsEngineRunning() && !bIsShifting)
    {
        float CurrentRPM = EngineComponent->GetCurrentRPM();
        float MinRPMForGear = 0.0f;
        
        switch (CurrentGear)
        {
        case EGear::Reverse: MinRPMForGear = ReverseGear.MinRPM; break;
        case EGear::First: MinRPMForGear = FirstGear.MinRPM; break;
        case EGear::Second: MinRPMForGear = SecondGear.MinRPM; break;
        case EGear::Third: MinRPMForGear = ThirdGear.MinRPM; break;
        case EGear::Fourth: MinRPMForGear = FourthGear.MinRPM; break;
        default: MinRPMForGear = 0.0f; break;
        }
        
        if (CurrentRPM < MinRPMForGear && GetCurrentSpeedKMH() > 5.0f)
        {
            float StallChance = 1.0f - (CurrentRPM / MinRPMForGear);
            if (FMath::FRand() < StallChance * DeltaTime * 2.0f)
            {
                EngineComponent->StopEngine();
            }
        }
    }
}

bool UGearboxComponent::ShiftUp()
{
    if (bIsShifting) return false;
    
    switch (CurrentGear)
    {
    case EGear::Reverse:
        return AttemptShift(EGear::Neutral);
    case EGear::Neutral:
        return AttemptShift(EGear::First);
    case EGear::First:
        return AttemptShift(EGear::Second);
    case EGear::Second:
        return AttemptShift(EGear::Third);
    case EGear::Third:
        return AttemptShift(EGear::Fourth);
    case EGear::Fourth:
        return false;
    default:
        return false;
    }
}

bool UGearboxComponent::ShiftDown()
{
    if (bIsShifting) return false;
    
    switch (CurrentGear)
    {
    case EGear::Reverse:
        return false;
    case EGear::Neutral:
        return AttemptShift(EGear::Reverse);
    case EGear::First:
        return AttemptShift(EGear::Neutral);
    case EGear::Second:
        return AttemptShift(EGear::First);
    case EGear::Third:
        return AttemptShift(EGear::Second);
    case EGear::Fourth:
        return AttemptShift(EGear::Third);
    default:
        return false;
    }
}

bool UGearboxComponent::ShiftToNeutral()
{
    return AttemptShift(EGear::Neutral);
}

bool UGearboxComponent::ShiftToReverse()
{
    if (GetCurrentSpeedKMH() > 3.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot shift to reverse while moving"));
        return false;
    }
    
    return AttemptShift(EGear::Reverse);
}

bool UGearboxComponent::ShiftToGear(EGear Gear)
{
    if (bAllowSkipShift || 
        (static_cast<int8>(Gear) == static_cast<int8>(CurrentGear) + 1) ||
        (static_cast<int8>(Gear) == static_cast<int8>(CurrentGear) - 1))
    {
        return AttemptShift(Gear);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Skip shifting is disabled"));
    return false;
}

void UGearboxComponent::ForceShiftToGear(EGear Gear)
{
    if (!bIsShifting)
    {
        TargetGear = Gear;
        bIsShifting = true;
        ShiftTimer = 0.0f;
        CurrentShiftTime = ShiftTime * 0.5f; 
        
        OnShiftStarted();
        
        UE_LOG(LogTemp, Warning, TEXT("Forced shift to gear: %d"), static_cast<int8>(Gear));
    }
}

FString UGearboxComponent::GetCurrentGearString() const
{
    switch (CurrentGear)
    {
    case EGear::Reverse: return TEXT("R");
    case EGear::Neutral: return TEXT("N");
    case EGear::First: return TEXT("1");
    case EGear::Second: return TEXT("2");
    case EGear::Third: return TEXT("3");
    case EGear::Fourth: return TEXT("4");
    default: return TEXT("?");
    }
}

float UGearboxComponent::GetCurrentRatio() const
{
    const FGearData* GearData = nullptr;
    
    switch (CurrentGear)
    {
    case EGear::Reverse: GearData = &ReverseGear; break;
    case EGear::First: GearData = &FirstGear; break;
    case EGear::Second: GearData = &SecondGear; break;
    case EGear::Third: GearData = &ThirdGear; break;
    case EGear::Fourth: GearData = &FourthGear; break;
    default: return 0.0f;
    }
    
    if (GearData)
    {
        return GearData->Ratio * FinalDriveRatio;
    }
    
    return 0.0f;
}

float UGearboxComponent::GetOutputTorque(float InputTorque) const
{
    float Ratio = GetCurrentRatio();
    if (FMath::Abs(Ratio) < KINDA_SMALL_NUMBER)
        return 0.0f;
    
    return InputTorque * FMath::Abs(Ratio);
}

float UGearboxComponent::GetOutputRPM(float InputRPM) const
{
    float Ratio = GetCurrentRatio();
    if (FMath::Abs(Ratio) < KINDA_SMALL_NUMBER)
        return 0.0f;
    
    return InputRPM / FMath::Abs(Ratio);
}

float UGearboxComponent::GetInputRPM(float WheelRPM) const
{
    float Ratio = GetCurrentRatio();
    if (FMath::Abs(Ratio) < KINDA_SMALL_NUMBER)
        return 0.0f;
    
    return WheelRPM * FMath::Abs(Ratio);
}

bool UGearboxComponent::AttemptShift(EGear NewGear)
{
    if (bIsShifting) return false;
    
    if (NewGear == CurrentGear)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already in gear %d"), static_cast<int8>(NewGear));
        return false;
    }
    
    if (!CheckClutchForShift())
    {
        GrindGears();
        return false;
    }
    
    if (!CheckRPMForShift(NewGear))
    {
        UE_LOG(LogTemp, Warning, TEXT("RPM not suitable for shift"));
        return false;
    }
    
    if (!CheckSpeedForShift(NewGear))
    {
        UE_LOG(LogTemp, Warning, TEXT("Speed not suitable for shift"));
        return false;
    }
    
    if (bPreventMoneyShift && EngineComponent)
    {
        float CurrentRPM = EngineComponent->GetCurrentRPM();
        const FGearData* NewGearData = nullptr;
        
        switch (NewGear)
        {
        case EGear::First: NewGearData = &FirstGear; break;
        case EGear::Second: NewGearData = &SecondGear; break;
        case EGear::Third: NewGearData = &ThirdGear; break;
        case EGear::Fourth: NewGearData = &FourthGear; break;
        }
        
        if (NewGearData)
        {
            float ExpectedRPM = CurrentRPM * (GetCurrentRatio() / (NewGearData->Ratio * FinalDriveRatio));
            
            if (ExpectedRPM > NewGearData->MaxRPM * 1.2f)
            {
                UE_LOG(LogTemp, Error, TEXT("Money shift prevented! Expected RPM: %.0f, Max RPM: %.0f"), 
                    ExpectedRPM, NewGearData->MaxRPM);
                return false;
            }
        }
    }
    
    TargetGear = NewGear;
    bIsShifting = true;
    ShiftTimer = 0.0f;
    
    if (bUseSyncromesh)
    {
        CurrentShiftTime = CalculateSyncromeshTime(NewGear);
    }
    else
    {
        CurrentShiftTime = ShiftTime;
    }
    
    OnShiftStarted();
    TotalShifts++;
    
    UE_LOG(LogTemp, Log, TEXT("Shifting from %s to %s"), 
        *GetCurrentGearString(), 
        *[&]() -> FString {
            switch (NewGear) {
                case EGear::Reverse: return TEXT("R");
                case EGear::Neutral: return TEXT("N");
                case EGear::First: return TEXT("1");
                case EGear::Second: return TEXT("2");
                case EGear::Third: return TEXT("3");
                case EGear::Fourth: return TEXT("4");
                default: return TEXT("?");
            }
        }());
    
    return true;
}

void UGearboxComponent::UpdateShift(float DeltaTime)
{
    ShiftTimer += DeltaTime;
    
    float Progress = FMath::Clamp(ShiftTimer / CurrentShiftTime, 0.0f, 1.0f);
    
    if (ShiftTimer >= CurrentShiftTime)
    {
        CompleteShift();
    }
}

void UGearboxComponent::CompleteShift()
{
    EGear OldGear = CurrentGear;
    CurrentGear = TargetGear;
    bIsShifting = false;
    
    OnShiftCompleted();
    OnGearChanged(OldGear, CurrentGear);
    
    if (OldGear != EGear::Neutral && CurrentGear != EGear::Neutral)
    {
        GearboxHealth -= 0.01f;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Shift completed to %s"), *GetCurrentGearString());
}

float UGearboxComponent::CalculateSyncromeshTime(EGear NewGear) const
{
    if (!EngineComponent) return ShiftTime;
    
    float CurrentRPM = EngineComponent->GetCurrentRPM();
    float ExpectedRPM = 0.0f;
    
    float CurrentRatio = GetCurrentRatio();
    float NewRatio = 0.0f;
    
    switch (NewGear)
    {
    case EGear::Reverse: NewRatio = ReverseGear.Ratio * FinalDriveRatio; break;
    case EGear::First: NewRatio = FirstGear.Ratio * FinalDriveRatio; break;
    case EGear::Second: NewRatio = SecondGear.Ratio * FinalDriveRatio; break;
    case EGear::Third: NewRatio = ThirdGear.Ratio * FinalDriveRatio; break;
    case EGear::Fourth: NewRatio = FourthGear.Ratio * FinalDriveRatio; break;
    default: NewRatio = 0.0f; break;
    }
    
    if (FMath::Abs(CurrentRatio) > KINDA_SMALL_NUMBER && FMath::Abs(NewRatio) > KINDA_SMALL_NUMBER)
    {
        ExpectedRPM = CurrentRPM * (CurrentRatio / NewRatio);
    }
    else
    {
        ExpectedRPM = CurrentRPM;
    }
    
    float RPMDifference = FMath::Abs(CurrentRPM - ExpectedRPM);
    float SyncTimeMultiplier = 1.0f + (RPMDifference / 1000.0f);
    
    return ShiftTime * SyncTimeMultiplier * SyncromeshTimeMultiplier;
}

void UGearboxComponent::GrindGears()
{
    GrindCount++;
    FailedShifts++;
    
    GearboxHealth -= 5.0f;
    
    OnGrindSound();
    
    UE_LOG(LogTemp, Warning, TEXT("Gear grind! Health: %.1f"), GearboxHealth);
    
    if (GearboxHealth <= 0.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("Gearbox broken!"));
    }
}

bool UGearboxComponent::CheckRPMForShift(EGear NewGear) const
{
    if (!EngineComponent) return true;
    
    float CurrentRPM = EngineComponent->GetCurrentRPM();
    
    if (CurrentRPM < MinShiftRPM || CurrentRPM > MaxShiftRPM)
    {
        return false;
    }
    
    const FGearData* GearData = nullptr;
    
    switch (NewGear)
    {
    case EGear::Reverse: GearData = &ReverseGear; break;
    case EGear::First: GearData = &FirstGear; break;
    case EGear::Second: GearData = &SecondGear; break;
    case EGear::Third: GearData = &ThirdGear; break;
    case EGear::Fourth: GearData = &FourthGear; break;
    default: return true; 
    }
    
    if (GearData)
    {
        return CurrentRPM >= GearData->MinRPM && CurrentRPM <= GearData->MaxRPM;
    }
    
    return true;
}

bool UGearboxComponent::CheckSpeedForShift(EGear NewGear) const
{
    float CurrentSpeed = GetCurrentSpeedKMH();
    
    if (NewGear == EGear::Reverse)
    {
        return CurrentSpeed <= SpeedTolerance;
    }
    
    if (CurrentGear == EGear::Reverse && NewGear == EGear::First)
    {
        return CurrentSpeed <= SpeedTolerance;
    }
    
    const FGearData* GearData = nullptr;
    
    switch (NewGear)
    {
    case EGear::First: GearData = &FirstGear; break;
    case EGear::Second: GearData = &SecondGear; break;
    case EGear::Third: GearData = &ThirdGear; break;
    case EGear::Fourth: GearData = &FourthGear; break;
    default: return true;
    }
    
    if (GearData)
    {
        return CurrentSpeed >= (GearData->SpeedRange.X - SpeedTolerance) && 
               CurrentSpeed <= (GearData->SpeedRange.Y + SpeedTolerance);
    }
    
    return true;
}

bool UGearboxComponent::CheckClutchForShift() const
{
    if (!ClutchComponent) return true;
    
    return ClutchComponent->IsSlipping();
}

float UGearboxComponent::GetCurrentSpeedKMH() const
{
    AActor* Owner = GetOwner();
    if (Owner)
    {
        FVector Velocity = Owner->GetVelocity();
        float SpeedCM = Velocity.Size();
        float SpeedKMH = SpeedCM * 0.036f;
        
        return SpeedKMH;
    }
    
    return 0.0f;
}

bool UGearboxComponent::CanShiftTo(EGear Gear) const
{
    if (bIsShifting) return false;
    if (Gear == CurrentGear) return false;
    
    return CheckClutchForShift() && 
           CheckRPMForShift(Gear) && 
           CheckSpeedForShift(Gear);
}

FVector2D UGearboxComponent::GetRecommendedShiftRange() const
{
    const FGearData* CurrentGearData = nullptr;
    
    switch (CurrentGear)
    {
    case EGear::First: CurrentGearData = &FirstGear; break;
    case EGear::Second: CurrentGearData = &SecondGear; break;
    case EGear::Third: CurrentGearData = &ThirdGear; break;
    case EGear::Fourth: CurrentGearData = &FourthGear; break;
    default: return FVector2D(0, 0);
    }
    
    if (CurrentGearData)
    {
        float RecommendUp = CurrentGearData->MaxRPM * 0.75f;
        float RecommendDown = CurrentGearData->MinRPM * 1.5f;
        
        return FVector2D(RecommendDown, RecommendUp);
    }
    
    return FVector2D(0, 0);
}

bool UGearboxComponent::ShouldShiftUp() const
{
    if (!EngineComponent || bIsShifting) return false;
    
    float CurrentRPM = EngineComponent->GetCurrentRPM();
    FVector2D RecommendedRange = GetRecommendedShiftRange();
    
    return CurrentRPM >= RecommendedRange.Y;
}

bool UGearboxComponent::ShouldShiftDown() const
{
    if (!EngineComponent || bIsShifting) return false;
    
    float CurrentRPM = EngineComponent->GetCurrentRPM();
    FVector2D RecommendedRange = GetRecommendedShiftRange();
    
    return CurrentRPM <= RecommendedRange.X && CurrentRPM > 0.0f;
}

void UGearboxComponent::SetGearData(EGear Gear, const FGearData& Data)
{
    switch (Gear)
    {
    case EGear::Reverse: ReverseGear = Data; break;
    case EGear::First: FirstGear = Data; break;
    case EGear::Second: SecondGear = Data; break;
    case EGear::Third: ThirdGear = Data; break;
    case EGear::Fourth: FourthGear = Data; break;
    default: break;
    }
}