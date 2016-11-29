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
				"CoreUObject", "Engine", "Slate", "SlateCore", "Midi"
			}
			);
		
		
		if (Target.Platform == UnrealTargetPlatform.Win32 || Target.Platform == UnrealTargetPlatform.Win64)
		{
			Definitions.Add("__WINDOWS_MM__=1");
			PublicAdditionalLibraries.Add("winmm.lib");
		}
		else
		{
			Definitions.Add("__RTMIDI_DUMMY__=1");
		}
    }
}