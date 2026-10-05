// Copyright Mohammed Kenawy. All Rights Reserved.


#include "DialogueActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Demo_3/Demo_3GameMode.h"
#include "MKScriptPlay.h"

// Sets default values
ADialogueActor::ADialogueActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	CharacterMesh->SetRelativeRotation(FRotator(0.0f));
	CharacterMesh->SetupAttachment(SceneRoot);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->ProjectionType = ECameraProjectionMode::Perspective;
	SceneCapture->SetRelativeLocation(FVector(0.000007f, 198.345697f, 130.000001f));
	SceneCapture->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	SceneCapture->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void ADialogueActor::BeginPlay()
{
	Super::BeginPlay();

	SceneCapture->ShowOnlyActorComponents(this);
	ManageBindings();
}

// Called every frame
void ADialogueActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADialogueActor::ManageBindings()
{
	if (ADemo_3GameMode* GameMode = Cast<ADemo_3GameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->OnStartDialogue.AddDynamic(this, &ADialogueActor::OnDialogueStart);
		GameMode->OnPlayDialogue.AddDynamic(this, &ADialogueActor::OnDialoguePlay);
	}
}

void ADialogueActor::LoadMesh(const FName& CharacterID)
{
	if (FDialogueActorDetails* FoundActor = Actors.Find(CharacterID))
	{
		FDialogueActorDetails& Actor = *FoundActor;

		if (Actor.CharacterMesh)
		{
			CharacterMesh->SetSkeletalMesh(Actor.CharacterMesh);

			if (Actor.CurrentAnimationIndex + 1 < Actor.Animations.Num())
			{
				CharacterMesh->SetAnimation(Actor.Animations[++Actor.CurrentAnimationIndex]);
				OnUpdateCharacterVisuals.Broadcast();
			}
			else
			{
				CharacterMesh->SetAnimation(nullptr);
			}
		}
		else
		{
			CharacterMesh->SetSkeletalMesh(nullptr);
			CharacterMesh->SetAnimation(nullptr);
		}
	}
	else
	{
		CharacterMesh->SetSkeletalMesh(nullptr);
		CharacterMesh->SetAnimation(nullptr);
	}
}

void ADialogueActor::OnDialogueStart()
{
	ResetData();
}

void ADialogueActor::OnDialoguePlay(const FMKLoadData& Data)
{
	LoadMesh(Data.DialogueBlock.CharacterID);
}

void ADialogueActor::ResetData()
{
	for (auto& Actor : Actors)
	{
		Actor.Value.CurrentAnimationIndex = -1;
	}

	CharacterMesh->SetSkeletalMesh(nullptr);
	CharacterMesh->SetAnimation(nullptr);
}
