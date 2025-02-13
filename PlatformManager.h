#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformSpawnRow.h"
#include "PlatformManager.generated.h"

class UPlatformClassData;
class AMovingPlatform;
class UBoxComponent;

UCLASS()
class STRIKEZONE_API APlatformManager : public AActor
{
	GENERATED_BODY()
	
public:	
	APlatformManager();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USceneComponent> Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UBoxComponent> BoxCollision;
private:
	// 에디터에서 설정한 Platform DataAsset
	UPROPERTY(EditAnywhere, Category = "Platform Management")
	TObjectPtr<UPlatformClassData> PlatformClassData;

	// 전체 플랫폼 리스트
	TArray<FPlatformSpawnRow> PlatformSpawnRowList;

	// 플랫폼 스폰 위치 및 속성 설정 변수
	UPROPERTY(EditAnywhere, Category = "Platform Management")
	int32 PlatformTotalAmount;

	UPROPERTY(EditAnywhere, Category = "Platform Management")
	FVector EndGameSpotLocation;

	UPROPERTY(EditAnywhere, Category = "Platform Management|RandRange|LinearPlatform")
	FVector2D MoveDistanceRandRange;
	UPROPERTY(EditAnywhere, Category = "Platform Management|RandRange|LinearPlatform")
	FVector2D MovementSpeedRandRange;
	UPROPERTY(EditAnywhere, Category = "Platform Management|RandRange|RotatePlatform")
	FVector2D RotationSpeedRandRange;

	// 레벨에 플랫폼 스폰 메서드
	void SpawnPlatforms();
	// 특정 위치에 플랫폼 스폰 메서드
	void SpawnPlatformAtLocation(const FVector& SpawnLocation);
	// 랜덤 플랫폼 Row 반환 메서드
	FPlatformSpawnRow GetRandomRow() const;
	// 랜덤 스폰 위치 반환 메서드
	FVector GetRandomSpawnPoint() const;

	// 플랫폼 속성 설정 메서드
	void InitializePlatform(AMovingPlatform* SpawnedPlatform);
	// Random Value 중 0이 아닌 값 반환 메서드
	float GetRandomValueNonZero(const FVector2D& Range);
	// 선형 이동 플랫폼 랜덤 이동방향 반환 메서드
	FVector GetRandomDirection();
};
