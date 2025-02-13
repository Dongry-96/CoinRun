#include "BombObstacle.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ABombObstacle::ABombObstacle()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);

	// 물리 활성화
	StaticMesh->SetSimulatePhysics(true);
	// 중력 활성화
	StaticMesh->SetEnableGravity(true);
	// HitEvent 활성화(NotifyHit에 알림)
	StaticMesh->SetNotifyRigidBodyCollision(true);

	DamageAmount = 30.0f;
	ExplosionEffect = nullptr;
	ExplosionSound = nullptr;
	ExplosionParticle = nullptr;
}

void ABombObstacle::NotifyHit(
	UPrimitiveComponent* MyComp,
	AActor* Other, 
	UPrimitiveComponent* OtherComp,
	bool bSelfMoved,
	FVector HitLocation,
	FVector HitNormal,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (Other && Other != this)
	{
		if (Other->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Other,
				DamageAmount,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}

		Explode();
	}
}

void ABombObstacle::Explode()
{
	if (ExplosionEffect)
	{
		ExplosionParticle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionEffect,
			GetActorLocation(),
			GetActorRotation(),
			false
		);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}
	
	if (ExplosionParticle)
	{
		FTimerHandle ExplosionTimerHandle;
		GetWorldTimerManager().SetTimer(
			ExplosionTimerHandle,
			this,
			&ABombObstacle::DestroyParticle,
			2.0f,
			false
		);
	}

	Destroy();
}

void ABombObstacle::DestroyParticle()
{
	if (ExplosionParticle)
	{
		ExplosionParticle->DestroyComponent();
		ExplosionParticle = nullptr;
	}
}



