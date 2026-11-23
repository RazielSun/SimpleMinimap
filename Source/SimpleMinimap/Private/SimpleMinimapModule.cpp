#include "SimpleMinimapModule.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "ShaderCore.h"

DEFINE_LOG_CATEGORY(LogSimpleMinimap);

class FSimpleMinimapModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		// Custom nodes in the plugin's materials include /Plugin/SimpleMinimap/Private/*.ush.
		// Needs LoadingPhase PostConfigInit: the mapping must exist before any material compiles.
		const FString ShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("SimpleMinimap"))->GetBaseDir(), TEXT("Shaders"));
		AddShaderSourceDirectoryMapping(TEXT("/Plugin/SimpleMinimap"), ShaderDir);
	}
};

IMPLEMENT_MODULE(FSimpleMinimapModule, SimpleMinimap)
