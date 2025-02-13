#pragma once

#include "CoreMinimal.h"
#include "PlatformSpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FPlatformSpawnRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPlatformSpawnRow() : SpawnChance(0.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PlatformClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance;

};
