#include "Drone.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.h"
#include "StrikeZonePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ADrone::ADrone()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxRoot"));
	SetRootComponent(BoxCollision);
	BoxCollision->SetCollisionProfileName("Pawn");

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(BoxCollision);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 500.0f;
	SpringArmComp->bUsePawnControlRotation = false;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritRoll = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	Acceleration = 1.0f;
	MoveSpeed = 1500.0f;
	UpAndDownSpeed = 400.0f;
	CurrentVelocity = FVector::ZeroVector;
	CurrentRotateSpeed = FRotator::ZeroRotator;

	RollRateMuliplier = 100.0f;
	YawRateMuliplier = 70.0f;
	PitchRateMuliplier = 100.0f;

	bIntentionalLook = false;
	bIntentionalRoll = false;
	bIntentionalMove = false;
	bIsOnGround = true;
}

void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 중력 작용
	float GroundZ = 0.0f;
	bIsOnGround = IsOnGround(GroundZ);
	if (bIsOnGround) // 지상일 경우
	{
		MoveSpeed = 1600.0f;
		// 지상에 착지
		FVector ActorLocation = GetActorLocation();
		if (FMath::IsNearlyZero(InputUpValue))
		{
			ActorLocation.Z = FMath::FInterpTo(ActorLocation.Z, GroundZ + 30.0f, DeltaTime, 2.0f);
			SetActorLocation(ActorLocation);
			CurrentVelocity.Z = 0.0f;
		}
		// 지상에서 Pitch와 Roll값 초기화
		FRotator CurrentRotation = GetActorRotation();
		float ZeroPitch = FMath::FInterpTo(CurrentRotation.Pitch, 0.0f, DeltaTime, 2.0f);
		float ZeroRoll = FMath::FInterpTo(CurrentRotation.Roll, 0.0, DeltaTime, 2.0f);
		FRotator NewRotation(ZeroPitch, CurrentRotation.Yaw, ZeroRoll);
		SetActorRotation(NewRotation);
	}
	else // 공중일 경우
	{
		MoveSpeed = 800.0f;
		// 상승 입력 없을 때, 중력 적용
		if (InputUpValue <= 0.0f)
		{
			const float GravityForce = -980.0f;
			float TargetVelocityZ = FMath::Clamp(CurrentVelocity.Z + GravityForce * DeltaTime, -1500.0f, MoveSpeed);
			CurrentVelocity.Z = FMath::FInterpTo(CurrentVelocity.Z, TargetVelocityZ, DeltaTime, 30.0f);
		}
		// 상승 입력 있을 때
		else
		{
			CurrentVelocity.Z = 0.0f;
		}
	}

	// 입력 없을 시, 이동 속도 복원
	if (InputMoveValue.X == 0.0f)
	{
		CurrentVelocity.X = FMath::FInterpTo(CurrentVelocity.X, 0.0f, DeltaTime, 0.30f);
	}
	if (InputMoveValue.Y == 0.0f)
	{
		CurrentVelocity.Y = FMath::FInterpTo(CurrentVelocity.Y, 0.0f, DeltaTime, 0.30f);
	}

	// 입력 없을 시, 회전 속도 복원 
	if (!bIntentionalLook)
	{
		CurrentRotateSpeed.Pitch = FMath::FInterpTo(CurrentRotateSpeed.Pitch, 0.0f, DeltaTime, 10.0f);
		CurrentRotateSpeed.Yaw = FMath::FInterpTo(CurrentRotateSpeed.Yaw, 0.0f, DeltaTime, 10.0f);
	}
	if (!bIntentionalRoll)
	{
		CurrentRotateSpeed.Roll = FMath::FInterpTo(CurrentRotateSpeed.Roll, 0.0f, DeltaTime, 10.0f);
		if (FMath::IsNearlyZero(CurrentRotateSpeed.Roll))
		{
			FRotator CurrentRotation = GetActorRotation();
			CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, 0.0f, DeltaTime, 1.0f);
			SetActorRotation(CurrentRotation);
		}
	}

	// 회전 업데이트
	FRotator DeltaRotation = CurrentRotateSpeed * DeltaTime;
	AddActorLocalRotation(DeltaRotation);
	SkeletalMeshComp->SetRelativeRotation(DeltaRotation);

	// 평면 이동 업데이트
	FVector LocalMove(CurrentVelocity.X * DeltaTime, CurrentVelocity.Y * DeltaTime, 0.0f);
	AddActorLocalOffset(LocalMove, true);

	// 수직 이동 업데이트 (로컬 z축에서 월드 z축으로 변환)
	FVector LocalUpMovement = FVector::ZeroVector;
	if (!FMath::IsNearlyZero(InputUpValue))
	{
		LocalUpMovement = GetActorUpVector() * InputUpValue * UpAndDownSpeed * DeltaTime;
	}
	FVector NewLocation = GetActorLocation() + LocalUpMovement;
	NewLocation.Z += CurrentVelocity.Z * DeltaTime;
	SetActorLocation(NewLocation, true);

	// 속도 디버깅
	GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Blue, FString::Printf(TEXT("CurrentXSpeed: %f"), CurrentVelocity.X));
	GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Blue, FString::Printf(TEXT("CurrentYSpeed: %f"), CurrentVelocity.Y));
	GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Blue, FString::Printf(TEXT("GravitySpeed: %f"), CurrentVelocity.Z));
}

