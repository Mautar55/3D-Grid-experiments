// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletImpact.h"
#include "Interactive.h"
#include "ReactiveCube.generated.h"

UENUM(BlueprintType)
enum class CubeState {
	Free,
	Edit
};

UCLASS()
class THRUST_API AReactiveCube : public AActor, public IBulletImpact, public IInteractive
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComp;

	// Sets default values for this actor's properties
	AReactiveCube();

	virtual float SimpleImpact_Implementation(float in);
	virtual bool QueryInteraction_Implementation(bool ImmediateCommit, FHitResult hit, bool isAlt);
	bool QueryAction_Implementation(bool Commit, FHitResult hit, bool isAlt);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// States related
private:
	UPROPERTY(VisibleAnywhere)
	CubeState State = CubeState::Edit;

	UPROPERTY(VisibleDefaultsOnly);
	FTransform InitialTransform;


	// Proxy cube related
private:
	UPROPERTY()
	UStaticMeshComponent* ProxyComponent;
	UPROPERTY()
	int ticksWithoutQueries = 0;

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FVector MaxLocalExtent = FVector(200, 200, 200);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UMaterialInterface* DefaultCubeMaterial;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UMaterialInterface* PositiveProxyMaterial;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UMaterialInterface* NegativeProxyMaterial;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UMaterialInterface* OverdrawProxyMaterial;


	// Compenents allocation
private:
	UFUNCTION()
	UStaticMeshComponent* AppendCubeAtWorldLocation(FVector Position);
	UFUNCTION()
	FVector SnapHitToLocalGrid(FHitResult hit, float cubeSize); // devuelve ublicacion en espacio de mundo
	FVector SnapHitToComponent(FHitResult hit); // same
	bool PointIsWithinExtent(FVector point);
};


