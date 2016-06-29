// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "MidiAssetEditorPrivatePCH.h"
#include "ModuleInterface.h"


#define LOCTEXT_NAMESPACE "FMidiAssetEditorModule"


/**
 * Implements the TextAssetEditor module.
 */
class FMidiAssetEditorModule
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


IMPLEMENT_MODULE(FMidiAssetEditorModule, MidiAssetEditor);


#undef LOCTEXT_NAMESPACE
