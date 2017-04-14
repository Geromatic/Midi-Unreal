// Copyright -> Scott Bishel

using UnrealBuildTool;
using System.IO;

public class MusicMacroLanguage : ModuleRules
{

    public MusicMacroLanguage(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				"MusicMacroLanguage/Public"
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"MusicMacroLanguage/Private",
                "MusicMacroLanguage/Classes",
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
