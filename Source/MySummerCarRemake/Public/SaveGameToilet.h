// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "SaveGameToilet.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API ASaveGameToilet : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:
	ASaveGameToilet();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category="Mesh")
	class UStaticMeshComponent* ToiletMesh;
	
	UPROPERTY(EditAnywhere, Category="Collider")
	class USphereComponent* SphereComponent;
	
	UFUNCTION(BlueprintCallable, Category="ToiletSaver")
	void Save();
	
	virtual void Interact_Implementation(AActor* Interactor) override;
};