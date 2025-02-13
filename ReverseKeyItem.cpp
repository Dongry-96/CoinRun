#include "ReverseKeyItem.h"
#include "PlayerCharacter.h"
#include "SZ_GameState.h"

AReverseKeyItem::AReverseKeyItem()
{
	DebuffDuration = 3.0f;
	ItemType = "ReverseKeyItem";
	PlayerCharacter = nullptr;
}

void AReverseKeyItem::BeginPlay()
{
	Super::BeginPlay();

	if (ASZ_GameState* GameState = GetWorld()->GetGameState<ASZ_GameState>())
	{
		GameState->OnWaveEnd.AddDynamic(this, &AReverseKeyItem::OnWaveEnd);
	}
}

void AReverseKeyItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		PlayerCharacter = Cast<APlayerCharacter>(Activator);
		if (PlayerCharacter)
		{
			PlayerCharacter->bIsMoveReverse = true;
			PlayerCharacter->SetStatusIconVisible("ReverseIcon", true);
		}

		float RemainingTime = GetWorldTimerManager().GetTimerRemaining(MoveReverseTimer);
		if (RemainingTime > 0.0f)
		{
			GetWorldTimerManager().SetTimer(
				MoveReverseTimer,
				this,
				&AReverseKeyItem::ResetMoveDirection,
				RemainingTime + DebuffDuration,
				false
			);
		}
		else
		{
			GetWorldTimerManager().SetTimer(
				MoveReverseTimer,
				this,
				&AReverseKeyItem::ResetMoveDirection,
				DebuffDuration,
				false
			);
		}
		
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

void AReverseKeyItem::OnWaveEnd()
{
	ResetMoveDirection();
}

void AReverseKeyItem::ResetMoveDirection()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->bIsMoveReverse = false;
		PlayerCharacter->SetStatusIconVisible("ReverseIcon", false);
	}
}
