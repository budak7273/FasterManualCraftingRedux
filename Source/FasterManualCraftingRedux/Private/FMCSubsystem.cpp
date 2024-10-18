#include "FMCSubsystem.h"


#include "SML/Public/Patching/BlueprintHookManager.h"
#include "SML/Public/Patching/BlueprintHookHelper.h"

#include "FMC_FasterManualCraftingReduxConfigStruct.h"
#include "Components/Overlay.h"

AFMCSubsystem::AFMCSubsystem() {
	ReplicationPolicy = ESubsystemReplicationPolicy::SpawnOnClient;
}

void OnCreateSparksFunc(FBlueprintHookHelper& helper) {
	UObject* context = helper.GetContext();

	int32* numSparksToAdd = helper.GetLocalVarPtr<FIntProperty>(TEXT("NumberOfSparks"));
	check(numSparksToAdd);

	FObjectProperty* containerProperty = CastFieldChecked<FObjectProperty>(context->GetClass()->FindPropertyByName(TEXT("Container")));
	check(containerProperty);
	UObject* containerObj = containerProperty->GetPropertyValue_InContainer(context, 0);
	UOverlay* container = CastChecked<UOverlay>(containerObj);
	check(container);

	auto fmcConfig = FFMC_FasterManualCraftingReduxConfigStruct::GetActiveConfig(context->GetWorld());

	int32 numExistingSparks = container->GetChildrenCount();
	if (numExistingSparks > fmcConfig.MaxVfxSparkCount) {
		*numSparksToAdd = 0;
	}
}

void AFMCSubsystem::BeginPlay() {
	Super::BeginPlay();

	UBlueprintHookManager* HookManager = GEngine->GetEngineSubsystem<UBlueprintHookManager>();
	check(HookManager);
	// Will crash if ran server side because it isn't packaged with widgets
	UClass* WidgetSparkParticlesClass = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Interface/UI/InGame/-Shared/Widget_SparkParticles.Widget_SparkParticles_C"));
	check(WidgetSparkParticlesClass);
	UFunction* CreateSparksFunc = WidgetSparkParticlesClass->FindFunctionByName(TEXT("CreateSparks"));
	check(CreateSparksFunc);

	HookManager->HookBlueprintFunction(CreateSparksFunc, OnCreateSparksFunc, EPredefinedHookOffset::Start);
}
