// Fill out your copyright notice in the Description page of Project Settings.


#include "MyThirdPersonBombermanCharacter.h"
#include "Bomb.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

AMyThirdPersonBombermanCharacter::AMyThirdPersonBombermanCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyThirdPersonBombermanCharacter::BeginOverlap);
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	HowManyBombAlive = 2;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyThirdPersonBombermanCharacter::SpawnBomb_Implementation()
{
	if (HowManyBombAlive > BombAlive)
	{
		UBlueprint* BPBomb = LoadObject<UBlueprint>(nullptr, *BPBombPath);
		AActor* ActorBomb = GetWorld()->SpawnActor<AActor>(BPBomb->GeneratedClass);
		ABomb* Bomb = Cast<ABomb>(ActorBomb);
		Bomb->SetCharacterOwner(this);
		FVector LocationToSet = GetTransform().GetLocation();
		int32 LocationTempAxys = LocationToSet.X / 50;
		LocationTempAxys = LocationTempAxys % 2 == 0 ? LocationTempAxys + 1 : LocationTempAxys;
		LocationToSet.X = LocationTempAxys * 50;
		LocationTempAxys = LocationToSet.Y / 50;
		LocationTempAxys = LocationTempAxys % 2 == 0 ? LocationTempAxys + 1 : LocationTempAxys;
		LocationToSet.Y = LocationTempAxys * 50;
		ActorBomb->SetActorLocation(LocationToSet);
		ActorBomb->SetActorRotation(FQuat::Identity);
		BombAlive++;
	}
}

void AMyThirdPersonBombermanCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyThirdPersonBombermanCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyThirdPersonBombermanCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyThirdPersonBombermanCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyThirdPersonBombermanCharacter::LookUpAtRate);

	//SpawnBomb
	PlayerInputComponent->BindKey(EKeys::E, EInputEvent::IE_Pressed, this, &AMyThirdPersonBombermanCharacter::SpawnBomb);
}

void AMyThirdPersonBombermanCharacter::BeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_GameTraceChannel4)
	{
		Destroy();
	}
}

void AMyThirdPersonBombermanCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyThirdPersonBombermanCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyThirdPersonBombermanCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyThirdPersonBombermanCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}