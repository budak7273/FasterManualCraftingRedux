// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystem/ModSubsystem.h"
#include "FMCSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FASTERMANUALCRAFTINGREDUX_API AFMCSubsystem : public AModSubsystem
{
	GENERATED_BODY()
public:
	AFMCSubsystem();

protected:
	virtual void BeginPlay() override;
};
