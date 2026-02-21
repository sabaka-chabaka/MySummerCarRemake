// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API ADoor : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:
	ADoor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact_Implementation(AActor* Interactor) override;
	
	UPROPERTY(EditAnywhere, Category="ID")
	int ID;
	
	UPROPERTY(VisibleAnywhere, Category="Status")
	bool bOpened;
	
	UPROPERTY(EditAnywhere, Category="Status")
	FRotator TargetRot;
	
protected:
	UPROPERTY(EditAnywhere, Category="Mesh")
	class UStaticMeshComponent* StaticMeshComponent;
	
	void Open();
	void Close();
	
	UPROPERTY(EditAnywhere, Category="Mesh")
	FRotator ClosedRot;
	
	UPROPERTY(EditAnywhere, Category="Mesh")
	FRotator OpenedRot;
};