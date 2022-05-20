// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceConstant.h"
#include "BreakableWall.generated.h"

UCLASS()
class BOMBERMANJSONMAPREADER_API ABreakableWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakableWall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(replicated,EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* StaticMeshComponent;

	UFUNCTION(Server, Reliable)
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
	const FString BombermanMeshPath = FString("StaticMesh'/Game/BombermanMapAsset/Mesh/BreakableWallMesh.BreakableWallMesh'");
	const FString BombermanBreakableWallMatPath = FString("MaterialInstanceConstant'/Game/BombermanMapAsset/Material/BreakableMat.BreakableMat'");
};
