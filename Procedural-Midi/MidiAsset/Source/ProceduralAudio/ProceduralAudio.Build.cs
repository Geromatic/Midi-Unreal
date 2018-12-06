// Copyright -> Scott Bishel

using UnrealBuildTool;
using System.IO;

public class ProceduralAudio : ModuleRules
{
#if WITH_FORWARDED_MODULE_RULES_CTOR
        public ProceduralAudio(ReadOnlyTargetRules Target) : base(Target)
#else
    public ProceduralAudio(TargetInfo Target)
#endif
	{

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
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
