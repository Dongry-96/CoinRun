#pragma once

#include "CoreMinimal.h"
#include "MovingPlatform.h"
#include "FloatingSinePlatform.generated.h"

class UBoxComponent;

UCLASS()
class STRIKEZONE_API AFloatingSinePlatform : public AMovingPlatform
{
	GENERATED_BODY()

private:
	FVector StartLocation;
	float RunningTime;
	FTimerHandle DestroyTimerHandle;

public:
	AFloatingSinePlatform();

	virtual void MovePlatform(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnTriggerPlayerCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void DestroyPlatform();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	TObjectPtr<UBoxComponent> TriggerBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DestroyDelay")
	float DelayTime;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "MovingPlatform|Settings")
	FVector MovementAmplitude;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "MovingPlatform|Settings")
	float MovementFrequency;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "MovingPlatform|Settings")
	float PhaseOffset;


};
