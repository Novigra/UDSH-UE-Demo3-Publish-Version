// Copyright Epic Games, Inc. All Rights Reserved.

/*
*	I highly highly recommend you don't replicate my code if you're working on a serious project.
	I wrote it like this because I was interested in the logic more than the architecture, and I wanted
	to finish the demos quickly. This. sloppiness has an advantage. You don't have to think about
	architecture, and the code is super straight forward.
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Blueprint/UserWidget.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextDialogueTriggered);

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */

	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> ActiveMappingContext;

	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> PreviousMappingContext;
	
	/** Default Mapping Context */
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	/** Pause Game Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseGameAction;

	/** Dialogue Mapping Context */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DialogueMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> NextDialogueAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> PauseDialogueAction;

	/** UI Context */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> UIMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ReturnAction;

	/** Gameplay */

	// Widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TMap<FName, TSubclassOf<UUserWidget>> AvailableWidgets; // We will use tsubclass to create a new instance and avoid weird bugs

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UUserWidget>> WidgetsStack;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	bool bCanStartScene;

public:
	APlayerCharacter();
	
	UPROPERTY(BlueprintAssignable)
	FOnNextDialogueTriggered OnNextDialogueTriggered;

	virtual void BeginPlay() override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	
	void Interact(const FInputActionValue& Value);

	void PauseGame(const FInputActionValue& Value);

	void NextDialogue(const FInputActionValue& Value);

	void ReturnGame(const FInputActionValue& Value);

	void SwitchMappingContext(UInputMappingContext* OldInputMappingContext, UInputMappingContext* NewInputMappingContext);

	void AddMappingContext(UInputMappingContext* InputMappingContext);

	void RemoveMappingContext(UInputMappingContext* InputMappingContext);

	UUserWidget* AddWidgetToViewport(TSubclassOf<UUserWidget> TargetWidget);

	void RemoveWidget(int32 Level);

protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UFUNCTION()
	void OnSceneFinished();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	TSubclassOf<UUserWidget> GetWidget(const FName& WidgetName);
};

