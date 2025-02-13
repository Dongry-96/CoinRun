#pragma once

#include "CoreMinimal.h"
#include "DebuffItem.h"
#include "ReverseKeyItem.generated.h"

class APlayerCharacter;

UCLASS()
class STRIKEZONE_API AReverseKeyItem : public ADebuffItem
{
	GENERATED_BODY()
	
public:
	AReverseKeyItem();

private:
	APlayerCharacter* PlayerCharacter;

protected:
	virtual void BeginPlay() override;
	virtual void ActivateItem(AActor* Activator) override;

	FTimerHandle MoveReverseTimer;

	UFUNCTION()
	void OnWaveEnd();

	void ResetMoveDirection();
};
