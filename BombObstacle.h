#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombObstacle.generated.h"

UCLASS()
class STRIKEZONE_API ABombObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	ABombObstacle();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* ExplosionEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float DamageAmount;

	void NotifyHit(
		class UPrimitiveComponent* MyComp,
		AActor* Other,
		class UPrimitiveComponent* OtherComp,
		bool bSelfMoved,
		FVector HitLocation,
		FVector HitNormal,
		FVector NormalImpulse,
		const FHitResult& Hit);
	
	void Explode();
	void DestroyParticle();

	UParticleSystemComponent* ExplosionParticle;

};
