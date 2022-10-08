#pragma once
#include "CoreMinimal.h"
#include "Configuration/ConfigManager.h"
#include "Engine/Engine.h"
#include "FMC_FasterManualCraftingReduxConfigStruct.generated.h"

/* Struct generated from Mod Configuration Asset '/FasterManualCraftingRedux/FMC_FasterManualCraftingReduxConfig' */
USTRUCT(BlueprintType)
struct FFMC_FasterManualCraftingReduxConfigStruct {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    float SpeedMultiplier;

    UPROPERTY(BlueprintReadWrite)
    float MaxSpeedMultiplier;

    UPROPERTY(BlueprintReadWrite)
    int32 LimitPerTick;

    UPROPERTY(BlueprintReadWrite)
    int32 MaxVfxSparkCount;

    /* Retrieves active configuration value and returns object of this struct containing it */
    static FFMC_FasterManualCraftingReduxConfigStruct GetActiveConfig() {
        FFMC_FasterManualCraftingReduxConfigStruct ConfigStruct{};
        FConfigId ConfigId{"FasterManualCraftingRedux", ""};
        UConfigManager* ConfigManager = GEngine->GetEngineSubsystem<UConfigManager>();
        ConfigManager->FillConfigurationStruct(ConfigId, FDynamicStructInfo{FFMC_FasterManualCraftingReduxConfigStruct::StaticStruct(), &ConfigStruct});
        return ConfigStruct;
    }
};

