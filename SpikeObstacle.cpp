#include "SpikeObstacle.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ASpikeObstacle::ASpikeObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(Scene);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ASpikeObstacle::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ASpikeObstacle::OnOverlapEnd);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Scene);

	DamageAmount = 20.0f;

	ForwardMoveSpeed = 700.0f;
	BackwardMoveSpeed = 100.0f;
	MaxMoveDistance = 150.0f;
	MoveDirection = 1;

	bIsActivate = false;
	bHasHitPlayer = false;
}

void ASpikeObstacle::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();
}

void ASpikeObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActivate)
	{
		MoveSpike(DeltaTime);
	}
}

void ASpikeObstacle::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasHitPlayer) return;

	TArray<AActor*> OverlappingActors;
	BoxCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(Actor, DamageAmount, nullptr, this, UDamageType::StaticClass());
			bHasHitPlayer = true;
		}
	}
}

void ASpikeObstacle::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bHasHitPlayer = false;
}

void ASpikeObstacle::MoveSpike(float DeltaTime)
{
	float MoveSpeed;
	if (MoveDirection == 1)
	{
		MoveSpeed = ForwardMoveSpeed;
	}
	else
	{
		MoveSpeed = BackwardMoveSpeed;
	}
	const float DeltaMovement = MoveSpeed * DeltaTime * MoveDirection;

	const float CurrentZ = GetActorLocation().Z;
	float NewZ = CurrentZ + DeltaMovement;
	
	const float DistanceFromStart = FMath::Abs(NewZ - StartLocation.Z);
	if (DistanceFromStart > MaxMoveDistance)
	{
		const float OverShoot = DistanceFromStart - MaxMoveDistance;
		NewZ -= OverShoot * MoveDirection;
		MoveDirection *= -1;
	}

	FVector NewLocation = GetActorLocation();
	NewLocation.Z = NewZ;

	SetActorLocation(NewLocation);
	
}




