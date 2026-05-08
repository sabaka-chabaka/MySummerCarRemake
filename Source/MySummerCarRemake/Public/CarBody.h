// 2026 sabaka-chabaka

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarBody.generated.h"

enum class ECarPartType : uint8;
class ACarPart;
class UCarAssemblyComponent;

UCLASS()
class MYSUMMERCARREMAKE_API ACarBody : public AActor
{
	GENERATED_BODY()

public:
	ACarBody();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CarBody|Components")
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CarBody|Components")
	UCarAssemblyComponent* AssemblyComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarBody")
	float BodyMass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarBody")
	float LinearDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarBody")
	float AngularDamping;

	UFUNCTION(BlueprintCallable, Category = "CarBody")
	bool TryInstallPart(ACarPart* Part);

	UFUNCTION(BlueprintCallable, Category = "CarBody")
	ACarPart* UninstallPart(ECarPartType PartType);

	UFUNCTION(BlueprintPure, Category = "CarBody")
	bool IsPartInstalled(ECarPartType PartType) const;

	UFUNCTION(BlueprintPure, Category = "CarBody")
	float GetAssemblyProgress() const;

	UFUNCTION(BlueprintPure, Category = "CarBody")
	bool CanStartEngine() const;
};