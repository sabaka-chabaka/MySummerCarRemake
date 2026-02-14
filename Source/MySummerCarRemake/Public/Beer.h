// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "Beer.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API ABeer : public AActor, public IInteractInterface
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
	virtual void Interact_Implementation(AActor* Interactor) override;
	void Drink(AActor* Interactor);
};