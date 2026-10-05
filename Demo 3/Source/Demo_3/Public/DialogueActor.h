// Copyright Mohammed Kenawy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DialogueActor.generated.h"

class USceneCaptureComponent2D;
struct FMKLoadData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateCharacterVisuals);

USTRUCT(BlueprintType)
struct FDialogueActorDetails
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<USkeletalMesh> CharacterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<TObjectPtr<UAnimationAsset>> Animations;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	int32 CurrentAnimationIndex = -1;
};

UCLASS()
class DEMO_3_API ADialogueActor : public AActor
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void LoadMesh(const FName& CharacterID);
	
public:	
	// Sets default values for this actor's properties
	ADialogueActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TMap<FName, FDialogueActorDetails> Actors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<USceneCaptureComponent2D> SceneCapture;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateCharacterVisuals OnUpdateCharacterVisuals;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ResetData();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void ManageBindings();

	UFUNCTION()
	void OnDialogueStart();

	UFUNCTION()
	void OnDialoguePlay(const FMKLoadData& Data);
};
