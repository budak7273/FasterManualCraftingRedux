
#include "FasterManualCraftingModule.h"
#include "FMC_FasterManualCraftingConfigStruct.h"

#include "FGWorkBench.h"
#include "FGCheatManager.h"
#include "FGGameState.h"
#include "SML/Public/Patching/NativeHookManager.h"



DEFINE_LOG_CATEGORY(LogFasterManualCrafting);

// There are 3 bytes of padding after mIsFatigueEnabled.
// We reuse them to store the amount of producted items.
static uint16& GetProducedCountRef(UFGWorkBench* bench) {
	bool* offset = &bench->mIsFatigueEnabled;
	return *reinterpret_cast<uint16*>(offset + 1);
}

void FFasterManualCraftingModule::StartupModule() {
#if !WITH_EDITOR
	SUBSCRIBE_METHOD(UFGWorkBench::SetRecipe, [](auto& scope, UFGWorkBench* self, TSubclassOf< class UFGRecipe > recipe) {
		GetProducedCountRef(self) = 0;
	});

	SUBSCRIBE_METHOD(UFGWorkBench::SetWorkBenchUser, [](auto& scope, UFGWorkBench* self, AFGCharacterPlayer* player) {
		GetProducedCountRef(self) = 0;
	});

	SUBSCRIBE_METHOD(UFGWorkBench::Produce, [](auto& scope, UFGWorkBench* self, float dt) {
		scope.Cancel();

		// TurboMode is a global variable, so it shouldn't matter which cheat manager instance we use
		UFGCheatManager* CheatManager = GetMutableDefault<UFGCheatManager>();
		bool turboMode = CheatManager->TurboMode_Get();

		float turboModeMultiplier = 1.0f;
		if (turboMode) {
			AFGGameState* gameState = self->GetWorld()->GetGameState<AFGGameState>();
			if (gameState) turboModeMultiplier = gameState->mTurboModeMultiplier;
		}

		auto fmcConfig = FFMC_FasterManualCraftingConfigStruct::GetActiveConfig();

		uint16 producedCount = GetProducedCountRef(self);
		float craftTimeMultiplier = 1.0f + producedCount * fmcConfig.SpeedMultiplier;
		craftTimeMultiplier = FMath::Clamp(craftTimeMultiplier, 1.0f, fmcConfig.MaxSpeedMultiplier);

		float progress = (dt / self->mRecipeDuration);
		progress *= self->mCurrentFatigue;
		progress *= turboModeMultiplier;
		progress *= craftTimeMultiplier;

		self->mCurrentManufacturingProgress += progress;
		if (self->mCurrentManufacturingProgress >= 1.0f) {
			if (self->mInventory || (self->mPlayerWorkingAtBench && self->mPlayerWorkingAtBench->GetInventory())) {
				while (self->mCurrentManufacturingProgress >= 1.0f) {
					self->CraftComplete();
					self->mCurrentManufacturingProgress -= 1.0f;
					GetProducedCountRef(self) += 1;
				}
				self->mCurrentFatigue = powf(self->mFatigueMultiplier, self->mCounter);
				self->mCurrentManufacturingProgress = 0.0f;
			}
		}
	});
#endif
}

IMPLEMENT_GAME_MODULE(FFasterManualCraftingModule, FasterManualCrafting);
