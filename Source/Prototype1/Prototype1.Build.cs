// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Prototype1 : ModuleRules
{
	public Prototype1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GameplayTags", "EnhancedInput","StateTreeModule" , "UMG"});

		PrivateDependencyModuleNames.AddRange(new string[] {  "Slate", "SlateCore", "AIModule", "GameplayStateTreeModule", "NavigationSystem" });

		PublicIncludePaths.AddRange(
			new string[]
			{
				"Prototype1",
				"Prototype1/General",
				"Prototype1/General/Entities",
				"Prototype1/General/Entities/Enemies",
				"Prototype1/General/Waves",
				"Prototype1/General/AISystem",
				"Prototype1/Components",
				"Prototype1/DataAssets",
				"Prototype1/Stats",
				"Prototype1/Utility",
				"Prototype1/Pathfinding",
				"Prototype1/UI",
				
				"Prototype1/TP_FirstPerson",
				
				"Prototype1/TP_FirstPerson/Variant_Shooter",
				"Prototype1/TP_FirstPerson/Variant_Shooter/AI",
				"Prototype1/TP_FirstPerson/Variant_Shooter/UI",
				"Prototype1/TP_FirstPerson/Variant_Shooter/Weapons",
				
				"Prototype1/TP_FirstPerson/Variant_Horror",
				"Prototype1/TP_FirstPerson/Variant_Horror/UI",
			});
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
