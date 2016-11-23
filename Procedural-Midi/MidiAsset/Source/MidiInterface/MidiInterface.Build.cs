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
		
		
		if (Target.Platform == UnrealTargetPlatform.Win32 || Target.Platform == UnrealTargetPlatform.Win64)
		{
			Definitions.Add("__WINDOWS_MM__=1");
			PublicAdditionalLibraries.Add("winmm.lib");
		}
		/*else if (Target.Platform == UnrealTargetPlatform.IOS || Target.Platform == UnrealTargetPlatform.Mac)
        {
			Definitions.Add("__MACOSX_CORE__=1");
            PublicFrameworks.AddRange(new string[] { "CoreMIDI", "CoreAudio", "CoreFoundation"});
        }
		else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
			Definitions.Add("__LINUX_ALSA__=1");
			PublicAdditionalLibraries.AddRange(new string[] { "asound", "pthread"});
        }
		else
		{
			Definitions.Add("__RTMIDI_DUMMY__=1");
		}*/
    }
}
