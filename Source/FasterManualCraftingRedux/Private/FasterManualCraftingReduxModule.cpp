#include "FasterManualCraftingReduxModule.h"

#include "FMC_FasterManualCraftingReduxConfigStruct.h"
#include "FMCBPLib.h"

#include "Components/Overlay.h"

#include "FGWorkBench.h"
#include "UI/FGManufacturingButton.h"
#include "FGCheatManager.h"
#include "FGGameState.h"
#include "FGGameMode.h"
#include "SML/Public/Patching/NativeHookManager.h"
#include "SML/Public/Patching/BlueprintHookManager.h"
//#include "SML/Public/Patching/BlueprintHookHelper.h"

DEFINE_LOG_CATEGORY(LogFasterManualCraftingRedux);

uint16& FFasterManualCraftingReduxModule::GetProducedCountRef(UFGWorkBench* bench) {
	bool* offset = &bench->mIsFatigueEnabled;
	return *reinterpret_cast<uint16*>(offset + 1);
}

void FFasterManualCraftingReduxModule::SetupHooks() {
	UFGWorkBench* workBenchCDO = GetMutableDefault<UFGWorkBench>();

	SUBSCRIBE_METHOD(UFGWorkBench::SetRecipe, [](auto& scope, UFGWorkBench* self, TSubclassOf< class UFGRecipe > recipe) {
		GetProducedCountRef(self) = 0;
	});

	SUBSCRIBE_METHOD(UFGWorkBench::SetWorkBenchUser, [](auto& scope, UFGWorkBench* self, AFGCharacterPlayer* player) {
		GetProducedCountRef(self) = 0;
	});

	// TODO 1.0: !self->mManufacturingButton->IsButtonHeld() check causes a crash when loading into the world
	// can't find a good replacement - just leaving it for now
	// Consequence: letting go of the button no longer loses your craft speed bonus
	/*
	SUBSCRIBE_METHOD_VIRTUAL(UFGWorkBench::TickComponent, workBenchCDO, [](auto& scope, UFGWorkBench* self, float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
		//if (self->mManufacturingButton && !self->mManufacturingButton->mIsHolding)
		//	GetProducedCountRef(self) = 0;
		// 
		// doesn't crash but doesn't seem to work
		// if (self->mManufacturingButton && self->mManufacturingButton->mHoldTime <= 0)
		//	GetProducedCountRef(self) = 0;

		// doesn't crash but doesn't seem to work
		if (self->mCurrentCooldown <= 0.01)
			GetProducedCountRef(self) = 0;
	});
	*/

	SUBSCRIBE_METHOD(UFGWorkBench::TickProducing, [](auto& scope, UFGWorkBench* self, float dt) {
		scope.Cancel();


		// FIXME: Experimental changed the cheat manager quite extensively
		//        So until EA & EXP get merged again we need to disable this
		//        to make the mod compatible with both versions.
		// TurboMode is a global variable, so it shouldn't matter which cheat manager instance we use
		// UFGCheatManager* CheatManager = GetMutableDefault<UFGCheatManager>();
		bool turboMode = false; //  CheatManager->TurboMode_Get();

		float turboModeMultiplier = 1.0f;
		if (turboMode) {
			AFGGameState* gameState = self->GetWorld()->GetGameState<AFGGameState>();
			if (gameState) turboModeMultiplier = gameState->mTurboModeMultiplier;
		}

		auto fmcConfig = FFMC_FasterManualCraftingReduxConfigStruct::GetActiveConfig(self->GetWorld());

		uint16 producedCount = GetProducedCountRef(self);
		float craftTimeMultiplier = UFMCBPLib::GetCurrentProductionMultiplier(producedCount, fmcConfig.SpeedMultiplier, fmcConfig.MaxSpeedMultiplier);

		float progress = (dt / self->mRecipeDuration);
		progress *= self->mCurrentFatigue;
		progress *= turboModeMultiplier;
		progress *= craftTimeMultiplier;

		self->mCurrentManufacturingProgress += progress;
		if (self->mCurrentManufacturingProgress >= 1.0f) {
			UFGInventoryComponent* inventory = self->mInventory;
			if (!inventory && self->mPlayerWorkingAtBench)
				inventory = self->mPlayerWorkingAtBench->GetInventory();

			if(inventory) {
				int productionLimit = fmcConfig.LimitPerTick;
				if (productionLimit == 0)
					productionLimit = INT_MAX;

				int numCrafts = 0;
				while (self->mCurrentManufacturingProgress >= 1.0f && self->CanProduce(self->mCurrentRecipe, inventory) && numCrafts < productionLimit) {
					self->CraftComplete();
					self->mCurrentManufacturingProgress -= 1.0f;
					numCrafts++;
				}
				GetProducedCountRef(self) += numCrafts;

				if (self->mPlayerWorkingAtBench) {
					AFGPlayerState* PlayerState = self->mPlayerWorkingAtBench->GetPlayerStateChecked<AFGPlayerState>();
					if (PlayerState) {
						PlayerState->Client_OnRecipeConstructed(self->mCurrentRecipe, numCrafts);
					}
				}

				self->mCurrentFatigue = powf(self->mFatigueMultiplier, self->mCounter);
				self->mCurrentManufacturingProgress = 0.0f;
			}
		}
	});
}

void FFasterManualCraftingReduxModule::StartupModule() {
#if !WITH_EDITOR
	SetupHooks();
#endif // !WITH_EDITOR
}

IMPLEMENT_GAME_MODULE(FFasterManualCraftingReduxModule, FasterManualCraftingRedux);