void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AStrikeZonePlayerController* PlayerController = Cast<AStrikeZonePlayerController>(GetController()))
		{
			if (PlayerController->D_MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->D_MoveAction, ETriggerEvent::Triggered, this, &ADrone::StartMove);
				EnhancedInput->BindAction(PlayerController->D_MoveAction, ETriggerEvent::Completed, this, &ADrone::StopMove);
			}
			if (PlayerController->D_UpDownAction)
			{
				EnhancedInput->BindAction(PlayerController->D_UpDownAction, ETriggerEvent::Triggered, this, &ADrone::StartUpDown);
				EnhancedInput->BindAction(PlayerController->D_UpDownAction, ETriggerEvent::Completed, this, &ADrone::StopUpDown);
			}
			if (PlayerController->D_RollAction)
			{
				EnhancedInput->BindAction(PlayerController->D_RollAction, ETriggerEvent::Triggered, this, &ADrone::StartRoll);
				EnhancedInput->BindAction(PlayerController->D_RollAction, ETriggerEvent::Completed, this, &ADrone::StopRoll);
			}
			if (PlayerController->D_LookAction)
			{
				EnhancedInput->BindAction(PlayerController->D_LookAction, ETriggerEvent::Triggered, this, &ADrone::StartLook);
				EnhancedInput->BindAction(PlayerController->D_LookAction, ETriggerEvent::Completed, this, &ADrone::StopLook);
			}
			if (PlayerController->D_UnpossessAction)
			{
				EnhancedInput->BindAction(PlayerController->D_UnpossessAction, ETriggerEvent::Triggered, this, &ADrone::StopPossess);
			}
		}
	}
}

void ADrone::StartMove(const FInputActionValue& Value)
{
	if (!Controller) return;

	InputMoveValue = Value.Get<FVector2D>();
	if (InputMoveValue.IsNearlyZero()) return;

	bIntentionalMove = true;

	if (!FMath::IsNearlyZero(InputMoveValue.X))
	{
		float TargetSpeed = InputMoveValue.X * MoveSpeed;
		CurrentVelocity.X = FMath::FInterpTo(CurrentVelocity.X, TargetSpeed, GetWorld()->GetDeltaSeconds(), 1.0f);
	}

	if (!FMath::IsNearlyZero(InputMoveValue.Y))
	{
		float TargetSpeed = InputMoveValue.Y * MoveSpeed;
		CurrentVelocity.Y = FMath::FInterpTo(CurrentVelocity.Y, TargetSpeed, GetWorld()->GetDeltaSeconds(), 1.0f);
	}
}

void ADrone::StopMove(const FInputActionValue& Value)
{
	if (!Controller) return;
	bIntentionalMove = true;
	InputMoveValue = Value.Get<FVector2D>();
}

void ADrone::StartUpDown(const FInputActionValue& Value)
{
	if (!Controller) return;

	InputUpValue = Value.Get<float>();
}

void ADrone::StopUpDown(const FInputActionValue& Value)
{
	if (!Controller) return;

	InputUpValue = 0.0f;
}

void ADrone::StartRoll(const FInputActionValue& Value)
{
	if (!Controller) return;

	float InputValue = Value.Get<float>();
	if (!FMath::IsNearlyZero(InputValue))
	{
		bIntentionalRoll = true;
		const float TargetRollSpeed = InputValue * RollRateMuliplier;
		CurrentRotateSpeed.Roll = FMath::FInterpTo(CurrentRotateSpeed.Roll, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.0f);
	}
}

void ADrone::StopRoll(const FInputActionValue& Value)
{
	bIntentionalRoll = false;
}

void ADrone::StartLook(const FInputActionValue& Value)
{
	if (!Controller) return;

	FVector2D InputValue = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(InputValue.X))
	{
		bIntentionalLook = true;
		const float TargetYawSpeed = InputValue.X * YawRateMuliplier;
		CurrentRotateSpeed.Yaw = FMath::FInterpTo(CurrentRotateSpeed.Yaw, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.0f);
	}

	if (!FMath::IsNearlyZero(InputValue.Y))
	{
		bIntentionalLook = true;
		const float TargetPitchSpeed = InputValue.Y * PitchRateMuliplier;
		CurrentRotateSpeed.Pitch = FMath::FInterpTo(CurrentRotateSpeed.Pitch, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.0f);
	}
}

void ADrone::StopLook(const FInputActionValue& Value)
{
	bIntentionalLook = false;
}

void ADrone::StopPossess()
{
	AController* PlayerController = GetController();
	if (PlayerController)
	{
		PlayerController->UnPossess();
		PlayerController->Possess(PlayerCharacter);
		PlayerCharacter->bIsPossess = true;
	}
}

void ADrone::Interact(APlayerCharacter* Player)
{
	PlayerCharacter = Player;
	AController* PlayerController = Player->GetController();
	if (PlayerController)
	{
		PlayerController->Possess(this);
	}
}

bool ADrone::IsOnGround(float& GroundZ)
{
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.0f, 0.0f, 80.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldDynamic, Params);
	GroundZ = HitResult.ImpactPoint.Z;
	
	return bHit;
}
