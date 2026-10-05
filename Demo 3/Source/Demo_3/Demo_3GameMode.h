// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Demo_3GameMode.generated.h"

class UMKScriptPlay;
struct FMKTextBlockProperty;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActiveLanguageChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartDialogue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayDialogue, const FMKLoadData&, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayAction, const TArray<FMKTextBlockProperty>&, Properties);

UCLASS(minimalapi)
class ADemo_3GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADemo_3GameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 ActiveLanguageIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TArray<FString> AvailableLanguages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<UMKScriptPlay> Scene;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Data")
	FOnActiveLanguageChanged OnActiveLanguageChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Data")
	FOnStartDialogue OnStartDialogue;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Data")
	FOnPlayDialogue OnPlayDialogue;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Data")
	FOnPlayAction OnPlayAction;

	UPROPERTY(BlueprintReadWrite, Category = "Data")
	bool bPlayAction;

	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable)
	FString GetActiveLanguageString();

	UFUNCTION(BlueprintCallable)
	FText GetActiveLanguageText();

	void StartDialogue();

private:
	void ManageBindings();

	void PlayDialogue();

	UFUNCTION()
	void NextDialogue();

	UFUNCTION()
	void TaggedBlockReached(const FName& Tag);

	TArray<FMKTextBlockProperty> GetTextProperties();
};



