#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlatformSpawnRow.h"
#include "PlatformClassData.generated.h"

UCLASS(BlueprintType)
class STRIKEZONE_API UPlatformClassData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlatformClasses")
	TArray<FPlatformSpawnRow> PlatformSpawnRows;
	
};
