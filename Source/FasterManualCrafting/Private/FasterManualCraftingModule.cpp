
#include "FasterManualCraftingModule.h"
#include "FMC_FasterManualCraftingConfigStruct.h"

#include "FGWorkBench.h"
#include "UI/FGManufacturingButton.h"
#include "FGCheatManager.h"
#include "FGGameState.h"
#include "FGGameMode.h"
#include "SML/Public/Patching/NativeHookManager.h"
#include "SML/Public/Patching/BlueprintHookManager.h"
#include "SML/Public/Patching/BlueprintHookHelper.h"



DEFINE_LOG_CATEGORY(LogFasterManualCrafting);

// There are 3 bytes of padding after mIsFatigueEnabled.
// We reuse them to store the amount of producted items.

uint16& FFasterManualCraftingModule::GetProducedCountRef(UFGWorkBench* bench) {
	bool* offset = &bench->mIsFatigueEnabled;
	return *reinterpret_cast<uint16*>(offset + 1);
}

// Limit for the number of active sparks in Widget_SparkParticles
static const int32 MaxSparkCount = 100;

void FFasterManualCraftingModule::StartupModule() {
#if !WITH_EDITOR
	UFGWorkBench* workBenchCDO = GetMutableDefault<UFGWorkBench>();

	SUBSCRIBE_METHOD(UFGWorkBench::SetRecipe, [](auto& scope, UFGWorkBench* self, TSubclassOf< class UFGRecipe > recipe) {
		GetProducedCountRef(self) = 0;
	});

	SUBSCRIBE_METHOD(UFGWorkBench::SetWorkBenchUser, [](auto& scope, UFGWorkBench* self, AFGCharacterPlayer* player) {
		GetProducedCountRef(self) = 0;
	});

	SUBSCRIBE_METHOD_VIRTUAL(UFGWorkBench::TickComponent, workBenchCDO, [](auto& scope, UFGWorkBench* self, float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
		if (self->mManufacturingButton && !self->mManufacturingButton->IsButtonHeld())
			GetProducedCountRef(self) = 0;
	});


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
			UFGInventoryComponent* inventory = self->mInventory;
			if (!inventory && self->mPlayerWorkingAtBench)
				inventory = self->mPlayerWorkingAtBench->GetInventory();

			if(inventory) {
				int productionLimit = fmcConfig.LimitPerTick;
				if (productionLimit == 0)
					productionLimit = INT_MAX;


				while (self->mCurrentManufacturingProgress >= 1.0f && self->CanProduce(self->mCurrentRecipe, inventory) && productionLimit > 0) {
					self->CraftComplete();
					self->mCurrentManufacturingProgress -= 1.0f;
					GetProducedCountRef(self) += 1;
					productionLimit--;
				}

				float floatDummy;
				if (productionLimit == 0 && self->CanProduce(self->mCurrentRecipe, inventory))
					self->mCurrentManufacturingProgress = modf(self->mCurrentManufacturingProgress, &floatDummy);

				self->mCurrentFatigue = powf(self->mFatigueMultiplier, self->mCounter);
				self->mCurrentManufacturingProgress = 0.0f;
			}
		}
	});

	AFGGameMode* gameModeCDO = GetMutableDefault<AFGGameMode>();
	SUBSCRIBE_METHOD_VIRTUAL(AFGGameMode::PostLogin, gameModeCDO, [](auto& scope, AFGGameMode* self, APlayerController* playerController) {
		UBlueprintHookManager* HookManager = GEngine->GetEngineSubsystem<UBlueprintHookManager>();
		check(HookManager);
		UClass* WidgetSparkParticlesClass = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Interface/UI/InGame/-Shared/Widget_SparkParticles.Widget_SparkParticles_C"));
		check(WidgetSparkParticlesClass);
		UFunction* CreateSparksFunc = WidgetSparkParticlesClass->FindFunctionByName(TEXT("CreateSparks"));
		check(CreateSparksFunc);

		HookManager->HookBlueprintFunction(CreateSparksFunc, [](FBlueprintHookHelper& helper) {
			UObject* context = helper.GetContext();

			int32* numSparksToAdd = helper.GetLocalVarPtr<FIntProperty>(TEXT("NumberOfSparks"));
			check(numSparksToAdd);

			FObjectProperty* containerProperty = CastFieldChecked<FObjectProperty>(context->GetClass()->FindPropertyByName(TEXT("Container")));
			check(containerProperty);
			UObject* containerObj = containerProperty->GetPropertyValue_InContainer(context, 0);
			UOverlay* container = CastChecked<UOverlay>(containerObj);
			check(container);

			int32 numExistingSparks = container->GetChildrenCount();
			if (numExistingSparks > MaxSparkCount) {
				*numSparksToAdd = 0;
			}
		}, EPredefinedHookOffset::Start);
	});

#endif // !WITH_EDITOR
}

IMPLEMENT_GAME_MODULE(FFasterManualCraftingModule, FasterManualCrafting);
