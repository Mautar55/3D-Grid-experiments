// Copyright Epic Games, Inc. All Rights Reserved.

#include "thrustCharacter.h"
#include "thrustProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Interactive.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AthrustCharacter

AthrustCharacter::AthrustCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void AthrustCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

void AthrustCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TraceForAnInteractiveActor();
}

//////////////////////////////////////////////////////////////////////////// Input

void AthrustCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AthrustCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AthrustCharacter::Look);

		//Interacting
		//EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AthrustCharacter::QueryPrimaryAction);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AthrustCharacter::RealizeInteraction);

		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Triggered, this, &AthrustCharacter::QueryPrimaryAction);
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Completed, this, &AthrustCharacter::RealizePrimaryAction);

		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Triggered, this, &AthrustCharacter::QuerySecondaryAction);
		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Completed, this, &AthrustCharacter::RealizeSecondaryAction);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AthrustCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AthrustCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AthrustCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AthrustCharacter::GetHasRifle()
{
	return bHasRifle;
}

void AthrustCharacter::TraceForAnInteractiveActor()
{
	FHitResult hitData;
	InteractiveIsOnTarget = false;

	FVector WOrigin = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	FVector WEnd = WOrigin + GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation().RotateVector(FVector(1000, 0, 0));

	if (GetWorld()->LineTraceSingleByChannel(hitData, WOrigin, WEnd, ECC_Visibility)) {

		bool has_interface = hitData.GetActor()->GetClass()->ImplementsInterface(UInteractive::StaticClass());
		if (has_interface) {
			InteractiveIsOnTarget = true;
		}
	}
	QueriedInteraction = hitData;
}

void AthrustCharacter::RealizeInteraction() {
	if (InteractiveIsOnTarget) IInteractive::Execute_QueryInteraction(QueriedInteraction.GetActor(), true, QueriedInteraction, false);
}

void AthrustCharacter::QueryPrimaryAction() {

	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Green, TEXT("primary query"));
	if (InteractiveIsOnTarget) IInteractive::Execute_QueryAction(QueriedInteraction.GetActor(), false, QueriedInteraction, false);
}

void AthrustCharacter::RealizePrimaryAction() {
	if (InteractiveIsOnTarget) IInteractive::Execute_QueryAction(QueriedInteraction.GetActor(), true, QueriedInteraction, false);
}

void AthrustCharacter::QuerySecondaryAction() {

	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Green, TEXT("secondary query"));
	if (InteractiveIsOnTarget) IInteractive::Execute_QueryAction(QueriedInteraction.GetActor(), false, QueriedInteraction, true);
}

void AthrustCharacter::RealizeSecondaryAction() {
	if (InteractiveIsOnTarget) IInteractive::Execute_QueryAction(QueriedInteraction.GetActor(), true, QueriedInteraction, true);
}