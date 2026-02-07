// 2026 sabaka-chabaka

#include "MSCHUD.h"

#include "MSCHUDWidget.h"
#include "SubtitlesWidget.h"
#include "Blueprint/UserWidget.h"

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
		}
	}
}

void AMSCHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
