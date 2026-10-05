// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Demo_3GameMode.h"
#include "MKScriptPlay.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APlayerCharacter

APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	CameraBoom->TargetArmLength = 966.0f; // The camera follows at this distance behind the character	
	CameraBoom->SocketOffset = FVector(0.0f, 260.0f, 0.0f);
	CameraBoom->bUsePawnControlRotation = false; // Disabling it because we will have a fixed cam. (2D Cam)
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bCanStartScene = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ADemo_3GameMode* GameMode = Cast<ADemo_3GameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (GameMode->Scene)
		{
			GameMode->Scene->OnScriptPlayCurrentScriptFinished.AddDynamic(this, &APlayerCharacter::OnSceneFinished);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	AddMappingContext(DefaultMappingContext);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Default

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);

		// Pause Game
		EnhancedInputComponent->BindAction(PauseGameAction, ETriggerEvent::Triggered, this, &APlayerCharacter::PauseGame);

		// Dialogue

		// Next Dialogue
		EnhancedInputComponent->BindAction(NextDialogueAction, ETriggerEvent::Triggered, this, &APlayerCharacter::NextDialogue);

		// Pause Dialogue
		EnhancedInputComponent->BindAction(PauseDialogueAction, ETriggerEvent::Triggered, this, &APlayerCharacter::PauseGame);

		// UI

		// Return
		EnhancedInputComponent->BindAction(ReturnAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ReturnGame);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Interact(const FInputActionValue& Value)
{
	if (!bCanStartScene)
		return;

	TSubclassOf<UUserWidget> DialogueWidget = GetWidget("DialogueWidget");
	AddWidgetToViewport(DialogueWidget);

	SwitchMappingContext(ActiveMappingContext, DialogueMappingContext);

	if (ADemo_3GameMode* GameMode = Cast<ADemo_3GameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->StartDialogue();
	}

	UE_LOG(LogTemp, Warning, TEXT("Start Scene..."));
}

void APlayerCharacter::PauseGame(const FInputActionValue& Value)
{
	TSubclassOf<UUserWidget> PauseWidget = GetWidget("PauseWidget");
	UUserWidget* Widget = AddWidgetToViewport(PauseWidget);

	SwitchMappingContext(ActiveMappingContext, UIMappingContext);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		FInputModeGameAndUI InputModeGameAndUI;
		InputModeGameAndUI.SetWidgetToFocus(Widget->TakeWidget());
		InputModeGameAndUI.SetHideCursorDuringCapture(false);

		PlayerController->SetInputMode(InputModeGameAndUI);
		PlayerController->SetShowMouseCursor(true);
	}
}

void APlayerCharacter::NextDialogue(const FInputActionValue& Value)
{
	OnNextDialogueTriggered.Broadcast();
}

void APlayerCharacter::ReturnGame(const FInputActionValue& Value)
{
	RemoveWidget(1);

	SwitchMappingContext(ActiveMappingContext, PreviousMappingContext);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
	}
}

void APlayerCharacter::SwitchMappingContext(UInputMappingContext* OldInputMappingContext, UInputMappingContext* NewInputMappingContext)
{
	RemoveMappingContext(OldInputMappingContext);
	AddMappingContext(NewInputMappingContext);
}

void APlayerCharacter::AddMappingContext(UInputMappingContext* InputMappingContext)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
			ActiveMappingContext = InputMappingContext;
		}
	}
}

void APlayerCharacter::RemoveMappingContext(UInputMappingContext* InputMappingContext)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(InputMappingContext);

			PreviousMappingContext = ActiveMappingContext;
			ActiveMappingContext = nullptr;
		}
	}
}

UUserWidget* APlayerCharacter::AddWidgetToViewport(TSubclassOf<UUserWidget> TargetWidget)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		UUserWidget* NewWidget = CreateWidget<UUserWidget>(PlayerController, TargetWidget);

		NewWidget->AddToViewport();
		WidgetsStack.Push(NewWidget);

		return NewWidget;
	}

	return nullptr;
}

void APlayerCharacter::OnSceneFinished()
{
	RemoveWidget(1);

	SwitchMappingContext(ActiveMappingContext, DefaultMappingContext);
}

void APlayerCharacter::RemoveWidget(int32 Level)
{
	for (int i = 0; i < Level; ++i)
	{
		if (WidgetsStack.IsEmpty())
			break;

		UUserWidget* Widget = WidgetsStack.Pop();
		Widget->RemoveFromParent();
	}
}

TSubclassOf<UUserWidget> APlayerCharacter::GetWidget(const FName& WidgetName)
{
	if (TSubclassOf<UUserWidget>* FoundWidget = AvailableWidgets.Find(WidgetName))
		return *FoundWidget;

	return nullptr;
}
