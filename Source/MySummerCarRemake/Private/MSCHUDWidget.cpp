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
		ThirstBar->SetPercent(MSCC->Thirst);
		HungerBar->SetPercent(MSCC->Hunger);
		StressBar->SetPercent(MSCC->Stress);
		UrineBar->SetPercent(MSCC->Urine);
		FatigueBar->SetPercent(MSCC->Fatigue);
		DirtinessBar->SetPercent(MSCC->Dirtiness);
	}
}