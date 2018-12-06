// Copyright -> Scott Bishel

using UnrealBuildTool;
using System.IO;

public class MidiInterface : ModuleRules
{
#if WITH_FORWARDED_MODULE_RULES_CTOR
	public MidiInterface(ReadOnlyTargetRules Target) : base(Target)
#else
    public MidiInterface(TargetInfo Target)
#endif
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
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
		
		// Windows
		if (Target.Platform == UnrealTargetPlatform.Win32 || Target.Platform == UnrealTargetPlatform.Win64)
		{
            PublicDefinitions.Add("__WINDOWS_MM__=1");
			PublicAdditionalLibraries.Add("winmm.lib");
		}
		// MAC / IOS
		else if (Target.Platform == UnrealTargetPlatform.Mac || Target.Platform == UnrealTargetPlatform.IOS) {
            PublicDefinitions.Add("__MACOSX_CORE__=1");	
			PublicIncludePaths.AddRange(new string[] {"Runtime/Core/Public/Apple"});
			
			if(Target.Platform == UnrealTargetPlatform.Mac)
				PublicIncludePaths.AddRange(new string[] {"Runtime/Core/Public/Mac"});
			else
				PublicIncludePaths.AddRange(new string[] {"Runtime/Core/Public/IOS"});

			PublicFrameworks.AddRange(new string[]
			{
				"CoreMIDI", "CoreAudio", "CoreFoundation"
			});
		}
		// LINUX
		else if (Target.Platform == UnrealTargetPlatform.Linux) {
            PublicDefinitions.Add("__LINUX_ALSA__=1");

			PublicIncludePaths.Add("Runtime/Core/Public/Linux");
			
			PublicAdditionalLibraries.AddRange(			
			new string[]
			{
				"libasound2.so", "libpthread.so"
			});
		}
    }
}
