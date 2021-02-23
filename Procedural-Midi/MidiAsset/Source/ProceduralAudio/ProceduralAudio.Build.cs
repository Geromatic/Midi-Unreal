// Credit -> Scott Bishel

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
				"CoreUObject", "Engine", "Slate", "SlateCore", "Midi", "MidiAsset"
			}
			);
			
		// Windows
		if (Target.Platform == UnrealTargetPlatform.Win32 || Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicDefinitions.Add("__STDC_WANT_SECURE_LIB__=1"); // ignore warning
		}

		PublicDefinitions.Add("TSF_IMPLEMENTATION=1");
		PublicDefinitions.Add("TML_IMPLEMENTATION=1");

		PublicDefinitions.Add("TSF_NO_STDIO=1");
		PublicDefinitions.Add("TML_NO_STDIO=1");
	}
}
