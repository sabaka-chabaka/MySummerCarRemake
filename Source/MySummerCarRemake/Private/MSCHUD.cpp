// 2026 sabaka-chabaka

#include "MSCHUD.h"

#include "MSCHUDWidget.h"
#include "SubtitlesWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

void AMSCHUD::BeginPlay()
{
	Super::BeginPlay();
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UMSCHUDWidget>(GetOwningPlayerController(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
	if (SubtitleWidgetClass)
	{
		SubtitleWidget = CreateWidget<USubtitlesWidget>(GetOwningPlayerController(), SubtitleWidgetClass);
		if (SubtitleWidget)
		{
			SubtitleWidget->AddToViewport();
			SubtitleWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AMSCHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMSCHUD::ShowSubtitle(FString InText, float Duration)
{
	if (!SubtitleWidget || !GetWorld())
		return;

	GetWorld()->GetTimerManager().ClearTimer(SubtitleTimerHandle);
	
	FText Text = FText::FromString(InText);
	SubtitleWidget->TextBlockSubtitles->SetText(Text);
	SubtitleWidget->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(SubtitleTimerHandle, this, &AMSCHUD::HideSubtitle, Duration, false);
}

void AMSCHUD::HideSubtitle()
{
	if (SubtitleWidget)
	{
		SubtitleWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}