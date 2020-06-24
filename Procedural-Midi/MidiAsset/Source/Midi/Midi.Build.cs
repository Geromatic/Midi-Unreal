// Copyright -> Scott Bishel

using UnrealBuildTool;
using System.IO;

public class Midi : ModuleRules
{

#if WITH_FORWARDED_MODULE_RULES_CTOR
	public Midi(ReadOnlyTargetRules Target) : base(Target)
#else
    public Midi(TargetInfo Target)
#endif
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		PrivateIncludePaths.AddRange(
			new string[] {
                "Midi/Classes",
            }
			);
			
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject", "Engine", "Slate", "SlateCore", "MidiAsset"
			}
			);
    }
}
