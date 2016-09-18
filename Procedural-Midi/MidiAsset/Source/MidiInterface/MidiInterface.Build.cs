// Copyright -> Scott Bishel

using UnrealBuildTool;
using System.IO;

public class MidiInterface : ModuleRules
{
    public MidiInterface(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				"MidiInterface/Public"
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "MidiInterface/Private",
                "MidiInterface/Classes",
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
				"CoreUObject", "Engine", "Slate", "SlateCore"
			}
			);
    }
}
