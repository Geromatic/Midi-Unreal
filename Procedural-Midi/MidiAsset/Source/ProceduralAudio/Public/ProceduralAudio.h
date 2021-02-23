// Credit -> Scott Bishel

#pragma once

#include "Modules/ModuleManager.h"



class FProceduralAudioModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};