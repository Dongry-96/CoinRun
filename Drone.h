#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Drone.generated.h"

class UBoxComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class AStrikeZonePlayerController;
class UEnhancedInputComponent;
class UInputAction;
class APlayerCharacter;
struct FInputActionValue;

UCLASS()
class STRIKEZONE_API ADrone : public APawn
{
	GENERATED_BODY()

private:
	float CurrentMoveSpeed;
	float UpAndDownSpeed;
	float InputUpValue;
	FVector2D InputMoveValue;
	FVector CurrentVelocity;
	FRotator CurrentRotateSpeed;

	bool bIntentionalLook;
	bool bIntentionalRoll;
	bool bIntentionalMove;
	bool bIsOnGround;

	bool IsOnGround(float& GroundZ);

	APlayerCharacter* PlayerCharacter;
public:
	ADrone();

	virtual void Tick(float DeltaTime) override;
	void Interact(APlayerCharacter* Player);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComp;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float Acceleration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RollRateMuliplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float YawRateMuliplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float PitchRateMuliplier;

	UFUNCTION()
	void StartMove(const FInputActionValue& Value);
	UFUNCTION()
	void StopMove(const FInputActionValue& Value);
	UFUNCTION()
	void StartUpDown(const FInputActionValue& Value);
	UFUNCTION()
	void StopUpDown(const FInputActionValue& Value);
	UFUNCTION()
	void StartRoll(const FInputActionValue& Value);
	UFUNCTION()
	void StopRoll(const FInputActionValue& Value);
	UFUNCTION()
	void StartLook(const FInputActionValue& Value);
	UFUNCTION()
	void StopLook(const FInputActionValue& Value);
	UFUNCTION()
	void StopPossess();
};
