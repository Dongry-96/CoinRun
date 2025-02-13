#include "LinearMovementPlatform.h"

ALinearMovementPlatform::ALinearMovementPlatform()
{
	MovementDirection = FVector(0.0f, 1.0f, 0.0f);
	MovementDirection.Normalize();
	MoveSpeed = 200.0f;
	MaxMoveDistance = 1000.0f;

	StartLocation = GetActorLocation();
}

void ALinearMovementPlatform::MovePlatform(float DeltaTime)
{
	CurrentLocation = GetActorLocation();
	FVector NewLocation = CurrentLocation + DeltaTime * MoveSpeed * MovementDirection;

	if (FVector::Dist(StartLocation, NewLocation) >= MaxMoveDistance)
	{
		MovementDirection *= -1;
		StartLocation = NewLocation;
	}

	SetActorLocation(NewLocation);
}

void ALinearMovementPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
}
