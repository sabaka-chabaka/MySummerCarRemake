// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "DrinkInterface.h"
#include "GameFramework/Actor.h"
#include "Beer.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API ABeer : public AActor, public IDrinkInterface
{
	GENERATED_BODY()

public:
	ABeer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category="Mesh")
	UStaticMeshComponent* Mesh;
	
public:
	virtual void Drink_Implementation(AActor* Interactor) override;
};