// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "EatInterface.h"
#include "GameFramework/Actor.h"
#include "SausagesAndPotatoes.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API ASausagesAndPotatoes : public AActor, public IEatInterface
{
	GENERATED_BODY()

public:
	ASausagesAndPotatoes();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category="Mesh")
	class UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(EditAnywhere, Category="Mesh")
	class UStaticMeshComponent* AfterMesh;
	
	virtual void Hunger_Implementation(AActor* Interactor) override;
};
