#include "StrikeZoneGameMode.h"
#include "PlayerCharacter.h"
#include "StrikeZonePlayerController.h"
#include "SZ_GameState.h"

AStrikeZoneGameMode::AStrikeZoneGameMode()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = AStrikeZonePlayerController::StaticClass();
	GameStateClass = ASZ_GameState::StaticClass();
}
