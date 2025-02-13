#include "StrikeZonePlayerController.h"
#include "SZ_GameState.h"
#include "SZ_GameInstance.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "PlayerCharacter.h"
#include "Drone.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/TextBlock.h"

AStrikeZonePlayerController::AStrikeZonePlayerController()
	: PlayerInputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	InteractAction(nullptr),
	D_MoveAction(nullptr),
	D_UpDownAction(nullptr),
	D_RollAction(nullptr),
	D_LookAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	EventWarningClass(nullptr),
	EventWarningWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr)
{
}

void AStrikeZonePlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitalizeDefaultInputContext();

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}

void AStrikeZonePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UpdateInputPriority(InPawn);
}

void AStrikeZonePlayerController::InitalizeDefaultInputContext()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (PlayerInputMappingContext)
			{
				Subsystem->AddMappingContext(PlayerInputMappingContext, 0);
			}
		}
	}
}

void AStrikeZonePlayerController::UpdateInputPriority(APawn* ControlledPawn)
{
	if (!ControlledPawn) return;

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (ControlledPawn->IsA<APlayerCharacter>())
			{
				Subsystem->RemoveMappingContext(DroneInputMappingContext);
				Subsystem->AddMappingContext(PlayerInputMappingContext, 0);
			}
			else if (ControlledPawn->IsA<ADrone>())
			{
				Subsystem->RemoveMappingContext(PlayerInputMappingContext);
				Subsystem->AddMappingContext(DroneInputMappingContext, 0);
			}
		}
	}
}

void AStrikeZonePlayerController::ShowGameHUD()
{
	SetCharacterWidgetVisible(true);

	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}
	}
	
	ASZ_GameState* GameState = GetWorld() ? GetWorld()->GetGameState<ASZ_GameState>() : nullptr;
	if (GameState)
	{
		GameState->UpdateHUD();
	}
}

void AStrikeZonePlayerController::ShowWarningEvent(const int32& CurrentWave)
{
	if (CurrentWave == 1) return;

	if (EventWarningWidgetInstance)
	{
		EventWarningWidgetInstance->RemoveFromParent();
		EventWarningWidgetInstance = nullptr;
	}

	if (EventWarningClass)
	{
		EventWarningWidgetInstance = CreateWidget<UUserWidget>(this, EventWarningClass);
		if (EventWarningWidgetInstance)
		{
			EventWarningWidgetInstance->AddToPlayerScreen();

			UFunction* PlayAnimFunc = EventWarningWidgetInstance->FindFunction(FName("PlayEventWarningAnim"));
			if (PlayAnimFunc)
			{
				EventWarningWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* WarningText = Cast<UTextBlock>(EventWarningWidgetInstance->GetWidgetFromName("WarningText")))
			{
				if (CurrentWave == 2)
				{
					WarningText->SetText(FText::FromString(FString(TEXT("Warning: Activate Spike!"))));
				}
				if (CurrentWave == 3)
				{
					WarningText->SetText(FText::FromString(FString(TEXT("Warning: Activate Bomb!"))));
				}
			}

			FTimerHandle EventWarningTimerHandle;
			GetWorldTimerManager().SetTimer(
				EventWarningTimerHandle,
				this,
				&AStrikeZonePlayerController::RemoveEventWarning,
				5.0f,
				false
			);
		}
	}
}

void AStrikeZonePlayerController::RemoveEventWarning()
{
	if (EventWarningWidgetInstance)
	{
		EventWarningWidgetInstance->RemoveFromParent();
		EventWarningWidgetInstance = nullptr;
	}
}

void AStrikeZonePlayerController::ShowMainMenu(bool bIsReStart)
{
	SetCharacterWidgetVisible(false);

	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (EventWarningWidgetInstance)
	{
		EventWarningWidgetInstance->RemoveFromParent();
		EventWarningWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}

	if (UTextBlock* GameOverText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("GameOverText")))
	{
		if (bIsReStart)
		{
			GameOverText->SetText(FText::FromString(TEXT("Game Over!")));
		}
		else
		{
			GameOverText->SetText(FText::FromString(TEXT("Coin Run!")));
		}
	}

	if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
	{
		if (bIsReStart)
		{
			ButtonText->SetText(FText::FromString(TEXT("Restart")));
		}
		else
		{
			ButtonText->SetText(FText::FromString(TEXT("Start")));
		}
	}
	
	if (bIsReStart)
	{
		UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
		if (PlayAnimFunc)
		{
			MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
		}

		if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
		{
			if (USZ_GameInstance* GameInstance = Cast<USZ_GameInstance>(UGameplayStatics::GetGameInstance(this)))
			{
				TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Total Score: %d"), GameInstance->TotalScore)));
			}
		}
	}
}

void AStrikeZonePlayerController::SetCharacterWidgetVisible(bool bVisible)
{
	APawn* ControlledPawn = GetPawn();
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(ControlledPawn))
	{
		PlayerCharacter->SetCharacterWidgetVisible(bVisible);
	}
}

void AStrikeZonePlayerController::StartGame()
{
	if (USZ_GameInstance* GameInstance = Cast<USZ_GameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GameInstance->CurrentLevelIndex = 0;
		GameInstance->CurrentWave = 1;
		GameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);
}

void AStrikeZonePlayerController::QuitGame()
{
	if (!GetWorld()) return;

	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
}
