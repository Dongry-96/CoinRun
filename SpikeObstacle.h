#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeObstacle.generated.h"

class UBoxComponent;

UCLASS()
class STRIKEZONE_API ASpikeObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpikeObstacle();

	virtual void Tick(float DeltaTime) override;

	bool bIsActivate;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float DamageAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ForwardMoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BackwardMoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxMoveDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int32 MoveDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector StartLocation;
	

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	bool bHasHitPlayer;

	void MoveSpike(float DeltaTime);
};
