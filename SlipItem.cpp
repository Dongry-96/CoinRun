#include "SlipItem.h"
#include "PlayerCharacter.h"
#include "StrikeZonePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SZ_GameState.h"

ASlipItem::ASlipItem()
{
	DebuffDuration = 5.0f;
	ItemType = "SlipItem";
	PlayerCharacter = nullptr;
}

void ASlipItem::BeginPlay()
{
	Super::BeginPlay();

	// GameState의 Wave 종료 델리게이트 구독
	if (ASZ_GameState* GameState = GetWorld()->GetGameState<ASZ_GameState>())
	{
		GameState->OnWaveEnd.AddDynamic(this, &ASlipItem::OnWaveEnd);
	}
}

void ASlipItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		PlayerCharacter = Cast<APlayerCharacter>(Activator);
		if (PlayerCharacter)
		{
			UCharacterMovementComponent* MovementComp = PlayerCharacter->GetCharacterMovement();
			if (MovementComp)
			{
				MovementComp->MaxWalkSpeed = 2000.0f;
				MovementComp->GroundFriction = 0.1f;
				MovementComp->BrakingFrictionFactor = 0.1f;
				MovementComp->BrakingDecelerationWalking = 0.1f;
			}

			PlayerCharacter->SetStatusIconVisible("SlideIcon", true);
		}

		float RemainingTime = GetWorldTimerManager().GetTimerRemaining(SlipTimerHandle);
		if (RemainingTime > 0.0f)
		{
			GetWorldTimerManager().SetTimer(
				SlipTimerHandle,
				this,
				&ASlipItem::ResetMovementSettings,
				RemainingTime + DebuffDuration,
				false
			);
		}
		else
		{
			GetWorldTimerManager().SetTimer(
				SlipTimerHandle,
				this,
				&ASlipItem::ResetMovementSettings,
				DebuffDuration,
				false
			);
		}
		
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

void ASlipItem::OnWaveEnd()
{
	ResetMovementSettings();
}

void ASlipItem::ResetMovementSettings()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SetOriginMovementSettings();
		PlayerCharacter->SetStatusIconVisible("SlideIcon", false);
		DestroyItem();
	}
}
