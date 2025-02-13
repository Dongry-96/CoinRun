#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SZ_GameInstance.generated.h"

UCLASS()
class STRIKEZONE_API USZ_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USZ_GameInstance();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 TotalScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentWave;

	UFUNCTION(BlueprintCallable, Category = "GameData")
	void AddToScore(int32 Amount);
};
