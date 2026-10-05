// Copyright Epic Games, Inc. All Rights Reserved.

#include "Demo_3GameMode.h"
#include "PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MKScriptPlay.h"
#include "Kismet/GameplayStatics.h"
#include "MKTextBlockProperty.h"
#include "MKCFile.h"

ADemo_3GameMode::ADemo_3GameMode()
{}

void ADemo_3GameMode::StartPlay()
{
	Super::StartPlay();

	bPlayAction = false;

	ManageBindings();
}

FString ADemo_3GameMode::GetActiveLanguageString()
{
	if (AvailableLanguages.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode: AvailableLanguages array is empty"))
			return FString();
	}
	else if (ActiveLanguageIndex < 0 || ActiveLanguageIndex >= AvailableLanguages.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode: ActiveLanguageIndex is out of AvailableLanguages array range"))
			return FString();
	}

	return AvailableLanguages[ActiveLanguageIndex];
}

FText ADemo_3GameMode::GetActiveLanguageText()
{
	if (AvailableLanguages.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode: AvailableLanguages array is empty"))
			return FText();
	}
	else if (ActiveLanguageIndex < 0 || ActiveLanguageIndex >= AvailableLanguages.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode: ActiveLanguageIndex is out of AvailableLanguages array range"))
			return FText();
	}

	return FText::FromString(AvailableLanguages[ActiveLanguageIndex]);
}

void ADemo_3GameMode::ManageBindings()
{
	if (Scene)
	{
		Scene->OnTaggedBlockReached.AddDynamic(this, &ADemo_3GameMode::TaggedBlockReached);
	}

	ACharacter* ActivePlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(ActivePlayer))
	{
		PlayerCharacter->OnNextDialogueTriggered.AddDynamic(this, &ADemo_3GameMode::NextDialogue);
	}
}

void ADemo_3GameMode::StartDialogue()
{
	if (!Scene)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode: Scene is nullptr"));
		return;
	}

	OnStartDialogue.Broadcast();

	Scene->ResetData();
	PlayDialogue();
}

void ADemo_3GameMode::PlayDialogue()
{
	if (bPlayAction)
	{
		OnPlayAction.Broadcast(GetTextProperties());
		bPlayAction = false;
	}
	else
	{
		OnPlayDialogue.Broadcast(Scene->LoadCurrentBlock(GetActiveLanguageString()));
	}
}

void ADemo_3GameMode::NextDialogue()
{
	if (!Scene)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode: Scene is nullptr"));
		return;
	}

	if (!bPlayAction)
		Scene->NextBlock();

	PlayDialogue();
}

void ADemo_3GameMode::TaggedBlockReached(const FName& Tag)
{
	if (Tag == FName(TEXT("play.next.action")))
	{
		bPlayAction = true;
	}
}

TArray<FMKTextBlockProperty> ADemo_3GameMode::GetTextProperties()
{
	if (UMKCFile* MKCFile = Cast<UMKCFile>(Scene->CurrentScript->MKFile))
	{
		int32 BlockIndex = -1;

		for (const auto& Block : MKCFile->FileContent)
		{
			if (Block.Value.BlockID == Scene->CurrentScript->LastVisitedDialogueBlock.BlockID)
			{
				BlockIndex = Block.Key + 2;
				break;
			}
		}

		if (BlockIndex == -1)
			return TArray<FMKTextBlockProperty>();

		if (FMKTextBlockPropertyArrayWrapper* PropertyWrapper = MKCFile->FileContent.Find(BlockIndex))
		{
			return (*PropertyWrapper).Properties;
		}
	}

	return TArray<FMKTextBlockProperty>();
}
