// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableWall.h"

// Sets default values
ABreakableWall::ABreakableWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *BombermanMeshPath);
	UMaterialInstanceConstant* Material = LoadObject<UMaterialInstanceConstant>(nullptr, *BombermanBreakableWallMatPath);
	StaticMesh->SetMaterial(0, Material);
	StaticMeshComponent->SetStaticMesh(StaticMesh);
	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ABreakableWall::BeginOverlap);
	StaticMeshComponent->SetCollisionProfileName(FName("BreakableWall"));
	RootComponent = StaticMeshComponent;
}

// Called when the game starts or when spawned
void ABreakableWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABreakableWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableWall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ABreakableWall, StaticMeshComponent, COND_SimulatedOnly);
}

void ABreakableWall::BeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_GameTraceChannel4)
	{
		Destroy();
	}
}

