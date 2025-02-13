#include "SZ_GameInstance.h"

USZ_GameInstance::USZ_GameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
	CurrentWave = 1;
}

void USZ_GameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
}
