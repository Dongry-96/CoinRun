// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovingPlatform.h"
#include "LinearMovementPlatform.generated.h"

UCLASS()
class STRIKEZONE_API ALinearMovementPlatform : public AMovingPlatform
{
	GENERATED_BODY()

private:
	FVector StartLocation;
	FVector CurrentLocation;

public:
	ALinearMovementPlatform();

	virtual void MovePlatform(float DeltaTime) override;

	void SetMoveSpeed(const float& Speed) { MoveSpeed = Speed; }
	void SetMaxMoveDistance(const float& Distance) { MaxMoveDistance = Distance; };
	void SetMovementDirection(const FVector& Direction) { MovementDirection = Direction; }

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "MovingPlatform|Settings")
	float MoveSpeed;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "MovingPlatform|Settings")
	float MaxMoveDistance;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "MovingPlatform|Settings")
	FVector MovementDirection;

	virtual void BeginPlay() override;
};
