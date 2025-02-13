#include "SZ_GameState.h"
#include "SZ_GameInstance.h"
#include "StrikeZonePlayerController.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "ItemSpawnVolume.h"
#include "BombSpawnManager.h"
#include "CoinItem.h"
#include "MineItem.h"
#include "Components/TextBlock.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Blueprint/UserWidget.h"
#include "SpikeObstacle.h"

ASZ_GameState::ASZ_GameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	WaveDuration = 10.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	CurrentWave = 1;
	MaxWaves = 3;
	bSpikeEventActivated = false;
}

void ASZ_GameState::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<AStrikeZonePlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());
	}

	if (CurrentWave == 1)
	{
		StartLevel();
	}
}

void ASZ_GameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USZ_GameInstance* SZ_GameInstance = Cast<USZ_GameInstance>(GameInstance);
		if (SZ_GameInstance)
		{
			SZ_GameInstance->AddToScore(Amount);
		}
	}
}

void ASZ_GameState::OnCoinCollected()
{
	CollectedCoinCount++;

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndWave();
	}
}

void ASZ_GameState::StartLevel()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USZ_GameInstance* SZ_GameInstance = Cast<USZ_GameInstance>(GameInstance);
		if (SZ_GameInstance)
		{
			CurrentLevelIndex = SZ_GameInstance->CurrentLevelIndex;
			CurrentWave = SZ_GameInstance->CurrentWave;
		}
	}

	if (PlayerController)
	{
		PlayerController->ShowGameHUD();
	}

	bSpikeEventActivated = false;
	bBombEventActivated = false;
	StartWave();
}

void ASZ_GameState::StartWave()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USZ_GameInstance* SZ_GameInstance = Cast<USZ_GameInstance>(GameInstance);
		if (SZ_GameInstance)
		{
			CurrentLevelIndex = SZ_GameInstance->CurrentLevelIndex;
			CurrentWave = SZ_GameInstance->CurrentWave;
		}
	}

	MovePlayerToStartPosition();

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemSpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = CurrentWave * 30;
	for (int32 i = 0; i < ItemToSpawn; ++i)
	{
		if (FoundVolumes.Num() > 0)
		{
			AItemSpawnVolume* SpawnVolume = Cast<AItemSpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				SpawnedItems.Add(SpawnedActor);
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}

			}
		}
	}

	UpdateHUD();
	ShowCountdownUI(true);

	CountdownTime = 3;
	UpdateCountdown();

	GetWorldTimerManager().SetTimer(
		WaveStartDelayTimerHandle,
		this,
		&ASZ_GameState::UpdateCountdown,
		1.0f,
		true
	);
}

void ASZ_GameState::MovePlayerToStartPosition()
{
	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	if (PlayerStart)
	{
		if (PlayerCharacter)
		{
			PlayerCharacter->SetActorLocation(PlayerStart->GetActorLocation());
			PlayerCharacter->SetActorRotation(PlayerStart->GetActorRotation());

			PlayerCharacter->SetInputBlocked(true);

			if (PlayerCharacter->ActorHasTag("Player"))
			{
				PlayerCharacter->Tags.Remove("Player");
			}
		}
		if (PlayerController)
		{
			PlayerController->SetControlRotation(PlayerStart->GetActorRotation());
		}
	}
}

void ASZ_GameState::BeginWaveAfterDelay()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SetInputBlocked(false);
		if (!PlayerCharacter->ActorHasTag("Player"))
		{
			PlayerCharacter->Tags.Add("Player");
		}
	}

	GetWorldTimerManager().SetTimer
	(
		WaveTimerHandle,
		this,
		&ASZ_GameState::OnWaveTimeUp,
		WaveDuration,
		false
	);

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASZ_GameState::UpdateHUD,
		0.1f,
		true
	);

	
}

void ASZ_GameState::OnWaveTimeUp()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USZ_GameInstance* SZ_GameInstance = Cast<USZ_GameInstance>(GameInstance))
		{
			if (SZ_GameInstance->CurrentWave < MaxWaves)
			{
				EndWave();
			}
			else
			{
				EndLevel();
			}
		}
	}
}

void ASZ_GameState::EndWave()
{
	// Wave 종료 델리게이트 브로드캐스트
	OnWaveEnd.Broadcast();

	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USZ_GameInstance* SZ_GameInstance = Cast<USZ_GameInstance>(GameInstance);
		if (SZ_GameInstance)
		{
			SZ_GameInstance->CurrentWave++;

			if (CurrentWave >= MaxWaves)
			{
				EndLevel();
			}

			// 월드 상의 아이템 정리
			CleanupSpawnedItems();

			StartWave();
		}
	}
}

