// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "MidiAssetPrivatePCH.h"
#include "ModuleInterface.h"


/**
 * Implements the MidiAsset module.
 */
class FMidiAssetModule
	: public IModuleInterface
{
public:

	// IModuleInterface interface

	virtual void StartupModule() override { }
	virtual void ShutdownModule() override { }

	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}
};


IMPLEMENT_MODULE(FMidiAssetModule, MidiAsset);
