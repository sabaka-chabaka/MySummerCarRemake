// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MSCCarBody.generated.h"

USTRUCT(BlueprintType)
struct FPartMountInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PartID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AMSCBasePart* CurrentPart;

	FPartMountInfo() : PartID(NAME_None), SocketName(NAME_None), CurrentPart(nullptr) {}
};

UCLASS()
class MYSUMMERCARREMAKE_API AMSCCarBody : public AActor
{
	GENERATED_BODY()
	
public:	
	AMSCCarBody();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Setup")
	TArray<FPartMountInfo> MountPoints;

	UFUNCTION(BlueprintCallable, Category = "Car Logic")
	bool TryAttachPart(class AMSCBasePart* Part);

	UFUNCTION(BlueprintCallable, Category = "Car Logic")
	void RemovePart(class AMSCBasePart* Part);

	UFUNCTION(BlueprintImplementableEvent, Category = "Car Logic")
	void OnPartAttached(AMSCBasePart* Part);

	UFUNCTION(BlueprintImplementableEvent, Category = "Car Logic")
	void OnPartRemoved(AMSCBasePart* Part);
};
