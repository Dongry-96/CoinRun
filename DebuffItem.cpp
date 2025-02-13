#include "DebuffItem.h"

ADebuffItem::ADebuffItem()
{
	DebuffDuration = 0.0f;
	ItemType = "DefaultDebuffItem";
}

void ADebuffItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

}
