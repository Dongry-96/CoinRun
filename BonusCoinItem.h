#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionItem.h"
#include "BonusCoinItem.generated.h"

UCLASS()
class STRIKEZONE_API ABonusCoinItem : public ABaseInteractionItem
{
	GENERATED_BODY()

public:
	ABonusCoinItem();

	virtual void ActivateItem(AActor* Activator) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PointValue;
};
