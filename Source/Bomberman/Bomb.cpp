// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"
// Sets default values
ABomb::ABomb()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	this->SetReplicates(true);
	this->SetReplicateMovement(true);
	TimerExplosion = 1;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *BombermanBombMeshPath);
	UMaterialInstanceConstant* Material = LoadObject<UMaterialInstanceConstant>(nullptr, *BombermanBombMatPath);
	StaticMesh->SetMaterial(0, Material);
	StaticMeshComponent->SetStaticMesh(StaticMesh);
	StaticMeshComponent->SetCollisionProfileName(FName("Bomb"));
	RootComponent = StaticMeshComponent;
	ExplosionParticle = LoadObject<UParticleSystem>(nullptr, *BombParticleSystemPath);
	ExplosionProjectile = LoadObject<UBlueprint>(nullptr, *BombProjectilePath);
	ProjectilesVector.Add(FVector(200, 0, 0));
	ProjectilesVector.Add(FVector(-200, 0, 0));
	ProjectilesVector.Add(FVector(0, 200, 0));
	ProjectilesVector.Add(FVector(0, -200, 0));
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
}

void ABomb::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ABomb, ExplosionParticle);
	DOREPLIFETIME_CONDITION(ABomb, ExplosionParticle, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ABomb, TimerExplosion, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ABomb, StaticMeshComponent, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ABomb, CharacterOwner, COND_SimulatedOnly);
}

void ABomb::SpawnEmitter_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetTransform());
}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimerExplosion -= DeltaTime;
	CheckExplosion(DeltaTime);
}

void ABomb::CheckExplosion_Implementation(float DeltaTime)
{
	if (TimerExplosion <= 0)
	{
		if (!IsInExplodingAnimation)
		{
			if (CharacterOwner)
			{
				CharacterOwner->BombExploded();
			}
			IsInExplodingAnimation = true;
			SpawnEmitter();
			for (int32 Index = 0; Index < 4; Index++)
			{
				ProjectilesSpawned.Add(GetWorld()->SpawnActor<AActor>(ExplosionProjectile->GeneratedClass));
				ProjectilesSpawned[Index]->SetActorLocation(GetTransform().GetLocation());
				UBoxComponent* ProjectileBox = Cast<UBoxComponent>(ProjectilesSpawned[Index]->GetRootComponent());
				ProjectileBox->SetPhysicsLinearVelocity(ProjectilesVector[Index]);
			}
			StaticMeshComponent->SetVisibility(false);
		}
		Explode(DeltaTime);
	}
}

void ABomb::Explode_Implementation(float DeltaTime)
{
	if (ExplosionAnimationTimer > 0)
	{
		ExplosionAnimationTimer -= DeltaTime;
		return;
	}
	for (AActor* Projectile : ProjectilesSpawned)
	{
		Projectile->Destroy();
	}
	ProjectilesSpawned.Empty();
	Destroy();
}

