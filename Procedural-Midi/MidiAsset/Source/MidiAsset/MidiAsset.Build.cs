// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class MidiAsset : ModuleRules
	{
#if WITH_FORWARDED_MODULE_RULES_CTOR
	public MidiAsset(ReadOnlyTargetRules Target) : base(Target)
#else
    public MidiAsset(TargetInfo Target)
#endif
		{
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicDependencyModuleNames.AddRange(
                new string[] {
					"Core",
					"CoreUObject",
                }
            );


            PrivateIncludePaths.AddRange(
				new string[] {
                }
			);
			PublicDefinitions.Add("__STDC_WANT_SECURE_LIB__=1"); // ignore warning
        }
	}
}
