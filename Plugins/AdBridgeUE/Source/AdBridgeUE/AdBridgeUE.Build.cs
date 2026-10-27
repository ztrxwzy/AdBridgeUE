using System.IO;
using UnrealBuildTool;

public class AdBridgeUE : ModuleRules
{
	public AdBridgeUE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"DeveloperSettings"
			}
		);

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PrivateDependencyModuleNames.Add("Launch");

			string PluginPath =
				Utils.MakePathRelativeTo(
					ModuleDirectory,
					Target.RelativeEnginePath
				);

			AdditionalPropertiesForReceipt.Add(
				"AndroidPlugin",
				Path.Combine(
					PluginPath,
					"Android",
					"AdBridgeUE_UPL.xml"
				)
			);
		}
	}
}