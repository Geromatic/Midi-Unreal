// Copyright -> Scott Bishel

using UnrealBuildTool;
using System.IO;

public class ProceduralAudio : ModuleRules
{

    public ProceduralAudio(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				"ProceduralAudio/Public"
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"ProceduralAudio/Private",
                "ProceduralAudio/Classes",
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
