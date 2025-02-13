#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SZ_GameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveEndDelegate);

UCLASS()
class STRIKEZONE_API ASZ_GameState : public AGameState
{
	GENERATED_BODY()
	
private:
	class APlayerCharacter* PlayerCharacter;
	class AStrikeZonePlayerController* PlayerController;

	int32 CountdownTime;

public:
	ASZ_GameState();

	virtual void BeginPlay() override;

	FOnWaveEndDelegate OnWaveEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	float WaveDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWave;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaves;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	TArray<AActor*> SpawnedItems;
	FTimerHandle WaveStartDelayTimerHandle;
	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
	bool bSpikeEventActivated;
	bool bBombEventActivated;
	float RemainingTime;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const { return Score; }
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	void StartLevel();
	void OnWaveTimeUp();
	void OnCoinCollected();
	void StartWave();
	void MovePlayerToStartPosition();
	void BeginWaveAfterDelay();
	void EndWave();
	void EndLevel();
	void CleanupSpawnedItems();

	void UpdateCountdown();
	void ShowCountdownUI(bool bShow);
	void UpdateHUD();
	void StartWaveEvent();
	void SetActivateSpike(bool bIsActivate);
};
