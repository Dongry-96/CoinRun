#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "DebuffItem.generated.h"

UCLASS()
class STRIKEZONE_API ADebuffItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ADebuffItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float DebuffDuration;

	virtual void ActivateItem(AActor* Activator) override;
};
