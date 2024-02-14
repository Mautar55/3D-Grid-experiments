// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactive.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractive : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THRUST_API IInteractive
{
	GENERATED_BODY()

public:

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool QueryInteraction(bool Commit, FHitResult hit, bool isAlt);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool QueryAction(bool commit, FHitResult hit, bool isAlt);
};
