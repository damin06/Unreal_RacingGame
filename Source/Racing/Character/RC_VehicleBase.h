// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputActionValue.h"
#include "RC_VehicleBase.generated.h"

/**
 * 
 */
UCLASS()
class RACING_API ARC_VehicleBase : public AWheeledVehiclePawn
{
	GENERATED_BODY()
	
public:
	ARC_VehicleBase();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(Category = Cameara, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr <class USpringArmComponent> SpringArm;
	
	UPROPERTY(Category = Cameara, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr <class UCameraComponent> Camera;

private:
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputMappingContext> Context;

	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> VerticalAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> HorizontalAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> HandBreakAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> GearChangeAction;

private:
	void ApplyMove(const FInputActionValue& Value);

	void ApplyAcceleration(float InputValue);
	void ApplySteering(float InputValue);

	void Look(const FInputActionValue& Value);

	void OnHandBreakPressed();
	void OnHandBreakReleased();

	void OnBreakPresse();
	void OnBreakReleased();
	void SetBreak(float Value);
	void OnGearChange(const FInputActionValue& Value);

	void DecelerateCar();
	void ResetSteeringAngle();

private:

	float ThrottleAxis;
	float SteeringAxis;
};
