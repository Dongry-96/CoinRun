#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombSpawnManager.generated.h"

class UBoxComponent;
class ABombObstacle;

UCLASS()
class STRIKEZONE_API ABombSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ABombSpawnManager();

	void StartBombsEvent();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<ABombObstacle> BombObstacleClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 MaxBombsCnt;
	
	int32 CurrentBombCount;
	FTimerHandle SpawnTimerHandle;

	void SpawnBomb();
};
