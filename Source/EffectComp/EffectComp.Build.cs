// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EffectComp : ModuleRules
{
	public EffectComp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"ChaosVehicles",
			"PhysicsCore",
			"UMG",
			"Slate",
			"Niagara"
		});

		PublicIncludePaths.AddRange(new string[] {
			"EffectComp",
			"EffectComp/SportsCar",
			"EffectComp/OffroadCar",
			"EffectComp/Variant_Offroad",
			"EffectComp/Variant_TimeTrial",
			"EffectComp/Variant_TimeTrial/UI"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "Niagara", "Niagara" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
