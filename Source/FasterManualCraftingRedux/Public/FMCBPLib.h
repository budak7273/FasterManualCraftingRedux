#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FMCBPLib.generated.h"


UCLASS()
class UFMCBPLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Get the Faster Manual Crafting produced item count stored in the padding in the Fatigue Enabled flag
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static int32 GetProducedCount(class UFGWorkBench* bench);

	// Calculate the Production Multiplier from the produced item count and the values in the configs
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static float GetCurrentProductionMultiplier(int32 producedCount, float speedMultiplier, float maxSpeedMultiplier);
};
