#include "FloatingSinePlatform.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"

AFloatingSinePlatform::AFloatingSinePlatform()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloatingSinePlatform::OnTriggerPlayerCharacter);
	
	RunningTime = 0.0f;
	MovementAmplitude = FVector(0.0f, 0.0f, 30.0f);
	MovementFrequency = 0.5f;
	PhaseOffset = 0.0f;
	DelayTime = 3.0f;
}

void AFloatingSinePlatform::MovePlatform(float DeltaTime)
{
	RunningTime += DeltaTime;
	
	FVector NewLocation = StartLocation;

	NewLocation.Z += FMath::Sin((RunningTime * MovementFrequency * 2 * PI) + PhaseOffset) * MovementAmplitude.Z;

	SetActorLocation(NewLocation);
}

void AFloatingSinePlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	if (PhaseOffset == 0.0f)
	{
		PhaseOffset = FMath::FRandRange(0.0f, 2 * PI);
	}
}

void AFloatingSinePlatform::OnTriggerPlayerCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AFloatingSinePlatform::DestroyPlatform, DelayTime);
	}
}

void AFloatingSinePlatform::DestroyPlatform()
{
	Destroy();
}
