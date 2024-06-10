// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RC_VehicleBase.h"
#include "Blueprint/UserWidget.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include<string>

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

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_MoveAction
	(TEXT("/Game/02_Input/IA_Move.IA_Move"));
	if (IA_MoveAction.Succeeded())
	{
		MoveAction = IA_MoveAction.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_HandBreak
	(TEXT("/Game/02_Input/IA_HandBreak.IA_HandBreak"));
	if (IA_HandBreak.Succeeded())
	{
		HandBreakAction = IA_HandBreak.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction>IA_GearChange
	(TEXT("/Game/02_Input/IA_GearChange.IA_GearChange"));
	if (IA_GearChange.Succeeded())
	{
		GearChangeAction = IA_GearChange.Object;
	}

	//GetVehicleMovement()->SetUseAutomaticGears(true);
#pragma endregion

}

void ARC_VehicleBase::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameAndUI());

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			SubSystem->AddMappingContext(Context, 0);
	}

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ARC_VehicleBase::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Log, TEXT("speed: %d"), GetVehicleMovement()->GetForwardSpeed());
}

void ARC_VehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//EnhancedInputComponent->BindAction(VerticalAction, ETriggerEvent::Triggered, this, &ARC_VehicleBase::ApplyAcceleration);
		//EnhancedInputComponent->BindAction(HorizontalAction, ETriggerEvent::Triggered, this, &ARC_VehicleBase::ApplySteering);
		EnhancedInputComponent->BindAction(HandBreakAction, ETriggerEvent::Triggered, this, &ARC_VehicleBase::OnHandBreakPressed);
		EnhancedInputComponent->BindAction(HandBreakAction, ETriggerEvent::Completed, this, &ARC_VehicleBase::OnHandBreakReleased);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARC_VehicleBase::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::None, this, &ARC_VehicleBase::ApplyMove);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARC_VehicleBase::ApplyMove);
		EnhancedInputComponent->BindAction(GearChangeAction, ETriggerEvent::Started, this, &ARC_VehicleBase::OnGearChange);
	}
}

void ARC_VehicleBase::ApplyMove(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();

	//UE_LOG(LogTemp, Log, TEXT("Vertical Horizontal Input : %d"), MoveVector.X, MoveVector.Y);

	if (MoveVector.Y == 0)
	{
		DecelerateCar();
	}
	else
	{
		ApplyAcceleration(MoveVector.Y);
	}

	if (MoveVector.X == 0) 
	{
		ResetSteeringAngle();
	}
	else 
	{
		ApplySteering(MoveVector.X);
	}
}

void ARC_VehicleBase::ApplyAcceleration(float InputValue)
{
	//float VerticalInput = Value.Get<float>();
	//GetVehicleMovement()->SetThrottleInput(InputValue);


	ThrottleAxis = FMath::Clamp(ThrottleAxis + InputValue * (FApp::GetDeltaTime() * 2), -1, 1);



	SetBreak(0);
	GetVehicleMovement()->SetThrottleInput(ThrottleAxis);
}

void ARC_VehicleBase::ApplySteering(float InputValue)
{
	//float HorizontalInput = Value.Get<float>();
	//UE_LOG(LogTemp, Log, TEXT("HorizontalInput : %d"), InputValue);

	SteeringAxis = FMath::Clamp(SteeringAxis + InputValue * (FApp::GetDeltaTime() * 10), -1, 1);

	GetVehicleMovement()->SetSteeringInput(SteeringAxis);
}

void ARC_VehicleBase::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	//UE_LOG(LogTemp, Log, TEXT("LookVector Input : %d"), LookVector.X);

	SpringArm->AddRelativeRotation(FRotator(LookVector.Y, LookVector.X, 0));
	//SpringArm->AddRelativeRotation(FRotator(, 0, 0));
}

void ARC_VehicleBase::OnHandBreakPressed()
{
	GetVehicleMovement()->SetHandbrakeInput(true);
}

void ARC_VehicleBase::OnHandBreakReleased()
{
	GetVehicleMovement()->SetHandbrakeInput(false);
}

void ARC_VehicleBase::OnBreakPresse()
{
	GetVehicleMovement()->SetBrakeInput(true);
}

void ARC_VehicleBase::OnBreakReleased()
{
	GetVehicleMovement()->SetBrakeInput(false);
}

void ARC_VehicleBase::SetBreak(float Value)
{
	GetVehicleMovement()->SetBrakeInput(Value);
}

void ARC_VehicleBase::OnGearChange(const FInputActionValue& Value)
{
	float MoveVector = Value.Get<float>();

	if (MoveVector >= 1) 
	{
		GetVehicleMovement()->SetChangeUpInput(true);
	}
	else if (MoveVector <= -1) 
	{
		GetVehicleMovement()->SetChangeDownInput(true);
	}
}

void ARC_VehicleBase::DecelerateCar()
{
	if (ThrottleAxis != 0) 
	{
		if (ThrottleAxis > 0) 
		{
			ThrottleAxis -= FApp::GetDeltaTime() * 10;
		}
		else if (ThrottleAxis < 0) 
		{
			ThrottleAxis += FApp::GetDeltaTime() * 10;
		}

		if (FMath::Abs(ThrottleAxis) < 0.15f) 
		{
			ThrottleAxis = 0;
		}
	}

	GetVehicleMovement()->SetThrottleInput(ThrottleAxis);
}

void ARC_VehicleBase::ResetSteeringAngle()
{
	if (SteeringAxis < 0) 
	{
		SteeringAxis += FApp::GetDeltaTime() * 10;
	}
	else if(SteeringAxis > 0)
	{
		SteeringAxis -= FApp::GetDeltaTime() * 10;
	}

	if(FMath::Abs(SteeringAxis) < 1)
	{
		SteeringAxis = 0;
	}

	GetVehicleMovement()->SetSteeringInput(SteeringAxis);
}
