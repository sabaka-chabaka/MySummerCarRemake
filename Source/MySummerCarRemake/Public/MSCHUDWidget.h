// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MSCHUDWidget.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API UMSCHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* ThirstBar;
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HungerBar;
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* StressBar;
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* UrineBar;

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* FatigueBar;
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* DirtinessBar;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MoneyBlock;
};
