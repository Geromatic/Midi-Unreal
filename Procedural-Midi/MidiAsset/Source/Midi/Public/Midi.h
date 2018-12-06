// Copyright -> Scott Bishel

#pragma once

#include "Modules/ModuleManager.h"



class FMidiModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};