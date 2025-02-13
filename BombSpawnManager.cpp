#include "BombSpawnManager.h"
#include "BombObstacle.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

ABombSpawnManager::ABombSpawnManager()
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SpawningBox->SetupAttachment(Scene);

	SpawnInterval = 0.2f;
	MaxBombsCnt = 60;
	CurrentBombCount = 0;
}

void ABombSpawnManager::StartBombsEvent()
{
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ABombSpawnManager::SpawnBomb,
		SpawnInterval,
		true
	);
}

void ABombSpawnManager::SpawnBomb()
{
	if (!BombObstacleClass) return;

	if (CurrentBombCount >= MaxBombsCnt)
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(
		SpawningBox->Bounds.Origin,
		SpawningBox->Bounds.BoxExtent
	);

	ABombObstacle* SpawnedBomb = GetWorld()->SpawnActor<ABombObstacle>(
		BombObstacleClass,
		SpawnLocation,
		FRotator::ZeroRotator
	);

	if (SpawnedBomb)
	{
		CurrentBombCount++;
	}
}


