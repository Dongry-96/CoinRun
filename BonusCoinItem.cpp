#include "BonusCoinItem.h"
#include "Engine/World.h"
#include "SZ_GameState.h"

ABonusCoinItem::ABonusCoinItem()
{
	PointValue = 100;
	ItemType = "BonusCoinItem";
}

void ABonusCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ASZ_GameState* GameState = Cast<ASZ_GameState>(World->GetGameState<ASZ_GameState>()))
			{
				GameState->AddScore(PointValue);
			}
		}
		DestroyItem();
	}
}
