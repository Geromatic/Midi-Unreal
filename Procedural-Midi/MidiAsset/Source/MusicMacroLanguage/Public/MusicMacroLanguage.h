// Copyright -> Scott Bishel

#pragma once

#include "ModuleManager.h"



class FMusicMacroLanguageModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};