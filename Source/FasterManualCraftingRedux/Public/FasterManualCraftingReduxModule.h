#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFasterManualCraftingRedux, Log, All);

class FFasterManualCraftingReduxModule : public FDefaultGameModuleImpl {
public:
	virtual void StartupModule() override;

	virtual bool IsGameModule() const override { return true; }

	/**
	There are 3 bytes of padding after mIsFatigueEnabled.
	 We reuse them to store the amount of produced items.
	*/
	UFUNCTION(BlueprintCallable)
	static uint16& GetProducedCountRef(class UFGWorkBench* bench);

private:
	void SetupHooks();
};