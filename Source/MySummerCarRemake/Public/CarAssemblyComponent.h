// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarPart.h"
#include "CarAssemblyComponent.generated.h"

USTRUCT(BlueprintType)
struct FCarPartSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	ECarPartType PartType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	FString SlotDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	bool bRequiredForEngine;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slot")
	ACarPart* InstalledPart;

	FCarPartSlot()
		: PartType(ECarPartType::None)
		, SocketName(NAME_None)
		, SlotDisplayName(TEXT("Unknown Slot"))
		, bRequiredForEngine(false)
		, InstalledPart(nullptr)
	{}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPartInstalled,   ECarPartType, PartType, ACarPart*, Part);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPartUninstalled, ECarPartType, PartType, ACarPart*, Part);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCarAssembled);

UCLASS(ClassGroup = (Car), meta = (BlueprintSpawnableComponent))
class MYSUMMERCARREMAKE_API UCarAssemblyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCarAssemblyComponent();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, Category = "CarAssembly|Events")
	FOnPartInstalled OnPartInstalled;

	UPROPERTY(BlueprintAssignable, Category = "CarAssembly|Events")
	FOnPartUninstalled OnPartUninstalled;

	UPROPERTY(BlueprintAssignable, Category = "CarAssembly|Events")
	FOnCarAssembled OnCarFullyAssembled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarAssembly")
	TArray<FCarPartSlot> PartSlots;

	UFUNCTION(BlueprintCallable, Category = "CarAssembly")
	bool TryInstallPart(ACarPart* Part);

	UFUNCTION(BlueprintCallable, Category = "CarAssembly")
	ACarPart* UninstallPart(ECarPartType PartType);
	
	UFUNCTION(BlueprintPure, Category = "CarAssembly")
	ACarPart* GetInstalledPart(ECarPartType PartType) const;

	UFUNCTION(BlueprintPure, Category = "CarAssembly")
	bool IsPartInstalled(ECarPartType PartType) const;

	UFUNCTION(BlueprintPure, Category = "CarAssembly")
	bool CanStartEngine() const;

	UFUNCTION(BlueprintPure, Category = "CarAssembly")
	int32 GetMissingRequiredPartsCount() const;

	UFUNCTION(BlueprintPure, Category = "CarAssembly")
	TArray<FString> GetMissingPartNames() const;

	UFUNCTION(BlueprintPure, Category = "CarAssembly")
	bool IsFullyAssembled() const;

	UFUNCTION(BlueprintPure, Category = "CarAssembly")
	float GetAssemblyProgress() const;

private:
	FCarPartSlot* FindSlot(ECarPartType PartType);
	const FCarPartSlot* FindSlot(ECarPartType PartType) const;

	void CheckFullyAssembled();

	bool bAssembledEventFired;
};