// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "EatInterface.h"
#include "GameFramework/Actor.h"
#include "Pizza.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API APizza : public AActor, public IEatInterface
{
	GENERATED_BODY()

public:
	APizza();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void Hunger_Implementation(AActor* Interactor) override;
	
	UPROPERTY(EditAnywhere, Category="Mesh")
	class UStaticMeshComponent* Mesh;
};
