using UnrealBuildTool;

public class SimpleMinimap : ModuleRules
{
	public SimpleMinimap(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UMG", "Slate", "SlateCore" });

		// AddShaderSourceDirectoryMapping (module startup), IPluginManager.
		PrivateDependencyModuleNames.AddRange(new string[] { "RenderCore", "Projects" });

		// ISlateRHIRendererModule::CreateInstanceBuffer; the module is fetched at runtime.
		PrivateIncludePathModuleNames.Add("SlateRHIRenderer");
	}
}
