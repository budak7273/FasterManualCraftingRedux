#pragma once
#include "CoreMinimal.h"
#include "Configuration/ConfigManager.h"
#include "Engine/Engine.h"
#include "FMC_FasterManualCraftingConfigStruct.generated.h"

/* Struct generated from Mod Configuration Asset '/FasterManualCrafting/FMC_FasterManualCraftingConfig' */
USTRUCT(BlueprintType)
struct FFMC_FasterManualCraftingConfigStruct {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    float SpeedMultiplier;

    UPROPERTY(BlueprintReadWrite)
    float MaxSpeedMultiplier;

    /* Retrieves active configuration value and returns object of this struct containing it */
    static FFMC_FasterManualCraftingConfigStruct GetActiveConfig() {
        FFMC_FasterManualCraftingConfigStruct ConfigStruct{};
        FConfigId ConfigId{"FasterManualCrafting", ""};
        UConfigManager* ConfigManager = GEngine->GetEngineSubsystem<UConfigManager>();
        ConfigManager->FillConfigurationStruct(ConfigId, FDynamicStructInfo{FFMC_FasterManualCraftingConfigStruct::StaticStruct(), &ConfigStruct});
        return ConfigStruct;
    }
};

