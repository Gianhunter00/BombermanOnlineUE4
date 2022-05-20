// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyThirdPersonBombermanCharacter.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Bomb.generated.h"

UCLASS()
class BOMBERMAN_API ABomb : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABomb();

	UPROPERTY(replicated, EditAnywhere, Category = BombSettings)
	float TimerExplosion;

	UPROPERTY(replicated, EditAnywhere, Category = Mesh)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(replicated)
	AMyThirdPersonBombermanCharacter* CharacterOwner;

	UPROPERTY(replicated, EditAnywhere, Category = BombSettings)
	UParticleSystem* ExplosionParticle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void CheckExplosion(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Explode(float DeltaTime);

	UFUNCTION(NetMulticast, UnReliable)
	void SpawnEmitter();
	
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;


	FORCEINLINE void SetCharacterOwner(AMyThirdPersonBombermanCharacter* InOwner) { CharacterOwner = InOwner; }

private:
	const FString BombermanBombMeshPath = FString("StaticMesh'/Game/BombermanBomb/Mesh/BombMesh.BombMesh'");
	const FString BombermanBombMatPath = FString("MaterialInstanceConstant'/Game/BombermanBomb/Material/BombMat.BombMat'");
	const FString BombParticleSystemPath = FString("ParticleSystem'/Game/BombermanBomb/ParticleSystem/BombExplosionTest.BombExplosionTest'");
	const FString BombProjectilePath = FString("Blueprint'/Game/BombermanBomb/Blueprint/BPExplosionProjectile.BPExplosionProjectile'");
	
	TArray<AActor*> ProjectilesSpawned;
	TArray<FVector> ProjectilesVector;
	const UBlueprint* ExplosionProjectile;
	float ExplosionAnimationTimer = 1;
	bool IsInExplodingAnimation = false;
};
