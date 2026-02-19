// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SubtitlesWidget.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API USubtitlesWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlockSubtitles;
};
