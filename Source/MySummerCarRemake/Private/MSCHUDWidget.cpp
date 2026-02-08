// 2026 sabaka-chabaka

#include "MSCHUDWidget.h"

#include "MSCCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMSCHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMSCHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (AMSCCharacter* MSCC = Cast<AMSCCharacter>(GetOwningPlayerPawn()))
	{
		MoneyBlock->SetText(FText::AsNumber(MSCC->Money));
		ThirstBar->SetPercent(MSCC->Thirst / 100.0f);
		HungerBar->SetPercent(MSCC->Hunger / 100.0f);
		StressBar->SetPercent(MSCC->Stress / 100.0f);
		UrineBar->SetPercent(MSCC->Urine / 100.0f);
		FatigueBar->SetPercent(MSCC->Fatigue / 100.0f);
		DirtinessBar->SetPercent(MSCC->Dirtiness / 100.0f);
	}
}