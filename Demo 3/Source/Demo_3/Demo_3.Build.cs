// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Demo_3 : ModuleRules
{
	public Demo_3(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UtilityDialogueSystemHamo", "UMG" });
	}
}
