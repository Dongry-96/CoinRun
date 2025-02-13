#pragma once

#include "CoreMinimal.h"
#include "DebuffItem.h"
#include "SlipItem.generated.h"

class APlayerCharacter;

UCLASS()
class STRIKEZONE_API ASlipItem : public ADebuffItem
{
	GENERATED_BODY()
	
public:
	ASlipItem();

private:
	APlayerCharacter* PlayerCharacter;

protected:
	virtual void BeginPlay() override;
	virtual void ActivateItem(AActor* Activator) override;

	FTimerHandle SlipTimerHandle;

	UFUNCTION()
	void OnWaveEnd();

	void ResetMovementSettings();
};
