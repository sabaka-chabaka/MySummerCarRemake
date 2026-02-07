// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MSCHUD.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API AMSCHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD")
	class UMSCHUDWidget* HUDWidget;
	
	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> HUDWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitle")
	class USubtitlesWidget* SubtitleWidget;
	
	UPROPERTY(EditAnywhere, Category = "Subtitle")
	TSubclassOf<class UUserWidget> SubtitleWidgetClass;
};