void ASZ_GameState::EndLevel()
{
	// 타이머 및 스포된 액터 정리
	GetWorldTimerManager().ClearAllTimersForObject(this);
	CleanupSpawnedItems();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USZ_GameInstance* SZ_GameInstance = Cast<USZ_GameInstance>(GameInstance);
		if (SZ_GameInstance)
		{
			CurrentLevelIndex++;
			SZ_GameInstance->CurrentLevelIndex = CurrentLevelIndex;
			SZ_GameInstance->CurrentWave = 1;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}
	
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASZ_GameState::CleanupSpawnedItems()
{
	for (AActor* Item : SpawnedItems)
	{
		if (IsValid(Item) && !Item->IsPendingKillPending())
		{
			GetWorldTimerManager().ClearAllTimersForObject(Item);
			Item->Destroy();
		}
	}
	GEngine->ForceGarbageCollection(true);
	SpawnedItems.Empty();
}

void ASZ_GameState::OnGameOver()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	if (PlayerController)
	{
		PlayerController->SetPause(true);
		PlayerController->ShowMainMenu(true);
	}
}

void ASZ_GameState::UpdateCountdown()
{
	if (PlayerController)
	{
		if (UUserWidget* HUDWidget = PlayerController->GetHUDWidget())
		{
			UFunction* PlayAnimFunc = HUDWidget->FindFunction(FName("PlayCountdownAnim"));
			if (PlayAnimFunc)
			{
				HUDWidget->ProcessEvent(PlayAnimFunc, nullptr);
			}
			if (UTextBlock * CountdownText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CountdownText"))))
			{
				CountdownText->SetText(FText::FromString(FString::Printf(TEXT("%d"), CountdownTime)));
			}
		}
	}

	if (CountdownTime <= 0)
	{
		ShowCountdownUI(false);

		GetWorldTimerManager().ClearTimer(WaveStartDelayTimerHandle);
		BeginWaveAfterDelay();
	}
	else
	{
		CountdownTime--;
	}
}

void ASZ_GameState::ShowCountdownUI(bool bShow)
{
	if (PlayerController)
	{
		if (UUserWidget* HUDWidget = PlayerController->GetHUDWidget())
		{
			if (UTextBlock* CountdownText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CountdownText"))))
			{
				CountdownText->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			}
		}
	}
}

void ASZ_GameState::UpdateHUD()
{
	if (PlayerController)
	{
		if (!IsValid(GetWorld()))
		{
			GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);
			return;
		}

		if (UUserWidget* HUDWidget = PlayerController->GetHUDWidget())
		{
			if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
			{
				if (GetWorldTimerManager().IsTimerActive(WaveTimerHandle))
				{
					RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
				}
				else
				{
					RemainingTime = WaveDuration;
				}
				
				TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
			}

			if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
			{
				if (UGameInstance* GameInstance = GetGameInstance())
				{
					USZ_GameInstance* SZ_GameInstance = Cast<USZ_GameInstance>(GameInstance);
					if (SZ_GameInstance)
					{
						ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SZ_GameInstance->TotalScore)));
					}
				}
			}

			if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
			{
				LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
			}

			if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
			{
				WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWave)));
			}

			if (UTextBlock* CollectCoinText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CollectCoinText"))))
			{
				CollectCoinText->SetText(FText::FromString(FString::Printf(TEXT("Coin:  %d"), CollectedCoinCount)));
			}

			if (UTextBlock* SpawnCoinText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("SpawnCoinText"))))
			{
				SpawnCoinText->SetText(FText::FromString(FString::Printf(TEXT("%d"), SpawnedCoinCount)));
			}
		}
	}

	if (GetWorldTimerManager().IsTimerActive(WaveTimerHandle))
	{
		StartWaveEvent();
	}
}

void ASZ_GameState::StartWaveEvent()
{
	if (RemainingTime > WaveDuration / 2) return;

	if (!bSpikeEventActivated && CurrentWave == 2)
	{
		SetActivateSpike(true);
		bSpikeEventActivated = true;

		if (PlayerController)
		{
			PlayerController->ShowWarningEvent(CurrentWave);
		}
	}

	if (!bBombEventActivated &&CurrentWave == 3)
	{
		TArray<AActor*> FoundVolumes2;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABombSpawnManager::StaticClass(), FoundVolumes2);

		ABombSpawnManager* SpawnManager = Cast<ABombSpawnManager>(FoundVolumes2[0]);
		if (SpawnManager)
		{
			SpawnManager->StartBombsEvent();
			bBombEventActivated = true;
		}

		if (PlayerController)
		{
			PlayerController->ShowWarningEvent(CurrentWave);
		}
	}
}

void ASZ_GameState::SetActivateSpike(bool bIsActivate)
{
	TArray<AActor*> SpikeObstacles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpikeObstacle::StaticClass(), SpikeObstacles);
	for (AActor* Actor : SpikeObstacles)
	{
		if (ASpikeObstacle* Spike = Cast<ASpikeObstacle>(Actor))
		{
			Spike->bIsActivate = bIsActivate;
		}
	}
}
