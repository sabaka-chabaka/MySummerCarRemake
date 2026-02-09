// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "MSCBasePart.generated.h"

UCLASS()
class MYSUMMERCARREMAKE_API AMSCBasePart : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	AMSCBasePart();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part Info")
	FName PartID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Part Status")
	bool bIsAttached;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Part Status")
	AActor* AttachedToActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Part Status")
	TArray<class AMSCBolt*> AttachedBolts;

	UFUNCTION(BlueprintPure, Category = "Part Logic")
	bool IsSecurelyBolted() const;

	UFUNCTION(BlueprintCallable, Category = "Part Logic")
	void RegisterBolt(class AMSCBolt* Bolt);

	UFUNCTION(BlueprintCallable, Category = "Part Logic")
	void UnregisterBolt(class AMSCBolt* Bolt);

	UFUNCTION(BlueprintCallable, Category = "Part Logic")
	void AttachToCar(AActor* CarActor, FName SocketName);

	UFUNCTION(BlueprintCallable, Category = "Part Logic")
	void DetachFromCar();

	UFUNCTION(BlueprintImplementableEvent, Category = "Part Logic")
	void OnAttached();

	UFUNCTION(BlueprintImplementableEvent, Category = "Part Logic")
	void OnDetached();
};
