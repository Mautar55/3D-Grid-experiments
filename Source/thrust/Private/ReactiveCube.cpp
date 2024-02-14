// Fill out your copyright notice in the Description page of Project Settings.


#include "ReactiveCube.h"
#include "Kismet/KismetMathLibrary.h"
#include "GenericPlatform/GenericPlatformMath.h"

// Sets default values
AReactiveCube::AReactiveCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SomeMesh"));
	StaticMeshComp->SetRelativeScale3D(FVector(1, 1, 1)); // capaz sirva de algo
	StaticMeshComp->SetMobility(EComponentMobility::Movable);
}

float AReactiveCube::SimpleImpact_Implementation(float in)
{
	

	return in;
}

bool AReactiveCube::QueryInteraction_Implementation(bool Commit, FHitResult hit, bool isAlt)
{
	ticksWithoutQueries = 0;
	if (State == CubeState::Edit) {
		StaticMeshComp->SetSimulatePhysics(true);
		StaticMeshComp->SetAllPhysicsLinearVelocity(FVector::Zero());
		StaticMeshComp->SetAllPhysicsAngularVelocityInRadians(FVector::Zero());
		State = CubeState::Free;
	}
	else if (State == CubeState::Free) {
		StaticMeshComp->SetSimulatePhysics(false);
		SetActorTransform(InitialTransform);
		State = CubeState::Edit;
	}
	return false;
}

bool AReactiveCube::QueryAction_Implementation(bool Commit, FHitResult hit, bool isAlt)
{

	if (State == CubeState::Edit) {
		ticksWithoutQueries = 0;

		if (!isAlt) {
			if (Commit) {

				if (PointIsWithinExtent(SnapHitToLocalGrid(hit, 50))) {
					AppendCubeAtWorldLocation(SnapHitToLocalGrid(hit, 50));
					return true;
				}
				//int count = K2_GetComponentsByClass(UStaticMeshComponent::StaticClass()).Num();
				//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Green, TEXT("Commit received " + FString::FromInt(count)));
				return false;
			}
			else {

				FVector aux = SnapHitToLocalGrid(hit, 50);

				ProxyComponent->SetWorldLocationAndRotation(aux, GetActorRotation());
				ProxyComponent->SetVisibility(true);
				
				if (PointIsWithinExtent(aux)) {
					ProxyComponent->SetMaterial(0, PositiveProxyMaterial);
					return true;
				}
				else {
					ProxyComponent->SetMaterial(0, NegativeProxyMaterial);
					return false;
				}

				//GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, TEXT("Call succesfull but doesnt do anything so returning false"));
				
			}
		}
		else {
			if (Commit) {
				
				if (hit.GetComponent() != GetRootComponent()) {
					hit.GetComponent()->DestroyComponent();
				}
			}
			else {
				ProxyComponent->SetWorldLocationAndRotation(SnapHitToComponent(hit), GetActorRotation());
				ProxyComponent->SetMaterial(0, OverdrawProxyMaterial);
				ProxyComponent->SetVisibility(true);
			}
		}
	}

	return false;
}

// Called when the game starts or when spawned
void AReactiveCube::BeginPlay()
{
	Super::BeginPlay();
	InitialTransform = GetActorTransform();

	ProxyComponent = AppendCubeAtWorldLocation(GetActorLocation());
	ProxyComponent->UnWeldFromParent();
	ProxyComponent->SetCollisionProfileName("NoCollision", false);
	ProxyComponent->SetVisibility(false);
}

// Called every frame
void AReactiveCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ticksWithoutQueries > 0 && IsValid(ProxyComponent)) {
		ProxyComponent->SetVisibility(false);
	}
	ticksWithoutQueries += 1;

}

UStaticMeshComponent* AReactiveCube::AppendCubeAtWorldLocation(FVector Position)
{
	// este dice relativo pero usa de mundo en automatico, re mentiroso
	UStaticMeshComponent* newcomp = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform(GetActorRotation(), Position, GetActorScale()), false));
	newcomp->SetStaticMesh(StaticMeshComp->GetStaticMesh());
	newcomp->SetMaterial(0, DefaultCubeMaterial);
	newcomp->WeldTo(StaticMeshComp);

	return newcomp;
}

FVector AReactiveCube::SnapHitToLocalGrid(FHitResult hit, float cellSize)
{
	FVector localHitLocation = GetActorTransform().InverseTransformPosition(hit.Location); // mundo a local para ambas
	FVector localHitNormal = GetActorTransform().InverseTransformVector(hit.Normal);

	FVector aux = (localHitLocation + localHitNormal * (cellSize / 2.0)) / cellSize;

	FVector rounded = FVector(FMath::RoundHalfFromZero(aux.X), FMath::RoundHalfFromZero(aux.Y), FMath::RoundHalfFromZero(aux.Z));

	FVector snappedVector = GetActorTransform().TransformPosition((rounded) * cellSize); // local a mundo

	return snappedVector;
}

FVector AReactiveCube::SnapHitToComponent(FHitResult hit)
{
	return hit.GetComponent()->GetComponentLocation();
}

bool AReactiveCube::PointIsWithinExtent(FVector point) {

	FVector absPos = GetActorTransform().InverseTransformPosition(point);
	absPos = FVector(FMath::Abs(absPos.X), FMath::Abs(absPos.Y), FMath::Abs(absPos.Z));

	return (absPos.X <= MaxLocalExtent.X && absPos.Y <= MaxLocalExtent.Y && absPos.Z <= MaxLocalExtent.Z);

}



