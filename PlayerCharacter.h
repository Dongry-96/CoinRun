#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class UWidgetComponent;
class ABonusCoinItem;
class ADrone;
struct FInputActionValue;

UCLASS()
class STRIKEZONE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	bool bIsPossess;
	bool bIsMoveReverse;
	bool bInputBlocked;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> ThirdPersonCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UWidgetComponent> OverheadWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractionSphere;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const { return Health; }
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetOriginMovementSettings();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetInputBlocked(bool bBlocked) { bInputBlocked = bBlocked; }
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetCharacterWidgetVisible(bool bVisible);
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetStatusIconVisible(FName EffectName, bool bVisible);

private:
	ABonusCoinItem* BonusCoin;
	ADrone* Drone;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultilier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float OriginGourndFriction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float OriginBrakingFrictionFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float OriginBrakingDecelerationWalking;

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
		);

	UFUNCTION(BlueprintPure)
	bool IsPlayerPossess() const { return bIsPossess; }

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);
	UFUNCTION()
	void Interaction(const FInputActionValue& Value);

	void OnDeath();
	void UpdateOverheadHP();
};
