// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RC_VehicleBase.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ARC_VehicleBase::ARC_VehicleBase()
{
	//UWheeledVehicleMovementComponent4W* 

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

#pragma region Input

	static ConstructorHelpers::FObjectFinder<UInputMappingContext>DEFAULT_CONTEXT
	(TEXT("/Game/02_Input/IMC_VehicleInput.IMC_VehicleInput"));
	if (DEFAULT_CONTEXT.Succeeded())
	{
		Context = DEFAULT_CONTEXT.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_VerticalAction
	(TEXT("/Game/02_Input/IA_Vertical.IA_Vertical"));
	if (IA_VerticalAction.Succeeded())
	{
		VerticalAction = IA_VerticalAction.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_HorizontalAction
	(TEXT("/Game/02_Input/IA_Horizontal.IA_Horizontal"));
	if (IA_HorizontalAction.Succeeded())
	{
		HorizontalAction = IA_HorizontalAction.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_LookAction
	(TEXT("/Game/02_Input/IA_Look.IA_Look"));
	if (IA_LookAction.Succeeded())
	{
		LookAction = IA_LookAction.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_HandBreak
	(TEXT("/Game/02_Input/IA_HandBreak.IA_HandBreak"));
	if (IA_HandBreak.Succeeded())
	{
		HandBreakAction = IA_HandBreak.Object;
	}

#pragma endregion

}

void ARC_VehicleBase::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			SubSystem->AddMappingContext(Context, 0);
	}
}

void ARC_VehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(VerticalAction, ETriggerEvent::Triggered, this, &ARC_VehicleBase::ApplyAcceleration);
		EnhancedInputComponent->BindAction(HorizontalAction, ETriggerEvent::Triggered, this, &ARC_VehicleBase::ApplySteering);
		EnhancedInputComponent->BindAction(HandBreakAction, ETriggerEvent::Triggered, this, &ARC_VehicleBase::OnHandBreakPressed);
		EnhancedInputComponent->BindAction(HandBreakAction, ETriggerEvent::Completed, this, &ARC_VehicleBase::OnHandBreakReleased);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARC_VehicleBase::Look);
	}
}

void ARC_VehicleBase::ApplyAcceleration(const FInputActionValue& Value)
{
	float VerticalInput = Value.Get<float>();

	GetVehicleMovement()->SetThrottleInput(VerticalInput);
}

void ARC_VehicleBase::ApplySteering(const FInputActionValue& Value)
{
	float HorizontalInput = Value.Get<float>();

	GetVehicleMovement()->SetSteeringInput(HorizontalInput);
}

void ARC_VehicleBase::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();

	SpringArm->AddRelativeRotation(FRotator(0, LookVector.Y, LookVector.X));
}

void ARC_VehicleBase::OnHandBreakPressed()
{
	GetVehicleMovement()->SetHandbrakeInput(true);
}

void ARC_VehicleBase::OnHandBreakReleased()
{
	GetVehicleMovement()->SetHandbrakeInput(false);
}
