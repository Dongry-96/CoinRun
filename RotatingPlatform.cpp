#include "RotatingPlatform.h"

ARotatingPlatform::ARotatingPlatform()
{
	RotationAxis = FRotator(0.0f, 1.0f, 0.0f);
	RotationSpeed = 200.0f;
}

void ARotatingPlatform::MovePlatform(float DeltaTime)
{
	AddActorLocalRotation(RotationAxis * RotationSpeed * DeltaTime);
}
