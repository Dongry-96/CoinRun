#pragma once

#include "CoreMinimal.h"
#include "MovingPlatform.h"
#include "RotatingPlatform.generated.h"

UCLASS()
class STRIKEZONE_API ARotatingPlatform : public AMovingPlatform
{
	GENERATED_BODY()

public:
	ARotatingPlatform();

	virtual void MovePlatform(float DeltaTime) override;

	void SetRotationSpeed(const float& Speed) { RotationSpeed = Speed; }

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "MovingPlatform|Settings")
	FRotator RotationAxis;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "MovingPlatform|Settings")
	float RotationSpeed;
	
};
