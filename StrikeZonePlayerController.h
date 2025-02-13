#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StrikeZonePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class STRIKEZONE_API AStrikeZonePlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	void InitalizeDefaultInputContext();
	void UpdateInputPriority(APawn* ControlledPawn);
	void RemoveEventWarning();
	void SetCharacterWidgetVisible(bool bVisible);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	AStrikeZonePlayerController();

	// 캐릭터 인풋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInput")
	TObjectPtr<UInputMappingContext> PlayerInputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInput")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInput")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInput")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInput")
	TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInput")
	TObjectPtr<UInputAction> InteractAction;

	// 드론 인풋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput")
	TObjectPtr<UInputMappingContext> DroneInputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput")
	TObjectPtr<UInputAction> D_MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput")
	TObjectPtr<UInputAction> D_UpDownAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput")
	TObjectPtr<UInputAction> D_RollAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput")
	TObjectPtr<UInputAction> D_LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput")
	TObjectPtr<UInputAction> D_UnpossessAction;

	
	// UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UUserWidget* HUDWidgetInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> EventWarningClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UUserWidget* EventWarningWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	UUserWidget* MainMenuWidgetInstance;

	UFUNCTION(BlueprintPure, Category = "HUD")
	UUserWidget* GetHUDWidget() const { return HUDWidgetInstance; }
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowWarningEvent(const int32& CurrentWave);

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainMenu(bool bIsReStart);
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void QuitGame();

};