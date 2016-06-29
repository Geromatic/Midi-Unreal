// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class MidiAsset : ModuleRules
	{
		public MidiAsset(TargetInfo Target)
		{
            PublicDependencyModuleNames.AddRange(
                new string[] {
					"Core",
					"CoreUObject",
                }
            );

			PrivateIncludePaths.AddRange(
				new string[] {
					"Runtime/MidiAsset/Private",
				}
			);
		}
	}
}
