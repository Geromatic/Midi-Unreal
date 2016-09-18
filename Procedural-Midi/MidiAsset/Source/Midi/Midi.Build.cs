// Copyright -> Scott Bishel

using UnrealBuildTool;
using System.IO;

public class Midi : ModuleRules
{

    public Midi(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				"Midi/Public"
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"Midi/Private",
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
