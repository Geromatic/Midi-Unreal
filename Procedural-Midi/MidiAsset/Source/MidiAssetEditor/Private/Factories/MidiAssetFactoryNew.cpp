// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "MidiAssetFactoryNew.h"
#include "MidiAssetEditorPrivatePCH.h"


/* UMidiAssetFactoryNew structors
 *****************************************************************************/

UMidiAssetFactoryNew::UMidiAssetFactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UMidiAsset::StaticClass();
	bCreateNew = false;
	bEditAfterNew = false;
}


/* UFactory overrides
 *****************************************************************************/

UObject* UMidiAssetFactoryNew::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMidiAsset>(InParent, InClass, InName, Flags);
}


bool UMidiAssetFactoryNew::ShouldShowInNewMenu() const
{
	return false;
}
