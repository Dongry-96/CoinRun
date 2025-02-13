#include "BaseInteractionItem.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"

ABaseInteractionItem::ABaseInteractionItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Scene);

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(StaticMesh);
	InteractWidget->SetWidgetSpace(EWidgetSpace::World);
	InteractWidget->SetVisibility(false);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(Scene);
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseInteractionItem::OnOverlapBegin);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseInteractionItem::OnOverlapEnd);

	Particle = nullptr;
}

void ABaseInteractionItem::BeginPlay()
{
	Super::BeginPlay();

	if (InteractWidget)
	{
		InteractWidget->SetVisibility(false);
	}
}


void ABaseInteractionItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tick Activate"));
		UpdateUIRotation();
	}
}

void ABaseInteractionItem::ActivateItem(AActor* Activator)
{
	if (PickupParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PickupParticle,
			GetActorLocation(),
			GetActorRotation(),
			true
		);
	}

	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			this,
			&ABaseInteractionItem::DestroyParticle,
			2.0f,
			false
		);
	}

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation()
		);
	}
}

void ABaseInteractionItem::DestroyItem()
{
	Destroy();
}

void ABaseInteractionItem::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		InteractWidget->SetVisibility(true);
		PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	}
}

void ABaseInteractionItem::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		InteractWidget->SetVisibility(false);
		PlayerCharacter = nullptr;
	}
}

void ABaseInteractionItem::UpdateUIRotation()
{
	if (PlayerCharacter && InteractWidget)
	{
		FVector PlayerLocation = PlayerCharacter->GetActorLocation();
		FVector WidgetLocation = InteractWidget->GetComponentLocation();

		FRotator LookAtRotation = (PlayerLocation - WidgetLocation).Rotation();
		InteractWidget->SetWorldRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f));
	}
}

void ABaseInteractionItem::DestroyParticle()
{
	if (Particle)
	{
		Particle->DestroyComponent();
	}
}
