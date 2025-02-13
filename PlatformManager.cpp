#include "PlatformManager.h"
#include "Components/BoxComponent.h"
#include "PlatformClassData.h"
#include "MovingPlatform.h"
#include "LinearMovementPlatform.h"
#include "RotatingPlatform.h"


APlatformManager::APlatformManager()
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(Scene);

	PlatformClassData = nullptr;

	PlatformTotalAmount = 200;

	EndGameSpotLocation = FVector(147.0f, -161.0f, 2734.0f);

	MoveDistanceRandRange = FVector2D(500.0f, 1000.0f);
	MovementSpeedRandRange = FVector2D(50.0f, 300.0f);
	RotationSpeedRandRange = FVector2D(10.0f, 100.0f);
}

void APlatformManager::BeginPlay()
{
	Super::BeginPlay();

	if (PlatformClassData)
	{
		PlatformSpawnRowList = PlatformClassData->PlatformSpawnRows;
	}

	SpawnPlatforms();
}

void APlatformManager::SpawnPlatforms()
{
	for (int32 i = 0; i < PlatformTotalAmount; ++i)
	{
		FVector SpawnLocaiton = GetRandomSpawnPoint();
		SpawnPlatformAtLocation(SpawnLocaiton);
	}
}

void APlatformManager::SpawnPlatformAtLocation(const FVector& SpawnLocation)
{
	FPlatformSpawnRow SelectedRow = GetRandomRow();
	TSubclassOf<AActor> PlatformClass = SelectedRow.PlatformClass;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(PlatformClass, SpawnLocation, FRotator::ZeroRotator);

	AMovingPlatform* SpawnedPlatform = Cast<AMovingPlatform>(SpawnedActor);
	if (SpawnedPlatform)
	{
		InitializePlatform(SpawnedPlatform);
	}
}

FPlatformSpawnRow APlatformManager::GetRandomRow() const
{
	if (PlatformSpawnRowList.Num() == 0) return FPlatformSpawnRow();

	// RowList의 SpawnChance 총합 계산
	float TotalChance = 0.0f;
	for (const FPlatformSpawnRow& Row : PlatformSpawnRowList)
	{
		TotalChance += Row.SpawnChance;
	}

	// 확률 누적 합산으로 랜덤 Row값 반환
	float RandomValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;

	for (const FPlatformSpawnRow& Row : PlatformSpawnRowList)
	{
		AccumulateChance += Row.SpawnChance;
		if (RandomValue <= AccumulateChance)
		{
			return Row;
		}
	}
	return FPlatformSpawnRow();
}

FVector APlatformManager::GetRandomSpawnPoint() const
{
	FVector BoxOrigin = BoxCollision->GetComponentLocation();
	FVector BoxExtent = BoxCollision->GetScaledBoxExtent();
	float MinDistanceFromEndSpot = 1000.0f;

	FVector SpawnLocation;
	do 
	{
		SpawnLocation = BoxOrigin + FVector(
			FMath::FRandRange(-BoxExtent.X, +BoxExtent.X),
			FMath::FRandRange(-BoxExtent.Y, +BoxExtent.Y),
			FMath::FRandRange(-BoxExtent.Z, +BoxExtent.Z)
		);
	} while (FVector::Dist(SpawnLocation, EndGameSpotLocation) < MinDistanceFromEndSpot);

	return SpawnLocation;
}


void APlatformManager::InitializePlatform(AMovingPlatform* SpawnedPlatform)
{
	if (SpawnedPlatform)
	{
		if (ALinearMovementPlatform* LinearPlatform = Cast<ALinearMovementPlatform>(SpawnedPlatform))
		{
			LinearPlatform->SetMaxMoveDistance(GetRandomValueNonZero(MoveDistanceRandRange));
			LinearPlatform->SetMoveSpeed(GetRandomValueNonZero(MovementSpeedRandRange));
			LinearPlatform->SetMovementDirection(GetRandomDirection());
		}
		else if (ARotatingPlatform* RotatePlatform = Cast<ARotatingPlatform>(SpawnedPlatform))
		{
			RotatePlatform->SetRotationSpeed(GetRandomValueNonZero(RotationSpeedRandRange));
		}
	}
}

float APlatformManager::GetRandomValueNonZero(const FVector2D& Range)
{
	float Value;
	do
	{
		Value = FMath::RandRange(Range.X, Range.Y);
	} while (FMath::IsNearlyZero(Value));

	return Value;
}

FVector APlatformManager::GetRandomDirection()
{
	int32 RandomAxis = FMath::RandRange(0, 2);

	switch (RandomAxis)
	{
	case(0):
		return FVector(1.0f, 0.0f, 0.0f);
	case(1):
		return FVector(0.0f, 1.0f, 0.0f);
	case(2):
		return FVector(0.0f, 0.0f, 1.0f);
	default:
		return FVector(0.0f, 0.0f, 1.0f);
	}
}


