#pragma once

#include "Modules/ModuleManager.h"

// TODO this crashes, need to find out how to not crash because different verbosity
// DECLARE_LOG_CATEGORY_EXTERN(LogFasterManualCraftingRedux, Log, Verbose);

class FFasterManualCraftingReduxModule : public FDefaultGameModuleImpl {
public:
	virtual void StartupModule() override;

	virtual bool IsGameModule() const override { return true; }

	/**
	There are 3 bytes of padding after mIsFatigueEnabled.
	 We reuse them to store the amount of producted items.
	*/
	UFUNCTION(BlueprintCallable)
		static uint16& GetProducedCountRef(class UFGWorkBench* bench);

	UFUNCTION(BlueprintCallable)
		static float GetCurrentProductionMultiplier(uint16 producedCount, float speedMultiplier, float maxSpeedMultiplier);
};