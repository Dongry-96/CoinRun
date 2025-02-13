#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UCLASS(Abstract)
class STRIKEZONE_API AMovingPlatform : public AActor
{
	GENERATED_BODY()	

public:	
	AMovingPlatform();

	virtual void Tick(float DeltaTime) override;
	virtual void MovePlatform(float DeltaTime) PURE_VIRTUAL(AMovingPlatform::MovePlatform, );
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MovingPlatform|Components")
	TObjectPtr<USceneComponent> SceneRoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MovingPlatform|Components")
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	virtual void BeginPlay() override;
};
