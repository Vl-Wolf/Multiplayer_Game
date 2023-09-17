// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TT : ModuleRules
{
	public TT(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "PhysicsCore", "Slate", "Niagara", "NiagaraCore" });
	}
}
