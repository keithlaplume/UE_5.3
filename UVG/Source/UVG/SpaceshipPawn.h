#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "SpaceshipPawn.generated.h"

UCLASS()
class UVG_API ASpaceshipPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpaceshipPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* SpaceshipMesh;

	FVector linearThrust;
    FVector angularThrust;
	float StoppingFactor = 5.0f;    // Adjust to control the stopping speed
	UPROPERTY(EditAnywhere)
    float ForceFactor;
	UPROPERTY(EditAnywhere)
    float TorqueFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	// Input functions
    void MoveForward(const FInputActionValue& Value);
    void MoveRight(const FInputActionValue& Value);
    void MoveUp(const FInputActionValue& Value);
    void Pitch(const FInputActionValue& Value);
    void Yaw(const FInputActionValue& Value);
    void Roll(const FInputActionValue& Value);
	void AllStop(const FInputActionValue& Value);
	void AllStopFinished(const FInputActionValue& Value);

protected:
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputMapping;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveForward;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveRight;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveUp;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Pitch;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Yaw;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Roll;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_AllStop;

private:
    void ApplyForces(float DeltaTime);

};
