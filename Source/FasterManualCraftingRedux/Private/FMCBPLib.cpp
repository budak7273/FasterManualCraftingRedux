#include "FMCBPLib.h"
#include "FGWorkBench.h"

int32 UFMCBPLib::GetProducedCount(class UFGWorkBench* bench) {
	bool* offset = &bench->mIsFatigueEnabled;
	auto value = *reinterpret_cast<uint16*>(offset + 1);
	return (int32)value;
}

float UFMCBPLib::GetCurrentProductionMultiplier(int32 producedCount, float speedMultiplier, float maxSpeedMultiplier) {
	float craftTimeMultiplier = 1.0f + producedCount * speedMultiplier;
	return FMath::Clamp(craftTimeMultiplier, 1.0f, maxSpeedMultiplier);
}