// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "MidiAssetEditorPrivatePCH.h"
#include "Modules/ModuleInterface.h"

#include "./Styles/MidiAssetEditorStyle.h"


#define LOCTEXT_NAMESPACE "FMidiAssetEditorModule"


/**
 * Implements the MidiAssetEditor module.
 */
class FMidiAssetEditorModule
	: public IModuleInterface
{
public:

	// IModuleInterface interface

	virtual void StartupModule() override { 
		// Initialize & Register MidiAsset Style
		FMidiAssetEditorStyle::Initialize();
	}
	virtual void ShutdownModule() override { 
		// Unregister MidiAsset Style
		FMidiAssetEditorStyle::Shutdown();
	}

	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}
};


IMPLEMENT_MODULE(FMidiAssetEditorModule, MidiAssetEditor);


#undef LOCTEXT_NAMESPACE
