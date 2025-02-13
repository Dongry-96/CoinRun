#include "PlayerCharacter.h"
#include "StrikeZonePlayerController.h"
#include "SZ_GameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BonusCoinItem.h"
#include "Drone.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	Drone = nullptr;
	BonusCoin = nullptr;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TP_Camera"));
	ThirdPersonCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
	OverheadWidget->SetVisibility(false);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBegin);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapEnd);

	bIsPossess = true;
	bIsMoveReverse = false;
	bInputBlocked = false;

	NormalSpeed = 800.0f;
	SprintSpeedMultilier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultilier;
	OriginGourndFriction = 8.0f;
	OriginBrakingFrictionFactor = 2.0f;
	OriginBrakingDecelerationWalking = 2048.0f;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	
	JumpMaxCount = 2;
	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void APlayerCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
}

void APlayerCharacter::OnDeath()
{
	ASZ_GameState* GameState = GetWorld() ? GetWorld()->GetGameState<ASZ_GameState>() : nullptr;
	if (GameState)
	{
		GameState->OnGameOver();
	}
}

void APlayerCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UProgressBar* HP_Bar = Cast<UProgressBar>(OverheadWidgetInstance->GetWidgetFromName("HP_Bar")))
	{
		float HP_Percent = FMath::Clamp(Health / MaxHealth, 0.0f, 1.0f);
		
		HP_Bar->SetPercent(HP_Percent);
	}

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName("OverheadHP")))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Health)));
	}
}

void APlayerCharacter::SetOriginMovementSettings()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	GetCharacterMovement()->GroundFriction = OriginGourndFriction;
	GetCharacterMovement()->BrakingFrictionFactor = OriginBrakingFrictionFactor;
	GetCharacterMovement()->BrakingDecelerationWalking = OriginBrakingDecelerationWalking;
}

void APlayerCharacter::SetCharacterWidgetVisible(bool bVisible)
{
	if (OverheadWidget)
	{
		OverheadWidget->SetVisibility(bVisible);
	}
}

void APlayerCharacter::SetStatusIconVisible(FName EffectIconName, bool bVisible)
{
	if (OverheadWidget)
	{
		UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
		if (OverheadWidgetInstance)
		{
			if (EffectIconName == "SlideIcon")
			{
				if (UImage* SlideIcon = Cast<UImage>(OverheadWidgetInstance->GetWidgetFromName("SlideIcon")))
				{
					if (bVisible)
					{
						SlideIcon->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						SlideIcon->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
			if (EffectIconName == "ReverseIcon")
			{
				if (UImage* ReverseIcon = Cast<UImage>(OverheadWidgetInstance->GetWidgetFromName("ReverseIcon")))
				{
					if (bVisible)
					{
						ReverseIcon->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						ReverseIcon->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
		}
	}
}

float APlayerCharacter::TakeDamage
(
	float DamageAmount, 
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UpdateOverheadHP();

	if (Health <= 0.0f)
	{
		OnDeath();
	}
	return ActualDamage;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverheadHP();
	SetOriginMovementSettings();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AStrikeZonePlayerController* PlayerController = Cast<AStrikeZonePlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StartJump);
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopJump);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StartSprint);
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);
			}
			if (PlayerController->InteractAction)
			{
				EnhancedInput->BindAction(PlayerController->InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interaction);
			}
		}
	}
}

void APlayerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ABonusCoinItem::StaticClass()))
	{
		BonusCoin = Cast<ABonusCoinItem>(OtherActor);
	}

	if (OtherActor && OtherActor->IsA(ADrone::StaticClass()))
	{
		Drone = Cast<ADrone>(OtherActor);
	}
}

void APlayerCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor == BonusCoin)
	{
		BonusCoin = nullptr;
	}

	if (OtherActor && OtherActor == Drone)
	{
		Drone = nullptr;
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;
	if (bInputBlocked) return;

	const FVector2D MoveInput = Value.Get<FVector2D>();
	int ReverseDirection = 1;
	if (bIsMoveReverse)
	{
		ReverseDirection = -1;
	}

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector() * ReverseDirection, MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector() * ReverseDirection, MoveInput.Y);
	}
}

void APlayerCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void APlayerCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller) return;
	if (bInputBlocked) return;

	const FVector2D LookInput = Value.Get<FVector2D>();

	AddControllerPitchInput(LookInput.Y);
	AddControllerYawInput(LookInput.X);
}

void APlayerCharacter::StartSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void APlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void APlayerCharacter::Interaction(const FInputActionValue& Value)
{
	if (BonusCoin)
	{
		BonusCoin->ActivateItem(this);
	}
	if (Drone)
	{
		Drone->Interact(this);
		bIsPossess = false;
	}
}
