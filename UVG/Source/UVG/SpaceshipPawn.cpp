#include "SpaceshipPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/GameEngine.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASpaceshipPawn::ASpaceshipPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize Components
    SpaceshipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpaceshipMesh"));
    RootComponent = SpaceshipMesh;
    SpaceshipMesh->SetSimulatePhysics(true);

	 // Create Spring Arm
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent); // Attach to root
    SpringArm->TargetArmLength = 300.0f;       // Distance to the spaceship
    SpringArm->bEnableCameraLag = true;        // Smooth movement
    SpringArm->CameraLagSpeed = 10.0f;

    // Create Camera
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm); // Attach to Spring Arm
}

// Called when the game starts or when spawned
void ASpaceshipPawn::BeginPlay()
{
	Super::BeginPlay();

	// Bind Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(InputMapping, 0);
        }
    }
}

// Called every frame
void ASpaceshipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ApplyForces(DeltaTime);
}

// Called to bind functionality to input
void ASpaceshipPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &ASpaceshipPawn::MoveForward);
		EnhancedInputComponent->BindAction(IA_MoveForward, ETriggerEvent::Completed, this, &ASpaceshipPawn::MoveForward);
		EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &ASpaceshipPawn::MoveRight);
		EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Completed, this, &ASpaceshipPawn::MoveRight);
		EnhancedInputComponent->BindAction(IA_MoveUp, ETriggerEvent::Triggered, this, &ASpaceshipPawn::MoveUp);
		EnhancedInputComponent->BindAction(IA_MoveUp, ETriggerEvent::Completed, this, &ASpaceshipPawn::MoveUp);
		EnhancedInputComponent->BindAction(IA_Pitch, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Pitch);
		EnhancedInputComponent->BindAction(IA_Pitch, ETriggerEvent::Completed, this, &ASpaceshipPawn::Pitch);
		EnhancedInputComponent->BindAction(IA_Yaw, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Yaw);
		EnhancedInputComponent->BindAction(IA_Yaw, ETriggerEvent::Completed, this, &ASpaceshipPawn::Yaw);
		EnhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Roll);
		EnhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Completed, this, &ASpaceshipPawn::Roll);
		EnhancedInputComponent->BindAction(IA_AllStop, ETriggerEvent::Triggered, this, &ASpaceshipPawn::AllStop);
		EnhancedInputComponent->BindAction(IA_AllStop, ETriggerEvent::Completed, this, &ASpaceshipPawn::AllStopFinished);
	}
}

void ASpaceshipPawn::MoveForward(const FInputActionValue& Value)
{
	linearThrust.Y = Value.Get<float>(); // Forward/backward
}
void ASpaceshipPawn::MoveRight(const FInputActionValue& Value)
{
	linearThrust.X = -Value.Get<float>(); // left/right
}
void ASpaceshipPawn::MoveUp(const FInputActionValue& Value)
{
	linearThrust.Z = Value.Get<float>(); // Forward/backward
}
void ASpaceshipPawn::Pitch(const FInputActionValue& Value)
{
	angularThrust.X = Value.Get<float>(); // Forward/backward
}
void ASpaceshipPawn::Yaw(const FInputActionValue& Value)
{
	angularThrust.Z = Value.Get<float>(); // Forward/backward
}
void ASpaceshipPawn::Roll(const FInputActionValue& Value)
{
	angularThrust.Y = Value.Get<float>(); // Forward/backward
}
void ASpaceshipPawn::AllStop(const FInputActionValue& Value)
{
	// Get current velocities
	FVector LocalVelocity = SpaceshipMesh->GetComponentTransform().InverseTransformVector(SpaceshipMesh->GetPhysicsLinearVelocity());
	FVector LocalAngularVelocity = SpaceshipMesh->GetComponentTransform().InverseTransformVector(SpaceshipMesh->GetPhysicsAngularVelocityInRadians());

	// Stop movement if velocity is close to zero
	if (LocalVelocity.Size() <2.0f)
	{
		// Stop linear motion
		linearThrust.Set(0.0f,0.0f,0.0f);
        SpaceshipMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	}
	else
	{
		linearThrust = -LocalVelocity.GetSafeNormal() * StoppingFactor;
	}
	if (LocalAngularVelocity.Size() < 0.1f)
	{
		// Stop angular motion
        angularThrust.Set(0.0f,0.0f,0.0f);
		SpaceshipMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
	}
	else
	{
		angularThrust = -LocalAngularVelocity.GetSafeNormal() * StoppingFactor;
	}

}

void ASpaceshipPawn::AllStopFinished(const FInputActionValue& Value)
{
	linearThrust.Set(0.0f,0.0f,0.0f);
	angularThrust.Set(0.0f,0.0f,0.0f);
}

void ASpaceshipPawn::ApplyForces(float DeltaTime)
{
	// Apply linear force
    FVector Force = linearThrust * ForceFactor;
	FVector WorldForce = SpaceshipMesh->GetComponentTransform().TransformVector(Force);
    SpaceshipMesh->AddForce(WorldForce);

    // Apply rotational torque
    FVector Torque = angularThrust * TorqueFactor;
	FVector WorldTorque = SpaceshipMesh->GetComponentTransform().TransformVector(Torque);
    SpaceshipMesh->AddTorqueInRadians(WorldTorque);
}

